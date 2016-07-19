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
# Copyright 2016 Pino Toscano <pino@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

find_path(PoDoFo_INCLUDE_DIRS
  NAMES podofo/podofo.h
)
find_library(PoDoFo_LIBRARIES
  NAMES libpodofo podofo
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PoDoFo DEFAULT_MSG PoDoFo_LIBRARIES PoDoFo_INCLUDE_DIRS)

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
endif()

mark_as_advanced(PoDoFo_INCLUDE_DIRS PoDoFo_LIBRARIES PoDoFo_DEFINITIONS)
