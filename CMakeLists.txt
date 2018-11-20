cmake_minimum_required(VERSION 3.0)

project(krename VERSION "5.0.60")

set(QT5_MIN_VERSION "5.7.0")
set(KF5_MIN_VERSION "5.18.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)

include(KDEInstallDirs)
include(ECMInstallIcons)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(Qt5 ${QT5_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Widgets
)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
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

add_definitions(
    -DQT_DEPRECATED_WARNINGS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050700
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_STRICT_ITERATORS
)


# Find KArchive
find_package(KF5Archive)
set_package_properties(KF5Archive PROPERTIES
    PURPOSE "Needed for ODF plugin"
    TYPE OPTIONAL)

# Find taglib
set(TAGLIB_MIN_VERSION "1.5")
find_package(Taglib ${TAGLIB_MIN_VERSION})
set_package_properties(Taglib PROPERTIES
    DESCRIPTION "Read and write tags in audio files"
    URL "https://taglib.github.io/"
    TYPE OPTIONAL)

# Find exiv2
find_package(Exiv2 0.13) # AccessMode was added in 0.13
set_package_properties(Exiv2 PROPERTIES
    DESCRIPTION "A library to access image metadata"
    URL "http://www.exiv2.org"
    TYPE OPTIONAL)

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
    URL "http://www.freetype.org"
    TYPE OPTIONAL)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(HAVE_TAGLIB TAGLIB_FOUND)
set(HAVE_EXIV2 EXIV2_FOUND)
set(HAVE_PODOFO PoDoFo_FOUND)
set(HAVE_KARCHIVE KF5Archive_FOUND)
set(HAVE_FREETYPE FREETYPE_FOUND)
configure_file(config-krename.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-krename.h)

# Directories

add_subdirectory(src)
add_subdirectory(autotests)
