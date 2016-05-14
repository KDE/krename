add_subdirectory( pics )
add_subdirectory( servicemenus )

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} 
		     ${CMAKE_CURRENT_SOURCE_DIR} 
		     ${CMAKE_CURRENT_BINARY_DIR}
		     ${TAGLIB_INCLUDE_DIR} )

IF(EXIV2_FOUND)
  INCLUDE_DIRECTORIES( ${EXIV2_INCLUDE_DIR} )
ENDIF(EXIV2_FOUND)

IF(LIBPODOFO_FOUND)
  INCLUDE_DIRECTORIES( ${PODOFO_INCLUDES} )
ENDIF(LIBPODOFO_FOUND)

IF(FREETYPE_FOUND)
  INCLUDE_DIRECTORIES( ${FREETYPE_INCLUDE_DIRS} )
ENDIF(FREETYPE_FOUND)

ADD_DEFINITIONS(${TAGLIB_CFLAGS} ${EXIV2_CFLAGS} ${LIBPODOFO_CFLAGS} )
# Exiv2 needs exceptions
kde_enable_exceptions()

########### next target ###############


SET(krename_SRCS
  main.cpp 
  batchrenamer.cpp
  customdialog.cpp
  datetimeplugin.cpp
  dirsortplugin.cpp
  exthistorycombo.cpp
  filedialogextwidget.cpp
  fileplugin.cpp
  increasecounterplugin.cpp
  krenamefile.cpp 
  krenameimpl.cpp 
  krenamelistview.cpp
  krenamemodel.cpp
  krenametest.cpp
  krenametokensorter.cpp
  krenamewindow.cpp
  modeltest.cpp
  numberdialog.cpp
  insertpartfilenamedlg.cpp
  threadedlister.cpp
  plugin.cpp
  pluginloader.cpp
  permissionsplugin.cpp
  previewlist.cpp
  progressdialog.cpp
  replacedialog.cpp
  richtextitemdelegate.cpp
  scriptplugin.cpp
  startupinfo.cpp
  systemplugin.cpp
  tokenhelpdialog.cpp
  tokensortmodedialog.cpp
  translitplugin.cpp
  snumplugin.cpp
  )

IF ( EXIV2_FOUND )
  SET(krename_SRCS
    ${krename_SRCS}
    exiv2plugin.cpp
    )
ENDIF ( EXIV2_FOUND )

IF ( TAGLIB_FOUND )
  SET(krename_SRCS 
    ${krename_SRCS}
    taglibplugin.cpp
    )
ENDIF ( TAGLIB_FOUND )

IF ( LIBPODOFO_FOUND )
  SET(krename_SRCS 
    ${krename_SRCS}
    podofoplugin.cpp
    )
ENDIF ( LIBPODOFO_FOUND )

IF(FREETYPE_FOUND)
  SET(krename_SRCS 
    ${krename_SRCS}
    fontplugin.cpp
    )
ENDIF(FREETYPE_FOUND)


ki18n_wrap_ui( krename_SRCS 
  customfilenamedlg.ui		
  datetimepluginwidget.ui
  dirsortpluginwidget.ui
  increasecounterpluginwidget.ui
  guimodeselector.ui 
  partfilenamewidget.ui
  krenamefiles.ui 
  krenamedestination.ui 
  krenamefilename.ui 
  krenameplugins.ui
  numberwidget.ui
  permissionspluginwidget.ui
  progressdialog.ui
  replacewidget.ui
  replaceitemdlg.ui
  scriptpluginwidget.ui
  scriptplugindialog.ui
  startupinfowidget.ui
  tokenhelpwidget.ui
  tokensortmodedialog.ui
  )

#kde4_automoc( ${krename_SRCS} )
add_executable(krename ${krename_SRCS})

# find Nepomuk's libkmetadata
find_path(KMETADATA_INCLUDE_DIR NAMES kmetadata/kmetadata.h PATHS )
find_library(KMETADATA_LIBRARY NAMES kmetadata PATHS ${KDE4_LIB_DIR})

if(KMETADATA_INCLUDE_DIR AND KMETADATA_LIBRARY)
  set(KDE4_KMETADATA kmetadata)
  set(HAVE_KMETADATA 1)
else(KMETADATA_INCLUDE_DIR AND KMETADATA_LIBRARY)
  set(KDE4_KMETADATA "")
endif(KMETADATA_INCLUDE_DIR AND KMETADATA_LIBRARY)

#configure_file(config-kmetadata.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kmetadata.h )


IF(EXIV2_FOUND)
  TARGET_LINK_LIBRARIES(krename 
    KF5::KIOCore
    KF5::KIOFileWidgets
    KF5::I18n
    KF5::JSApi
    KF5::IconThemes
    Qt5::Widgets
    ${TAGLIB_LIBRARIES}
    ${LIBPODOFO_LIB}
    ${FREETYPE_LIBRARIES}
    ${EXIV2_LIBRARIES} )
ELSE(EXIV2_FOUND)
  TARGET_LINK_LIBRARIES(krename 
    KF5::KIOCore
    KF5::KIOFileWidgets
    KF5::I18n
    KF5::JSApi
    KF5::IconThemes
    Qt5::Widgets
    ${LIBPODOFO_LIB}
    ${FREETYPE_LIBRARIES}
    ${TAGLIB_LIBRARIES} )
ENDIF(EXIV2_FOUND)

# Others Files to install

INSTALL(TARGETS krename DESTINATION ${BIN_INSTALL_DIR})
INSTALL(FILES krename.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

