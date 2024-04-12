include(ECMAddTests)

include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(Qt${QT_MAJOR_VERSION}Test)

ecm_add_test(
    krenamefiletest.cpp
    LINK_LIBRARIES
        libkrename
        Qt${QT_MAJOR_VERSION}::Test
    TEST_NAME krenamefiletest
)

ecm_add_test(
    tokentest.cpp
    LINK_LIBRARIES
        libkrename
        Qt${QT_MAJOR_VERSION}::Test
    TEST_NAME tokentest
)

