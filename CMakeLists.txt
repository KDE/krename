project(krename)


# search packages used by KDE
find_package(KDE4 REQUIRED)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
                      ${KDE4_DATA_DIR}/cmake/modules
                      ${CMAKE_SOURCE_DIR}/cmake/modules )


SET(KRENAME_VERSION "4.0.8")

INCLUDE (KDE4Defaults)
INCLUDE (MacroLibrary)
INCLUDE (FindGettext)

# Find taglib
set(TAGLIB_MIN_VERSION "1.5")
macro_optional_find_package(Taglib REQUIRED)
macro_log_feature( TAGLIB_FOUND "taglib" "Read and write tags in audio files" "http://www.freshmeat.net/projects/taglib" TRUE "1.5.0" "")

IF(TAGLIB_FOUND)
  ADD_DEFINITIONS( -DWITH_TAGLIB )
  MESSAGE("-- The tablib library was found. KRename will be built with tablib support.")
ELSE(TAGLIB_FOUND)
  MESSAGE("-- The tablib library was not found. KRename will not be built with tablib support.")
ENDIF(TAGLIB_FOUND)

# Find exiv2
set(EXIV2_MIN_VERSION "0.13") # AccessMode was added in 0.13
macro_optional_find_package(Exiv2)
macro_log_feature(EXIV2_FOUND "Exiv2" "A library to access image metadata" "http://www.exiv2.org" FALSE "")

IF(EXIV2_FOUND)
  ADD_DEFINITIONS( -DWITH_EXIV2 )
  MESSAGE("-- The exiv2 library was found. KRename will be built with exiv2 support.")
ELSE(EXIV2_FOUND)
  MESSAGE("-- The exiv2 library was not found. KRename will not be built with exiv2 support.")
ENDIF(EXIV2_FOUND)

# Find podofo
macro_optional_find_package(LIBPODOFO)
macro_log_feature(LIBPODOFO_FOUND "libPodofo" "A library to access pdf metadata" "http://podofo.sourceforge.net/" FALSE "")
IF(LIBPODOFO_FOUND)
  ADD_DEFINITIONS( -DWITH_PODOFO )
  MESSAGE("-- The podofo library was found. KRename will be built with podofo support.")
ELSE(LIBPODOFO_FOUND)
  MESSAGE("-- The podofo library was not found. KRename will not be built with podofo support.")
  SET(LIBPODOFO_LIB "")
ENDIF(LIBPODOFO_FOUND)

# Find freetype
macro_optional_find_package(Freetype)
macro_log_feature(FREETYPE_FOUND "FreeType2" "A library to access font metadata" "http://www.freetype.org" FALSE "")
IF(FREETYPE_FOUND)
  ADD_DEFINITIONS( -DWITH_FREETYPE )
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

