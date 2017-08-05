#include "ciao/trie.h"
#include "ciao/utils.h"
#include "test/test_common.h"

TEST_F(TrieTest, ErrorRegexNodeDefineTest) {
    ciao::Trie trie;
    EXPECT_EXIT(trie.add_node("/a/:(^abc)"), testing::ExitedWithCode(1), "");
    EXPECT_EXIT(trie.add_node("/a/(^abc)"), testing::ExitedWithCode(1), "");
    EXPECT_EXIT(trie.add_node("/a/:abc(^abc"), testing::ExitedWithCode(1), "");
    EXPECT_EXIT(trie.add_node("/a/:abc^abc)"), testing::ExitedWithCode(1), "");
}

TEST_F(TrieTest, CorrectRegexNodeDefineTest) {
    ciao::Trie trie;
    ASSERT_NE(nullptr, trie.add_node("/a/:b(^abc)"));
    ASSERT_NE(nullptr, trie.add_node("/a/b/:c(^abc)"));
    auto n1 = trie.add_node("/a/:b(^abc)");
    ASSERT_STREQ("^abc", n1->regex.c_str());
}
