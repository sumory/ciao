#include "ciao/trie.h"
#include "ciao/utils.h"
#include "test/test_common.h"

TEST_F(TrieTest, ParentChildren1Test) {
    ciao::Trie trie;
    ciao::Node* node = trie.add_node("/a/b");
    ASSERT_STREQ("", node->name.c_str());
    ASSERT_STREQ("/a/b", node->pattern.c_str());

    ASSERT_EQ(node, trie.add_node("/a/b"));
    ASSERT_NE(node, trie.add_node("a/b/"));
    ASSERT_NE(node, trie.add_node("/a/b/"));
    ASSERT_EQ(trie.add_node("/a/b/"), trie.add_node("a/b/"));

    ciao::Node* parent = trie.add_node("/a");
    ASSERT_EQ(node->parent, parent);
    ASSERT_EQ(parent->find_child("b"), node);
    ciao::Node* child = trie.add_node("/a/b/c");
    ASSERT_EQ(child->parent, node);
    ASSERT_EQ(node->find_child("c"), child);

    EXPECT_EXIT(trie.add_node("/a//b"), testing::ExitedWithCode(1), "");
}

TEST_F(TrieTest, ParentChildren2Test) {
    ciao::Trie trie;
    ciao::Node* root = trie.root;

    ciao::Node* slash_level = trie.add_node("/");
    ciao::Node* level0 = trie.add_node("/00");
    ciao::Node* level1 = trie.add_node("/01");
    ciao::Node* level2 = trie.add_node("/02");

    ciao::Node* level0_0 = trie.add_node("/00/0");
    ciao::Node* level0_1 = trie.add_node("/00/1");
    trie.add_node("/00/2");

    ciao::Node* level1_0 = trie.add_node("/01/0");
    ciao::Node* level1_1 = trie.add_node("/01/1");
    trie.add_node("/01/2");

    trie.add_node("/02/0");
    trie.add_node("/02/1");
    trie.add_node("/02/2");

    ciao::Node* level0_0_0 = trie.add_node("/00/0/0");
    trie.add_node("/00/0/1");
    trie.add_node("/00/0/2");

    ASSERT_STREQ("", root->name.c_str());
    ASSERT_STREQ("", root->pattern.c_str());

    ASSERT_STREQ("", slash_level->name.c_str());
    ASSERT_STREQ("/", slash_level->pattern.c_str());
    ASSERT_STREQ("", level0->name.c_str());
    ASSERT_STREQ("/00", level0->pattern.c_str());

    ASSERT_EQ(slash_level->parent, level1->parent);
    ASSERT_EQ(level0->parent, level1->parent);
    ASSERT_EQ(level1->parent, level2->parent);

    ASSERT_EQ(root, level0->parent);
    ASSERT_EQ(root, level1->parent);
    ASSERT_EQ(root, level2->parent);

    ASSERT_EQ(level0, level0_0->parent);
    ASSERT_EQ(level0_0->parent, level0_1->parent);
    ASSERT_EQ(level1, level1_0->parent);
    ASSERT_EQ(level1_0->parent, level1_1->parent);

    ASSERT_EQ(root, level0_0_0->parent->parent->parent);
    ASSERT_EQ(level0, level0_0_0->parent->parent);
}
