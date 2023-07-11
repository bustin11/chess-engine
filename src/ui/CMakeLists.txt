
add_library(
  chess_web
  OBJECT 
  web.cpp
  window.cpp
  glad.c
)


set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:chess_web>
  PARENT_SCOPE
)


