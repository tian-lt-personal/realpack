include(FetchContent)
# The policy uses the download time for timestamp, instead of the timestamp in the archive. This
# allows for proper rebuilds when a projects URL changes.
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/releases/download/v1.15.2/googletest-1.15.2.tar.gz
  URL_HASH SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # For Windows: Prevent overriding the parent project's compiler/linker settings
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE) # Disable GMock
FetchContent_MakeAvailable(googletest)

add_executable(realpack_ut
               create_tests.cpp
               n_tests.cpp
               op_tests.cpp
               z_tests.cpp)
target_link_libraries(realpack_ut PRIVATE
                      realpack
                      gtest_main)
