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

#ifndef VERSION
    #define VERSION "unknown"
#endif

// OS includes
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
// chmod:
#include <sys/types.h>
#include <sys/stat.h>

// QT includes
#include <qdir.h>
#include <qregexp.h>

// KDE includes
#include <kapplication.h>
#include <kio/job.h>
#include <kio/netaccess.h>
#include <klocale.h>

// Own includes
#include "batchrenamer.h"
#include "progressdialog.h"
#include "pluginloader.h"
#include "plugin.h"

using namespace KIO;

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

            KUrl url = (*m_files)[i].srcUrl();
            url.setFileName( QString::null );

            (*m_files)[i].setDstUrl( url );
            
        } 
        else 
        {
            (*m_files)[i].setDstUrl( m_destination );
            (*m_files)[i].setDstDirectory( m_destination.path() );
        }

        if( i > 0 && m_reset )
            findCounterReset( i );

        (*m_files)[i].setDstFilename( processString( text, (*m_files)[i].srcFilename(), i ) );
        (*m_files)[i].setDstExtension( processString( extext, (*m_files)[i].srcExtension(), i ) );

        (void)applyManualChanges( i );

        // Assemble filenames
        //parseSubdirs( &m_files[i] );
        // TODO: DOM 
        // ESCAPE HERE
        /*
        m_files[i].src.name = BatchRenamer::buildFilename( &m_files[i].src, true );

        // Let's run the plugins that change the final filename,
        // i.e the encodingsplugin
        m_files[i].dst.name = parsePlugins( i, m_files[i].dst.name, TYPE_FINAL_FILENAME );

        m_files[i].dst.name = BatchRenamer::buildFilename( &m_files[i].dst, true );
        */

#if 0      
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
#endif // 0
    }
}


void BatchRenamer::processFiles( ProgressDialog* p )
{
    int     errors = 0;
    KUrl    dest   = (*m_files)[0].dstUrl();
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
            p->print( i18n("Files will be copied to: %1", dest.prettyUrl() ) );
            break;
        case eRenameMode_Move:
            p->print( i18n("Files will be moved to: %1", dest.prettyUrl() ) );
            break;
        case eRenameMode_Link:
            p->print( i18n("Symbolic links will be created in: %1", dest.prettyUrl() ) );
            break;
    }

    for( unsigned int i = 0; i < static_cast<unsigned int>(m_files->count()); i++) 
    {
        KUrl    dstUrl    = this->buildDestinationUrl( (*m_files)[i] );

        //p->print( QString( "%1 -> %2" ).arg( (*m_files)[i].srcUrl().prettyUrl() ).arg( dstUrl.prettyUrl() ) );
        p->setProgress( i + 1 );

        if( p->wasCancelled() )
            break;

        KIO::Job* job;
        switch( m_renameMode ) 
        {
            default:
            case eRenameMode_Rename:
            case eRenameMode_Move:
                job = KIO::file_move( (*m_files)[i].srcUrl(), dstUrl, m_overwrite, false, false );
                break;
            case eRenameMode_Copy:
                job = KIO::file_copy( (*m_files)[i].srcUrl(), dstUrl, -1, m_overwrite, false, false );
                break;
            case eRenameMode_Link:
                //job = KIO::link( (*m_files)[i].srcUrl(), dstUrl, m_overwrite, false, false );
                break;
        }

        if( !NetAccess::synchronousRun( job, p ) ) 
        {
            p->error( i18n("Error during renaming %1", dstUrl.prettyUrl()) );
            (*m_files)[i].setError( 1 );
            errors++;
        } 
    }

    if( errors > 0 ) 
        p->warning( i18n("%1 errors occurred!", errors ) );

    p->print( i18n("KRename finished the renaming process."), "krename" );
    p->print( i18n("Press close to quit!") );
    bool enableUndo = (m_renameMode != eRenameMode_Copy);
    p->done( enableUndo, this, errors );
    
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

        (void)applyManualChanges( i );

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
    KUrl    dest   = (*m_files)[0].dstUrl();
    
    // Give the user some information...
    p->setProgressTotalSteps( m_files->count() );
    p->setProgress( 0 );
    p->setDestination( dest );
    p->print( i18n("Undoing all renamed files.") );

    for( unsigned int i = 0; i < static_cast<unsigned int>(m_files->count()); i++) 
    {
        KUrl dstUrl = this->buildDestinationUrl( (*m_files)[i] );

        //p->print( QString( "%1 -> %2" ).arg( (*m_files)[i].srcUrl().prettyUrl() ).arg( dstUrl.prettyUrl() ) );
        p->setProgress( i + 1 );

        if( p->wasCancelled() )
            break;

        KIO::Job* job;
        switch( m_renameMode ) 
        {
            default:
            case eRenameMode_Rename:
            case eRenameMode_Move:
                job = KIO::file_move( dstUrl, (*m_files)[i].srcUrl(), m_overwrite, false, false );
                break;
            case eRenameMode_Link:
                // In case of link delete created file
                job = KIO::file_delete( dstUrl, false );
                break;
                //case eRenameMode_Copy: // no undo possible
        }

        if( !NetAccess::synchronousRun( job, p ) ) 
        {
            p->error( i18n("Error during undoing %1", dstUrl.prettyUrl()) );
            (*m_files)[i].setError( 1 );
            errors++;
        } 
    }

    if( errors > 0 ) 
        p->warning( i18n("%1 errors occurred!", errors ) );

    p->print( i18n("KRename finished the undo process."), "krename" );
    p->print( i18n("Press close to quit!") );
    p->done( false, this, errors ); // do not allow undo from undo

}

QString BatchRenamer::processString( QString text, QString oldname, int i )
{
    /*
     * Come on! Grep for this text and help me!
     *
     * note about krename escape sequences
     * for certain characters:
     *
     * Krename will have problems with files
     * which contain one of the following
     * unicode characters: 60000, 60001, 60002
     * 60003, 60004, 60005, 60006.
     *
     * This is not a good solution, if you have a
     * better one please tell me about it!
     */
     
    doEscape( oldname );
    /*
     * Call here all functions that handle
     * arguments that are single tokens (&,%,...).
     * or in [brackets]
     */
    text = findBrackets( oldname, text, i );
    text = findAndProcess( "$", text, oldname );
    text = findAndProcess( "%", text, oldname.toLower() );
    text = findAndProcess( "&", text, oldname.toUpper() );
    text = findAndProcess( "\\", text, oldname.trimmed() );
    text = findStar( oldname, text );
    text = findNumbers( text, m_files->count(), i );
    /*
     * text is used as argument token for plugins!
     */
    //text = parsePlugins( i, text, TYPE_TOKEN );

    /*
     * Replace after Plugins !
     * Replace shoud be the last the
     * before re-escaping tokens !
     */
    text = findReplace( text );

    // convert special chars back (e.g. &,$)
    // TODO: this is to early, because 
    // parseSubdirs creates subdirectories
    // for "/" returned by plugins!!!!
    // text = unEscape( text );

    return text;
}

QString BatchRenamer::parsePlugins( int i, const QString& text, int type )
{
#if 0
    QPtrListIterator<PluginLoader::PluginLibrary> it( plug->libs );
    QString ret = text;

    if( type == TYPE_FINAL_FILE )
        ret = "";
        
    for( ; it.current(); ++it )
        if( (*it)->usePlugin && (*it)->plugin->type() == type )
        {
            ret = (*it)->plugin->processFile( this, i, text, type );
            doEscape( ret );
        }

    return ret;
#endif // 0
}

void BatchRenamer::work( ProgressDialog* p )
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
                (*tundo) << "echo \"" << src.fileName()
                         << " -> " << dst.fileName() << "\"" << endl;
                (*tundo) << "mv --force -b --suffix=.krename_ \"" << m_files[i].dst.name
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

const KUrl BatchRenamer::buildDestinationUrl( const KRenameFile & file ) const
{
    KUrl    dstUrl    = file.dstUrl();
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

    dstUrl.setDirectory( directory );
    dstUrl.setFileName( filename );
    
    return dstUrl;
}

void BatchRenamer::escape( QString & text, const QString & token, const QString & sequence )
{
    /*
     * NEVER, ABSOLUTELY NEVER change pos = 0
     * to pos = -1, it won't work !
     * This bug took hours to find and was
     * a serious bug in 1.7.
     */
#if QT_VERSION >= 0x030100
    text.replace( token, sequence );
#else
    int pos = 0;
    do {
       pos = text.find( token, pos );
       if( pos >= 0 )
           text.replace( pos, token.length(), sequence );
    } while ( pos >= 0 );
#endif
}

QString & BatchRenamer::doEscape( QString & text, bool filename )
{
    if( filename ) {
        BatchRenamer::escape( text, "&", QChar( 60000 ) );
        BatchRenamer::escape( text, "$", QChar( 60001 ) );
        BatchRenamer::escape( text, "%", QChar( 60002 ) );
        BatchRenamer::escape( text, "#", QChar( 60004 ) );
        BatchRenamer::escape( text, "[", QChar( 60005 ) );
        BatchRenamer::escape( text, "]", QChar( 60006 ) );
        BatchRenamer::escape( text, "\\", QChar( 60007 ) );
        BatchRenamer::escape( text, "/", QChar( 60008 ) );
        BatchRenamer::escape( text, "{", QChar( 60009 ) );        
        BatchRenamer::escape( text, "}", QChar( 60010 ) );        
        BatchRenamer::escape( text, "*", QChar( 60011 ) );        
    } else {
        BatchRenamer::escape( text, "\\&", QChar( 60000 ) );
        BatchRenamer::escape( text, "\\$", QChar( 60001 ) );
        BatchRenamer::escape( text, "\\%", QChar( 60002 ) );
        BatchRenamer::escape( text, "\\#", QChar( 60004 ) );
        BatchRenamer::escape( text, "\\[", QChar( 60005 ) );
        BatchRenamer::escape( text, "\\]", QChar( 60006 ) );
        BatchRenamer::escape( text, "\\\\", QChar( 60007 ) ); 
        BatchRenamer::escape( text, "\\/", QChar( 60008 ) );
        BatchRenamer::escape( text, "\\{", QChar( 60009 ) );        
        BatchRenamer::escape( text, "\\}", QChar( 60010 ) );      
        BatchRenamer::escape( text, "\\*", QChar( 60011 ) );          
    }

    return text;
}

QString & BatchRenamer::unEscape( QString & text )
{ 
    BatchRenamer::escape( text, QChar( 60000 ), "&" );
    BatchRenamer::escape( text, QChar( 60001 ), "$" );
    BatchRenamer::escape( text, QChar( 60002 ), "%" );
    BatchRenamer::escape( text, QChar( 60004 ), "#" );
    BatchRenamer::escape( text, QChar( 60005 ), "[" );
    BatchRenamer::escape( text, QChar( 60006 ), "]" );
    BatchRenamer::escape( text, QChar( 60007 ), "\\" );
    // %252f == /, it seems that filenames on unix cannot contain
    // a /. So I use %252f, at least konqui displays it correctly
    // this was needed, so that plugins that return a slash do not cause errors
    BatchRenamer::escape( text, QChar( 60008 ), "%2f" );
    BatchRenamer::escape( text, QChar( 60009 ), "{" );        
    BatchRenamer::escape( text, QChar( 60010 ), "}" );        
    BatchRenamer::escape( text, QChar( 60011 ), "*" );

    return text;
}

int BatchRenamer::getCharacters( int n )
{
    QString s;
    s.sprintf( "%i", n );
    return s.length();
}

QString BatchRenamer::findAndProcess( const QString & token, QString text, const QString & replace )
{
    /*
     * pos can here be -1 because
     * findRev is called with it as a
     * value !
     */
#if QT_VERSION >= 0x030100
    text.replace( token, replace );
#else
    int pos = -1;
    do {
        pos = text.findRev( token, pos );
        if( pos >= 0 )
            text.replace( pos, token.length(), replace );
    } while( pos >= 0 );
#endif
    return text;
}

QString BatchRenamer::findNumbers( QString text, int count, int i )
{
    // Rewritten in Version 0.8
    // Added numbers skipping in 1.3
    // Changed again in Version 1.8 to optimize it and fix a bug with skipping numbers
    int pos = 0, counter = 1;
    tCounterValues countervalues;
    countervalues.start = m_index;
    countervalues.step = m_step;

    if( text.contains( '#', Qt::CaseSensitive ) <= 0 )
        return text;

    pos = text.indexOf('#', pos);
    pos++;
    while( text[pos] == '#' ) {
        text.remove(pos, 1);
        counter++;
    }

    findNumberAppendix( text, pos, &countervalues.start, &countervalues.step );

    pos = text.indexOf('#', 0);

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
    
    /*
    int v = start + (i*step) + m_skip_add[m_counter_index];
    for( unsigned int z = 0; z < m_skip.count(); z++ ) {
        if( m_skip[z] == v ) {
            m_skip_add[m_counter_index] += step;
            v += step;
        }
    }
    */

    QString temp;
    temp.sprintf("%0*i", counter, m_counters[m_counter_index].value );
    text.replace( pos, 1, temp);

    ++m_counter_index;
    return findNumbers( text, count, i );
}

void BatchRenamer::findNumberAppendix( QString & text, int pos, int* start, int* step )
{
    QString appendix = QString::null;
    int tmp = 0;
    int end = 0;
    bool ok = false;
    
    if( text[pos] == '{' && (end = text.indexOf( '}', pos )) > -1)
    {
        //qDebug("Found an appendix:" + appendix );
        appendix = text.mid( pos + 1, end - pos - 1);
        text.remove( pos, end - pos + 1 );
       
        tmp = appendix.section( ';', 0, 0 ).toInt( &ok ); // first section = start index
        if( ok )
            *start = tmp;
        
        tmp = appendix.section( ';', 1, 1 ).toInt( &ok ); // second section = stepping
        if( ok )
            *step = tmp;
    }
}

QString BatchRenamer::findStar( const QString & oldname, QString text )
{
    int pos = -1;
    do {
        pos = text.lastIndexOf('*', pos);
        if( pos >= 0 ) {
            QString tmp = oldname.toLower();
            if( tmp[0].isLetter() )
                tmp[0] = tmp[0].toUpper();

            for( int i = 0; i < tmp.length(); i++ )
                if( tmp[i+1].isLetter() && !tmp[i].isLetter() &&
                    tmp[i] != '\'' && tmp[i] != '?' && tmp[i] != '`' )
                    tmp[i+1] = tmp[i+1].toUpper();

            text.replace( pos, 1, tmp);
        }
    } while( pos >= 0 );
    return text;
}

QString BatchRenamer::findBrackets( QString oldname, QString text, int i )
{
    /*
     * looks for a statement in brackets [ ]
     * and calls findToken() with this statement.
     */

    int pos = -1, a;
    QString token;

    if( !text.contains(']', Qt::CaseSensitive) || text.isEmpty() )
        return text;

    if(!text.contains('[', Qt::CaseSensitive) )
        return text;

    pos = text.lastIndexOf('[', pos);
    a = text.indexOf(']', pos );
    if( a < 0 && pos >= 0 )
        return text;

    if( pos < 0 && a >= 0 )
        return text;

     if( pos >= 0 && a >= 0 ) {
        token = text.mid( pos+1, (a-pos)-1 );
        
        // support [4-[length]]
        token = findBrackets( oldname, token, i );
        
        text.remove( pos, (a-pos)+1 );
        text.insert( pos, findToken( oldname, token, i ));
    }
    return findBrackets( oldname, text, i );
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
        
    Plugin* p = PluginLoader::Instance()->findPlugin( token );
    if( p )
    {
        tmp = p->processFile( this, i, token );
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

QString BatchRenamer::findToken( QString oldname, QString token, int i )
{
    enum conversion { LOWER, UPPER, MIXED, STAR, STRIP, NONE, EMPTY, NUMBER };
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
    else if( !token.left(1).compare("\\") )
        c = STRIP;
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
            token = findStar( token, "*" );
            break;
        case STRIP:
            token = token.trimmed();
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

    doEscape( token );
    return token;
}

QString BatchRenamer::findPartStrings( QString oldname, QString token )
{
    QString first, second;
    int pos = -1;
    
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

QString BatchRenamer::findReplace( const QString & text )
{
    QList<TReplaceItem>::const_iterator it = m_replace.begin();

    QString t( text );
    while( it != m_replace.end() )
    {
        QString find( (*it).find );

        // Call for each element in replace strings doReplace with correct values
        t = doReplace( t, unEscape( find ), (*it).replace, (*it).reg );
        ++it;
    }

    return t;
}

QString BatchRenamer::doReplace( const QString & text, const QString & find, const QString & replace, bool reg )
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
        t = doEscape( unEscape( t ).replace( QRegExp( find ), replace ) );
    }

    return t;
}

void BatchRenamer::writeUndoScript( QTextStream* t )
{
    // write header comments
    (*t) << "#!/bin/bash" << endl
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

#if 0
void BatchRenamer::parseSubdirs( data* f )
{
    int pos = 0;
    if( (pos = f->dst.name.findRev( "/", -1 ) ) > 0 ) {
        QString dirs = f->dst.name.left( pos );
        f->dst.name = f->dst.name.right( f->dst.name.length() - pos - 1 );
        f->dst.directory += ( f->dst.directory.right( 1 ) == "/" ) ? "" : "/";

        // create the missing subdir now
        int i = 0;
        QString d = "";
        while( (d = dirs.section( "/", i, i, QString::SectionSkipEmpty )) && ! d.isEmpty() ) { // asignment here!
            KURL url = f->dst.url;
            // it is important to unescape here
            // to support dirnames containing "&" or 
            // similar tokens
            url.addPath( unEscape( d ) );
            if( !NetAccess::exists( url ) && !NetAccess::mkdir( url ) )
                // TODO: GUI bug report
                qDebug("Can't create %s", url.prettyURL().latin1() ); 

            f->dst.url.addPath( d );
            f->dst.directory.append( d + "/" );
            i++;
        }
    }
}

QString BatchRenamer::buildFilename( fileentry* entry, bool dir )
{
    QString filename = ( dir ? entry->directory : QString::null ) + entry->name + ( entry->extension.isEmpty() ? QString::null : QString(".") ) + entry->extension;
    // unescape here as filename is still escaped
    unEscape( filename );
    return filename;
}
#endif // 0

bool BatchRenamer::applyManualChanges( int i )
{
    /*
     * The last step: make changes of
     * the user visible
     */

    if( !m_changes.isEmpty() )
        for( unsigned int z = 0; z < m_changes.count(); z++ ) {
            KUrl file = m_changes[z].url;
            if( file == (*m_files)[i].srcUrl() ) {
                (*m_files)[i].setDstFilename( m_changes[z].user );

                // the file extension is already included
                // in the users name
                (*m_files)[i].setDstExtension( QString::null );
                return true;
            }
        }

    return false;
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

#include "batchrenamer.moc"

