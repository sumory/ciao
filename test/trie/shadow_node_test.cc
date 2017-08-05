#include "ciao/trie.h"
#include "ciao/utils.h"
#include "test/test_common.h"

TEST_F(TrieTest, ShadowNodeTest) {
    ciao::Trie trie;

    ciao::Node* level0_0_0 = trie.add_node("/00/0/:0");
    ciao::Node* level0_0_0_shadow = trie.add_node("/00/0/:0");
    ciao::Node* level0_0_0_shadow2 = trie.add_node("/00/0/:0");
    ASSERT_EQ(level0_0_0, level0_0_0_shadow);
    ASSERT_EQ(level0_0_0_shadow, level0_0_0_shadow2);
}
