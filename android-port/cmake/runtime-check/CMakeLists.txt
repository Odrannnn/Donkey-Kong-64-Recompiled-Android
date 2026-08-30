cmake_minimum_required(VERSION 3.22)
project(DK64AndroidRuntimeCheck LANGUAGES C CXX)
if(NOT ANDROID OR NOT ANDROID_ABI STREQUAL "arm64-v8a")
    message(FATAL_ERROR "This check targets Android arm64-v8a. Supply the NDK toolchain.")
endif()
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
get_filename_component(DK64_SOURCE "${CMAKE_CURRENT_LIST_DIR}/../../upstream" ABSOLUTE)
if(NOT EXISTS "${DK64_SOURCE}/lib/N64ModernRuntime/N64Recomp/lib/rabbitizer/CMakeLists.txt")
    # Rabbitizer does not need a CMakeLists.txt in every revision; check a source instead.
    if(NOT EXISTS "${DK64_SOURCE}/lib/N64ModernRuntime/N64Recomp/lib/rabbitizer/src/common/Utils.c")
        message(FATAL_ERROR "Initialize the upstream runtime's recursive submodules first.")
    endif()
endif()
add_subdirectory("${DK64_SOURCE}/lib/N64ModernRuntime" runtime EXCLUDE_FROM_ALL)
# Archive compilation checks NDK compatibility. It does NOT prove game linkage or execution.
add_custom_target(android_runtime_check DEPENDS ultramodern librecomp LiveRecomp)

