
add_subdirectory( pics )
add_subdirectory( servicemenus )

include_directories( ${CMAKE_SOURCE_DIR} ${KDE4_INCLUDE_DIR} 
  ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )
add_definitions( ${KDE4_DEFINITIONS} )

########### next target ###############


set(krename_SRCS
  main.cpp 
  batchrenamer.cpp
  exthistorycombo.cpp
  filedialogextwidget.cpp
  firststartdlg.cpp
  krenamefile.cpp 
  krenameimpl.cpp 
  krenamemodel.cpp
  krenametest.cpp
  krenamewindow.cpp
  numberdialog.cpp
  )

kde4_automoc( ${krename_SRCS} )

kde4_add_ui_files( krename_SRCS 
  guimodeselector.ui 
  krenamefiles.ui 
  krenamedestination.ui 
  krenamefilename.ui 
  krenamesimple.ui 
  krenameplugins.ui
  numberwidget.ui
  )

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

target_link_libraries(krename ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS}
${KDE4_KIO_LIBS} ${KDE4_KMETADATA} )

install(TARGETS krename DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############

install( FILES  krename.desktop DESTINATION ${XDG_APPS_DIR} )

kde4_install_icons( ${ICON_INSTALL_DIR} )
