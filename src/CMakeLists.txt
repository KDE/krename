add_subdirectory(pics)
add_subdirectory(servicemenus)

include_directories(${CMAKE_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
)

if(EXIV2_FOUND)
    include_directories(${EXIV2_INCLUDE_DIR})
endif()

if(PoDoFo_FOUND)
    include_directories(${PoDoFo_INCLUDE_DIRS})
    add_definitions(${PoDoFo_DEFINITIONS})
endif()

if(FREETYPE_FOUND)
    include_directories(${FREETYPE_INCLUDE_DIRS})
endif()

# Exiv2 needs exceptions
kde_enable_exceptions()

########### next target ###############


set(krename_SRCS
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

if(LibExiv2_FOUND)
    set(krename_SRCS ${krename_SRCS}
        exiv2plugin.cpp
    )
endif()

if(TAGLIB_FOUND)
    set(krename_SRCS ${krename_SRCS}
        taglibplugin.cpp
    )
endif()

if(PoDoFo_FOUND)
    set(krename_SRCS ${krename_SRCS}
        podofoplugin.cpp
    )
endif()

if(FREETYPE_FOUND)
    set(krename_SRCS ${krename_SRCS}
        fontplugin.cpp
    )
endif()

if(KF5Archive_FOUND)
    set(krename_SRCS ${krename_SRCS}
        odfplugin.cpp
    )
endif()


ki18n_wrap_ui(krename_SRCS
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

add_executable(krename ${krename_SRCS})

target_link_libraries(krename
    KF5::Crash
    KF5::KIOCore
    KF5::KIOFileWidgets
    KF5::I18n
    KF5::IconThemes
    Qt5::Widgets
    Qt5::Qml
)
if(TAGLIB_FOUND)
    target_link_libraries(krename
        Taglib::Taglib
    )
endif()
if(LibExiv2_FOUND)
    target_link_libraries(krename
        LibExiv2::LibExiv2
    )
endif()
if(PoDoFo_FOUND)
    target_link_libraries(krename
        ${PoDoFo_LIBRARIES}
    )
endif()
if(KF5Archive_FOUND)
    target_link_libraries(krename
        Qt5::Xml
        KF5::Archive
    )
endif()
if(FREETYPE_FOUND)
    target_link_libraries(krename
        ${FREETYPE_LIBRARIES}
    )
endif()

# Others Files to install

install(TARGETS krename DESTINATION ${KDE_INSTALL_BINDIR})
install(PROGRAMS org.kde.krename.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.krename.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

