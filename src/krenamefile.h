/***************************************************************************
                          krenamefile.h  -  description
                             -------------------
    begin                : Wed Apr 18 2007
    copyright            : (C) 2007 by Dominik Seichter
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

#ifndef _KRENAME_FILE_H_
#define _KRENAME_FILE_H_

#include <QVector>

#include <kfileitem.h>
#include <kurl.h>
#include <QPixmap>

class KFileItem;

/** An enum to describe the mode to split 
 *  filename and extension.
 */
enum ESplitMode {
    eSplitMode_FirstDot,    ///< Extension starts at the first dot found in the filename
    eSplitMode_LastDot,     ///< Extension starts at the last dot found in the filename
    eSplitMode_NoExtension, ///< Do not use file extension handling
    eSplitMode_CustomDot    ///< Extension starts at a user defined dot in the filename
};

/**
 * Type of manual change made.
 * Specifies on what kind of filename, the manual changes are based.
 */
enum EManualChangeMode {
    eManualChangeMode_None, ///< Use filename created by KRename
    eManualChangeMode_Input, ///< Use custom filename, based on input filename
    eManualChangeMode_Custom ///< Use custom filename
};

class KRenameFile {
    struct TFileDescription {
        QString filename;
        QString extension;
        QString directory;
        
        KUrl    url;

        const TFileDescription & operator=( const TFileDescription & rhs ) 
        {
            filename  = rhs.filename;
            extension = rhs.extension;
            directory = rhs.directory;
            url       = rhs.url;

            return *this;
        }
    };

 public:
    
    /** A list of KRenameFile objects
     */
    typedef QVector<KRenameFile> List;

    /** Empty default constructor
     *  which creates an invalid KRenameFile.
     *
     *  \see isValid
     */
    KRenameFile()
        : m_bValid( false )
    {
    }

    /** Construct a new KRenameFile from an url.
     *  
     *  The url will be tested for existance
     *  and isValid() returns only true
     *  if the url is existing.
     *
     *  \param src an url of a file or directory
     *  \param eSplitMode splitmode which is used to separate
     *                    filename and extension
     *  \param dot dot to use as separator for eSplitMode_CustomDot
     *  \see isValid()
     */
    KRenameFile( const KUrl & src, ESplitMode eSplitMode, unsigned int dot );

    /** Construct a new KRenameFile from an url.
     *
     *  The url is expected to exist and is not 
     *  tested for existance. This is much faster than
     *  the other constructor.
     *
     *  \param src an url of a file or directory
     *  \param directory must be true if the url referes
     *                   to a directory.
     *  \param eSplitMode splitmode which is used to separate
     *                    filename and extension
     *  \param dot dot to use as separator for eSplitMode_CustomDot
     */
    KRenameFile( const KUrl & src, bool directory, ESplitMode eSplitMode, unsigned int dot );

    /** Construct a new KRenameFile form a KFileItem which is faster
     *  than construction from an URL.
     *
     *  \param item a KFileItem
     *  \param eSplitMode splitmode which is used to separate
     *                    filename and extension
     *  \param dot dot to use as separator for eSplitMode_CustomDot
     */
    KRenameFile( const KFileItem& item, ESplitMode eSplitMode, unsigned int dot );

    /** Copy constructor
     *  \param rhs KRenameFile to copy
     */
    KRenameFile( const KRenameFile & rhs );

    /** Set the splitmode to separate filename from fileextension
     *  
     *  \param eSplitMode splitmode which is used to separate
     *                    filename and extension
     *  \param dot dot to use as separator for eSplitMode_CustomDot
     *
     *  \see srcFilename() 
     *  \see srcExtension()
     */
    void setCurrentSplitMode( ESplitMode eSplitMode, unsigned int dot = 1 );

    /** 
     * \returns the number of dots in this filename that can be used to separate filename and extension
     */
    int dots() const;

    /** Convert the KRenameFile into a string
     *  that can be displayed to the user.
     *
     *  \returns original source url as string representation
     */
    inline const QString toString() const
    {
        return m_src.url.prettyUrl();
    }        

    /** Get a preview icon of the KRenameFile
     *  
     *  @returns a QPixmap containing a preview of this KRenameFile.
     *           This might be only a mimetype icon depending on the current KDE settings.
     */
    inline const QPixmap & icon() const
    {
        if( m_icon.isNull() )
            const_cast<KRenameFile*>(this)->loadPreviewIcon();

        return m_icon;
    }

    /** Set the preview icon of the KRenameFile
     *
     *  @param icon a preview icon.
     *
     *  This method is only here for performance reason
     *  to easily and fast set the icon from one 
     *  KRenameFile onto another.
     *
     *  Normally KRenameFile knows how to load the icon itselfs.
     */
    inline void setIcon( const QPixmap & icon ) 
    {
        m_icon = icon;
    }

    /** Set this error code to value != 0
     *  if an error has occured during renaming this
     *  particular file.
     *
     *  @param error an error code (0 means no error)
     */
    inline void setError( int error ) 
    {
        m_error = error;;
    }

    /**
     * @returns true if an error code was set for this KRenameFile
     */
    inline bool hasError() const
    {
        return m_error != 0;
    }

    /** Assigns another KRenameFile to this KRenameFile
     *  \param rhs object to assign
     */
    const KRenameFile & operator=( const KRenameFile & rhs );

    /** Compare a KRenameFile object to a KFileItem
     * 
     *  \returns true if the file item of this KRenameFile
     *           is identical to the parameter
     */
    bool operator==( const KFileItem & item ) const;

    /** 
     * \returns true if this file references 
     *               an existing file or directory
     */
    inline bool isValid() const
    {
        return m_bValid;
    }

    /** 
     * \returns manualChanges the user has made to the filename
     */
    inline const QString & manualChanges() const
    {
        return m_manual;
    }

    /**
     * Sets manual changes made by the user
     *
     * \param manual manual changes for filename and extension
     * \param mode mode of change
     */
    inline void setManualChanges( const QString & manual, EManualChangeMode mode ) 
    {
        m_manual = manual;
        m_manualMode = mode;
    }

    /**
     * \returns the change mode
     */
    inline EManualChangeMode manualChangeMode() const
    {
        return m_manualMode;
    }

    /** 
     * \returns always the original source directory
     */
    inline const QString & realSrcDirectory() const 
    {
            return m_src.directory;
    }

    inline void setOverrideSrcDirectory( const QString & dir ) 
    {
        m_overrideDir = dir;
    }

    inline const QString & srcFilename() const 
    {
        return m_src.filename;
    }

    inline const QString & srcExtension() const 
    {
        return m_src.extension;
    }
    
    inline const QString & srcDirectory() const 
    {
        return (m_overrideDir.isNull() ? m_src.directory : m_overrideDir);
    }
    
    const KUrl srcUrl() const; 
    
    inline void setDstFilename( const QString & filename ) 
    {
        m_dst.filename = filename;
    }
    
    inline const QString & dstFilename() const 
    {
        return m_dst.filename;
    }
    
    inline void setDstExtension( const QString & extension ) 
    {
        m_dst.extension = extension;
    }
    
    inline const QString & dstExtension() const 
    {
        return m_dst.extension;
    }
    
    inline void setDstDirectory( const QString & directory ) 
    {
        m_dst.directory = directory;
    }
    
    inline const QString & dstDirectory() const 
    {
        return m_dst.directory;
    }
    
    inline void setDstUrl( const KUrl & url ) 
    {
        m_dst.url = url;
    }

    inline const KUrl & dstUrl() const 
    {
        return m_dst.url;
    }
    
    inline bool isDirectory() const
    {
        return m_bDirectory;
    }

    /**
     * Get access to the internal file item
     *
     * @returns a KFileItem
     */
    const KFileItem & fileItem() const;

    /**
     * @returns the default icon size
     */
    static int getDefaultIconSize();

 private:
    void initFileDescription( TFileDescription & rDescription, const KUrl & url, ESplitMode eSplitMode, unsigned int dot ) const;

    /** Load a preview icon for this KRenameFile object
     *  using KDEs preview loading mechanism.
     */
    void loadPreviewIcon();

    static const int DEFAULT_ICON_SIZE;
    static const char* EXTRA_DATA_KEY;

 private:
    KFileItem        m_fileItem;

    TFileDescription m_src;
    TFileDescription m_dst;

    QString          m_overrideDir;  ///< A changed sourcedirectory (required when renaming directories)

    bool             m_bDirectory;   ///< If true this is a directory
    bool             m_bValid;       ///< If true this item is valid

    QPixmap          m_icon;         ///< This is the file preview icon

    int              m_error;        ///< This value is set to != 0 if an error occurred during renaming
    QString          m_manual;       ///< Manual changes to the filename+extension by the user are stored here
    EManualChangeMode m_manualMode;  
};

#endif // _KRENAME_FILE_H_
