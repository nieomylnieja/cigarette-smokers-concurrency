cmake_minimum_required(VERSION 3.15)
project(smokers C)

set(CMAKE_C_STANDARD 99)

add_executable(smokers main.c smokers.c smokers.h shared.c shared.h agent.c agent.h)