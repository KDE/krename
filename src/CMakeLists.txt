
add_subdirectory( pics )
add_subdirectory( servicemenus )

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} 
                     ${KDE4_INCLUDE_DIR} 
		     ${QT_INCLUDES} 
		     ${CMAKE_CURRENT_SOURCE_DIR} 
		     ${CMAKE_CURRENT_BINARY_DIR}
		     ${TAGLIB_INCLUDE_DIR} )

IF(EXIV2_FOUND)
  INCLUDE_DIRECTORIES( ${EXIV2_INCLUDE_DIR} )
ENDIF(EXIV2_FOUND)

ADD_DEFINITIONS(${TAGLIB_CFLAGS} ${EXIV2_CFLAGS} ${KDE4_DEFINITIONS} )
# Exiv2 needs exceptions
ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})

########### next target ###############


SET(krename_SRCS
  main.cpp 
  batchrenamer.cpp
  customdialog.cpp
  datetimeplugin.cpp
  exiv2plugin.cpp
  exthistorycombo.cpp
  filedialogextwidget.cpp
  fileplugin.cpp
  increasecounterplugin.cpp
  krenamefile.cpp 
  krenameimpl.cpp 
  krenamelistview.cpp
  krenamemodel.cpp
  krenametest.cpp
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
  translitplugin.cpp
  )

IF ( TAGLIB_FOUND )
  SET(krename_SRCS 
    ${krename_SRCS}
    taglibplugin.cpp
    )
ENDIF ( TAGLIB_FOUND )

kde4_add_ui_files( krename_SRCS 
  customfilenamedlg.ui		
  datetimepluginwidget.ui
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
  )

#kde4_automoc( ${krename_SRCS} )
kde4_add_executable(krename ${krename_SRCS})

# find Nepomuk's libkmetadata
find_path(KMETADATA_INCLUDE_DIR NAMES kmetadata/kmetadata.h PATHS ${KDE4_INCLUDE_DIR})
find_library(KMETADATA_LIBRARY NAMES kmetadata PATHS ${KDE4_LIB_DIR})

if(KMETADATA_INCLUDE_DIR AND KMETADATA_LIBRARY)
  set(KDE4_KMETADATA kmetadata)
  set(HAVE_KMETADATA 1)
else(KMETADATA_INCLUDE_DIR AND KMETADATA_LIBRARY)
  set(KDE4_KMETADATA "")
endif(KMETADATA_INCLUDE_DIR AND KMETADATA_LIBRARY)

#configure_file(config-kmetadata.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kmetadata.h )


IF(EXIV2_FOUND)
	TARGET_LINK_LIBRARIES(krename ${KDE4_KDECORE_LIBS} 
                              ${KDE4_KDEUI_LIBS}
						      ${KDE4_KIO_LIBS} 
							  ${KDE4_KMETADATA}
							  ${KDE4_KJSAPI_LIBS} 
							  ${TAGLIB_LIBRARIES}
							  ${EXIV2_LIBRARIES} )
ELSE(EXIV2_FOUND)
	TARGET_LINK_LIBRARIES(krename ${KDE4_KDECORE_LIBS} 
                              ${KDE4_KDEUI_LIBS}
						      ${KDE4_KIO_LIBS} 
							  ${KDE4_KMETADATA}
							  ${KDE4_KJSAPI_LIBS} 
							  ${TAGLIB_LIBRARIES} )
ENDIF(EXIV2_FOUND)


# Others Files to install

INSTALL(TARGETS krename DESTINATION ${BIN_INSTALL_DIR})
INSTALL(FILES krename.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

