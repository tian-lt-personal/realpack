cmake_minimum_required(VERSION 3.5)

project(realpack
        VERSION 0.1.0
        LANGUAGES CXX)

add_subdirectory(realpack)
add_subdirectory(libmath)

include(CTest)
if (BUILD_TESTING)
  add_subdirectory(tests)
endif()
