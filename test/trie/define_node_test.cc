#include "ciao/node.h"
#include "ciao/trie.h"
#include "ciao/utils.h"
#include "test/test_common.h"

TEST_F(TrieTest, WrongPatternWithSlashTest) {
    ciao::Trie trie;
    std::string cases[] = {"//", "///", "/a/b//", "//a/b/", "/a//b/", "/a///b/"};
    for (const auto& s : cases) {
        EXPECT_EXIT(trie.add_node(s), testing::ExitedWithCode(1), "");
    }
}

TEST_F(TrieTest, WrongPatternWithSpaceTest) {
    ciao::Trie trie;
    std::string cases[] = {" / / ", "/ /"};
    for (const auto& s : cases) {
        EXPECT_EXIT(trie.add_node(s), testing::ExitedWithCode(1), "");
    }
}

TEST_F(TrieTest, SpaceShouldBeTrimedTest) {
    ciao::Trie trie;
    ciao::Node* n1 = trie.add_node("/");
    ciao::Node* n2 = trie.add_node(" ");
    ciao::Node* n3 = trie.add_node(" /");
    ciao::Node* n4 = trie.add_node(" / ");
    ASSERT_EQ(n1, n2);
    ASSERT_EQ(n2, n3);
    ASSERT_EQ(n3, n4);
}

TEST_F(TrieTest, CorrectPatternTest) {
    ciao::Trie trie;
    std::string cases[] = {" ", "", "/a", "/a/b", "/a/b/c"};
    for (const auto& s : cases) {
        ASSERT_NE(nullptr, trie.add_node(s));
    }
}

TEST_F(TrieTest, PatternWithSameNodeTest) {
    ciao::Trie trie;
    ciao::Node* n1 = trie.add_node("/");
    ciao::Node* n2 = trie.add_node("");
    ASSERT_NE(nullptr, n1);
    ASSERT_EQ(n1, n2);
    ASSERT_TRUE(n1 == n2);

    ciao::Node* n3 = trie.add_node("/");
    ciao::Node* n4 = trie.add_node("");
    ASSERT_EQ(n1, n2);
    ASSERT_EQ(n2, n3);
    ASSERT_EQ(n3, n4);

    ciao::Node* n5 = trie.add_node("/abc");
    ciao::Node* n6 = trie.add_node("abc");
    ASSERT_EQ(n5, n6);

    ciao::Node* n7 = trie.add_node("/:abc");
    ciao::Node* n8 = trie.add_node(":abc");
    ASSERT_EQ(n7, n8);
}

TEST_F(TrieTest, WrongPathTest) {
    ciao::Trie trie;
    std::string cases[] = {"/#+", ":+abc", ":&abc", ":abc*"};
    for (const auto& s : cases) {
        EXPECT_EXIT(trie.add_node(s), testing::ExitedWithCode(1), "");
    }
}

TEST_F(TrieTest, CorrectPathTest) {
    ciao::Trie trie;
    std::string cases[] = {"abc", "/abc", "/abc", ":abc", "/:abc"};
    for (const auto& s : cases) {
        ASSERT_NE(nullptr, trie.add_node(s));
    }
}

TEST_F(TrieTest, NodeNameTest) {
    ciao::Trie trie;
    ciao::Node* n1 = trie.add_node("/");
    ASSERT_STREQ("", n1->name.c_str());
}