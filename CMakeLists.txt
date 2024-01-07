cmake_minimum_required(VERSION 3.22 VERSION 0.1.0)

project(http_server)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(STANDARD_C_VERSION_FLAG "-std=c17")
set(OPTIMIZE_FLAG "-O3")
set(WARNING_FLAGS "-Werror -Wpedantic -Wall -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${STANDARD_C_VERSION_FLAG} ${OPTIMIZE_FLAG} ${WARNING_FLAGS}")

file(COPY static DESTINATION ${CMAKE_BINARY_DIR})

# -Wdeclaration-after-statement
# -Wno-shadow-ivar

add_executable(http_server 
    main.c
)
