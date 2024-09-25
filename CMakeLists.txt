cmake_minimum_required(VERSION 3.22 VERSION 0.1.0)

project(http_server)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(STANDARD_C_VERSION_FLAG "-std=c17")
set(OPTIMIZE_FLAG "-O2")
set(WARNING_FLAGS "-Werror -Wpedantic -Wall")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	# Clang compiler
	set(DEBUG_FLAGS "-g -ggdb")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	# GNU compiler
	set(DEBUG_FLAGS "-g -glldb")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${STANDARD_C_VERSION_FLAG} ${OPTIMIZE_FLAG} ${WARNING_FLAGS} ${DEBUG_FLAGS}")

file(COPY static DESTINATION ${CMAKE_BINARY_DIR})
file(COPY lint_codebase.sh DESTINATION ${CMAKE_BINARY_DIR})

file(GLOB_RECURSE SOURCE_FILES "src/*.c")

add_executable(${PROJECT_NAME}
    ${SOURCE_FILES}
)
