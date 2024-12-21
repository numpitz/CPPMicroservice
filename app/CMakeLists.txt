add_executable(app)

set(APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_sources(app PRIVATE ${APP_SOURCES})

# Link to the base interface library
target_link_libraries(app PRIVATE 
    serviceA 
    nlohmann_json::nlohmann_json
    MsQuic::MsQuic
)

if(WIN32)
    target_link_libraries(app PRIVATE ws2_32) # Windows-specific linkage for dynamic loading
endif()


# Path to the directory containing msquic.dll
set(MSQUIC_DLL_DIR "${CMAKE_BINARY_DIR}/_deps/msquic-src/build/bin/Debug")

# Copy the DLL and PDB to the output directory
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${MSQUIC_DLL_DIR}/msquic.dll"
    $<TARGET_FILE_DIR:app>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${MSQUIC_DLL_DIR}/msquic.pdb"
    $<TARGET_FILE_DIR:app>
)
