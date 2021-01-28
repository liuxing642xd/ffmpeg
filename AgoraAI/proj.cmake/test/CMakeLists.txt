cmake_minimum_required(VERSION 3.4.1)
project(testJnd)

include_directories("../../src/libAgoraAI")
include_directories("../../src/libJnd")
include_directories("../../include/opencl")

if (IOS)
#do something here
elseif (APPLE)
#do something here
endif()

if (WIN32)
    include_directories(../../include)
endif()

set(PROJECT_NAME testJnd)

if (IOS)
  link_directories("../../build/ios/lib")
elseif (APPLE)
    link_directories("../../build/mac/lib")
elseif (UNIX)
    link_directories("../../build/linux/lib")
    link_directories("../../lib/opencl/")
elseif (ANDROID)
    if (ARMEABI_V7A)
        link_directories("../../build/android/armeabi-v7a/lib")
        link_directories("../../lib/opencl/armeabi-v7a")
    endif()

    if (ARM64_V8A)
        link_directories("../../build/android/arm64-v8a/lib")
        link_directories("../../lib/opencl/arm64-v8a")
    endif()
elseif (WIN32)
    if (X86)
        link_directories("../../build/build_vs2017_agora_ai_x86/lib/Release")
        link_directories("../../lib/opencl/x86")
    elseif (X86_64)
        link_directories("../../build/build_vs2017_agora_ai_x64/lib/Release")
        link_directories("../../lib/opencl/x64")
    endif()
endif()

if (IOS) 
#
elseif (APPLE)
#
elseif (UNIX)
    set(HEADERS
        ../../src/test/KernelMacro.h
        ../../src/test/jnd_model.h
    )
    set(SOURCES
        #../../src/test/TestOpenCL.cpp
        ../../src/test/Test.cpp
        #../../src/test/jnd.cpp
        ../../src/test/jnd_filter_sample.cpp
    )
elseif (ANDROID)
    set(HEADERS
        ../../src/test/KernelMacro.h
        ../../src/test/jnd_model.h
    )
    set(SOURCES
        #../../src/test/TestOpenCL.cpp
        ../../src/test/Test.cpp
        #../../src/test/jnd.cpp
        ../../src/test/jnd_filter_sample.cpp
    )
elseif (WIN32)
    set(HEADERS
        ../../src/test/KernelMacro.h
        ../../src/test/jnd_model.h
    )
    set(SOURCES
        #../../src/test/TestOpenCL.cpp
        ../../src/test/Test.cpp
        #../../src/test/jnd.cpp
        ../../src/test/jnd_filter_sample.cpp
    )
endif()

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


#set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../lib)
#add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

#set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../lib/${ABI}")

if (IOS)
#add something here for ios
elseif (APPLE)
#add something here for mac
elseif (ANDROID)
    add_dependencies(${PROJECT_NAME} Jnd)
    target_link_libraries(${PROJECT_NAME} libJnd.a libOpenCL.so)
elseif (WIN32)
    add_dependencies(${PROJECT_NAME} libJnd)
    target_link_libraries(${PROJECT_NAME} libJnd OpenCL)
elseif (UNIX)
    add_dependencies(${PROJECT_NAME} Jnd)
    target_link_libraries(${PROJECT_NAME} libJnd.a libOpenCL.so)
endif()

