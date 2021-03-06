cmake_minimum_required(VERSION 3.0.0)
project(RsaFractal VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 11)

include_directories(Fractal/include)

add_executable(RsaFractal ./Fractal/src/main.cpp ./Fractal/src/Config.cpp ./Fractal/src/bmp.cpp
                          ./Fractal/src/generateImage.cpp
                          ./Fractal/src/Worker.cpp
                          ./Fractal/src/ImageGenerator.cpp)

if (NOT ANDROID)
    message("PC build")

    # Android doesnt need lpthread and doesn't support -march=native
    target_link_libraries(RsaFractal -lpthread)
    target_compile_options(RsaFractal PRIVATE -march=native)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
