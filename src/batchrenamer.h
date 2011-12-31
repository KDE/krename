/***************************************************************************
                          batchrenamer.h  -  description
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

#ifndef BATCHRENAMER_H
#define BATCHRENAMER_H

#include <QDateTime>
#include <QList>
#include <QObject>

#include "krenamefile.h"

class QFile;
class QProgressDialog;
class QString;
class QTextStream;

typedef struct __tag_tCounterValues {
    int value;  // current value of this counter
    int start;  // start value of this counter (for findResetCounter)
    int step;   // stepping value of this counter;
} tCounterValues;

/** A structure describing a string or regular
 *  expression that is to be found in the final/resulting
 *  filename and to be replaced with another string.
 */
typedef struct __tag_TReplaceItem {
    QString find;         ///< Text to replace
    QString replace;      ///< Replace with
    bool reg;             ///< is it a reg expression ?
    bool doProcessTokens; ///< Process tokens in result
} TReplaceItem;

/** An enum to set the renaming mode of KRename
 */
enum ERenameMode {
    eRenameMode_Rename,  ///< All files are renamed in place
    eRenameMode_Move,    ///< All files are moved to a new directory and renamed
    eRenameMode_Copy,    ///< Only copies of the files are renamed
    eRenameMode_Link     ///< Symbolic links are created and renamed
};

class QObject;
class QListView;
class ProgressDialog;

/** This is the core class for renaming.
 *  
 *  It transforms the filenames according to 
 *  the users settings using custom functions
 *  and external plugins.
 *  Also the actual renaming, copying or moving
 *  of the files is done by this class
 *
 *  @author Dominik Seichter
 */
class BatchRenamer : public QObject {
    Q_OBJECT

    public:
        BatchRenamer();
        ~BatchRenamer();

        /** Sets the list of files
         *  which contains all filenames which should be transformed.
         * 
         *  The renamed filenames are stored in this list, too,.
         *
         *  @param list a list of KRenameFile objects
         */
        inline void setFiles( KRenameFile::List* list );

        /** Get access to the list of files used for renaming. 
         *  @returns the list of files or NULL if none was set.
         */
        inline const KRenameFile::List* files() const;

        /** 
         *  @returns the current renaming mode
         */
        inline ERenameMode renameMode() const;

        /** 
         * @returns the current start index for counters
         */
        inline int numberStartIndex() const;

        /**
         * @returns the stepping for counters
         */
        inline int numberStepping() const;

        /**
         * @returns if counters are reset on new directories
         */
        inline int numberReset() const;

        /** 
         * @returns the list of numbers that are skipped during renaming
         */
        inline QList<int> numberSkipList() const;

        /** Does the actuall renaming,
         *  transforming all source filenames in the file list
         *  to the new destination filenames.
         *
         *  No actuall renaming is done, only the new filenames
         *  are calculated.
         */
        void processFilenames();

        /** Process all files
         * 
         *  processFilenames has to be called before
         *
         *  All files will be renamed according to the users settings.
         *
         *  Output will be reported through a progress dialog
         *
         *  @param p use this dialog to report progress to the user
         */
        void processFiles( ProgressDialog* p );


        /** undo all files
         * 
         *  processFiles has to be called before
         *
         *  A previous renaming operation will be undone.
         *
         *  Output will be reported through a progress dialog
         *
         *  @param p use this dialog to report progress to the user
         */
        void undoFiles( ProgressDialog* p );

        /** Build the destination url from a KRenameFile
         *
         *  @param file a KRenameFile
         *  @returns a valid KUrl
         */
        const KUrl buildDestinationUrl( const KRenameFile & file ) const;

        inline void setUndoScript( const QString & t ) { m_undoScript = t; }
        inline void setUndo( bool b ) { undo = b; }

        inline void setReplaceList( const QList<TReplaceItem> & r ) { m_replace = r; }
        inline const QList<TReplaceItem> & replaceList() const { return m_replace; }

        inline void setMode( int m) { m_mode = m; }
        inline int mode() const { return m_mode; }

        QString findToken( const QString & oldname, QString token, int i );
        QString findPartStrings( QString oldname, QString token );
        static QString findDirName( QString token, QString path );

        /**
         * Replace the token [dirsep] with a slash "/".
         * This token is useful to create directories from within regular expressions.
         */
        static QString findDirSep( const QString & token, const QString & path );

        static QString & doEscape( QString & text );
        static QString & unEscape( QString & text );
        static void escape( QString & text, const QString & token, const QString & sequence );


        /** Capitalize a string.
         *
         *  Used to implement the * token.
         *
         *  @param a text string
         *  @return a capitalized version of this string (every first letter is a capital letter now)
         */
        QString capitalize( const QString & text ) const;
        
        /** Handle the [length] tokens
         *
         *  @param token a token found in square brackets
         *  @param name the filename of the current file
         *
         *  @return QString::null if no length token was found or the a new string
         */
        QString findLength( const QString & token, const QString & name );

        /** Handle the [trimmed] token
         *
         *  @param token a token found in square brackets
         *  @param name the filename of the current file
	 *  @param index index of the current file
         *
         *  @return QString::null if no length token was found or the a new string
         */
        QString findTrimmed( const QString & token, const QString & name, int index );

        QString processString( QString text, const QString & originalName, int i, bool doFindReplace = true );
        QString processBrackets( QString text, int* length, const QString & oldname, int index );
        QString processNumber( int length, const QString & appendix );
        QString processToken( QString token, QString oldname, int i );

    public slots:

        /** Sets the current mode of renaming.
         *  KRename can rename files, move them while
         *  renaming to another directory, rename copies
         *  or create renamed sym-links.
         *
         *  This mode specifies what should be done with the files.
         *
         *  @param mode the renaming mode.
         */
        inline void setRenameMode( ERenameMode mode );

        /** Sets the template for the filename that is used
         *  to transform the filename to its final representation.
         *
         *  @param t the new template
         */
        inline void setFilenameTemplate( const QString & t );

        /** Sets the template for the filename that is used
         *  to transform the filename to its final representation.
         *
         *  @param t the new template
         */
        inline void setExtensionTemplate( const QString & t );

        /** Set the start index for the basic counters
         *  which do not specify an own start index in their
         *  appendiy (e.g. ###{7}, means 7 is the start index
         *
         *  @param i start index
         */
        inline void setNumberStartIndex( int i ) { m_index = i; }

        /** Set the stepping for the basic counters
         *  which do not specify an own stepping in their
         *  appendiy (e.g. ###{1,2}, means 2 is the stepping
         *
         *  @param s stepping
         */
        inline void setNumberStepping( int s ) { m_step = s; }

        /** Sets wether all counters should be reset for new 
         *  directories
         *
         *  @param r if true counters will be reset
         */
	inline void setNumberReset( bool r ) { m_reset = r; }

        /** Sets the list of numbers that are skipped by counters
         *
         *  @param s a list of numbers that is skipped
         */
        inline void setNumberSkipList( const QList<int> & s ) { m_skip = s; }

        /** Sets if existing files maybe overwritten during renaming
         *
         *  @param overwrite if true existing files will be overwritten
         */
        inline void setOverwriteExistingFiles( bool overwrite ) { m_overwrite = overwrite; }

        /** Sets the destination url (a directory) for copy, move and link operations
         *
         *  @param url destination directory
         */
        inline void setDestinationDir( const KUrl & url ) { m_destination = url; }

     private:
        /** Do find and replace on the final resulting filename.
         *  
         *  \param text the new final filename with all other changes applied.
         *
         *  \returns the new filename with all find and replace being done.
         *
         *  \see m_replace
         */
        QString findReplace( const QString & text, const QString & origFilename, int index );

        /**
         * Replace one string (which might be a regular expression) in the final filename
         * with another string and return a new filename.
         *
         *  \param text the new final filename with all other changes applied.
         *  \param find the string or regular expression to find
         *  \param replace replace a matched string with this value
         *  \param reg if true treat find as regular expression
         *  \param doProcessTokens process tokens in replaced results
         *  \param originalName original filename for replacing tokens
         *  \param index current index
         *
         *  \returns the new filename with find and replace being done.
         */
        QString doReplace( const QString & text, const QString & find, const QString & replace, 
                           bool reg, bool doProcessTokens, const QString & origFilename, int index );

    private:
        /** Execute all plugins of a certain type
         *
         *  @param index the current index
         *  @param filenameOrPath the current filename or path
         *  @param type the type of the plugins to run
         *  @param errorCount the number of errors will be written to this value
         *  @param p dialog for error reporting (maybe NULL)
         *
         *  @returns either a new filename or an error message (depends on plugin type)
         */
        QString executePlugin( int index, const QString & filenameOrPath, int type, int & errorCount, ProgressDialog* p );

        void work( ProgressDialog* p );
        void writeUndoScript( QTextStream* t );
     
        /**
         * Parse a new filename and create missing subdirectories.
         *
         * This will look for '/' and create all not existing directories
         *
         * @param file to check for not existing directories
         * @param p ProgressDialog for error reporting
         */
         void createMissingSubDirs( const KRenameFile & file, ProgressDialog* p );

	     /** resets all counters to there start value if the directory name at @p i
          *  in m_files changes.
          *  The caller has to check m_reset before calling this function.
          */
        void findCounterReset( int i );

        QString text;           // template
        QString extext;         // Extension template
        QString m_undoScript;     // Filename of undoscript
        bool undo;              // create an undo script
	bool m_reset;             // reset counter on new directories
        int m_mode;             // renaming mode
        QList<int> m_skip; // Numbers to skip

        // a is used in find number and
        // required for skipping.
        int m_counter_index;
        int m_index;              // index for numbers
        int m_step;               // step for numbers

	QList<tCounterValues> m_counters;

 private:
        KRenameFile::List* m_files;       ///< The list of files to rename and the resulting renamed filenames
        ERenameMode        m_renameMode;  ///< The rename mode specifies if files are renamed, copied or moved (or linked)
        bool               m_overwrite;   ///< Overwrite existing files
        KUrl               m_destination; ///< Destination directory for copy, move and link

        QList<TReplaceItem> m_replace;    ///< List of strings for find and replace

 protected:
        QFile* f;
        QTime t;
        QProgressDialog* progress;
};

void BatchRenamer::setFiles( KRenameFile::List* list )
{
    m_files = list;
}

const KRenameFile::List* BatchRenamer::files() const
{
    return m_files;
}

void BatchRenamer::setRenameMode( ERenameMode mode )
{
    m_renameMode = mode;
}

ERenameMode BatchRenamer::renameMode() const
{
    return m_renameMode;
}

void BatchRenamer::setFilenameTemplate( const QString & t ) 
{ 
    text = t;
}

void BatchRenamer::setExtensionTemplate( const QString & t ) 
{ 
    extext = t;
}

int BatchRenamer::numberStartIndex() const
{
    return m_index;
}

int BatchRenamer::numberStepping() const
{
    return m_step;
}

int BatchRenamer::numberReset() const
{
    return m_reset;
}

QList<int> BatchRenamer::numberSkipList() const
{
    return m_skip;
}

#endif
