add_library(realpack_math
  source/tokenizer.cpp
)
target_include_directories(realpack_math PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(realpack_math PUBLIC
  realpack
)

if(MSVC)
  target_compile_options(realpack_math PRIVATE /W4 /WX)
else()
  target_compile_options(realpack_math PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
