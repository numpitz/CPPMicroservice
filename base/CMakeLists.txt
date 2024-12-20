# Define the base library as an interface library
add_library(base INTERFACE)

# Specify the output directory for .ifc files
set(CMAKE_IFC_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/modules")

# Set the include directory for the interface library
target_include_directories(base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

# Add IService.ixx explicitly to the interface library
set(SERVICE_MODULES
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/IService.ixx
)

target_sources(base INTERFACE ${SERVICE_MODULES})
