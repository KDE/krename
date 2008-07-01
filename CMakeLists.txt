project(krename)


# search packages used by KDE
find_package(KDE4 REQUIRED)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
                      ${KDE4_DATA_DIR}/cmake/modules
                      ${CMAKE_SOURCE_DIR}/cmake/modules )


include (KDE4Defaults)
include (MacroLibrary)

# Find taglib
set(TAGLIB_MIN_VERSION "1.5")
find_package(Taglib REQUIRED)
macro_log_feature( TAGLIB_FOUND "taglib" "Read and write tags in audio files" "http://www.freshmeat.net/projects/taglib" TRUE "1.5.0" "")


add_subdirectory(src)
