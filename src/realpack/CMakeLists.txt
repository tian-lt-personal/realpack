add_library(realpack INTERFACE)
target_compile_features(realpack INTERFACE cxx_std_23)
target_include_directories(realpack INTERFACE include)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
endif()
