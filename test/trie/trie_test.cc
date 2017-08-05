#include "ciao/trie.h"
#include "ciao/utils.h"
#include "test/test_common.h"

TEST_F(TrieTest, TrieCtorTest) {
    ciao::Trie trie;
    ASSERT_TRUE(trie._ignore_case);
    ASSERT_TRUE(trie._strict_route);
    ASSERT_STRNE("", trie._segment_pattern.c_str());
    ASSERT_EQ(100, trie._max_uri_segments);
    ASSERT_EQ(100, trie._max_fallback_depth);

    ciao::Trie trie1(true);
    ASSERT_TRUE(trie1._ignore_case);
    ASSERT_TRUE(trie1._strict_route);
    ASSERT_EQ(100, trie1._max_uri_segments);
    ASSERT_EQ(100, trie1._max_fallback_depth);

    ciao::Trie trie2(false);
    ASSERT_FALSE(trie2._ignore_case);
    ASSERT_TRUE(trie2._strict_route);

    ciao::Trie trie3(false, false);
    ASSERT_FALSE(trie3._ignore_case);
    ASSERT_FALSE(trie3._strict_route);

    ciao::Trie trie4(true, false, "abcdefg", 123, 456);
    ASSERT_TRUE(trie4._ignore_case);
    ASSERT_FALSE(trie4._strict_route);
    ASSERT_STREQ("abcdefg", trie4._segment_pattern.c_str());
    ASSERT_EQ(123, trie4._max_uri_segments);
    ASSERT_EQ(456, trie4._max_fallback_depth);
}

TEST_F(TrieTest, URIPatternMatchTest) {
    ciao::Trie trie;
    std::string segments[] = {"a", "abc", "abc0123", "abc_~.-", "a_b_c~d.e-f"};
    for (const auto& s : segments) {
        bool matched = trie._check_segment(s);
        ASSERT_TRUE(matched);
    }
}

TEST_F(TrieTest, URIPatternNotMatchTest) {
    ciao::Trie trie;
    std::string segments[] = {"/", "/abc", "%", "\\", "中文"};
    for (const auto& s : segments) {
        bool matched = trie._check_segment(s);
        ASSERT_FALSE(matched);
    }
}
