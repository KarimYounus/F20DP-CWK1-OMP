cmake_minimum_required(VERSION 3.27)
project(F20DP_CWK1_OMP C)
set(CMAKE_C_STANDARD 17)

add_library(F20DP_CWK1_OMP STATIC
        SumTot.c)

set(CMAKE_C_COMPILER "gcc-13")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
add_executable(program sumTot.c)
#set(CMAKE_BUILD_TYPE Debug)

add_custom_target(run
        COMMAND program 0 1000
        DEPENDS program
        WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

