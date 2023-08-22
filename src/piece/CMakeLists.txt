

add_library(
  chess_piece
  OBJECT 
  piece_list.cpp
  # piece.cpp
)


set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:chess_piece>
  PARENT_SCOPE
)