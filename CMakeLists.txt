cmake_minimum_required(VERSION 3.22.1)


set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Require c++17 support

project(main
        VERSION 2023.1
        DESCRIPTION "Chess Engine running on a http server"
        LANGUAGES C CXX)


add_executable(main main.cpp) # top level

# ================== dependencies =================================

# Ctest
enable_testing()

# ==================== Must be added after configuring compilier parameters =========================


# Includes.
set(CHESS_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
set(CHESS_THIRD_PARTY_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/third_party
        ${PROJECT_SOURCE_DIR}/third_party/httplib
)

include_directories(${CHESS_SRC_INCLUDE_DIR} ${CHESS_THIRD_PARTY_INCLUDE_DIR} utils)

add_subdirectory(third_party)
add_subdirectory(src)
add_subdirectory(test)
target_link_libraries(main chess)

set_target_properties(main
          PROPERTIES
          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src"
          COMMAND main
          )
# ==================== imgs =================================

file(GLOB IMAGE_SOURCES "${PROJECT_SOURCE_DIR}/src/ui/imgs/*.png")
# message(${IMAGE_SOURCES})

foreach(image_source ${IMAGE_SOURCES})
  message(${image_source})
  get_filename_component(image_name ${image_source} NAME)
  set(image_destination "${CMAKE_CURRENT_BINARY_DIR}/imgs/${image_name}")
  configure_file(${image_source} ${image_destination} COPYONLY)
endforeach()

target_sources(main PRIVATE ${IMAGE_SOURCES})

# ===================== linker options =========================

target_link_options(main PRIVATE -ldl -lglfw -lstdc++ -lGL -lglut -lGL -lGLU -lGLEW -lSOIL)


# ==================== make format =========================
# additional target to perform clang-format run, requires clang-format
# get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h *.hpp)
foreach (SOURCE_FILE ${ALL_SOURCE_FILES})
    string(FIND ${SOURCE_FILE} third_party PROJECT_TRDPARTY_DIR_FOUND)
    if (NOT ${PROJECT_TRDPARTY_DIR_FOUND} EQUAL -1)
        list(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
    endif ()
endforeach ()

add_custom_target(
        format
        COMMAND /usr/bin/clang-format
        -style=LLVM
        -i
        ${ALL_SOURCE_FILES}
)

