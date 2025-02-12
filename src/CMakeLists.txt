add_subdirectory(pics)
add_subdirectory(servicemenus)

include_directories(${CMAKE_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
)

if(EXIV2_FOUND)
    include_directories(${EXIV2_INCLUDE_DIR})
endif()

if(FREETYPE_FOUND)
    include_directories(${FREETYPE_INCLUDE_DIRS})
endif()

# Exiv2 needs exceptions
kde_enable_exceptions()

########### next target ###############

set(krename_SRCS
    main.cpp
)

set(libkrename_SRCS
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
    set(libkrename_SRCS ${libkrename_SRCS}
        exiv2plugin.cpp
    )
endif()

if(TAGLIB_FOUND)
    set(libkrename_SRCS ${libkrename_SRCS}
        taglibplugin.cpp
    )
endif()

if(PoDoFo_FOUND)
    set(libkrename_SRCS ${libkrename_SRCS}
        podofoplugin.cpp
    )
endif()

if(FREETYPE_FOUND)
    set(libkrename_SRCS ${libkrename_SRCS}
        fontplugin.cpp
    )
endif()

if(KF6Archive_FOUND)
    set(libkrename_SRCS ${libkrename_SRCS}
        odfplugin.cpp
    )
endif()


ki18n_wrap_ui(libkrename_SRCS
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

add_library(libkrename STATIC ${libkrename_SRCS})
set_target_properties(libkrename PROPERTIES
    OUTPUT_NAME krename
)

target_link_libraries(libkrename
    KF6::Crash
    KF6::KIOCore
    KF6::KIOFileWidgets
    KF6::I18n
    KF6::IconThemes
    KF6::XmlGui
    Qt6::Widgets
    Qt6::Qml
    Qt6::Core5Compat
)
if (BUILD_MODELSELFTEST)
    set(BUILD_MODELSELFTEST_VALUE 1)
    target_link_libraries(libkrename
        Qt6::Test
    )
else()
    set(BUILD_MODELSELFTEST_VALUE 0)
endif()
set_source_files_properties(krenameimpl.cpp PROPERTIES
    COMPILE_DEFINITIONS "BUILD_MODELSELFTEST=${BUILD_MODELSELFTEST_VALUE}"
)

if(TAGLIB_FOUND)
    target_link_libraries(libkrename
        Taglib::Taglib
    )
endif()
if(LibExiv2_FOUND)
    target_link_libraries(libkrename
        LibExiv2::LibExiv2
    )
endif()
if(PoDoFo_FOUND)
    target_link_libraries(libkrename
        PoDoFo::PoDoFo
    )
endif()
if(KF6Archive_FOUND)
    target_link_libraries(libkrename
        Qt6::Xml
        KF6::Archive
    )
endif()
if(FREETYPE_FOUND)
    target_link_libraries(libkrename
        ${FREETYPE_LIBRARIES}
    )
endif()

add_executable(krename ${krename_SRCS})
target_link_libraries(krename
    libkrename
)

# Others Files to install

install(TARGETS krename DESTINATION ${KDE_INSTALL_BINDIR})
install(PROGRAMS org.kde.krename.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.krename.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

