set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_VERSION 1)
set(UNIX True)
set(APPLE True)
set(IOS True)

# Required as of cmake 2.8.10
set(CMAKE_OSX_DEPLOYMENT_TARGET "" CACHE STRING "Force unset of the deployment target for iOS" FORCE)

# Force the compilers to Clang for iOS
include(CMakeForceCompiler)
set(CMAKE_C_COMPILER /usr/bin/clang Clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++ Clang)
set(CMAKE_AR ar CACHE FILEPATH "" FORCE)

# Skip the platform compiler checks for cross compiling
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_C_COMPILER_WORKS TRUE)

# All iOS/Darwin specific settings - some may be redundant
set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".dylib")
set(CMAKE_SHARED_MODULE_PREFIX "lib")
set(CMAKE_SHARED_MODULE_SUFFIX ".so")
set(CMAKE_MODULE_EXISTS 1)
set(CMAKE_DL_LIBS "")

set(CMAKE_C_OSX_COMPATIBILITY_VERSION_FLAG "-compatibility_version ")
set(CMAKE_C_OSX_CURRENT_VERSION_FLAG "-current_version ")
set(CMAKE_CXX_OSX_COMPATIBILITY_VERSION_FLAG "${CMAKE_C_OSX_COMPATIBILITY_VERSION_FLAG}")
set(CMAKE_CXX_OSX_CURRENT_VERSION_FLAG "${CMAKE_C_OSX_CURRENT_VERSION_FLAG}")

# Hidden visibilty is required for cxx on iOS 
set(CMAKE_C_FLAGS_INIT "")
set(CMAKE_CXX_FLAGS_INIT "-fvisibility=hidden -fvisibility-inlines-hidden")

set(CMAKE_C_LINK_FLAGS "-Wl,-search_paths_first ${CMAKE_C_LINK_FLAGS}")
set(CMAKE_CXX_LINK_FLAGS "-Wl,-search_paths_first ${CMAKE_CXX_LINK_FLAGS}")

set(CMAKE_PLATFORM_HAS_INSTALLNAME 1)
set(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "-dynamiclib -headerpad_max_install_names")
set(CMAKE_SHARED_MODULE_CREATE_C_FLAGS "-bundle -headerpad_max_install_names")
set(CMAKE_SHARED_MODULE_LOADER_C_FLAG "-Wl,-bundle_loader,")
set(CMAKE_SHARED_MODULE_LOADER_CXX_FLAG "-Wl,-bundle_loader,")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".dylib" ".so" ".a")

# hack: if a new cmake (which uses CMAKE_INSTALL_NAME_TOOL) runs on an old build tree
# (where install_name_tool was hardcoded) and where CMAKE_INSTALL_NAME_TOOL isn't in the cache
# and still cmake didn't fail in CMakeFindBinUtils.cmake (because it isn't rerun)
# hardcode CMAKE_INSTALL_NAME_TOOL here to install_name_tool, so it behaves as it did before, Alex
if (NOT DEFINED CMAKE_INSTALL_NAME_TOOL)
    find_program(CMAKE_INSTALL_NAME_TOOL install_name_tool)
endif (NOT DEFINED CMAKE_INSTALL_NAME_TOOL)

# Setup iOS platform unless specified manually with IOS_PLATFORM
if (NOT DEFINED IOS_PLATFORM)
    set(IOS_PLATFORM "OS")
endif (NOT DEFINED IOS_PLATFORM)
set(IOS_PLATFORM ${IOS_PLATFORM} CACHE STRING "Type of iOS Platform")

# Setup building for arm64 or not
if (NOT DEFINED BUILD_ARM64)
    set(BUILD_ARM64 true)
endif (NOT DEFINED BUILD_ARM64)
set(BUILD_ARM64 ${BUILD_ARM64} CACHE STRING "Build arm64 arch or not")

# Check the platform selection and setup for developer root
if (IOS_PLATFORM STREQUAL "OS")
    set(IOS_PLATFORM_LOCATION "iPhoneOS.platform")

    # This causes the installers to properly locate the output libraries
    set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos")
elseif (IOS_PLATFORM STREQUAL "SIMULATOR")
    set(SIMULATOR true)
    set(IOS_PLATFORM_LOCATION "iPhoneSimulator.platform")

    # This causes the installers to properly locate the output libraries
    set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphonesimulator")
else ()
    message(FATAL_ERROR "Unsupported IOS_PLATFORM value selected. Please choose OS or SIMULATOR")
endif ()

# Setup iOS developer location unless specified manually with CMAKE_IOS_DEVELOPER_ROOT
set(XCODE_ROOT "/Applications/Xcode.app/Contents/Developer/Platforms/${IOS_PLATFORM_LOCATION}/Developer")
if (NOT DEFINED CMAKE_IOS_DEVELOPER_ROOT)
    set(CMAKE_IOS_DEVELOPER_ROOT ${XCODE_ROOT})
endif (NOT DEFINED CMAKE_IOS_DEVELOPER_ROOT)
set(CMAKE_IOS_DEVELOPER_ROOT ${CMAKE_IOS_DEVELOPER_ROOT} CACHE PATH "Location of iOS Platform")

# Find and use the most recent iOS sdk unless specified manually with CMAKE_IOS_SDK_ROOT
if (NOT DEFINED CMAKE_IOS_SDK_ROOT)
    file(GLOB _CMAKE_IOS_SDKS "${CMAKE_IOS_DEVELOPER_ROOT}/SDKs/*")
    if (_CMAKE_IOS_SDKS)
        list(SORT _CMAKE_IOS_SDKS)
        list(REVERSE _CMAKE_IOS_SDKS)
        list(GET _CMAKE_IOS_SDKS 0 CMAKE_IOS_SDK_ROOT)
    else (_CMAKE_IOS_SDKS)
        message(FATAL_ERROR "No iOS SDK's found in default search path ${CMAKE_IOS_DEVELOPER_ROOT}. Manually set CMAKE_IOS_SDK_ROOT or install the iOS SDK.")
    endif (_CMAKE_IOS_SDKS)
    message(STATUS "Toolchain using default iOS SDK: ${CMAKE_IOS_SDK_ROOT}")
endif (NOT DEFINED CMAKE_IOS_SDK_ROOT)
set(CMAKE_IOS_SDK_ROOT ${CMAKE_IOS_SDK_ROOT} CACHE PATH "Location of the selected iOS SDK")

# Set the sysroot default to the most recent SDK
set(CMAKE_OSX_SYSROOT ${CMAKE_IOS_SDK_ROOT} CACHE PATH "Sysroot used for iOS support")

# set the architecture for iOS 
if (IOS_PLATFORM STREQUAL "OS")
    set(IOS_ARCH armv7 arm64)
elseif (IOS_PLATFORM STREQUAL "SIMULATOR")
    set(IOS_ARCH i386 x86_64)
endif ()

set(CMAKE_OSX_ARCHITECTURES ${IOS_ARCH} CACHE string "Build architecture for iOS")

# Set the find root to the iOS developer roots and to user defined paths
set(CMAKE_FIND_ROOT_PATH ${CMAKE_IOS_DEVELOPER_ROOT} ${CMAKE_IOS_SDK_ROOT} ${CMAKE_PREFIX_PATH} CACHE string "iOS find search path root")

# default to searching for frameworks first
set(CMAKE_FIND_FRAMEWORK FIRST)

# set up the default search directories for frameworks
set(CMAKE_SYSTEM_FRAMEWORK_PATH
        ${CMAKE_IOS_SDK_ROOT}/System/Library/Frameworks
        ${CMAKE_IOS_SDK_ROOT}/System/Library/PrivateFrameworks
        ${CMAKE_IOS_SDK_ROOT}/Developer/Library/Frameworks
        )

# only search the iOS sdks, not the remainder of the host filesystem
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(CMAKE_CONFIGURATION_TYPES "Debug;Release")

set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "YES")
set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE "YES")
set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "7.0")
set(CMAKE_XCODE_ATTRIBUTE_GCC_ENABLE_CPP_EXCEPTIONS "NO")
set(CMAKE_XCODE_ATTRIBUTE_GCC_ENABLE_CPP_RTTI "NO")
set(CMAKE_XCODE_ATTRIBUTE_GCC_WARN_INHIBIT_ALL_WARNINGS "YES")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES")
set(CMAKE_XCODE_ATTRIBUTE_ENABLE_NS_ASSERTIONS[variant=Release] NO)

# This little macro lets you set any XCode specific property
macro(set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
    set_property(TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
endmacro(set_xcode_property)

