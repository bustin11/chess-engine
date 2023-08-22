

add_library(
  chess_player
  OBJECT 
  human_player.cpp
)


set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:chess_player>
  PARENT_SCOPE
)