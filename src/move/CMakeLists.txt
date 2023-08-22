

add_library(
  chess_move
  OBJECT 
  move.cpp
  precomputed_move_data.cpp
  move_generator.cpp
)


set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:chess_move>
  PARENT_SCOPE
)

add_custom_target(
  precompute_move_data
  COMMAND /usr/bin/clang-format
  -style=LLVM
  -i
  ${ALL_SOURCE_FILES}
)