cmake_minimum_required(VERSION 3.5.1)

project(test-mnist)

set(CMAKE_CXX_STANDARD 17)
option( BUILD_STATIC "Set to ON to include static versions of the library" OFF)

find_package(OpenFHE)

set( CMAKE_CXX_FLAGS ${OpenFHE_CXX_FLAGS} )

link_directories( ${OpenFHE_LIBDIR} )
link_directories( ${OPENMP_LIBRARIES} )

set( CMAKE_EXE_LINKER_FLAGS ${OpenFHE_EXE_LINKER_FLAGS} )
link_libraries( ${OpenFHE_SHARED_LIBRARIES} )

add_subdirectory(src)

add_executable(test_mnist
        apps/main.cpp)
add_executable(keygen
        apps/keygen.cpp)
add_executable(encrypt
        apps/encrypt.cpp)
add_executable(test
        apps/test.cpp)
add_executable(testCPP
        apps/testCPP.cpp)


target_link_libraries(test_mnist
        Components)
target_link_libraries(keygen
        Components)
target_link_libraries(encrypt
        Components)
target_link_libraries(test
        Components)
#target_link_libraries(testCPP
#        Components)


install(TARGETS test_mnist
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS keygen
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS encrypt
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS test
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS testCPP
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
