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

#include <kurl.h>

/** An enum to describe the mode to split 
 *  filename and extension.
 */
enum ESplitMode {
    eSplitMode_FirstDot, ///< Extension starts at the first dot found in the filename
    eSplitMode_LastDot,  ///< Extension starts at the last dot found in the filename
    eSplitMode_CustomDot ///< Extension starts at a user defined dot in the filename
};

class KRenameFile {
    typedef struct TFileDescription {
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
    KRenameFile()
        : m_bValid( false )
    {
    }

    KRenameFile( KUrl src );
    KRenameFile( const KRenameFile & rhs );

    typedef QVector<KRenameFile> List;

    inline const QString toString() const
        {
            return m_src.url.prettyUrl();
        }        

    const KRenameFile & operator=( const KRenameFile & rhs );

    /** 
     * \returns true if this file references 
     *               an existing file or directory
     */
    inline bool isValid() const
    {
        return m_bValid;
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
            return m_src.directory;
        }

    inline const KUrl & srcUrl() const 
        {
            return m_src.url;
        }

    inline const QString & dstFilename() const 
        {
            return m_dst.filename;
        }

    inline const QString & dstExtension() const 
        {
            return m_dst.extension;
        }

    inline const QString & dstDirectory() const 
        {
            return m_dst.directory;
        }

    inline const KUrl & dstUrl() const 
        {
            return m_dst.url;
        }

    inline bool isDirectory() const
        {
            return m_bDirectory;
        }

 private:
    void initFileDescription( TFileDescription & rDescription, const KUrl & url, ESplitMode eSplitMode, int dot ) const;

 private:
    TFileDescription m_src;
    TFileDescription m_dst;

    bool             m_bDirectory;
    bool             m_bValid;
};

#endif // _KRENAME_FILE_H_
