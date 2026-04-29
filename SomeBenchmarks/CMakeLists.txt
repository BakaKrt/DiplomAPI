cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(SomeBenchmarks VERSION 1.0 LANGUAGES CXX)

################################################################################
# Platform configuration
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()

message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

# Note: x86 support would require additional handling if needed
if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"
     OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

################################################################################
# Global compiler options (matching vcxproj settings)
################################################################################
if(MSVC)
    # Debug configuration: x64
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od")                    # Optimizations disabled
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")                    # Debug info
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast")               # Fast FP model
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")                  # Exception handling std
    endif()

    # Release configuration: x64  
    if(CMAKE_BUILD_TYPE STREQUAL "Release" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")                    # Max optimizations
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast")               # Fast FP model  
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")                  # Exception handling std
    endif()

    # Set language standard (stdcpplatest)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Runtime library settings (matching vcxproj)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MDd")                   # MultiThreadedDLL Debug
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")                    # MultiThreadedDLL Release
    endif()

endif()

################################################################################
# Global linker options
################################################################################
if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
endif()

################################################################################
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects - Add main executable
################################################################################
add_executable(SomeBenchmarks
    cpuinf.ixx
    FlatArray.ixx
    iter.ixx
    normalsum.ixx
    random.ixx
    SharedMemoryHelper.ixx
    sse.ixx
    test.cpp
    test.ixx
)

# Add nanobench.h as header for the project
target_include_directories(SomeBenchmarks PRIVATE .)

################################################################################
# Debug/Release configuration types (to match VS behavior)
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)