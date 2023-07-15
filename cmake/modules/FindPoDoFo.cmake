# SPDX-FileCopyrightText: 2016 Pino Toscano <pino@kde.org>
# SPDX-FileCopyrightText: 2023 Heiko Becker <heiko.becker@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause


#[=======================================================================[.rst:
FindPoDoFo
------------

Try to find PoDoFo, a C++ library to work with the PDF file format

This will define the following variables:

``PoDoFo_FOUND``
    True if PoDoFo is available
``PoDoFo_VERSION``
    The version of PoDoFo
``PoDoFo_LIBRARIES``
    The libraries of PoDoFofor use with target_link_libraries()
``PoDoFo_INCLUDE_DIRS``
    The include dirs of PoDoFo for use with target_include_directories()

If ``PoDoFo_FOUND`` is TRUE, it will also define the following imported
target:

``PoDoFo::PoDoFo``
    The PoDoFo library

In general we recommend using the imported target, as it is easier to use.
Bear in mind, however, that if the target is in the link interface of an
exported library, it must be made available by the package config file.

#]=======================================================================]

find_package(PkgConfig QUIET)
pkg_search_module(PC_PoDoFo QUIET libpodofo libpodofo-0)

find_library(PoDoFo_LIBRARIES
    NAMES podofo
    HINTS ${PC_PoDoFo_LIBRARY_DIRS}
)

find_path(PoDoFo_INCLUDE_DIRS
    NAMES podofo.h
    HINTS ${PC_PoDoFo_INCLUDE_DIRS}
)

if(PoDoFo_INCLUDE_DIRS)
  # NOTE: I have no idea if that's still needed and no possibility to test on
  # Windows.
  #if(WIN32)
  #  if(NOT DEFINED PoDoFo_USE_SHARED)
  #    message(SEND_ERROR "Win32 users MUST set PoDoFo_USE_SHARED")
  #    message(SEND_ERROR "Set -DPoDoFo_USE_SHARED=0 if linking to a static library PoDoFo")
  #    message(SEND_ERROR "or -DPoDoFo_USE_SHARED=1 if linking to a DLL build of PoDoFo")
  #    message(FATAL_ERROR "PoDoFo_USE_SHARED unset on win32 build")
  #  else()
  #    if(PoDoFo_USE_SHARED)
  #      set(PoDoFo_DEFINITIONS "${PoDoFo_DEFINITIONS} -DUSING_SHARED_PODOFO")
  #    endif(PoDoFo_USE_SHARED)
  #  endif()
  #endif()

  find_file(PoDoFo_CONFIG podofo_config.h PATHS ${PoDoFo_INCLUDE_DIRS} PATH_SUFFIXES auxiliary base)
  file(STRINGS "${PoDoFo_CONFIG}" PoDoFo_MAJOR_VER_LINE REGEX "^#define[ \t]+PODOFO_VERSION_MAJOR[ \t]+[0-9]+$")
  file(STRINGS "${PoDoFo_CONFIG}" PoDoFo_MINOR_VER_LINE REGEX "^#define[ \t]+PODOFO_VERSION_MINOR[ \t]+[0-9]+$")
  file(STRINGS "${PoDoFo_CONFIG}" PoDoFo_PATCH_VER_LINE REGEX "^#define[ \t]+PODOFO_VERSION_PATCH[ \t]+[0-9]+$")
  string(REGEX REPLACE "^#define[ \t]+PODOFO_VERSION_MAJOR[ \t]+([0-9]+)$" "\\1" PoDoFo_MAJOR_VER "${PoDoFo_MAJOR_VER_LINE}")
  string(REGEX REPLACE "^#define[ \t]+PODOFO_VERSION_MINOR[ \t]+([0-9]+)$" "\\1" PoDoFo_MINOR_VER "${PoDoFo_MINOR_VER_LINE}")
  string(REGEX REPLACE "^#define[ \t]+PODOFO_VERSION_PATCH[ \t]+([0-9]+)$" "\\1" PoDoFo_PATCH_VER "${PoDoFo_PATCH_VER_LINE}")
  set(PoDoFo_VERSION "${PoDoFo_MAJOR_VER}.${PoDoFo_MINOR_VER}.${PoDoFo_PATCH_VER}")

  # PoDoFo-0.9.5 unconditionally includes openssl/opensslconf.h in a public
  # header. The fix is in https://sourceforge.net/p/podofo/code/1830/ and will
  # hopefully be released soon with 0.9.6. Note that krename doesn't use
  # OpenSSL in any way.
  if(PoDoFo_VERSION VERSION_EQUAL "0.9.5")
    find_package(OpenSSL)
    if (OpenSSL_FOUND)
      message("OpenSSL found, which is required for this version of PoDofo (0.9.5)")
      set(PoDoFo_INCLUDE_DIRS ${PoDoFo_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
    else()
      unset(PoDoFo_FOUND)
      message("OpenSSL NOT found, which is required for this version of PoDofo (0.9.5)")
    endif()
  endif()
endif()

if(PoDoFo_VERSION VERSION_GREATER_EQUAL 0.10.0)
    set(CMAKE_CXX_STANDARD 17)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PoDoFo
    FOUND_VAR
        PoDoFo_FOUND
    REQUIRED_VARS
        PoDoFo_LIBRARIES
        PoDoFo_INCLUDE_DIRS
    VERSION_VAR
        PoDoFo_VERSION
)

if(PoDoFo_FOUND AND NOT TARGET PoDoFo::PoDoFo)
    add_library(PoDoFo::PoDoFo UNKNOWN IMPORTED)
    set_target_properties(PoDoFo::PoDoFo PROPERTIES
        IMPORTED_LOCATION "${PoDoFo_LIBRARIES}"
        INTERFACE_COMPILE_OPTIONS "${PC_PoDoFo_CFLAGS}"
        INTERFACE_INCLUDE_DIRECTORIES "${PoDoFo_INCLUDE_DIRS}"
    )
    if(TARGET PkgConfig::PC_PoDoFo)
        target_link_libraries(PoDoFo::PoDoFo INTERFACE PkgConfig::PC_PoDoFo)
    endif()
endif()

mark_as_advanced(PoDoFo_LIBRARIES PoDoFo_INCLUDE_DIRS PoDoFo_VERSION)

include(FeatureSummary)
set_package_properties(PoDoFo PROPERTIES
    DESCRIPTION "A C++ libary to work with the PDF file format"
    URL "https://github.com/podofo/podofo"
)

