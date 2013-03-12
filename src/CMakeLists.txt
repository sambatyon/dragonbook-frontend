CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(frontend CXX)

SET(BIN_OUTPUT_DIR ${frontend_BINARY_DIR}/bin)
SET(LIB_OUTPUT_DIR ${frontend_BINARY_DIR}/lib)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR})

IF(MSVC)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS) # does not prompt warnings for printf, memcpy, etc
    ADD_DEFINITIONS(-D_WIN32_WINDOWS=_WIN32_WINNT_WIN7)
ENDIF()

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

FILE(GLOB sources "lexer/*.cpp" "symbols/*.cpp" "inter/*.cpp")
FILE(GLOB headers "lexer/*.hpp" "symbols/*.hpp" "inter/*.hpp")

SOURCE_GROUP("Source Files" FILES ${sources})
SOURCE_GROUP("Header Files" FILES ${headers})

ADD_LIBRARY(frontend STATIC ${sources} ${headers})