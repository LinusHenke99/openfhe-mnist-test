cmake_minimum_required(VERSION 3.5.1)

add_library(Components
        LoadImage.cpp Deserialize.cpp LinTools.cpp random_double.cpp MatrixFormatting.cpp KeyGen.cpp)

target_include_directories(Components
        PUBLIC ../include
        ${OPENMP_INCLUDES}
        ${OpenFHE_INCLUDE}
        ${OpenFHE_INCLUDE}/third-party/include
        ${OpenFHE_INCLUDE}/core
        ${OpenFHE_INCLUDE}/pke
        )

target_link_libraries(Components
        ${OpenFHE_SHARED_LIBRARIES}
        )
