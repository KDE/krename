include(ECMAddTests)

include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(Qt5Test CONFIG)

ecm_add_test(
    krenamefiletest.cpp
    ../src/krenamefile.cpp
    LINK_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Test
        KF5::KIOCore
        KF5::KIOWidgets
        KF5::IconThemes
    TEST_NAME krenamefiletest
)

