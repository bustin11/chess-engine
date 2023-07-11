

add_library(
  chess_board
  OBJECT 
  board.cpp
)


set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:chess_board>
  PARENT_SCOPE
)