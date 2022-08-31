cmake_minimum_required(VERSION 3.7)

project(krename)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
# search packages used by KDE
find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

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
    Crash
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
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules
                      ${CMAKE_MODULE_PATH})

add_definitions(
    -DQT_DEPRECATED_WARNINGS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050700
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_STRICT_ITERATORS
)

set(KRENAME_VERSION "5.0.2")

include(FindGettext)

# Find taglib
set(TAGLIB_MIN_VERSION "1.5")
find_package(Taglib ${TAGLIB_MIN_VERSION})
set_package_properties(Taglib PROPERTIES
    DESCRIPTION "Read and write tags in audio files"
    URL "https://taglib.github.io/"
    TYPE OPTIONAL)

# Find exiv2
find_package(LibExiv2 0.13) # AccessMode was added in 0.13
set_package_properties(LibExiv2 PROPERTIES TYPE OPTIONAL)

if (LibExiv2_VERSION VERSION_GREATER_EQUAL "0.27")
    set(HAVE_LIBEXIV2_0_27 TRUE)
endif()

# Find podofo
find_package(PoDoFo)
set_package_properties(PoDoFo PROPERTIES
    DESCRIPTION "A library to access PDF metadata"
    URL "http://podofo.sourceforge.net/"
    TYPE OPTIONAL)

# Find freetype
find_package(Freetype)
set_package_properties(Freetype PROPERTIES
    DESCRIPTION "A library to access font metadata"
    URL "https://www.freetype.org"
    TYPE OPTIONAL)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(HAVE_TAGLIB ${TAGLIB_FOUND})
set(HAVE_EXIV2 ${LibExiv2_FOUND})
set(HAVE_PODOFO ${PoDoFo_FOUND})
set(HAVE_FREETYPE ${FREETYPE_FOUND})
configure_file(config-krename.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-krename.h)

# Directories

add_subdirectory(src)
add_subdirectory(autotests)
