# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Server")

# Enable C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")
include(CPM)
cpmaddpackage("gh:nlohmann/json#v3.11.3")
cpmaddpackage("gh:microsoft/msquic#v2.4.7")
cpmaddpackage("gh:oatpp/oatpp#1.3.1")

# Define MsQuic paths
set(MSQUIC_INCLUDE_DIR "${CMAKE_BINARY_DIR}/_deps/msquic-src/src/inc")
set(MSQUIC_LIB_DIR "${CMAKE_BINARY_DIR}/_deps/msquic-src/build/obj/Debug")

# Add MsQuic library manually
add_library(MsQuic SHARED IMPORTED)
set_target_properties(MsQuic PROPERTIES
    IMPORTED_LOCATION "${MSQUIC_LIB_DIR}/msquic.lib"       # Path to the .lib file
    IMPORTED_IMPLIB "${MSQUIC_LIB_DIR}/msquic.lib"         # Import library
    INTERFACE_INCLUDE_DIRECTORIES "${MSQUIC_INCLUDE_DIR}"  # Include directory
)

# Alias MsQuic target for consistency
add_library(MsQuic::MsQuic ALIAS MsQuic)

# Add subdirectories
add_subdirectory(base)
add_subdirectory(services)
add_subdirectory(app)
