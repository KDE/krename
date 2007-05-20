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

/* How many diferrent users and groups
 * KRename supports. Values over 1000
 * make KRename slow, but it may be
 * necessary on bigger systems to
 * increase this value.
 * MAXENTRIES must be < sizeof(int)
 */
#define MAXENTRIES 1000

/* May Value for SpinBoxes
 *
 */
#define SPINMAX 100000

enum {
    COPY, MOVE, RENAME, PREVIEW, LINK
};

/*
 * Changes made by hand by the user
 * in the preview list view are
 * stored here.
 */
typedef struct manualchanges {
    KUrl url;      // input filename
    QString user;  // name the user wants
};

typedef struct replacestrings {
    QString find;      // Text to replace
    QString replace;   // Replace with
    bool reg;          // is it a reg expression ?
};

typedef struct tCounterValues {
    int value;  // current value of this counter
    int start;  // start value of this counter (for findResetCounter)
    int step;   // stepping value of this counter;
};


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
class PluginLoader;

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

        /** Does the actuall renaming,
         *  transforming all source filenames in the file list
         *  to the new destination filenames.
         *
         *  No actuall renaming is done, only the new filenames
         *  are calculated.
         */
        void processFilenames();



        void processFiles( ProgressDialog* p, QObject* object );
        void createPreview( QListView* list );

        inline void setDirname( const KUrl & url ) { dirname = url; }
        inline void setUndoScript( const QString & t ) { m_undoScript = t; }
        inline void setUndo( bool b ) { undo = b; }
        inline void setOverwrite( bool b ) { overwrite = b; }
        inline void setIndex( int i ) { m_index = i; }
        inline void setStep( int s ) { m_step = s; }
	inline void setResetCounter( bool r ) { m_reset = r; }
        inline void setSkipList( const QList<int> & s ) { m_skip = s; }
        inline void setReplaceList( const QList<replacestrings> & r ) { m_replace = r; }
        
        inline void setChanges( const QList<manualchanges> & m ) { m_changes = m; }

        inline void setMode( int m) { m_mode = m; }
        inline int mode() const { return m_mode; }

        // Since 2.1 public, because plugins may want to access them to:
        QString findAndProcess( const QString & token, QString text, const QString & replace );

        QString findNumbers( QString text, int count, int i );
        QString findStar( const QString & oldname, QString text );
        QString findBrackets( QString oldname, QString text, int i );
        QString findToken( QString oldname, QString token, int i );
        QString processToken( QString token, QString oldname, int i );
        QString findPartStrings( QString oldname, QString token );
        static QString findDirName( QString token, QString path );
        QString findLength( const QString & token, const QString & name );
        QString findReplace( QString text );  // text is here already the new filename !
        QString doReplace( QString text, QString find, QString replace, bool reg ); // text is here already the new filename !

        QString processString( QString text, QString oldname, int i );
        
        static QString & doEscape( QString & text, bool filename = true );
        static QString & unEscape( QString & text );
        static void escape( QString & text, const QString & token, const QString & sequence );

        //static QString buildFilename( fileentry* entry, bool dir = true );

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

    private:
        /** 
         * Returns the length of the string when int n is converted to
         * a string.
         * @param n   a number whose length as string is needed
         * @returns stringlength of n converted to a string 
         */
        int getCharacters( int n ) ;

        void work( ProgressDialog* p );
        void writeUndoScript( QTextStream* t );
        //void parseSubdirs( data* f );
        void findNumberAppendix( QString & text, int pos, int* start, int* step );
	/** resets all counters to there start value if the directory name at @p i
	 *  in m_files changes.
	 *  The caller has to check m_reset before calling this function.
	 */
        void findCounterReset( int i );
        QString parsePlugins( int i, const QString & text, int type );
        bool applyManualChanges( int i );

        QString text;           // template
        KUrl dirname;           // destination dir
        QString extext;         // Extension template
        QString m_undoScript;     // Filename of undoscript
        bool undo;              // create an undo script
        int m_index;              // index for numbers
        int m_step;               // step for numbers
	bool m_reset;             // reset counter on new directories
        bool overwrite;         // overwrite existing files
        int m_mode;             // renaming mode
        QList<int> m_skip; // Numbers to skip
        QList<replacestrings> m_replace; // Replace strings
        QList<manualchanges> m_changes;  // User made changes
        PluginLoader* plug;

        // a is used in find number and
        // required for skipping.
        int m_counter_index;
	QList<tCounterValues> m_counters;

 private:
        KRenameFile::List* m_files;      ///< The list of files to rename and the resulting renamed filenames
        ERenameMode        m_renameMode; ///< The rename mode specifies if files are renamed, copied or moved (or linked)

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
    doEscape( text, false ); 
}

void BatchRenamer::setExtensionTemplate( const QString & t ) 
{ 
    extext = t;
    doEscape( extext, false ); 
}

#endif
