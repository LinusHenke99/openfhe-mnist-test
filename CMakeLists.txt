cmake_minimum_required(VERSION 3.5.1)

project(test-mnist)

set(CMAKE_CXX_STANDARD 17)
option( BUILD_STATIC "Set to ON to include static versions of the library" OFF)

find_package(OpenFHE REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

set( CMAKE_CXX_FLAGS ${OpenFHE_CXX_FLAGS} )

link_directories( ${OpenFHE_LIBDIR} )
link_directories( ${OPENMP_LIBRARIES} )

set( CMAKE_EXE_LINKER_FLAGS ${OpenFHE_EXE_LINKER_FLAGS} )
link_libraries( ${OpenFHE_SHARED_LIBRARIES} )


file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

foreach(subdir IN ZIP_LISTS keys ciphertexts)
	file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/${subdir})
	foreach(model IN ZIP_LISTS very_sensible_nn cryptonet)
		file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/${subdir}/${model})
	endforeach()
endforeach()


add_subdirectory(src)

add_executable(mnist_inference_cryptonet
        apps/mnist_inference_cryptonet.cpp)
add_executable(mnist_inference_very_sensible_nn
        apps/mnist_inference_very_sensible_model.cpp)
add_executable(keygen
        apps/keygen.cpp)
add_executable(encrypt
        apps/encrypt.cpp)
add_executable(test_matMul
        apps/test.cpp)
add_executable(eval_mnist
        apps/eval_mnist.cpp)
add_executable(fix_output
        apps/fix_output.cpp)
add_executable(test_MLOperators
        apps/test_MLOperators.cpp)


target_link_libraries(mnist_inference_cryptonet
        Components)
target_link_libraries(mnist_inference_very_sensible_nn
        Components)
target_link_libraries(keygen
        Components)
target_link_libraries(encrypt
        Components)
target_link_libraries(test_matMul
        Components)
target_link_libraries(eval_mnist
        Components)
target_link_libraries(fix_output
        Components)
target_link_libraries(test_MLOperators
        Components)


install(TARGETS mnist_inference_cryptonet
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS mnist_inference_very_sensible_nn
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS keygen
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS encrypt
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS test_matMul
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS eval_mnist
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS test_MLOperators
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
