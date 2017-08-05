#include "ciao/node.h"
#include "test/test_common.h"

TEST_F(CiaoTest, NodeBasicTest) {
    ciao::Node node(false);

    ASSERT_FALSE(node.is_root);
    ASSERT_TRUE(node.check_method("get"));
    ASSERT_TRUE(node.check_method("post"));
    ASSERT_TRUE(node.check_method("put"));
    ASSERT_TRUE(node.check_method("delete"));

    ASSERT_FALSE(node.check_method("GET"));
    ASSERT_FALSE(node.check_method("Get"));

    ASSERT_FALSE(node.check_method("UNKNOWN"));
}
