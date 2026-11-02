cmake_minimum_required(VERSION 3.22)
project(HurricanAndroid LANGUAGES CXX)

if(NOT DEFINED ANDROID_NDK)
    message(FATAL_ERROR "This CMakeLists.txt is intended for Android NDK builds only.")
endif()

set(HURRICAN_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Hurrican")
message(STATUS "Configuring Hurrican Android SDL build from ${HURRICAN_ROOT}")

include(ExternalProject)

ExternalProject_Add(hurrican_android
    SOURCE_DIR "${HURRICAN_ROOT}"
    CMAKE_GENERATOR "${CMAKE_GENERATOR}"
    CMAKE_ARGS
        -DANDROID=ON
        -DANDROID_ABI=${ANDROID_ABI}
        -DANDROID_PLATFORM=${ANDROID_PLATFORM}
        -DANDROID_STL=c++_shared
        -DRENDERER=GLES2
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
    INSTALL_COMMAND ""
)

# Keep the native build output in the APK by exposing the generated library path.
set(HURRICAN_BINARY_DIR "${CMAKE_BINARY_DIR}/hurrican_android-prefix/src/hurrican_android-build")
add_custom_target(android_hurrican ALL DEPENDS hurrican_android)

# This project intentionally delegates the actual engine build to the existing Hurrican CMake project.
# The port is designed for the SDL2 + GLES renderer already present in the game source tree.
