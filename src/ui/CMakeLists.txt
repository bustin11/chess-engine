
add_library(
  chess_ui
  OBJECT 
  web.cpp
  window.cpp
  glad.c
)


set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:chess_ui>
  PARENT_SCOPE
)


