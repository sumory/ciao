include_directories(${PROJECT_SOURCE_DIR})
set(LIBRARIES ciao_static ${DEPENDENT_LIBRARIES})

add_executable(example_http_server http_server/http_server.cc)
target_link_libraries(example_http_server ${LIBRARIES})

