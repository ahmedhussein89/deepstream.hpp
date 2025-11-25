# ${CMAKE_SOURCE_DIR}/examples/deepstream-app/CMakeLists.txt
cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(deepstream-app
    VERSION 7.1.0
    DESCRIPTION "NVIDIA DeepStream Application"
    LANGUAGES C CXX
)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Options
set(NVDS_VERSION "7.1" CACHE STRING "DeepStream version")

# Add cmake module path for FindDeepStream.cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Installation directories
set(LIB_INSTALL_DIR "/opt/nvidia/deepstream/deepstream-${NVDS_VERSION}/lib/"
    CACHE PATH "Library installation directory")
set(APP_INSTALL_DIR "/opt/nvidia/deepstream/deepstream-${NVDS_VERSION}/bin/"
    CACHE PATH "Application installation directory")

# Detect platform
if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(PLATFORM_TEGRA ON)
    add_compile_definitions(PLATFORM_TEGRA)
endif()

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(CUDAToolkit REQUIRED)

# Find DeepStream SDK
find_package(DeepStream ${NVDS_VERSION} REQUIRED COMPONENTS
    nvdsgst_meta
    nvds_meta
    nvdsgst_helper
    nvdsgst_customhelper
    nvdsgst_smartrecord
    nvds_utils
    nvds_msgbroker
)

# Find GStreamer and other dependencies
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GSTREAMER_VIDEO REQUIRED gstreamer-video-1.0)
pkg_check_modules(X11 REQUIRED x11)
pkg_check_modules(JSON_GLIB REQUIRED json-glib-1.0)
pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)

# Collect source files
file(GLOB SRCS_C "*.c")
file(GLOB SRCS_CPP "*.cpp")
file(GLOB COMMON_SRCS_C "${DeepStream_ROOT_DIR}/sources/apps/apps-common/*.c")
file(GLOB COMMON_SRCS_SRCS_CPP "${DeepStream_ROOT_DIR}/sources/apps/apps-common/src/*.c")
file(GLOB YAML_SRCS_CPP "${DeepStream_ROOT_DIR}/sources/apps/apps-common/src/deepstream-yaml/*.cpp")

set(ALL_SOURCES
    ${SRCS_C}
    ${SRCS_CPP}
    ${COMMON_SRCS_C}
    ${COMMON_SRCS_SRCS_CPP}
    ${YAML_SRCS_CPP}
)

# Create executable
add_executable(${PROJECT_NAME} ${ALL_SOURCES})

# Set target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "deepstream-app"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CUDAToolkit_INCLUDE_DIRS}
    ${DeepStream_INCLUDE_DIRS}
    ${GSTREAMER_INCLUDE_DIRS}
    ${GSTREAMER_VIDEO_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    ${JSON_GLIB_INCLUDE_DIRS}
    ${YAML_CPP_INCLUDE_DIRS}
    ${DeepStream_ROOT_DIR}
    ${DeepStream_ROOT_DIR}/sources/apps/includes
    ${DeepStream_ROOT_DIR}/sources/apps/apps-common/includes
)

# Compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    DS_VERSION_MINOR=1
    DS_VERSION_MAJOR=5
)

# Link directories
target_link_directories(${PROJECT_NAME} PRIVATE
    ${CUDAToolkit_LIBRARY_DIR}
    ${DeepStream_LIBRARY_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    # CUDA
    CUDA::cudart
    CUDA::cuda_driver

    # DeepStream libraries (using imported targets)
    DeepStream::nvdsgst_meta
    DeepStream::nvds_meta
    DeepStream::nvdsgst_helper
    DeepStream::nvdsgst_customhelper
    DeepStream::nvdsgst_smartrecord
    DeepStream::nvds_utils
    DeepStream::nvds_msgbroker

    # GStreamer and other dependencies
    ${GSTREAMER_LIBRARIES}
    ${GSTREAMER_VIDEO_LIBRARIES}
    ${X11_LIBRARIES}
    ${JSON_GLIB_LIBRARIES}
    ${YAML_CPP_LIBRARIES}

    # System libraries
    m
    dl
)

# Set RPATH for runtime library search
set_target_properties(${PROJECT_NAME} PROPERTIES
    INSTALL_RPATH "${DeepStream_LIBRARY_DIRS}"
    BUILD_RPATH "${DeepStream_LIBRARY_DIRS}"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

# Installation rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "${APP_INSTALL_DIR}"
    COMPONENT Runtime
)

# Print configuration summary
message(STATUS "=== DeepStream Application Configuration ===")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "CUDA Toolkit: ${CUDAToolkit_VERSION}")
message(STATUS "DeepStream Version: ${DeepStream_VERSION}")
message(STATUS "DeepStream Root: ${DeepStream_ROOT_DIR}")
message(STATUS "Platform: ${CMAKE_SYSTEM_PROCESSOR}")
if(PLATFORM_TEGRA)
    message(STATUS "Tegra Platform: YES")
endif()
message(STATUS "Install Directory: ${APP_INSTALL_DIR}")
message(STATUS "Library Directory: ${DeepStream_LIBRARY_DIRS}")
message(STATUS "==========================================")
