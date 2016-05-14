project(krename)


cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
# search packages used by KDE
find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(ECMInstallIcons)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(KF5 REQUIRED COMPONENTS I18n JS KIO IconThemes)
# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
                      ${CMAKE_SOURCE_DIR}/cmake/modules )

# To catch potential errors when porting from KUrl
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)

SET(KRENAME_VERSION "4.0.9")

INCLUDE (FindGettext)

INCLUDE (MacroLogFeature)

# Find taglib
set(TAGLIB_MIN_VERSION "1.5")
find_package(Taglib)
macro_log_feature( TAGLIB_FOUND "taglib" "Read and write tags in audio files" "http://www.freshmeat.net/projects/taglib" TRUE "1.5.0" "")
#set_package_properties( FEATURE PROPERTIES DESCRIPTION "taglib" URL "http://www.freshmeat.net/projects/taglib" TYPE RECOMMENDED PURPOSE "Read and write tags in audio files" )

IF(TAGLIB_FOUND)
  ADD_DEFINITIONS( -DWITH_TAGLIB )
  #  include_directories(${TAGLIB_INCLUDE_DIRS})
  MESSAGE("-- The tablib library was found. KRename will be built with tablib support.")
ELSE(TAGLIB_FOUND)
  MESSAGE("-- The tablib library was not found. KRename will not be built with tablib support.")
ENDIF(TAGLIB_FOUND)

# Find exiv2
set(EXIV2_MIN_VERSION "0.13") # AccessMode was added in 0.13
find_package(Exiv2)
macro_log_feature(EXIV2_FOUND "Exiv2" "A library to access image metadata" "http://www.exiv2.org" FALSE "")
#set_package_properties( FEATURE PROPERTIES DESCRIPTION "Exiv2" URL "http://www.exiv2.org" TYPE RECOMMENDED PURPOSE "A library to access image metadata" )

IF(EXIV2_FOUND)
  ADD_DEFINITIONS( -DWITH_EXIV2 )
#  include_directories(${EXIV2_INCLUDE_DIRS})
  MESSAGE("-- The exiv2 library was found. KRename will be built with exiv2 support.")
ELSE(EXIV2_FOUND)
  MESSAGE("-- The exiv2 library was not found. KRename will not be built with exiv2 support.")
ENDIF(EXIV2_FOUND)

# Find podofo
find_package(LIBPODOFO)
macro_log_feature(LIBPODOFO_FOUND "libPodofo" "A library to access pdf metadata" "http://podofo.sourceforge.net/" FALSE "")
#set_package_properties( FEATURE PROPERTIES DESCRIPTION "libPodofo" URL "http://podofo.sourceforge.net/" TYPE RECOMMENDED PURPOSE "A library to access pdf metadata" )
IF(LIBPODOFO_FOUND)
  ADD_DEFINITIONS( -DWITH_PODOFO )
  #  include_directories(${PODOFO_INCLUDE_DIRS})
  MESSAGE("-- The podofo library was found. KRename will be built with podofo support.")
ELSE(LIBPODOFO_FOUND)
  MESSAGE("-- The podofo library was not found. KRename will not be built with podofo support.")
  SET(LIBPODOFO_LIB "")
ENDIF(LIBPODOFO_FOUND)

# Find freetype
find_package(Freetype)
macro_log_feature(FREETYPE_FOUND "FreeType2" "A library to access font metadata" "http://www.freetype.org" FALSE "")
#set_package_properties( FEATURE PROPERTIES DESCRIPTION "FreeType2" URL "http://www.freetype.org" TYPE RECOMMENDED PURPOSE "A library to access font metadata" )
IF(FREETYPE_FOUND)
  ADD_DEFINITIONS( -DWITH_FREETYPE )
  include_directories(${FREETYPE_INCLUDE_DIRS})
  MESSAGE("-- The freetype library was found. KRename will be built with freetype support.")
ELSE(FREETYPE_FOUND)
  MESSAGE("-- The freetype library was not found. KRename will not be built with freetype support.")
  SET(FREETYPE_LIBRARIES "")
ENDIF(FREETYPE_FOUND)


# Update configuration
CONFIGURE_FILE(config-krename.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-krename.h )

# Directories

add_subdirectory(src)
add_subdirectory(po)

# Message stuff 

FILE(GLOB _POFILES po/*.po)
FOREACH(_p ${_POFILES})
  SET(POFILES "${_p} ${POFILES}")
ENDFOREACH(_p)

SET(MESSAGES_DIR ${CMAKE_SOURCE_DIR}/src)
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules/package_messages.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/package_messages.sh"
    IMMEDIATE
    @ONLY)
ADD_CUSTOM_TARGET(package-messages
    "${CMAKE_CURRENT_BINARY_DIR}/package_messages.sh")

