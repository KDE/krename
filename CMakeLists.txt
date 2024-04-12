cmake_minimum_required(VERSION 3.16)

project(krename VERSION "5.0.60")

set(QT_MIN_VERSION "5.15.2")
set(KF_MIN_VERSION "5.100.0")

set(KDE_COMPILERSETTINGS_LEVEL "5.82.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(ECMInstallIcons)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMDeprecationSettings)
include(FeatureSummary)

option(BUILD_MODELSELFTEST "Build model self test." OFF)
add_feature_info(BUILD_MODELSELFTEST BUILD_MODELSELFTEST "Build model self test.")

if (QT_MAJOR_VERSION STREQUAL "6")
    set(QT_MIN_VERSION "6.5.0")
    set(KF_MIN_VERSION "6.0.0")
    set(KF_MAJOR_VERSION "6")
else()
    set(KF_MAJOR_VERSION "5")
endif()

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Widgets
    Qml
)
if (BUILD_MODELSELFTEST)
    find_package(Qt${QT_MAJOR_VERSION}Test REQUIRED)
endif()
if (QT_MAJOR_VERSION STREQUAL "6")
    find_package(Qt6Core5Compat REQUIRED)
endif()

find_package(KF${KF_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Completion
    Config
    CoreAddons
    Crash
    I18n
    IconThemes
    ItemViews
    JobWidgets
    KIO
    Service
    WidgetsAddons
    XmlGui
)

add_definitions(
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_STRICT_ITERATORS
)

# Find KArchive
find_package(KF${KF_MAJOR_VERSION}Archive)
set_package_properties(KF${KF_MAJOR_VERSION}Archive PROPERTIES
    PURPOSE "Needed for ODF plugin"
    TYPE OPTIONAL)

# Find taglib
set(TAGLIB_MIN_VERSION "1.5")
find_package(Taglib ${TAGLIB_MIN_VERSION})
set_package_properties(Taglib PROPERTIES
    TYPE OPTIONAL)

# Find exiv2
find_package(LibExiv2 0.27) # EXIV2_TEST_VERSION was added in 0.27
set_package_properties(LibExiv2 PROPERTIES TYPE OPTIONAL)

# Find podofo
find_package(PoDoFo)
set_package_properties(PoDoFo PROPERTIES TYPE OPTIONAL)

# Find freetype
find_package(Freetype)
set_package_properties(Freetype PROPERTIES
    DESCRIPTION "A library to access font metadata"
    URL "https://www.freetype.org"
    TYPE OPTIONAL)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

ecm_set_disabled_deprecation_versions(
    QT 5.15.2
    KF 5.100
)

set(HAVE_TAGLIB ${TAGLIB_FOUND})
set(HAVE_EXIV2 ${LibExiv2_FOUND})
set(HAVE_PODOFO ${PoDoFo_FOUND})
set(HAVE_KARCHIVE ${KF${KF_MAJOR_VERSION}Archive_FOUND})
set(HAVE_FREETYPE ${FREETYPE_FOUND})
configure_file(config-krename.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-krename.h)

# Directories

add_subdirectory(src)
add_subdirectory(autotests)

ki18n_install(po)
