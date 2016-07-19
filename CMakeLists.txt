project(krename)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
# search packages used by KDE
find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(ECMInstallIcons)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(KF5 REQUIRED COMPONENTS
    Completion
    Config
    CoreAddons
    I18n
    IconThemes
    ItemViews
    JobWidgets
    JS
    KIO
    Service
    WidgetsAddons
    XmlGui
)
# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
                      ${CMAKE_SOURCE_DIR}/cmake/modules )

# To catch potential errors when porting from KUrl
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)

set(KRENAME_VERSION "4.0.9")

include(FindGettext)

# Find taglib
set(TAGLIB_MIN_VERSION "1.5")
find_package(Taglib ${TAGLIB_MIN_VERSION})
set_package_properties(Taglib PROPERTIES
    DESCRIPTION "Read and write tags in audio files"
    URL "https://taglib.github.io/"
    TYPE OPTIONAL)

if(TAGLIB_FOUND)
    add_definitions(-DWITH_TAGLIB)
endif()

# Find exiv2
find_package(Exiv2 0.13) # AccessMode was added in 0.13
set_package_properties(Exiv2 PROPERTIES
    DESCRIPTION "A library to access image metadata"
    URL "http://www.exiv2.org"
    TYPE OPTIONAL)

if(EXIV2_FOUND)
    add_definitions(-DWITH_EXIV2)
endif()

# Find podofo
find_package(PoDoFo)
set_package_properties(PoDoFo PROPERTIES
    DESCRIPTION "A library to access PDF metadata"
    URL "http://podofo.sourceforge.net/"
    TYPE OPTIONAL)
if(PoDoFo_FOUND)
    add_definitions(-DWITH_PODOFO)
endif()

# Find freetype
find_package(Freetype)
set_package_properties(Freetype PROPERTIES
    DESCRIPTION "A library to access font metadata"
    URL "http://www.freetype.org"
    TYPE OPTIONAL)
if(FREETYPE_FOUND)
    add_definitions(-DWITH_FREETYPE)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)


# Update configuration
configure_file(config-krename.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-krename.h)

# Directories

add_subdirectory(src)
