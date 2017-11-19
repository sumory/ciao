file(GLOB ciao_unittest_SRCS *_test.cc app/*_test.cc group/*_test.cc node/*_test.cc trie/*_test.cc utils/*_test.cc)
include_directories(${PROJECT_SOURCE_DIR}/)

add_executable(ciao_unittest ${ciao_unittest_SRCS})
target_link_libraries(ciao_unittest ciao_static gtest gtest_main ${DEPENDENT_LIBRARIES})

include (CTest)
add_test(NAME ciao_unittest COMMAND ciao_unittest)
