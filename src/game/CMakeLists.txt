
add_library(
  chess_game
  OBJECT 
  game.cpp
)


set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:chess_game>
  PARENT_SCOPE
)