# FindDeepStream.cmake
# ----------------------
# Finds the NVIDIA DeepStream SDK installation
#
# This module defines:
#  DeepStream_FOUND          - True if DeepStream SDK is found
#  DeepStream_VERSION        - Version of DeepStream SDK
#  DeepStream_INCLUDE_DIRS   - Include directories for DeepStream
#  DeepStream_LIBRARIES      - All DeepStream libraries
#  DeepStream_LIBRARY_DIRS   - Library directories for DeepStream
#  DeepStream_ROOT_DIR       - Root directory of DeepStream installation
#  DeepStream_PLATFORM       - Platform type (dGPU or Jetson)
#
# Individual library targets (if found):
#  DeepStream::nvdsgst_meta
#  DeepStream::nvds_meta
#  DeepStream::nvdsgst_helper
#  DeepStream::nvdsgst_customhelper
#  DeepStream::nvdsgst_smartrecord
#  DeepStream::nvds_utils
#  DeepStream::nvds_msgbroker
#  DeepStream::nvds_yml_parser
#  DeepStream::nvds_batch_meta
#
# Usage:
#  # Find DeepStream with all libraries
#  find_package(DeepStream 7.1 REQUIRED)
#  target_link_libraries(myapp PRIVATE DeepStream::DeepStream)
#
#  # Find DeepStream with specific components
#  find_package(DeepStream 7.1 REQUIRED COMPONENTS nvds_meta nvdsgst_meta)
#  target_link_libraries(myapp PRIVATE DeepStream::nvds_meta DeepStream::nvdsgst_meta)
#
#  # Find DeepStream with optional components
#  find_package(DeepStream 7.1 COMPONENTS nvds_msgbroker nvds_yml_parser)
#  if(DeepStream_nvds_msgbroker_FOUND)
#      target_link_libraries(myapp PRIVATE DeepStream::nvds_msgbroker)
#  endif()
#
# Environment variables used:
#  DEEPSTREAM_ROOT or DeepStream_ROOT - Root directory of DeepStream installation
#
# Platform Support:
#  - Linux x86_64 (dGPU)
#  - Linux aarch64 (Jetson)
#
################################################################################

# Ensure we're on a supported platform
if(NOT UNIX)
    message(FATAL_ERROR "DeepStream SDK is only supported on Linux platforms")
endif()

# Define DeepStream version to search for
if(NOT DeepStream_FIND_VERSION)
    set(DeepStream_FIND_VERSION "7.1")
endif()

# Detect platform type
if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(DeepStream_PLATFORM "Jetson")
else()
    set(DeepStream_PLATFORM "dGPU")
endif()

# Search paths for DeepStream installation
set(_DEEPSTREAM_SEARCH_PATHS
    /opt/nvidia/deepstream/deepstream-${DeepStream_FIND_VERSION}
    /opt/nvidia/deepstream/deepstream
    $ENV{DEEPSTREAM_ROOT}
    $ENV{DeepStream_ROOT}
    ${DeepStream_ROOT}
)

# Find DeepStream root directory
find_path(DeepStream_ROOT_DIR
    NAMES lib/libnvdsgst_meta.so sources/includes/nvds_version.h
    PATHS ${_DEEPSTREAM_SEARCH_PATHS}
    DOC "DeepStream SDK root directory"
)

# Find version header
if(DeepStream_ROOT_DIR)
    find_file(DeepStream_VERSION_HEADER
        NAMES nvds_version.h
        PATHS ${DeepStream_ROOT_DIR}/sources/includes
              ${DeepStream_ROOT_DIR}/include
        NO_DEFAULT_PATH
    )

    # Parse version from header if found
    if(DeepStream_VERSION_HEADER AND EXISTS ${DeepStream_VERSION_HEADER})
        file(READ ${DeepStream_VERSION_HEADER} _VERSION_HEADER_CONTENTS)

        string(REGEX MATCH "#define[ \t]+NVDS_VERSION_MAJOR[ \t]+([0-9]+)" 
               _MATCH "${_VERSION_HEADER_CONTENTS}")
        set(DeepStream_VERSION_MAJOR ${CMAKE_MATCH_1})

        string(REGEX MATCH "#define[ \t]+NVDS_VERSION_MINOR[ \t]+([0-9]+)" 
               _MATCH "${_VERSION_HEADER_CONTENTS}")
        set(DeepStream_VERSION_MINOR ${CMAKE_MATCH_1})

        string(REGEX MATCH "#define[ \t]+NVDS_VERSION_MICRO[ \t]+([0-9]+)" 
               _MATCH "${_VERSION_HEADER_CONTENTS}")
        set(DeepStream_VERSION_MICRO ${CMAKE_MATCH_1})

        if(DeepStream_VERSION_MAJOR AND DeepStream_VERSION_MINOR)
            set(DeepStream_VERSION
                "${DeepStream_VERSION_MAJOR}.${DeepStream_VERSION_MINOR}")
            if(DeepStream_VERSION_MICRO)
                set(DeepStream_VERSION "${DeepStream_VERSION}.${DeepStream_VERSION_MICRO}")
            endif()
        endif()
    endif()
endif()

# Validate version if specified
if(DeepStream_FIND_VERSION AND DeepStream_VERSION)
    if(DeepStream_VERSION VERSION_LESS DeepStream_FIND_VERSION)
        if(DeepStream_FIND_REQUIRED)
            message(FATAL_ERROR 
                "DeepStream version ${DeepStream_VERSION} is older than requested ${DeepStream_FIND_VERSION}")
        else()
            message(WARNING 
                "DeepStream version ${DeepStream_VERSION} is older than requested ${DeepStream_FIND_VERSION}")
            set(DeepStream_FOUND FALSE)
        endif()
    endif()
endif()

# Set include directories
if(DeepStream_ROOT_DIR)
    set(DeepStream_INCLUDE_DIRS "")

    # Add sources/includes if it exists (primary location)
    if(EXISTS "${DeepStream_ROOT_DIR}/sources/includes")
        list(APPEND DeepStream_INCLUDE_DIRS "${DeepStream_ROOT_DIR}/sources/includes")
    endif()

    # Add include if it exists (fallback location)
    if(EXISTS "${DeepStream_ROOT_DIR}/include")
        list(APPEND DeepStream_INCLUDE_DIRS "${DeepStream_ROOT_DIR}/include")
    endif()

    # If no include directories found, log a warning
    if(NOT DeepStream_INCLUDE_DIRS)
        if(NOT DeepStream_FIND_QUIETLY)
            message(WARNING "DeepStream include directories not found at expected locations")
        endif()
    endif()

    set(DeepStream_LIBRARY_DIRS
        ${DeepStream_ROOT_DIR}/lib
    )
endif()

# Find required system dependencies
# GStreamer is required for DeepStream
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(GSTREAMER gstreamer-1.0>=1.14)
    pkg_check_modules(GSTREAMER_BASE gstreamer-base-1.0>=1.14)
    pkg_check_modules(GSTREAMER_VIDEO gstreamer-video-1.0>=1.14)
    pkg_check_modules(GSTREAMER_RTSP_SERVER gstreamer-rtsp-server-1.0)
    
    if(NOT GSTREAMER_FOUND)
        if(NOT DeepStream_FIND_QUIETLY)
            message(WARNING "GStreamer not found. DeepStream requires GStreamer >= 1.14")
        endif()
    endif()
    
    if(NOT GSTREAMER_RTSP_SERVER_FOUND)
        if(NOT DeepStream_FIND_QUIETLY)
            message(WARNING "GStreamer RTSP Server not found. Some DeepStream features may not work.")
        endif()
    endif()
else()
    if(NOT DeepStream_FIND_QUIETLY)
        message(WARNING "PkgConfig not found. Cannot verify GStreamer dependency.")
    endif()
endif()# CUDA is required for DeepStream
find_package(CUDAToolkit QUIET)
if(NOT CUDAToolkit_FOUND)
    if(NOT DeepStream_FIND_QUIETLY)
        message(WARNING "CUDA Toolkit not found. DeepStream requires CUDA.")
    endif()
endif()

# Define all DeepStream libraries to find with their dependencies
# Format: library_name -> list of dependencies
set(_DEEPSTREAM_LIBRARY_DEPENDENCIES
    "nvds_meta:"
    "nvds_batch_meta:nvds_meta"
    "nvdsgst_meta:nvds_meta"
    "nvdsgst_helper:nvdsgst_meta"
    "nvdsgst_customhelper:nvdsgst_meta"
    "nvdsgst_smartrecord:nvdsgst_meta"
    "nvds_utils:nvds_meta"
    "nvds_msgbroker:nvds_meta"
    "nvds_yml_parser:nvds_meta"
)

# Core libraries that must be found
set(_DEEPSTREAM_CORE_LIBRARIES
    nvdsgst_meta
    nvds_meta
)

# Find each library and create imported targets
foreach(_lib_dep ${_DEEPSTREAM_LIBRARY_DEPENDENCIES})
    string(REPLACE ":" ";" _lib_dep_list ${_lib_dep})
    list(GET _lib_dep_list 0 _lib)
    list(LENGTH _lib_dep_list _dep_count)

    if(_dep_count GREATER 1)
        list(GET _lib_dep_list 1 _deps)
        string(REPLACE "," ";" _deps_list "${_deps}")
    else()
        set(_deps_list "")
    endif()

    find_library(DeepStream_${_lib}_LIBRARY
        NAMES ${_lib}
        PATHS ${DeepStream_LIBRARY_DIRS}
        NO_DEFAULT_PATH
    )

    # Create imported target if library is found
    if(DeepStream_${_lib}_LIBRARY)
        list(APPEND DeepStream_LIBRARIES ${DeepStream_${_lib}_LIBRARY})

        if(NOT TARGET DeepStream::${_lib})
            add_library(DeepStream::${_lib} SHARED IMPORTED)

            # Build interface link libraries including dependencies
            set(_interface_libs "")
            foreach(_dep ${_deps_list})
                if(TARGET DeepStream::${_dep})
                    list(APPEND _interface_libs DeepStream::${_dep})
                endif()
            endforeach()

            # Add GStreamer if available
            if(GSTREAMER_FOUND)
                list(APPEND _interface_libs ${GSTREAMER_LIBRARIES})
            endif()
            
            # Add GStreamer RTSP Server if available
            if(GSTREAMER_RTSP_SERVER_FOUND)
                list(APPEND _interface_libs ${GSTREAMER_RTSP_SERVER_LIBRARIES})
            endif()

            # Add CUDA if available
            if(CUDAToolkit_FOUND)
                list(APPEND _interface_libs CUDA::cudart)
            endif()

            set_target_properties(DeepStream::${_lib} PROPERTIES
                IMPORTED_LOCATION "${DeepStream_${_lib}_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${DeepStream_INCLUDE_DIRS}"
                IMPORTED_NO_SONAME TRUE
            )

            # Set RPATH for the library
            set_property(TARGET DeepStream::${_lib} PROPERTY
                INSTALL_RPATH "${DeepStream_LIBRARY_DIRS}"
            )

            # Add interface link libraries if any
            if(_interface_libs)
                set_property(TARGET DeepStream::${_lib} PROPERTY
                    INTERFACE_LINK_LIBRARIES "${_interface_libs}"
                )
            endif()

            # Add GStreamer include directories if found
            if(GSTREAMER_FOUND)
                set_property(TARGET DeepStream::${_lib} APPEND PROPERTY
                    INTERFACE_INCLUDE_DIRECTORIES "${GSTREAMER_INCLUDE_DIRS}"
                )
            endif()
            
            # Add GStreamer RTSP Server include directories if found
            if(GSTREAMER_RTSP_SERVER_FOUND)
                set_property(TARGET DeepStream::${_lib} APPEND PROPERTY
                    INTERFACE_INCLUDE_DIRECTORIES "${GSTREAMER_RTSP_SERVER_INCLUDE_DIRS}"
                )
            endif()
        endif()

        mark_as_advanced(DeepStream_${_lib}_LIBRARY)
    endif()
endforeach()

# Verify core libraries are found
set(_CORE_LIBS_MISSING FALSE)
foreach(_core_lib ${_DEEPSTREAM_CORE_LIBRARIES})
    if(NOT DeepStream_${_core_lib}_LIBRARY)
        set(_CORE_LIBS_MISSING TRUE)
        if(NOT DeepStream_FIND_QUIETLY)
            message(WARNING "Core DeepStream library ${_core_lib} not found")
        endif()
    endif()
endforeach()

if(_CORE_LIBS_MISSING AND DeepStream_FIND_REQUIRED)
    message(FATAL_ERROR "Required core DeepStream libraries not found")
endif()

# Handle required components
set(_COMPONENTS_FOUND TRUE)
if(DeepStream_FIND_COMPONENTS)
    foreach(_comp ${DeepStream_FIND_COMPONENTS})
        if(NOT DeepStream_${_comp}_LIBRARY)
            set(_COMPONENTS_FOUND FALSE)
            set(DeepStream_${_comp}_FOUND FALSE)

            if(DeepStream_FIND_REQUIRED_${_comp})
                if(NOT DeepStream_FIND_QUIETLY)
                    message(SEND_ERROR "DeepStream REQUIRED component '${_comp}' not found")
                endif()
            else()
                if(NOT DeepStream_FIND_QUIETLY)
                    message(STATUS "Optional DeepStream component '${_comp}' not found")
                endif()
            endif()
        else()
            set(DeepStream_${_comp}_FOUND TRUE)
            if(NOT DeepStream_FIND_QUIETLY)
                message(STATUS "Found DeepStream component: ${_comp}")
            endif()
        endif()
    endforeach()

    # If components were specified, only mark as found if all required components are found
    if(NOT _COMPONENTS_FOUND)
        set(DeepStream_FOUND FALSE)
    endif()
endif()

# Standard package finding
include(FindPackageHandleStandardArgs)

# Build list of required variables based on whether components were specified
set(_REQUIRED_VARS DeepStream_ROOT_DIR DeepStream_INCLUDE_DIRS DeepStream_LIBRARY_DIRS)

# If specific components were requested, check them instead of all libraries
if(DeepStream_FIND_COMPONENTS)
    foreach(_comp ${DeepStream_FIND_COMPONENTS})
        if(DeepStream_FIND_REQUIRED_${_comp})
            list(APPEND _REQUIRED_VARS DeepStream_${_comp}_LIBRARY)
        endif()
    endforeach()
endif()

find_package_handle_standard_args(DeepStream
    REQUIRED_VARS ${_REQUIRED_VARS}
    VERSION_VAR DeepStream_VERSION
    HANDLE_COMPONENTS
)

# Set output variables and create convenience target
if(DeepStream_FOUND AND NOT _CORE_LIBS_MISSING)
    # Remove duplicates from library list
    if(DeepStream_LIBRARIES)
        list(REMOVE_DUPLICATES DeepStream_LIBRARIES)
    endif()

    # If components were specified, only include those in the convenience target
    if(DeepStream_FIND_COMPONENTS)
        set(_selected_targets "")
        foreach(_comp ${DeepStream_FIND_COMPONENTS})
            if(TARGET DeepStream::${_comp} AND DeepStream_${_comp}_FOUND)
                list(APPEND _selected_targets DeepStream::${_comp})
            endif()
        endforeach()
        set(_all_targets ${_selected_targets})
    else()
        # Build list of all available targets
        set(_all_targets "")
        foreach(_lib_dep ${_DEEPSTREAM_LIBRARY_DEPENDENCIES})
            string(REPLACE ":" ";" _lib_dep_list ${_lib_dep})
            list(GET _lib_dep_list 0 _lib)
            if(TARGET DeepStream::${_lib})
                list(APPEND _all_targets DeepStream::${_lib})
            endif()
        endforeach()
    endif()

    # Create interface target for convenience
    if(NOT TARGET DeepStream::DeepStream)
        add_library(DeepStream::DeepStream INTERFACE IMPORTED)

        set_target_properties(DeepStream::DeepStream PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${DeepStream_INCLUDE_DIRS}"
            INTERFACE_LINK_LIBRARIES "${_all_targets}"
        )

        # Add GStreamer to the interface target
        if(GSTREAMER_FOUND)
            set_property(TARGET DeepStream::DeepStream APPEND PROPERTY
                INTERFACE_INCLUDE_DIRECTORIES "${GSTREAMER_INCLUDE_DIRS}"
            )
            set_property(TARGET DeepStream::DeepStream APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES "${GSTREAMER_LIBRARIES}"
            )
        endif()
        
        # Add GStreamer RTSP Server to the interface target
        if(GSTREAMER_RTSP_SERVER_FOUND)
            set_property(TARGET DeepStream::DeepStream APPEND PROPERTY
                INTERFACE_INCLUDE_DIRECTORIES "${GSTREAMER_RTSP_SERVER_INCLUDE_DIRS}"
            )
            set_property(TARGET DeepStream::DeepStream APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES "${GSTREAMER_RTSP_SERVER_LIBRARIES}"
            )
        endif()

        # Set RPATH for executables linking against DeepStream
        set_property(TARGET DeepStream::DeepStream PROPERTY
            INTERFACE_LINK_DIRECTORIES "${DeepStream_LIBRARY_DIRS}"
        )
    endif()

    # Set RPATH variable for manual use
    set(DeepStream_RPATH "${DeepStream_LIBRARY_DIRS}")

    # Add to CMAKE_INSTALL_RPATH if building an executable
    list(APPEND CMAKE_INSTALL_RPATH "${DeepStream_LIBRARY_DIRS}")
    if(GSTREAMER_FOUND AND GSTREAMER_LIBRARY_DIRS)
        list(APPEND CMAKE_INSTALL_RPATH "${GSTREAMER_LIBRARY_DIRS}")
    endif()
else()
    set(DeepStream_FOUND FALSE)
endif()

# Mark cache variables as advanced
mark_as_advanced(
    DeepStream_ROOT_DIR
    DeepStream_VERSION_HEADER
    DeepStream_INCLUDE_DIRS
    DeepStream_LIBRARY_DIRS
    DeepStream_PLATFORM
)

# Print information if in verbose mode
if(DeepStream_FOUND AND NOT DeepStream_FIND_QUIETLY)
    message(STATUS "Found DeepStream: ${DeepStream_ROOT_DIR} (version ${DeepStream_VERSION})")
    message(STATUS "  Platform: ${DeepStream_PLATFORM}")
    message(STATUS "  Include dirs: ${DeepStream_INCLUDE_DIRS}")
    message(STATUS "  Library dirs: ${DeepStream_LIBRARY_DIRS}")

    if(DeepStream_FIND_COMPONENTS)
        message(STATUS "  Components requested: ${DeepStream_FIND_COMPONENTS}")
        set(_found_components "")
        set(_missing_components "")
        foreach(_comp ${DeepStream_FIND_COMPONENTS})
            if(DeepStream_${_comp}_FOUND)
                list(APPEND _found_components ${_comp})
            else()
                list(APPEND _missing_components ${_comp})
            endif()
        endforeach()
        if(_found_components)
            message(STATUS "  Components found: ${_found_components}")
        endif()
        if(_missing_components)
            message(STATUS "  Components missing: ${_missing_components}")
        endif()
    else()
        message(STATUS "  Libraries found: ${DeepStream_LIBRARIES}")
    endif()

    if(GSTREAMER_FOUND)
        message(STATUS "  GStreamer: ${GSTREAMER_VERSION}")
    endif()
    if(GSTREAMER_RTSP_SERVER_FOUND)
        message(STATUS "  GStreamer RTSP Server: ${GSTREAMER_RTSP_SERVER_VERSION}")
    endif()
    if(CUDAToolkit_FOUND)
        message(STATUS "  CUDA: ${CUDAToolkit_VERSION}")
    endif()
endif()
