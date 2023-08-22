

add_subdirectory(board)
add_subdirectory(game)
add_subdirectory(ui)
add_subdirectory(move)
add_subdirectory(player)
add_subdirectory(piece)

add_library(chess STATIC ${ALL_OBJECT_FILES})

set(CHESS_LIBS
  chess_board
  chess_game
  chess_ui
  chess_move
  chess_player
  chess_piece
)


set(CHESS_THIRD_PARTY_LIBS
  httplib::httplib
)


target_link_libraries(
  chess 
  ${CHESS_LIBS}
  ${CHESS_THIRD_PARTY_LIBS}
)

# https://www.reddit.com/r/learnprogramming/comments/j1yyb7/cmake_target_link_directories_vs_target_link/
# https://stackoverflow.com/questions/58729233/what-is-the-use-case-for-generator-expression-on-target-include-directories
target_include_directories(
  chess PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   message(STATUS "dir='${dir}'")
# endforeach()