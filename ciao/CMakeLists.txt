file(GLOB ciao_SRCS *.cc */*.cc *.h */*.h *.cpp */*.hpp)
file(GLOB ciao_PUBLIC_HEADERS *.h *.hpp)

message(STATUS "ciao_SRCS : " ${ciao_SRCS})

include_directories(${PROJECT_SOURCE_DIR})

add_library(ciao_static STATIC ${ciao_SRCS})
target_link_libraries(ciao_static ${DEPENDENT_LIBRARIES})

add_library(ciao SHARED ${ciao_SRCS})
target_link_libraries(ciao ${DEPENDENT_LIBRARIES})

install (
    TARGETS ciao ciao_static
    EXPORT ${PACKAGE_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)


install (FILES ${ciao_PUBLIC_HEADERS} DESTINATION "include/ciao")