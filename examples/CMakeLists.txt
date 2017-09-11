include_directories(${PROJECT_SOURCE_DIR})
set(LIBRARIES ciao_static ${DEPENDENT_LIBRARIES})

add_executable(example_simple_server http_server/simple_server.cc)
target_link_libraries(example_simple_server ${LIBRARIES})

add_executable(example_middeware_server http_server/middleware_server.cc)
target_link_libraries(example_middeware_server ${LIBRARIES})

