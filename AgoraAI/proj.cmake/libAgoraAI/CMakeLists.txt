cmake_minimum_required(VERSION 3.4.1)
project(AgoraAI)

if (WIN32)
    set(PROJECT_NAME libAgoraAI)
elseif (ANDROID)
    set(PROJECT_NAME AgoraAI)
elseif (UNIX)
    set(PROJECT_NAME AgoraAI)
endif()

include_directories(../../src/libAgoraAI)
include_directories(../../include/opencl)

if (WIN32)
    include_directories(../../include)
endif()

set(SRCPATH ../../src/libAgoraAI/core)

if (IOS)
elseif (APPLE)
    # set(HEADERS
    #         ${SRCPATH}/common.h
    #         ${SRCPATH}/concat.h
    #         ${SRCPATH}/conv.h
    #         ${SRCPATH}/deconv.h
    #         ${SRCPATH}/eltwise.h
    #         ${SRCPATH}/layer.h
    #         ${SRCPATH}/net.h
    #         ${SRCPATH}/pad.h
    #         ${SRCPATH}/relu.h
    #         ${SRCPATH}/resize.h
    #         ${SRCPATH}/tensor.h
    #         ${SRCPATH}/utils.h)
    # set(SOURCES
    #         ${SRCPATH}/tensor.cpp
    #         ${SRCPATH}/utils.cpp
    #         )
elseif (UNIX)
    set(HEADERS
            ${SRCPATH}/common.h
            ${SRCPATH}/concat.h
            ${SRCPATH}/conv.h
            ${SRCPATH}/deconv.h
            ${SRCPATH}/eltwise.h
            ${SRCPATH}/layer.h
            ${SRCPATH}/net.h
            ${SRCPATH}/pad.h
            ${SRCPATH}/relu.h
            ${SRCPATH}/resize.h
            ${SRCPATH}/tensor.h
            ${SRCPATH}/utils.h)
    set(SOURCES
            ${SRCPATH}/tensor.cpp
            ${SRCPATH}/utils.cpp
            )
elseif (ANDROID)
    set(HEADERS
            ${SRCPATH}/common.h
            ${SRCPATH}/concat.h
            ${SRCPATH}/conv.h
            ${SRCPATH}/deconv.h
            ${SRCPATH}/eltwise.h
            ${SRCPATH}/layer.h
            ${SRCPATH}/net.h
            ${SRCPATH}/pad.h
            ${SRCPATH}/relu.h
            ${SRCPATH}/resize.h
            ${SRCPATH}/tensor.h
            ${SRCPATH}/utils.h
            )

    set(SOURCES
            ${SRCPATH}/tensor.cpp
            ${SRCPATH}/utils.cpp
            )
elseif (WIN32)
    set(HEADERS
            ../../include/NEON_2_SSE.h
            ${SRCPATH}/common.h
            ${SRCPATH}/concat.h
            ${SRCPATH}/conv.h
            ${SRCPATH}/deconv.h
            ${SRCPATH}/eltwise.h
            ${SRCPATH}/layer.h
            ${SRCPATH}/net.h
            ${SRCPATH}/pad.h
            ${SRCPATH}/relu.h
            ${SRCPATH}/resize.h
            ${SRCPATH}/tensor.h
            ${SRCPATH}/utils.h
            )

    set(SOURCES
            ${SRCPATH}/tensor.cpp
            ${SRCPATH}/utils.cpp
            )
endif()

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../lib)

if (WIN32)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()

add_compile_options(-std=c++11)
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})