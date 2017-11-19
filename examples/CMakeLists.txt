include_directories(${PROJECT_SOURCE_DIR})
set(LIBRARIES ciao_static ${DEPENDENT_LIBRARIES})

add_executable(example_simple_server http_server/simple_server.cc)
target_link_libraries(example_simple_server ${LIBRARIES})

add_executable(example_middleware_server http_server/middleware_server.cc)
target_link_libraries(example_middleware_server ${LIBRARIES})

add_executable(example_group_router group_router/group_router.cc)
target_link_libraries(example_group_router ${LIBRARIES})