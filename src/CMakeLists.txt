cmake_minimum_required(VERSION 3.5)

project(realpack
        VERSION 0.1.0
        LANGUAGES CXX)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

add_subdirectory(realpack)
add_subdirectory(libmath)

include(CTest)
if (BUILD_TESTING)
  add_subdirectory(tests)
endif()
