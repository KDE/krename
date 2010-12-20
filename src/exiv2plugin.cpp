/***************************************************************************
                      exiv2plugin.cpp  -  description
                             -------------------
    begin                : Thu Oct 9th 2008
    copyright            : (C) 2008 by Dominik Seichter
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

#include "../config-krename.h"

#ifdef HAVE_EXIV2
#include "exiv2plugin.h"

#include <klocale.h>

#include <exiv2/error.hpp>
#include <exiv2/exif.hpp>
#include <exiv2/image.hpp>
#include <exiv2/iptc.hpp>
#include <exiv2/xmp.hpp>
#include <exiv2/tags.hpp>

#include "batchrenamer.h"

using namespace Exiv2;

static const char* exifTags[] = {
    "Exif.Image.ImageWidth",
    "Exif.Image.ImageLength",
    "Exif.Image.BitsPerSample",
    "Exif.Image.Compression",
    "Exif.Image.PhotometricInterpretation",
    "Exif.Image.Orientation",
    "Exif.Image.SamplesPerPixel",
    "Exif.Image.PlanarConfiguration",
    "Exif.Image.YCbCrSubSampling",
    "Exif.Image.YCbCrPositioning",
    "Exif.Image.XResolution",
    "Exif.Image.YResolution",
    "Exif.Image.ResolutionUnit",
    "Exif.Image.TransferFunction",
    "Exif.Image.WhitePoint",
    "Exif.Image.PrimaryChromaticities",
    "Exif.Image.YCbCrCoefficients",
    "Exif.Image.ReferenceBlackWhite",
    "Exif.Image.DateTime",
    "Exif.Image.ImageDescription",
    "Exif.Image.Make",
    "Exif.Image.Model",
    "Exif.Image.Software",
    "Exif.Image.Artist",
    "Exif.Image.Copyright",
    "Exif.Photo.ExifVersion",
    "Exif.Photo.FlashpixVersion",
    "Exif.Photo.ColorSpace",
    "Exif.Photo.ComponentsConfiguration",
    "Exif.Photo.CompressedBitsPerPixel",
    "Exif.Photo.PixelXDimension",
    "Exif.Photo.PixelYDimension",
    "Exif.Photo.UserComment",
    "Exif.Photo.RelatedSoundFile",
    "Exif.Photo.DateTimeOriginal",
    "Exif.Photo.DateTimeDigitized",
    "Exif.Photo.ExposureTime",
    "Exif.Photo.FNumber",
    "Exif.Photo.ExposureProgram",
    "Exif.Photo.SpectralSensitivity",
    "Exif.Photo.ISOSpeedRatings",
    "Exif.Photo.OECF",
    "Exif.Photo.ShutterSpeedValue",
    "Exif.Photo.ApertureValue",
    "Exif.Photo.BrightnessValue",
    "Exif.Photo.ExposureBiasValue",
    "Exif.Photo.MaxApertureValue",
    "Exif.Photo.SubjectDistance",
    "Exif.Photo.MeteringMode",
    "Exif.Photo.LightSource",
    "Exif.Photo.Flash",
    "Exif.Photo.FocalLength",
    "Exif.Photo.SubjectArea",
    "Exif.Photo.FlashEnergy",
    "Exif.Photo.SpatialFrequencyResponse",
    "Exif.Photo.FocalPlaneXResolution",
    "Exif.Photo.FocalPlaneYResolution",
    "Exif.Photo.FocalPlaneResolutionUnit",
    "Exif.Photo.SubjectLocation",
    "Exif.Photo.ExposureIndex",
    "Exif.Photo.SensingMethod",
    "Exif.Photo.FileSource",
    "Exif.Photo.SceneType",
    "Exif.Photo.CFAPattern",
    "Exif.Photo.CustomRendered",
    "Exif.Photo.ExposureMode",
    "Exif.Photo.WhiteBalance",
    "Exif.Photo.DigitalZoomRatio",
    "Exif.Photo.FocalLengthIn35mmFilm",
    "Exif.Photo.SceneCaptureType",
    "Exif.Photo.GainControl",
    "Exif.Photo.Contrast",
    "Exif.Photo.Saturation",
    "Exif.Photo.Sharpness",
    "Exif.Photo.DeviceSettingDescription",
    "Exif.Photo.SubjectDistanceRange",
    "Exif.Photo.ImageUniqueID",
    "Exif.GPSInfo.GPSVersionID",
    "Exif.GPSInfo.GPSLatitude",
    "Exif.GPSInfo.GPSLongitude",
    "Exif.GPSInfo.GPSAltitudeRef",
    "Exif.GPSInfo.GPSAltitude",
    "Exif.GPSInfo.GPSTimeStamp",
    "Exif.GPSInfo.GPSSatellites",
    "Exif.GPSInfo.GPSStatus",
    "Exif.GPSInfo.GPSMeasureMode",
    "Exif.GPSInfo.GPSDOP",
    "Exif.GPSInfo.GPSSpeedRef",
    "Exif.GPSInfo.GPSSpeed",
    "Exif.GPSInfo.GPSTrackRef",
    "Exif.GPSInfo.GPSTrack",
    "Exif.GPSInfo.GPSImgDirectionRef",
    "Exif.GPSInfo.GPSImgDirection",
    "Exif.GPSInfo.GPSMapDatum",
    "Exif.GPSInfo.GPSDestLatitude",
    "Exif.GPSInfo.GPSDestLongitude",
    "Exif.GPSInfo.GPSDestBearingRef",
    "Exif.GPSInfo.GPSDestBearing",
    "Exif.GPSInfo.GPSDestDistanceRef",
    "Exif.GPSInfo.GPSDestDistance",
    "Exif.GPSInfo.GPSProcessingMethod",
    "Exif.GPSInfo.GPSAreaInformation",
    "Exif.GPSInfo.GPSDifferential",
    NULL
};

static const char* xmpTags[] = {
    "Xmp.tiff.ImageWidth",
    "Xmp.tiff.ImageLength",
    "Xmp.tiff.BitsPerSample",
    "Xmp.tiff.Compression",
    "Xmp.tiff.PhotometricInterpretation",
    "Xmp.tiff.Orientation",
    "Xmp.tiff.SamplesPerPixe",
    "Xmp.tiff.PlanarConfiguration",
    "Xmp.tiff.YCbCrSubSampling",
    "Xmp.tiff.YCbCrPositioning",
    "Xmp.tiff.XResolution",
    "Xmp.tiff.YResolution",
    "Xmp.tiff.ResolutionUnit",
    "Xmp.tiff.TransferFunction",
    "Xmp.tiff.WhitePoint",
    "Xmp.tiff.PrimaryChromaticities",
    "Xmp.tiff.YCbCrCoefficients",
    "Xmp.tiff.ReferenceBlackWhite",
    "Xmp.tiff.DateTime",
    "Xmp.dc.description",
    "Xmp.tiff.Make",
    "Xmp.tiff.Model",
    "Xmp.tiff.Software",
    "Xmp.dc.creator",
    "Xmp.dc.rights",
    "Xmp.exif.ExifVersion",
    "Xmp.exif.FlashpixVersion",
    "Xmp.exif.ColorSpace",
    "Xmp.exif.ComponentsConfiguration",
    "Xmp.exif.CompressedBitsPerPixel",
    "Xmp.exif.PixelXDimension",
    "Xmp.exif.PixelYDimension",
    "Xmp.exif.UserComment",
    "Xmp.exif.RelatedSoundFile",
    "Xmp.exif.DateTimeOriginal",
    "Xmp.exif.DateTimeDigitized",
    "Xmp.exif.ExposureTime",
    "Xmp.exif.FNumber",
    "Xmp.exif.ExposureProgram",
    "Xmp.exif.SpectralSensitivity",
    "Xmp.exif.ISOSpeedRatings",
    "Xmp.exif.OECF",
    "Xmp.exif.ShutterSpeedValue",
    "Xmp.exif.ApertureValue",
    "Xmp.exif.BrightnessValue",
    "Xmp.exif.ExposureBiasValue",
    "Xmp.exif.MaxApertureValue",
    "Xmp.exif.SubjectDistance",
    "Xmp.exif.MeteringMode",
    "Xmp.exif.LightSource",
    "Xmp.exif.Flash",
    "Xmp.exif.FocalLength",
    "Xmp.exif.SubjectArea",
    "Xmp.exif.FlashEnergy",
    "Xmp.exif.SpatialFrequencyResponse",
    "Xmp.exif.FocalPlaneXResolution",
    "Xmp.exif.FocalPlaneYResolution",
    "Xmp.exif.FocalPlaneResolutionUnit",
    "Xmp.exif.SubjectLocation",
    "Xmp.exif.ExposureIndex",
    "Xmp.exif.SensingMethod",
    "Xmp.exif.FileSource",
    "Xmp.exif.SceneType",
    "Xmp.exif.CFAPattern",
    "Xmp.exif.CustomRendered",
    "Xmp.exif.ExposureMode",
    "Xmp.exif.WhiteBalance",
    "Xmp.exif.DigitalZoomRatio",
    "Xmp.exif.FocalLengthIn35mmFilm",
    "Xmp.exif.SceneCaptureType",
    "Xmp.exif.GainControl",
    "Xmp.exif.Contrast",
    "Xmp.exif.Saturation",
    "Xmp.exif.Sharpness",
    "Xmp.exif.DeviceSettingDescription",
    "Xmp.exif.SubjectDistanceRange",
    "Xmp.exif.ImageUniqueID",
    "Xmp.exif.GPSVersionID",
    "Xmp.exif.GPSLatitude",
    "Xmp.exif.GPSLongitude",
    "Xmp.exif.GPSAltitudeRef",
    "Xmp.exif.GPSAltitude",
    "Xmp.exif.GPSTimeStamp",
    "Xmp.exif.GPSSatellites",
    "Xmp.exif.GPSStatus",
    "Xmp.exif.GPSMeasureMode",
    "Xmp.exif.GPSDOP",
    "Xmp.exif.GPSSpeedRef",
    "Xmp.exif.GPSSpeed",
    "Xmp.exif.GPSTrackRef",
    "Xmp.exif.GPSTrack",
    "Xmp.exif.GPSImgDirectionRef",
    "Xmp.exif.GPSImgDirection",
    "Xmp.exif.GPSMapDatum",
    "Xmp.exif.GPSDestLatitude",
    "Xmp.exif.GPSDestLongitude",
    "Xmp.exif.GPSDestBearingRef",
    "Xmp.exif.GPSDestBearing",
    "Xmp.exif.GPSDestDistanceRef",
    "Xmp.exif.GPSDestDistance",
    "Xmp.exif.GPSProcessingMethod",
    "Xmp.exif.GPSAreaInformation",
    "Xmp.exif.GPSDifferential",
    "Xmp.dc.title",
    "Xmp.photoshop.Urgency",
    "Xmp.photoshop.Category",
    "Xmp.photoshop.SupplementalCategory",
    "Xmp.dc.subject",
    "Xmp.iptc.Location",
    "Xmp.photoshop.Instruction",
    "Xmp.photoshop.DateCreated",
    "Xmp.dc.creator",
    "Xmp.photoshop.AuthorsPosition",
    "Xmp.photoshop.City",
    "Xmp.photoshop.State",
    "Xmp.iptc.CountryCode",
    "Xmp.photoshop.Country",
    "Xmp.photoshop.TransmissionReference",
    "Xmp.photoshop.Headline",
    "Xmp.photoshop.Credit",
    "Xmp.photoshop.Source",
    "Xmp.dc.rights",
    "Xmp.dc.description",
    "Xmp.photoshop.CaptionWriter",
    NULL
};

static const char* iptcTags[] = {
    "Iptc.Application2.ObjectName",
    "Iptc.Application2.Urgency",
    "Iptc.Application2.Category",
    "Iptc.Application2.SuppCategory",
    "Iptc.Application2.Keywords",
    "Iptc.Application2.LocationName",
    "Iptc.Application2.SpecialInstructions",
    "Iptc.Application2.DateCreated",
    "Iptc.Application2.Byline",
    "Iptc.Application2.BylineTitle",
    "Iptc.Application2.City",
    "Iptc.Application2.ProvinceState",
    "Iptc.Application2.CountryCode",
    "Iptc.Application2.CountryName",
    "Iptc.Application2.TransmissionReference",
    "Iptc.Application2.Headline",
    "Iptc.Application2.Credit",
    "Iptc.Application2.Source",
    "Iptc.Application2.Copyright",
    "Iptc.Application2.Caption",
    "Iptc.Application2.Writer",
    NULL
};


Exiv2Plugin::Exiv2Plugin( PluginLoader* loader )
    : FilePlugin( loader )
{
    const QString prefix("exif");
    const char** token;

    this->addSupportedToken("exifComment");
    //this->addSupportedToken("exifWidth");
    //this->addSupportedToken("exifHeight");
    m_help.append( "[exifComment]" + Plugin::S_TOKEN_SEPARATOR + i18n("Insert the comment of an image") );
    //m_help.append( "[exifWidth]" + Plugin::S_TOKEN_SEPARATOR + i18n("Insert the width in pixel of an image") );
    //m_help.append( "[exifHeight]" + Plugin::S_TOKEN_SEPARATOR + i18n("Insert the height in pixel of an image") );

    // Add exif taks
    token = exifTags;
    while( *token ) 
    {
        ExifKey key( *token );
        QString help;
        try {
            help = QString::fromUtf8(key.tagLabel().c_str());
        } catch( std::exception e ) {
            help = "";
            // exiv2 throws sometimes exceptions 
            // because it trys to create std::string from NULL pointers
        }

        QString cur( *token );
        this->addSupportedToken( prefix + cur );
        m_help.append( QString("[") + prefix + cur + QString("]")  + Plugin::S_TOKEN_SEPARATOR + help );

        m_mapRealKeys[QString( prefix + cur ).toLower()] = cur;
        ++token;
    }

    // Add exif taks
    token = xmpTags;
    while( *token ) 
    {
        XmpKey key( *token );
        QString help;
        try {
            help = QString::fromUtf8(key.tagLabel().c_str());
        } catch( std::exception e ) {
            help = "";
            // exiv2 throws sometimes exceptions 
            // because it trys to create std::string from NULL pointers
        }
        
        QString cur( *token );
        this->addSupportedToken( prefix + cur );
        m_help.append( QString("[") + prefix + cur + QString("]") + Plugin::S_TOKEN_SEPARATOR + help );

        m_mapRealKeys[QString( prefix + cur ).toLower()] = cur;
        ++token;
    }

    // Add exif taks
    token = iptcTags;
    while( *token ) 
    {
        IptcKey key( *token );
        QString help;
        try {
            help = QString::fromUtf8(key.tagLabel().c_str());
        } catch( std::exception e ) {
            help = "";
            // exiv2 throws sometimes exceptions 
            // because it trys to create std::string from NULL pointers
        }

        QString cur( *token );
        this->addSupportedToken( prefix + cur );
        m_help.append( QString("[") + prefix + cur + QString("]") + Plugin::S_TOKEN_SEPARATOR + help );

        m_mapRealKeys[QString( prefix + cur ).toLower()] = cur;
        ++token;
    }


    m_name = i18n("Exiv2 (JPEG/TIFF Exif) Plugin");
    m_comment = i18n("<qt>This plugin supports reading tags from "
                     "JPEG and TIFF files.</qt>");

    m_icon = "image-x-generic";
}


QString Exiv2Plugin::processFile( BatchRenamer* b, int index, const QString & filenameOrToken, EPluginType )
{
    QString token( filenameOrToken.toLower() );
    QString filename = (*b->files())[index].srcUrl().path();

    if( !this->supports( token ) )
        return QString("");

    //const QByteArray asc = filename.toAscii(); 
    // Use toUtf8 so that unicode filenames will work
    const QByteArray asc = filename.toUtf8(); 
    std::string strFilename(asc.constData(), asc.length());

    try 
    {
        Image::AutoPtr image = Exiv2::ImageFactory::open( strFilename );
        if( image.get() != NULL && image->good() )
        {
            image->readMetadata();
            
            if( token == "exifcomment" )
                return QString::fromUtf8( image->comment().c_str() );
            /*
              else if( token =="exifwidth" )
              return QString::number( image->pixelWidth() );
              else if( token =="exifheight" )
              return QString::number( image->pixelHeight() );
            */
            if( token.startsWith("exifexif.") ) 
            {         
                ExifKey key( m_mapRealKeys[token].toAscii().data() );
                ExifData::const_iterator it = image->exifData().findKey( key );
                if( it != image->exifData().end() )
                {
                    std::string val = (*it).toString();
                    return QString::fromUtf8( val.c_str() );
                }
            }
            else if( token.startsWith("exifxmp.") )
            {
                XmpKey key( m_mapRealKeys[token].toAscii().data() );
                XmpData::const_iterator it = image->xmpData().findKey( key );
                if( it != image->xmpData().end() )
                {
                    std::string val = (*it).toString();
                    return QString::fromUtf8( val.c_str() );
                }
            }
            else if( token.startsWith("exifiptc.") )
            {
                IptcKey key( m_mapRealKeys[token].toAscii().data() );
                IptcData::const_iterator it = image->iptcData().findKey( key );
                if( it != image->iptcData().end() )
                {
                    std::string val = (*it).toString();
                    return QString::fromUtf8( val.c_str() );
                }
            }
        } 
    }
    catch( std::exception & err ) 
    {
        return QString::fromUtf8( err.what() );
    }

    return QString("");
}

#endif // HAVE_EXIV2
