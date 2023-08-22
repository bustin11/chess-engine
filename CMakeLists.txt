cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # For clang-tidy.
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Require c++17 support

project(main
        VERSION 2023.1
        DESCRIPTION "Chess Engine running on a http server"
        LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        message(STATUS "Setting build type to `Debug` as none was specified.")
        set(CMAKE_BUILD_TYPE "Debug")
endif()
        

add_executable(main main.cpp) # top level

# add a sanitizer
if(NOT DEFINED CHESS_SANITIZER)
        set(CHESS_SANITIZER address)
endif()

# flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-unused-parameter -Wno-attributes") # TODO: remove
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fsanitize=${CHESS_SANITIZER} -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")

# ================== linting and formating suppport ===============
set(CHESS_BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build_support")

set(CHESS_CLANG_SEARCH_PATH "/usr/local/bin" "/usr/bin" "/usr/local/opt/llvm/bin" "/usr/local/opt/llvm@14/bin"
"/opt/homebrew/opt/llvm@14/bin/")

# ==================== dependencies ==============================

# Ctest
enable_testing()

find_program(CLANG_FORMAT_BIN
        NAMES clang-format clang-format-14
        HINTS ${CHESS_CLANG_SEARCH_PATH})

if("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
        message(WARNING "Chess/main couldn't find clang-format.")
else()
        message(STATUS "Chess/main found clang-format at ${CLANG_FORMAT_BIN}")
endif()

find_program(CLANG_TIDY_BIN
        NAMES clang-tidy clang-tidy-14
        HINTS ${CHESS_CLANG_SEARCH_PATH})

if("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
        message(WARNING "Chess/main couldn't find clang-tidy.")
else()
        # Output compile_commands.json
        set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
        message(STATUS "Chess/main found clang-tidy at ${CLANG_TIDY_BIN}")
endif()

find_program(CLANG_APPLY_REPLACEMENTS_BIN
        NAMES clang-apply-replacements clang-apply-replacements-14
        HINTS ${CHESS_CLANG_SEARCH_PATH})

if("${CLANG_APPLY_REPLACEMENTS_BIN}" STREQUAL "CLANG_APPLY_REPLACEMENTS_BIN-NOTFOUND")
        message(WARNING "Chess/main couldn't find clang-apply-replacements.")
else()
        # Output compile_commands.json
        set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
        message(STATUS "Chess/main found clang-apply-replacements at ${CLANG_APPLY_REPLACEMENTS_BIN}")
endif()

# cpplint
find_program(CPPLINT_BIN
        NAMES cpplint cpplint.py
        HINTS "${CHESS_BUILD_SUPPORT_DIR}")


if("${CPPLINT_BIN}" STREQUAL "CPPLINT_BIN-NOTFOUND")
        message(WARNING "Chess/main couldn't find cpplint.")
else()
        message(STATUS "Chess/main found cpplint at ${CPPLINT_BIN}")
endif()

# ================== searching for the compiler path ===============

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if(CMAKE_CXX_COMPILER_VERSION MATCHES "^14.")
                message(STATUS "You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
        else()
                message(WARNING "!! We recommend that you use clang-14 for developing Chess. You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}, a different version.")
        endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        message(STATUS "You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
else()
        message(WARNING "!! We recommend that you use clang-14 for developing Chess. You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}, which is not clang.")
endif()

# ls -l path_to_link
# ln -s to from
# unlink path_to_link
# /usr/bin/clang: symbolic link to ../lib/llvm-14/bin/clang

# ================== custom targets =================================



string(CONCAT CHESS_FORMAT_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/src,"
        "${CMAKE_CURRENT_SOURCE_DIR}/test,"
)

# Runs clang format and updates files in place.
add_custom_target(format ${CHESS_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${CHESS_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${CHESS_FORMAT_DIRS}
        --fix
        --quiet
)

# Runs clang format and exits with a non-zero exit code if any files need to be reformatted
add_custom_target(check-format ${CHESS_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${CHESS_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${CHESS_FORMAT_DIRS}
        --quiet
)

add_custom_target(check-clang-tidy
        ${CHESS_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
)
add_custom_target(fix-clang-tidy
        ${CHESS_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
        -clang-apply-replacements-binary ${CLANG_APPLY_REPLACEMENTS_BIN} # using our clang-apply-replacements binary
        -fix # apply suggested changes generated by clang-tidy
)
add_custom_target(check-clang-tidy-diff
        ${CHESS_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
        -only-diff # only check diff files to master
)
add_custom_target(fix-clang-tidy-diff
        ${CHESS_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
        -clang-apply-replacements-binary ${CLANG_APPLY_REPLACEMENTS_BIN} # using our clang-apply-replacements binary
        -fix # apply suggested changes generated by clang-tidy
        -only-diff # only check diff files to master
)

# ==================== Must be added after configuring compilier parameters =========================


# Includes.
set(CHESS_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
set(CHESS_THIRD_PARTY_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/third_party
        ${PROJECT_SOURCE_DIR}/third_party/httplib
)

include_directories(${CHESS_SRC_INCLUDE_DIR} ${CHESS_THIRD_PARTY_INCLUDE_DIR} utils)
include_directories(BEFORE src)

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

target_link_options(main PRIVATE -ldl -lglfw -lstdc++ -lGL -lglut -lGL -lGLU -lGLEW -lSOIL -lm)


# ==================== make check-lint =========================
# additional target to perform clang-format run, requires clang-format
# get all project files
file(GLOB_RECURSE CHESS_LINT_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
)

add_custom_target(check-lint echo '${CHESS_LINT_FILES}' | xargs -n12 -P8
        ${CPPLINT_BIN}
        --verbose=2 --quiet
        --linelength=120
        --filter=-legal/copyright,-build/header_guard,-runtime/references # https://github.com/cpplint/cpplint/issues/148
)

add_dependencies(check-clang-tidy gtest main)