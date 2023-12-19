cmake_minimum_required(VERSION 3.27)
project(BigHomework C)

set(CMAKE_C_STANDARD 11)

add_executable(BigHomework main.c
        commonutil.h
        socketLib.h
        tinycsv.h
        webuiapi.h
        wrapapi.c)

target_link_libraries(BigHomework -lws2_32)

