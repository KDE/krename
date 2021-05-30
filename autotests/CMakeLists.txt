include(ECMAddTests)

include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(Qt5Test CONFIG)

ecm_add_test(
    krenamefiletest.cpp
    LINK_LIBRARIES
        libkrename
        Qt5::Test
    TEST_NAME krenamefiletest
)

ecm_add_test(
    tokentest.cpp
    LINK_LIBRARIES
        libkrename
        Qt5::Test
    TEST_NAME tokentest
)

