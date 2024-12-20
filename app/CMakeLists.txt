add_executable(app)

set(APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_sources(app PRIVATE ${APP_SOURCES})

# Link to the base interface library
target_link_libraries(app PRIVATE serviceA)

if(WIN32)
    target_link_libraries(app PRIVATE ws2_32) # Windows-specific linkage for dynamic loading
endif()