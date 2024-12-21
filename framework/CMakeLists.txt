add_library(framework STATIC)

target_sources(framework PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Framework_Module.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Framework_Module.cpp
)

target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)