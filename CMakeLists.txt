cmake_minimum_required(VERSION 3.20)
project(NanoC_Compiler)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES
        src/*.h
        src/*.cpp
        )

add_executable(NanoC_Compiler ${SOURCES} src/Parser.cpp src/Parser.h src/VirtualMachine_NC_BYTE-CODE.cpp src/VirtualMachine_NC_BYTE-CODE.h src/Compiler_to_NCASM.cpp src/Compiler_to_NCASM.h)
