project(krename)


# search packages used by KDE
find_package(KDE4 REQUIRED)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
                      ${KDE4_DATA_DIR}/cmake/modules
                      ${CMAKE_SOURCE_DIR}/cmake/modules )


SET(KRENAME_VERSION "3.9.2")

INCLUDE (KDE4Defaults)
INCLUDE (MacroLibrary)
INCLUDE (FindGettext)

# Find taglib
set(TAGLIB_MIN_VERSION "1.5")
find_package(Taglib REQUIRED)
macro_log_feature( TAGLIB_FOUND "taglib" "Read and write tags in audio files" "http://www.freshmeat.net/projects/taglib" TRUE "1.5.0" "")

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

