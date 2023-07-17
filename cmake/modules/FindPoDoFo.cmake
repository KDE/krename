# - Try to find the PoDoFo library
#
# Windows users MUST set when building:
#
# PoDoFo_USE_SHARED - whether use PoDoFo as shared library
#
# Once done this will define:
#
# PoDoFo_FOUND - system has the PoDoFo library
# PoDoFo_INCLUDE_DIRS - the PoDoFo include directory
# PoDoFo_LIBRARIES - the libraries needed to use PoDoFo
# PoDoFo_DEFINITIONS - the definitions needed to use PoDoFo
#
# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2016 Pino Toscano <pino@kde.org>


include(FindPkgConfig)
pkg_search_module(PoDoFo libpodofo libpodofo-0)

set(PoDoFo_DEFINITIONS)
if(PoDoFo_FOUND)
  if(WIN32)
    if(NOT DEFINED PoDoFo_USE_SHARED)
      message(SEND_ERROR "Win32 users MUST set PoDoFo_USE_SHARED")
      message(SEND_ERROR "Set -DPoDoFo_USE_SHARED=0 if linking to a static library PoDoFo")
      message(SEND_ERROR "or -DPoDoFo_USE_SHARED=1 if linking to a DLL build of PoDoFo")
      message(FATAL_ERROR "PoDoFo_USE_SHARED unset on win32 build")
    else()
      if(PoDoFo_USE_SHARED)
        set(PoDoFo_DEFINITIONS "${PoDoFo_DEFINITIONS} -DUSING_SHARED_PODOFO")
      endif(PoDoFo_USE_SHARED)
    endif()
  endif()

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

mark_as_advanced(PoDoFo_INCLUDE_DIRS PoDoFo_LIBRARIES PoDoFo_DEFINITIONS)
