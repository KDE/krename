/***************************************************************************
                          batchrenamer.cpp  -  description
                             -------------------
    begin                : Sat Aug 18 2001
    copyright            : (C) 2001 by Dominik Seichter
    email                : domseichter@web.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

// OS includes
#include <stdio.h>
#ifndef _WIN32
#include <unistd.h>
#endif // _WIN32

#include "../config-krename.h"

// chmod:
#include <sys/types.h>
#include <sys/stat.h>

// QT includes
#include <QTextStream>

// KDE includes
#include <kio/job.h>
#include <KIO/MkpathJob>
#include <KJobWidgets>

// Own includes
#include "batchrenamer.h"
#include "progressdialog.h"
#include "pluginloader.h"
#include "plugin.h"

using namespace KIO;

static bool isToken( const QChar & token ) 
{
    const QChar tokens[] = {
        QChar('&'),
        QChar('$'),
        QChar('%'),
        QChar('#'),
        QChar('['),
        QChar(']'),
        QChar('\\'),
        QChar('/'),
        QChar('{'),
        QChar('}'),
        QChar('*')
    };
    const int count = 11;

    for( int i=0;i<count;i++ )
        if( token == tokens[i] )
            return true;

    return false;
}

static int getNextToken( const QString & text, QString & token, int pos = 0 ) 
{
    bool escaped = false;
    token = QString::null;

    if( pos < 0 )
        return -1;

    while( pos < text.length() ) 
    {
        if( !escaped && text[pos] == QChar('\\') )
        {
            escaped = true;
        }
        else if( !escaped && isToken( text[pos] ) )
        {
            token = text[pos];
            return ++pos;
        }
        else 
        {
            escaped = false;
        }

        ++pos;
    }

    return -1;
}

BatchRenamer::BatchRenamer()
    : m_index( 0 ), m_step( 1 ), m_files( NULL ), m_renameMode( eRenameMode_Rename )
{
    m_counter_index = 0;
    m_reset         = false;
    m_overwrite     = false;
}

BatchRenamer::~BatchRenamer()
{
}

void BatchRenamer::processFilenames()
{
    m_counters.clear();
    
    for( unsigned int i = 0; i < static_cast<unsigned int>(m_files->count()); i++) 
    {
        m_counter_index = 0;
        if( m_renameMode == eRenameMode_Rename ) // final Path = source Path
        {
            (*m_files)[i].setDstDirectory( (*m_files)[i].srcDirectory() );
            
            QUrl url = (*m_files)[i].srcUrl();
            url = url.adjusted(QUrl::RemoveFilename);
            
            (*m_files)[i].setDstUrl( url );
            
        } 
        else 
        {
            (*m_files)[i].setDstUrl( m_destination );
            (*m_files)[i].setDstDirectory( m_destination.path() );
        }
        
        if( i > 0 && m_reset )
            findCounterReset( i );
        
        //qDebug("SRCFILENAME       : %s", (*m_files)[i].srcFilename().toUtf8().data() );
        //qDebug("DSTFILENAME SHOULD: %s", processString( text, (*m_files)[i].srcFilename(), i ).toUtf8().data() );
        (*m_files)[i].setDstFilename( processString( text, (*m_files)[i].srcFilename(), i ) );
        //qDebug("DSTFILENAME IS    : %s", (*m_files)[i].dstFilename().toUtf8().data());
        (*m_files)[i].setDstExtension( processString( extext, (*m_files)[i].srcExtension(), i ) );
        
        // Let's run the plugins that change the final filename,
        // i.e the encodingsplugin
        int errors = 0;
        QString name = executePlugin( i, (*m_files)[i].dstFilename(), ePluginType_Filename, errors, NULL );
        if( !name.isNull() ) 
            (*m_files)[i].setDstFilename( name );
                
        /*
         * take care of renamed directories and
         * correct the paths of their contents
         */
        if( (m_renameMode == eRenameMode_Rename ||
             m_renameMode == eRenameMode_Move) &&
            (*m_files)[i].isDirectory() )
        {
            const QString topDir  = (*m_files)[i].realSrcDirectory() + '/' + (*m_files)[i].srcFilename(); 
            const QString replace = (*m_files)[i].dstDirectory() + '/' + (*m_files)[i].dstFilename();
            
            for( int z = i + 1; z < m_files->count(); z++ ) 
            {
                const QString & dir = (*m_files)[z].realSrcDirectory();
                if( dir.startsWith( topDir ) )
                {
                    QString newDir = replace + dir.right( dir.length() - topDir.length() );
                    if( newDir != dir ) {
                        (*m_files)[z].setOverrideSrcDirectory( newDir ); 
                    }
                }
            }
        }
        
#if 0      
        if( m_files[i].dir && (m_mode == RENAME || m_mode == MOVE) ) {
            for( unsigned int c = i; c < m_files.count(); c++ ) {
                if( m_files[c].src.directory.left( m_files[i].src.name.length() + 1 )
                    == ( m_files[i].src.name + "/" ) ) {

                    m_files[c].src.directory.replace( 0, m_files[i].src.name.length(), m_files[i].dst.name );
                    m_files[c].src.url.setPath( BatchRenamer::buildFilename( &m_files[c].src, true ) );
                }
            }
        }
#endif // 0
    }
}


void BatchRenamer::processFiles( ProgressDialog* p )
{
    int     errors = 0;
    QUrl    dest   = (*m_files)[0].dstUrl();
    // TODO: error handling if dest is empty
    
    // Give the user some information...
    p->setProgressTotalSteps( m_files->count() );
    p->setProgress( 0 );
    p->setDestination( dest );

    switch( m_renameMode ) 
    {
        default:
        case eRenameMode_Rename:
            p->print( i18n("Input files will be renamed.") );
            break;
        case eRenameMode_Copy:
            p->print( i18n("Files will be copied to: %1", dest.toDisplayString(QUrl::PreferLocalFile) ) );
            break;
        case eRenameMode_Move:
            p->print( i18n("Files will be moved to: %1", dest.toDisplayString(QUrl::PreferLocalFile) ) );
            break;
        case eRenameMode_Link:
            p->print( i18n("Symbolic links will be created in: %1", dest.toDisplayString(QUrl::PreferLocalFile) ) );
            break;
    }

    for( unsigned int i = 0; i < static_cast<unsigned int>(m_files->count()); i++) 
    {
        QUrl    dstUrl    = this->buildDestinationUrl( (*m_files)[i] );

        //p->print( QString( "%1 -> %2" ).arg( (*m_files)[i].srcUrl().prettyUrl() ).arg( dstUrl.toDisplayString() ) );
        p->setProgress( i + 1 );

        if( p->wasCancelled() )
            break;

        // Assemble filenames
        createMissingSubDirs( (*m_files)[i], p );

        KIO::JobFlags flags  = (m_overwrite ? KIO::Overwrite : KIO::DefaultFlags) | KIO::HideProgressInfo;
        KIO::Job*     job    = NULL;
        const QUrl &srcUrl =  (*m_files)[i].srcUrl();
        if( srcUrl == dstUrl ) 
        {
            p->warning( i18n("Cannot rename: source and target filename are equal: %1", srcUrl.toDisplayString(QUrl::PreferLocalFile)) );
            //(*m_files)[i].setError( 1 );
            //errors++;
            continue;
        }
                    
        switch( m_renameMode ) 
        {
            default:
            case eRenameMode_Rename:
            case eRenameMode_Move:
                job = KIO::file_move( srcUrl, dstUrl, -1, flags );
                break;
            case eRenameMode_Copy:
                job = KIO::file_copy( srcUrl, dstUrl, -1, flags );
                break;
            case eRenameMode_Link:
            {
                if( !srcUrl.isLocalFile() ) 
                {
                    // We can only do symlinks to local urls
                    p->error( i18n("Cannot create symlink to non-local URL: %1", srcUrl.toDisplayString()) );
                    (*m_files)[i].setError( 1 );
                    errors++;
                }
                else
                    job = KIO::symlink( srcUrl.path(), dstUrl, flags );

                break;
            }
        }

        if (job) {
            KJobWidgets::setWindow(job, p);
            if(!job->exec())
            {
                p->error( i18n("Error renaming %2 (to %1)",
                               dstUrl.toDisplayString(QUrl::PreferLocalFile),
                               srcUrl.toDisplayString(QUrl::PreferLocalFile)) );
                (*m_files)[i].setError( job->error() );
                errors++;
            }
        }

        /*
         * The renamed file should be on its correct location now,
         * so that we can call the last plugins (e.g. for changing permissions)
         *
         * Remember, the token argument is the filename for this type of plugins!
         *
         * If the return value is not empty an error has occured!
         * The plugin should return an error message in this case!
         */
        int errorCount = 0;
        this->executePlugin( i, dstUrl.path(), ePluginType_File, errorCount, p );
        errors += errorCount;
    }

    if( errors > 0 ) 
        p->warning( i18n("%1 errors occurred!", errors ) );

    p->print( i18n("KRename finished the renaming process."), "krename" );
    p->print( i18n("Press close to quit!") );
    bool enableUndo = (m_renameMode != eRenameMode_Copy);
    p->renamingDone( true, enableUndo, this, errors );
    
#if 0
    delete object;
    t.start();

    m_counters.clear();

    for( unsigned int i = 0; i < m_files->count(); i++) 
    {
	m_counter_index = 0;

        if( m_mode == RENAME ) {// final Path = source Path
            m_files[i].dst.directory = m_files[i].src.directory;
            m_files[i].dst.url = m_files[i].src.url;
            m_files[i].dst.url.setFileName( QString::null );
        } else {
            m_files[i].dst.directory = m_destination.path();
            m_files[i].dst.url = m_destination;
        }

        if( i == 0 )
            p->setDestination( m_files[i].dst.url );
	else
	{
	    if( m_reset )
		findCounterReset( i );
	}
	
        m_files[i].dst.name = processString( text, m_files[i].src.name, i );
        if( !extext.isEmpty() )
            m_files[i].dst.extension = processString( extext, m_files[i].src.extension, i );

        // Assemble filenames
        parseSubdirs( &m_files[i] );
        // TODO: DOM 
        // ESCAPE HERE
   
        m_files[i].src.name = BatchRenamer::buildFilename( &m_files[i].src, true );

        // Let's run the plugins that change the final filename,
        // i.e the encodingsplugin
        m_files[i].dst.name = parsePlugins( i, m_files[i].dst.name, TYPE_FINAL_FILENAME );

        m_files[i].dst.name = BatchRenamer::buildFilename( &m_files[i].dst, true );
                
        /*
         * take care of renamed directories and
         * correct the paths of their contents
         */
        if( m_files[i].dir && (m_mode == RENAME || m_mode == MOVE) ) {
            for( unsigned int c = i; c < m_files.count(); c++ ) {
                if( m_files[c].src.directory.left( m_files[i].src.name.length() + 1 )
                    == ( m_files[i].src.name + "/" ) ) {

                    m_files[c].src.directory.replace( 0, m_files[i].src.name.length(), m_files[i].dst.name );
                    m_files[c].src.url.setPath( BatchRenamer::buildFilename( &m_files[c].src, true ) );
                }
            }
        }
    }

    p->print( QString( i18n("Filenames Processed after %1 seconds.")).arg(t.elapsed()/1000) );

    work( p );
#endif // 0
}

void BatchRenamer::undoFiles( ProgressDialog* p )
{
    int     errors = 0;
    QUrl    dest   = (*m_files)[0].dstUrl();
    
    // Give the user some information...
    p->setProgressTotalSteps( m_files->count() );
    p->setProgress( 0 );
    p->setDestination( dest );
    p->print( i18n("Undoing all renamed files.") );

    for( unsigned int i = 0; i < static_cast<unsigned int>(m_files->count()); i++) 
    {
        QUrl dstUrl = this->buildDestinationUrl( (*m_files)[i] );

        //p->print( QString( "%1 -> %2" ).arg( (*m_files)[i].srcUrl().prettyUrl() ).arg( dstUrl.toDisplayString() ) );
        p->setProgress( i + 1 );

        if( p->wasCancelled() )
            break;

        KIO::JobFlags flags = (m_overwrite ? KIO::Overwrite : KIO::DefaultFlags) | KIO::HideProgressInfo;
        KIO::Job*     job   = NULL;
        switch( m_renameMode ) 
        {
            default:
            case eRenameMode_Rename:
            case eRenameMode_Move:
                job = KIO::file_move( dstUrl, (*m_files)[i].srcUrl(), -1, flags );
                break;
            case eRenameMode_Link:
                // In case of link delete created file
                job = KIO::file_delete( dstUrl, KIO::HideProgressInfo );
                break;
            case eRenameMode_Copy: // no undo possible
                // TODO: Maybe we should delete the created files
                break;
        }

        if (job) {
            KJobWidgets::setWindow(job, p);
            if(!job->exec())
            {
                p->error( i18n("Error during undoing %1", dstUrl.toDisplayString(QUrl::PreferLocalFile)) );
                (*m_files)[i].setError( job->error() );
                errors++;
            }
        }

    }

    if( errors > 0 ) 
        p->warning( i18n("%1 errors occurred!", errors ) );

    p->print( i18n("KRename finished the undo process."), "krename" );
    p->print( i18n("Press close to quit!") );
    p->renamingDone( false, false, this, errors ); // do not allow undo from undo

}

QString BatchRenamer::processBrackets( QString text, int* length, const QString & oldname, int index )
{
    int  pos   = 0;
    QString token;
    QString result;
 
    *length = 0;

    // MSG: qDebug("processBrackets: %s\n", text.toUtf8().data() );
    while( (pos = getNextToken( text, token, pos )) != -1 )
    {
        if( token == "[" ) 
        {
            int localLength = 0;
            QString substitute = processBrackets( text.right( text.length() - pos ), &localLength, oldname, index );
            text.replace( pos - 1, localLength, substitute );
            // MSG: qDebug("substituted: %s\n", text.toUtf8().data() );
            // Assure that *length does not become negative,
            // this will cause infinite loops
            if( localLength < substitute.length() ) 
            {
                *length += localLength;
            }
            else
            {
                *length += (localLength - substitute.length() );
            }
        }
        else if( token == "]" ) 
        {
            // Done with this token
            // MSG: qDebug("END: %s\n", text.left( pos - 1 ).toUtf8().data() );
            result = findToken( oldname, text.left( pos - 1 ), index );
            *length += pos + 1;
            break;
        }
    }
    // MSG: qDebug("processedBrackets: %s\n", result.toUtf8().data() );
    
    /*
     
      if( pos != -1 ) 
      {
      result = findToken( oldname, text.left( pos - 1 ), index );
      *length = pos+1; // skip any closing bracket
      }
    */
    return result;
}

QString BatchRenamer::processNumber( int length, const QString & appendix ) 
{
    tCounterValues countervalues;
    countervalues.start = m_index;
    countervalues.step = m_step;

    if( !appendix.isEmpty() ) 
    {
        bool ok = false;
        int tmp = appendix.section( ';', 0, 0 ).toInt( &ok ); // first section = start index
        if( ok )
            countervalues.start = tmp;
        
        tmp= appendix.section( ';', 1, 1 ).toInt( &ok ); // second section = stepping
        if( ok )
            countervalues.step = tmp;
    }

    if( (signed int)m_counters.count() <= m_counter_index )
    {
	countervalues.value = countervalues.start - countervalues.step;
	// other wise the counter would start at:
	// start + step instead of start
	m_counters.append( countervalues );
    }

    do {
	m_counters[m_counter_index].value += m_counters[m_counter_index].step;
    } while( m_skip.contains( m_counters[m_counter_index].value ) );
    
    QString number;
    number.sprintf("%0*i", length, m_counters[m_counter_index].value );

    ++m_counter_index;
    return number;
}

QString BatchRenamer::processString( QString text, const QString & originalName, int index, bool doFindReplace )
{
    QString oldname = originalName;
    doEscape( oldname );

    // Parse into tokens
    int pos=0;
    QString token;
    while( (pos = getNextToken( text, token, pos )) != -1 )
    {

        // Handle simple tokens
        if( token == "$" )
        {
            text.replace( pos - 1, token.length(), oldname );
            pos += oldname.length() - 1;
        }
        else if( token == "%" )
        {
            text.replace( pos - 1, token.length(), oldname.toLower() );
            pos += oldname.length() - 1;
        }
        else if( token == "&" )
        {
            text.replace( pos - 1, token.length(), oldname.toUpper() );
            pos += oldname.length() - 1;
        }
        else if( token == "*" ) 
        {
            QString tmp = capitalize( oldname ); 

            text.replace( pos - 1, token.length(), tmp );
            pos += tmp.length() - 1;            
        }
        else if( token == "[" ) 
        {
            int length = 0;
            QString substitute = processBrackets( text.right( text.length() - pos ), &length, oldname, index );
            text.replace( pos - 1, length, substitute );
            if( substitute.length() > 0 )
                pos += substitute.length() - 1;
        }
        else if( token == "]" ) 
        {
            // Ignore
        }
        else if( token == "#" ) 
        {
            int curPos = pos;
            int count  = 1;
            while( text[curPos] == '#' ) 
            {
                ++curPos;
                count++;
            }

            int length = curPos - pos + 1;
            int appendixLength = 0;
            QString appendix;
            if( text[curPos] == '{' ) 
            {
                int     appendixPos = curPos + 1;
                QString appendixToken;
                while( (appendixPos = getNextToken( text, appendixToken, appendixPos )) != -1 )
                {
                    if( appendixToken == "}" ) 
                    {
                        break;
                    }
                }

                if( appendixPos == -1 ) 
                {
                    // Do go into endless loop if token is not completed correctly
                    appendixPos = text.length();
                }

                // -2 because we have to go, before the current found token
                appendix = text.mid( curPos + 1, appendixPos - curPos - 2 );
                appendixLength = appendixPos - curPos;
            }

            QString number   = processNumber( count, appendix );
            text.replace( pos - 1, (length + appendixLength), number );

            if( number.length() > 0 )
                pos += number.length() - 1;
        }
    }

    //text = parsePlugins( i, text, TYPE_TOKEN );

    /*
     * Replace after Plugins !
     * Replace shoud be the last the
     * before re-escaping tokens !
     */
    if( doFindReplace ) {
        text = findReplace( text, originalName, index  );
    }
    text = unEscape( text );
    return text;
}

QString BatchRenamer::capitalize( const QString & text ) const
{
    QString tmp = text.toLower();
    if( tmp[0].isLetter() )
        tmp[0] = tmp[0].toUpper();
    
    for( int i = 0; i < tmp.length(); i++ )
        if( tmp[i+1].isLetter() && !tmp[i].isLetter() &&
            tmp[i] != '\'' && tmp[i] != '?' && tmp[i] != '`' )
            tmp[i+1] = tmp[i+1].toUpper();
    
    return tmp;
}

QString BatchRenamer::executePlugin( int index, const QString & filenameOrPath, int type, int & errorCount, ProgressDialog* p )
{
    const QList<Plugin*> & plugins = PluginLoader::Instance()->plugins();
    QList<Plugin*>::const_iterator it = plugins.begin();

    errorCount = 0;
    QString ret = filenameOrPath;
    while( it != plugins.end() ) 
    {
        if( (*it)->isEnabled() && ((*it)->type() & type) )
        {
            // Every plugin should use the return value of the previous as the new filename to work on
            ret = (*it)->processFile( this, index, ret, static_cast<EPluginType>(type) );
            if( type == ePluginType_File ) 
            {
                if( ret != QString::null ) 
                {
                    // An error occurred -> report it
                    if( p != NULL )
                        p->error( ret );
                    ++errorCount;
                }
                
                ret = filenameOrPath;
            }
        }

        ++it;
    }
    
    return ret;
}

void BatchRenamer::work( ProgressDialog*  )
{
#if 0 
    // TODO: use CopyJob here

    FileOperation fop;
    QFile* fundo ( NULL );
    QTextStream* tundo ( NULL );

    if( undo ) {
        // Create header for undo script
        fundo = new QFile( m_undoScript );
        if( fundo->open( IO_WriteOnly ) ) {
            tundo = new QTextStream( fundo );
            writeUndoScript( tundo );
        } else {
            undo = false;
            p->error( i18n("Can't create undo script :") + fundo->name() );
            delete fundo;
        }
    }

    int error = 0;
    RenamedList* renamedFiles = new RenamedList[m_files.count()];
    p->setProgressTotalSteps( m_files.count() + 1 );

    /*
     * Give the user some information...
     */
    if( m_mode == COPY)
        p->print( QString( i18n("Files will be copied to: %1") ).arg(m_files[0].dst.directory) );
    else if( m_mode == MOVE )
        p->print( QString( i18n("Files will be moved to: %1") ).arg(m_files[0].dst.directory) );
    else if( m_mode == LINK )
        p->print( QString( i18n("Symbolic links will be created in: %1") ).arg(m_files[0].dst.directory) );
    else if( m_mode == RENAME )
        p->print( i18n("Input files will be renamed.") );
    
    unsigned int i;
    for( i = 0; i < m_files.count(); i++) {
        p->setProgress( i+1 );

        if( p->wasCancelled() )
            break;

        KURL src = m_files[i].src.url;
        KURL dst = m_files[i].dst.url;
        dst.setPath( m_files[i].dst.name );

        renamedFiles[i].src = src;
        renamedFiles[i].dst = dst;
        renamedFiles[i].dir = m_files[i].dir;
        
        FileOperation fop;
        if( !fop.start( src, dst, m_mode, overwrite ) ) {
            p->error( fop.error() );
            renamedFiles[i].error = true;
            error++;
            continue;
        } else {
            renamedFiles[i].error = false;
        }

        // TODO: overwriting of files!
        /*
         * The renamed file should be on its correct location now,
         * so that we can call the last plugins (e.g. for changing permissions)
         *
         * Remember, the token argument is the filename for this type of plugins!
         *
         * If the return value is not empty an error has occured!
         * The plugin should return an error message in this case!
         */

        QString eplug = parsePlugins( i, QString::null, TYPE_FINAL_FILE );
        if( !eplug.isEmpty() ) {
            p->error( eplug );
            error++;
        }

        /* Create the undo script now */
        if( undo )
            if( dst.isLocalFile() && src.isLocalFile() ) {
                // Plugins ???
                (*tundo) << "echo \"" << dst.fileName()
                         << " -> " << src.fileName() << "\"" << endl;
                (*tundo) << "mv -f \"" << m_files[i].dst.name
                         << "\" \"" << m_files[i].src.name << "\"" << endl;
            } else
                p->warning( QString( i18n("Undo is not possible for remote file: %1") ).arg( dst.prettyURL() ) );

    }

    if( !p->wasCancelled() ) {
        QPtrListIterator<PluginLoader::PluginLibrary> it( plug->libs );
        for( ; it.current(); ++it ) {
            if( (*it)->usePlugin )
                (*it)->plugin->finished();
        }
    }

    const QString m = QString( i18n("Renamed %1 files successfully.") ).arg(i-error);
    ( i - error ) ? p->print( m ) : p->warning( m );

    if( error > 0 ) 
        p->warning( QString( i18n("%2 errors occurred!") ).arg(error));

    p->print( QString( i18n("Elapsed time: %1 seconds") ).arg( t.elapsed()/1000 ), "kalarm" );
    p->print( i18n("KRename finished the renaming process."), "krename" );
    p->print( i18n("Press close to quit!") );
    p->setRenamedFiles( renamedFiles, m_files.count() );
    
    if( undo ) {
        (*tundo) << endl << "echo \"Finished undoing " << m_files.count() << " actions.\"" << endl;
        delete tundo;
        fundo->close();

        // Make fundo exuteable
        if( chmod( (const char*)m_undoScript, (unsigned int) S_IRUSR | S_IWUSR | S_IXUSR ) )
            p->error( i18n("Can't set executable bit on undo script.") );
        delete fundo;
    }

    p->done( error, i-error, m_mode == MOVE || m_mode == RENAME );
    m_files.clear();
    delete []renamedFiles;
    delete this;
#endif // 0
}

const QUrl BatchRenamer::buildDestinationUrl( const KRenameFile & file ) const
{
    QUrl    dstUrl    = file.dstUrl();
    QString directory = file.dstDirectory();
    QString filename  = file.dstFilename();
    QString extension = file.dstExtension();
    QString manual    = file.manualChanges();

    if( !extension.isEmpty() )
    {
        filename += ".";
        filename += extension;
    }
    
    if( !manual.isNull() )
        filename = manual;

    dstUrl.setPath(directory + '/' +  filename );
    
    return dstUrl;
}

void BatchRenamer::escape( QString & text, const QString & token, const QString & sequence )
{
    text.replace( token, sequence );
}

QString & BatchRenamer::doEscape( QString & text )
{
    BatchRenamer::escape( text, "\\", "\\\\" );
    BatchRenamer::escape( text, "&", "\\&" );
    BatchRenamer::escape( text, "$", "\\$" );
    BatchRenamer::escape( text, "%", "\\%" );
    BatchRenamer::escape( text, "#", "\\#" );
    BatchRenamer::escape( text, "[", "\\[" );
    BatchRenamer::escape( text, "]", "\\]" );
    BatchRenamer::escape( text, "/", "\\/" );
    BatchRenamer::escape( text, "{", "\\{" );
    BatchRenamer::escape( text, "}", "\\}" );
    BatchRenamer::escape( text, "*", "\\*" );

    return text;
}

QString & BatchRenamer::unEscape( QString & text )
{ 
    BatchRenamer::escape( text, "\\\\", "\\" );
    BatchRenamer::escape( text, "\\&", "&" );
    BatchRenamer::escape( text, "\\$", "$" );
    BatchRenamer::escape( text, "\\%", "%" );
    BatchRenamer::escape( text, "\\#", "#" );
    BatchRenamer::escape( text, "\\[", "[" );
    BatchRenamer::escape( text, "\\]", "]" );
    // %252f == /, it seems that filenames on unix cannot contain
    // a /. So I use %252f, at least konqui displays it correctly
    // this was needed, so that plugins that return a slash do not cause errors
    BatchRenamer::escape( text, "\\/", "%2f" );
    BatchRenamer::escape( text, "\\{", "{" );
    BatchRenamer::escape( text, "\\}", "}" );
    BatchRenamer::escape( text, "\\*", "*" );
    
    return text;
}

QString BatchRenamer::processToken( QString token, QString oldname, int i )
{
    QString tmp;

    /*
     * Call here all functions that handle
     * arguments in brackets.
     */
    tmp = findPartStrings( oldname, token );
    if( !tmp.isEmpty() )
        return tmp;

    tmp = findDirName( token, (*m_files)[i].srcDirectory() );
    if( !tmp.isEmpty() )
        return tmp;

    tmp = findLength( token, (*m_files)[i].srcFilename() );
    if( !tmp.isEmpty() )
        return tmp;
        
    tmp = findTrimmed( token, (*m_files)[i].srcFilename(), i );
    if( !tmp.isEmpty() )
        return tmp;

    tmp = findDirSep( token, (*m_files)[i].srcFilename() );
    if( !tmp.isEmpty() )
        return tmp;

    Plugin* p = PluginLoader::Instance()->findPlugin( token );
    if( p )
    {
        tmp = p->processFile( this, i, token, ePluginType_Token );
        if( !tmp.isNull() )
        {
            doEscape( tmp );
            return tmp;
        }
    }

    /*
     * Maybe I should remove this!
     * KRename simply ignores unknown tokens!
     * Usefull for the MP3 Plugin!
     */
    return QString::null;
}

QString BatchRenamer::findToken( const QString & oldname, QString token, int i )
{
    enum conversion { LOWER, UPPER, MIXED, STAR, NONE, EMPTY, NUMBER };
    unsigned int numwidth = 0;
    
    conversion c = EMPTY;
    if( !token.left(1).compare("$") )
        c = NONE;
    else if( !token.left(1).compare("%") )
        c = LOWER;
    else if( !token.left(1).compare("&") )
        c = UPPER;
    else if( !token.left(1).compare("") )
        c = MIXED;
    else if( !token.left(1).compare("*") )
        c = STAR;
    else if( !token.left(1).compare("#") ) {
        while( !token.left(1).compare("#") ) {
            token.remove( 0, 1 );
            ++numwidth;
        }
        
        c = NUMBER;
    }

    if( c != EMPTY && c != NUMBER )
        token.remove( 0, 1 );
    
    QString save = token;
    token = processToken( token, oldname, i );

    switch( c ) {
        case LOWER:
            token = token.toLower();
            break;
        case UPPER:
            token = token.toUpper();
            break;
        case MIXED:
            token = token.toLower();
            token.replace( 0, 1, token[0].toUpper());
            break;
        case STAR:
            token = capitalize( token );
            break;
        case NUMBER:
            {
                bool b = false;
                int n = token.toInt( &b );
                if( b )
                    token = token.sprintf("%0*i", numwidth, n );
            }
            break;
        default:
            break;
    }

    return token;
}

QString BatchRenamer::findPartStrings( QString oldname, QString token )
{
    QString first, second;
    int pos = -1;
    
    // MSG: qDebug("PART: %s", token.toUtf8().data() );
    // parse things like [2;4{[dirname]}]
    if( token.count( '{' ) >= 1 && token.count( '}' ) >= 1 ) {
        int pos = token.indexOf( '{' );
        oldname = token.mid( pos + 1, token.lastIndexOf( '}' ) - pos - 1 );
        token = token.left( pos );
    }

    if( token.contains('-') ) {
        pos = token.indexOf( '-', 0 );
        first = token.left( pos );
        // ------- Code OK ^ !

        second = token.mid( pos+1, token.length() );

        // version < 1.7
        // return oldname.mid( first.toInt()-1, second.toInt()-first.toInt() +1 );
        // version > 1.7
        //return oldname.mid( first.toInt()-1, second.toInt()-first.toInt() );
        // version > 1.8

        bool ok;
        int sec = second.toInt( &ok );
        if( !ok || sec == 0 )
            sec = oldname.length();

        /*
         * x should not be larger than the old name
         * and not smaller than zero.
         */
        int x = sec-first.toInt( &ok );
        // if first is no number, but for example length, we return here so that findLength can do its job
        if( !ok ) 
            return QString::null;

        if( x > (signed int)oldname.length() || x < 0 )
            x = oldname.length()-first.toInt();

        /*
         * if I would comment my code I would understand this line :)
         * without this line, there is sometimes the last letter
         * of a filename missing.
         */
        if( x != -1 )
            x++;

        return oldname.mid( first.toInt()-1, x );
    } else if( token.contains(';') ) {
        pos = token.indexOf( ';', 0 );

        first = token.left( pos );
        second = token.mid( pos+1, token.length() );

        return oldname.mid( first.toInt()-1, second.toInt() );
    } else {
        bool ok = false;
        int number = token.toInt( &ok );

        if( ok && (number <= (signed int)oldname.length() && number > 0 ) )
            return QString(oldname[ number -1 ]);
        else
            return QString::null;
    }
}

QString BatchRenamer::findDirName( QString token, QString path )
{
    if( token.toLower().startsWith( "dirname" ) ) {
        if( path.right( 1 ) == "/" )
            path = path.left( path.length() - 1);

        int recursion = 1;
        if( token.length() > 7 ) {
            token = token.right( token.length() - 7 );
            recursion = token.count( '.' );
            if(  recursion != (signed int)token.length() )
                return QString::null;

            recursion++;
        }

        return path.section( "/", recursion * -1, recursion * -1);
    }
    
    return QString::null;
}

QString BatchRenamer::findDirSep( const QString & token, const QString & path )
{
    if( token.toLower() == "dirsep" ) {
        return "/";
    }
    
    return QString::null;
}

QString BatchRenamer::findLength( const QString & token, const QString & name )
{
    if( token.toLower().startsWith( "length" ) ) {
        int minus = 0;
        if( token.length() > 6 && token[6] == '-' ) {
            bool n = false;
            minus = token.mid( 7, token.length() - 7 ).toInt( &n );
            if( !n )
                minus = 0;
        }
        
        return QString::number( name.length() - minus );
    }
   
    return QString::null;
}

QString BatchRenamer::findTrimmed( const QString & token, const QString & name, int index )
{
    if( token.toLower().startsWith( "trimmed" ) ) {
        if( token.contains( ';' ) )
	{
	    QString processed = processString( 
		token.section( ';', 1, 1 ), name, index ).trimmed();
	    
	    if( processed.isNull() )
		return name.trimmed();
	    else
		return processed.trimmed();
	}
        else
            return name.trimmed();
    }
   
    return QString::null;
}

QString BatchRenamer::findReplace( const QString & text, const QString & origFilename, int index )
{
    QList<TReplaceItem>::const_iterator it = m_replace.begin();

    QString t( text );
    while( it != m_replace.end() )
    {
        QString find( (*it).find );

        // Call for each element in replace strings doReplace with correct values
        t = doReplace( t, unEscape( find ), (*it).replace, 
                       (*it).reg, (*it).doProcessTokens, 
                       origFilename, index );
        ++it;
    }

    return t;
}

QString BatchRenamer::doReplace( const QString & text, const QString & find, const QString & replace, bool reg, bool doProcessTokens, const QString & origFilename, int index )
{
    QString t( text );
    if( !reg ) 
    {
        QString escaped = find;
        escaped = doEscape( escaped );

        // we use the escaped text here because the user might want 
        // to find a "&" and replace it
        t.replace( escaped, replace );
    } 
    else
    {
        // no doEscape() here for the regexp, because it would destroy our regular expression
        // other wise we will not find stuff like $, [ in the text 
        t = unEscape( t ).replace( QRegExp( find ), replace );
        t = doEscape( t );
    }

        
    if( doProcessTokens ) {
        t = processString( unEscape( t ), origFilename, index, false );
    }


    return t;
}

void BatchRenamer::writeUndoScript( QTextStream* t )
{
    // write header comments
    (*t) << "#!/bin/sh" << endl
         << "# KRename Undo Script" << endl << "#" << endl
         << "# KRename was written by:" << endl
         << "# Dominik Seichter <domseichter@web.de>" << endl
         << "# http://krename.sourceforge.net" << endl << "#" << endl
         << "# Script generated by KRename Version: " << VERSION << endl << endl
         << "# This script must be started with the option --krename to work!" << endl;

    // write functions:
    (*t) << "echo \"KRename Undo Script\"" << endl
         << "echo \"http://krename.sourceforge.net\"" << endl
         << "echo \"\"" << endl;

    (*t) << "if test --krename = $1 ; then" << endl
         << "   echo \"\"" << endl
         << "else" << endl
         << "   echo \"You have to start this script\"" << endl
         << "   echo \"with the command line option\"" << endl
         << "   echo \"--krename\"" << endl
         << "   echo \"to undo a rename operation.\"" << endl
         << "   exit" << endl
         << "fi" << endl;
}

void BatchRenamer::createMissingSubDirs( const KRenameFile & file, ProgressDialog *dialog )
{
    QUrl url = file.dstUrl().adjusted(QUrl::RemoveFilename);
    if (url.isEmpty()) {
        return;
    }

    KIO::MkpathJob *job = KIO::mkpath(url);
    KJobWidgets::setWindow(job, dialog);
    if(!job->exec()) {
        dialog->error( i18n("Cannot create directory %1: %2",
                            url.toDisplayString(QUrl::PreferLocalFile),
                            job->errorString()) );
    }
}

void BatchRenamer::findCounterReset( int i )
{
    int z;
    if( (*m_files)[i-1].srcDirectory() != (*m_files)[i].srcDirectory() )
	for( z=0;z<(int)m_counters.count();z++ )
	{
	    m_counters[z].value = m_counters[z].start - m_counters[z].step;
	}
}
