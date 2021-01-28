cmake_minimum_required(VERSION 3.4.1)
project(Jnd)

include_directories("../../src/libAgoraAI")
include_directories("../../include/opencl")

if (WIN32)
    include_directories(../../include)
endif()

if (ANDROID)
    if (ARMEABI_V7A)
        link_directories("../../build/android/armeabi-v7a/lib")
    endif()

    if (ARM64_V8A)
        link_directories("../../build/android/arm64-v8a/lib")
    endif()
elseif (WIN32)
    if (X86)
        link_directories("../../build/build_vs2017_agora_ai_x86/lib/Release")
    elseif (X86_64)
        link_directories("../../build/build_vs2017_agora_ai_x64/lib/Release")
    endif()
endif()

if (WIN32)
    set(PROJECT_NAME libJnd)
elseif (ANDROID)
    set(PROJECT_NAME Jnd)
elseif (UNIX)
    set(PROJECT_NAME Jnd)
endif()

set(SRCPATH ../../src/libJnd)


if (IOS)
    set(HEADERS
        ${SRCPATH}/ios/half_float.h
        ${SRCPATH}/ios/metal_define.h
        ${SRCPATH}/ios/metal_kernel_cfg.h
        ${SRCPATH}/ios/metal_engine.h
        ${SRCPATH}/ios/jnd_filter_oc.h
        ${SRCPATH}/ios/jnd_filter_metal.h
    )
    set(SOURCES
        ${SRCPATH}/ios/metal_kernel_cfg.mm
        ${SRCPATH}/ios/metal_engine.mm
        ${SRCPATH}/ios/half_float.mm
        ${SRCPATH}/ios/jnd_filter_oc.mm
        ${SRCPATH}/ios/jnd_filter_metal.mm
        #${SRCPATH}/ios/jnd_kernels.metal
    )
elseif (APPLE)
    set(HEADERS
        ${SRCPATH}/ios/half_float.h
        ${SRCPATH}/ios/metal_define.h
        ${SRCPATH}/ios/metal_kernel_cfg.h
        ${SRCPATH}/ios/metal_engine.h
        ${SRCPATH}/ios/jnd_filter_oc.h
        ${SRCPATH}/ios/jnd_filter_metal.h
        ${SRCPATH}/jnd_filter_wrapper.h
        ${SRCPATH}/Jnd4_Padding_ConvsAndBias_paras.h
    )
    set(SOURCES
        ${SRCPATH}/ios/metal_kernel_cfg.mm
        ${SRCPATH}/ios/metal_engine.mm
        ${SRCPATH}/ios/half_float.mm
        ${SRCPATH}/ios/jnd_filter_oc.mm
        ${SRCPATH}/ios/jnd_filter_metal.mm
        ${SRCPATH}/ios/jnd_kernels.metal
        ${SRCPATH}/jnd_filter_wrapper.cpp
    )
elseif (UNIX)
    set(HEADERS
        ${SRCPATH}/cl_engine.h
        ${SRCPATH}/cl_kernel_cfg.h
        ${SRCPATH}/half_float.h
        ${SRCPATH}/jnd_filter_cl.h
        ${SRCPATH}/jnd_filter.h
        ${SRCPATH}/jnd_funcs.h
	${SRCPATH}/jnd_filter_wrapper.h
        ${SRCPATH}/Jnd4_Padding_ConvsAndBias_paras.h
    )
    set(SOURCES
        ${SRCPATH}/cl_engine.cpp
        ${SRCPATH}/cl_kernel_cfg.cpp
        ${SRCPATH}/half_float.cpp
        ${SRCPATH}/jnd_filter_cl.cpp
        ${SRCPATH}/jnd_filter.cpp
        ${SRCPATH}/jnd_funcs.cpp
        ${SRCPATH}/jnd_kernel_cl.cpp
	${SRCPATH}/jnd_filter_wrapper.cpp
    )
elseif (ANDROID)
    set(HEADERS
        ${SRCPATH}/cl_engine.h
        ${SRCPATH}/cl_kernel_cfg.h
        ${SRCPATH}/half_float.h
        ${SRCPATH}/jnd_filter_cl.h
        ${SRCPATH}/jnd_filter.h
        ${SRCPATH}/jnd_funcs.h
    )
    set(SOURCES
        ${SRCPATH}/cl_engine.cpp
        ${SRCPATH}/cl_kernel_cfg.cpp
        ${SRCPATH}/half_float.cpp
        ${SRCPATH}/jnd_filter_cl.cpp
        ${SRCPATH}/jnd_filter.cpp
        ${SRCPATH}/jnd_funcs.cpp
        ${SRCPATH}/jnd_kernel_cl.cpp
    )
elseif (WIN32)
    set(HEADERS
        ${SRCPATH}/cl_engine.h
        ${SRCPATH}/cl_kernel_cfg.h
        ${SRCPATH}/half_float.h
        ${SRCPATH}/jnd_filter_cl.h
        ${SRCPATH}/jnd_filter.h
        ${SRCPATH}/jnd_funcs.h
    )
    set(SOURCES
        ${SRCPATH}/cl_engine.cpp
        ${SRCPATH}/cl_kernel_cfg.cpp
        ${SRCPATH}/half_float.cpp
        ${SRCPATH}/jnd_filter_cl.cpp
        ${SRCPATH}/jnd_filter.cpp
        ${SRCPATH}/jnd_funcs.cpp
        ${SRCPATH}/jnd_kernel_cl.cpp
    )
endif()

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -fno-exceptions -fno-rtti \
#                    -fno-bounds-check -funroll-loops -funswitch-loops -fvisibility=hidden -finline-functions -funsafe-loop-optimizations")

#add_compile_options(-std=c++11)
if (APPLE) 
#add something for mac or iOS
elseif (ANDROID)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings -fopenmp")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
elseif (WIN32)
    FIND_PACKAGE( OpenMP REQUIRED)
    if(OPENMP_FOUND)
        message("OPENMP FOUND")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS"${CMAKE_EXE_LINKER_FLAGS}${OpenMP_EXE_LINKER_FLAGS}")
    endif()
    
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -openmp")
    #SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -openmp")
elseif (UNIX)
    FIND_PACKAGE( OpenMP REQUIRED)
    if(OPENMP_FOUND)
        message("OPENMP FOUND")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS"${CMAKE_EXE_LINKER_FLAGS}${OpenMP_EXE_LINKER_FLAGS}")
    endif()
endif()

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../lib)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

if (APPLE)
#add something for mac or iOS
elseif (UNIX) 
    target_link_libraries(${PROJECT_NAME} libAgoraAI.a)
elseif (ANDROID)
    target_link_libraries(${PROJECT_NAME} libAgoraAI.a)
elseif (WIN32)
    target_link_libraries(${PROJECT_NAME} libAgoraAI)
endif()

