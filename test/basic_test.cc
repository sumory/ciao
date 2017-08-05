#include "test/test_common.h"

TEST_F(CiaoTest, BasicTest) {
    int a = 1;
    int b = a;
    ASSERT_TRUE(a == b);
    int c = a + b;
    ASSERT_TRUE(c == a + b);
}

TEST_F(CiaoTest, BuildTimeTest) {
#ifdef INJECT_BUILD_DATE
    ASSERT_STREQ("1", ciao::CIAO_BUILD_TIME.c_str());
#else
    ASSERT_STREQ("0", ciao::CIAO_BUILD_TIME.c_str());
#endif
}

TEST_F(CiaoTest, SupporetdHttpMethodsTest) {
    ASSERT_TRUE(true == ciao::supported_http_methods["get"]);
    ASSERT_TRUE(true == ciao::supported_http_methods["post"]);
    ASSERT_TRUE(true == ciao::supported_http_methods["put"]);
    ASSERT_TRUE(true == ciao::supported_http_methods["delete"]);
    ASSERT_TRUE(true == ciao::supported_http_methods["all"]);
}
