add_library(serviceA SHARED)

# Explicitly list module and source files
set(SERVICE_MODULES
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/ServiceA_Module.ixx
)

set(SERVICE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ServiceA_Impl.cpp
)

target_sources(serviceA PRIVATE ${SERVICE_SOURCES} PUBLIC ${SERVICE_MODULES})

# Link to the base interface library
target_link_libraries(serviceA PUBLIC base)

# Export symbols for shared library
set_target_properties(serviceA PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

