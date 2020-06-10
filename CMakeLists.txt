cmake_minimum_required(VERSION 3.0.0)
project(RsaFractal VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 11)

include_directories(Fractal/)

add_executable(RsaFractal ./Fractal/main.cpp ./Fractal/CommandArgsParser.cpp ./Fractal/RgbLut.cpp ./Fractal/bmp.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
