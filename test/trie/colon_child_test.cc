#include "ciao/trie.h"
#include "ciao/utils.h"
#include "test/test_common.h"

TEST_F(TrieTest, FailToDefineConflictNodeTest) {
    ciao::Trie trie;

    trie.add_node("/");
    trie.add_node("/00");
    trie.add_node("/00/0");
    trie.add_node("/00/0/:0");

    EXPECT_EXIT(trie.add_node("/00/0/:1"), testing::ExitedWithCode(1), "");
    EXPECT_EXIT(trie.add_node("/00/0/:01"), testing::ExitedWithCode(1), "");

    ASSERT_NE(nullptr, trie.add_node("/00/0/absolute"));
}

TEST_F(TrieTest, SucceedToDefineNotConflictNodeTest) {
    ciao::Trie trie;

    trie.add_node("/");
    trie.add_node("/00");
    trie.add_node("/00/0");
    trie.add_node("/00/0/:0");

    ASSERT_NE(nullptr, trie.add_node("/00/0/absolute"));
    ASSERT_NE(nullptr, trie.add_node("/00/0/123"));
    ASSERT_NE(nullptr, trie.add_node("/00/0/123/456"));
}