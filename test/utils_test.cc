#include "ciao/utils.h"
#include "test/test_common.h"

TEST_F(UtilsTest, GenRandomStrTest) {
    std::string a = gen_random_str();
    ASSERT_FALSE(a.empty());
}

TEST_F(UtilsTest, LowerUpperStrTest) {
    std::string a = "abcdefGhIjKlMn123";
    std::string upper_a = "ABCDEFGHIJKLMN123";
    std::string lower_a = "abcdefghijklmn123";
    ASSERT_STREQ(lower_a.c_str(), string_to_lower(a).c_str());
    ASSERT_STREQ(upper_a.c_str(), string_to_upper(a).c_str());
}

TEST_F(UtilsTest, TrimSpaceTest) {
    std::string ss[] = {"    abcd ", "abcd ", " abcd"};
    for (const auto& s : ss) {
        std::string result = trim_path_spaces(s);
        ASSERT_STREQ("abcd", result.c_str());
    }

    std::string ss2[] = {"    ab cd ", "ab cd ", " ab cd", " ab cd "};
    for (const auto& s : ss2) {
        std::string result = trim_path_spaces(s);
        ASSERT_STREQ("ab cd", result.c_str());
    }
}

TEST_F(UtilsTest, TrimPrefixSlashTest) {
    std::string ss[] = {"/abcd", "//abcd", "///abcd"};
    for (auto& s : ss) {
        std::string result = trim_prefix_slash(s);
        ASSERT_STREQ("abcd", result.c_str());
    }

    std::string ss2[] = {"abcd/", "/abcd/", "//abcd/"};
    for (const auto& s : ss2) {
        std::string result = trim_prefix_slash(s);
        ASSERT_STREQ("abcd/", result.c_str());
    }

    std::string ss3[] = {"ab c/d/", "/ab c/d/", "//ab c/d/"};
    for (const auto& s : ss3) {
        std::string result = trim_prefix_slash(s);
        ASSERT_STREQ("ab c/d/", result.c_str());
    }
    std::string ss4[] = {"/a/b/c/d/", "//a/b/c/d/", "///a/b/c/d/"};
    for (const auto& s : ss4) {
        std::string result = trim_prefix_slash(s);
        ASSERT_STREQ("a/b/c/d/", result.c_str());
    }
}

TEST_F(UtilsTest, StringSplitTest) {
    std::unordered_map<std::string, std::vector<std::string>> cases = {
        {"a/b/c/d", {"a", "b", "c", "d"}},
        {"1/2/3/4", {"1", "2", "3", "4"}},
        {"1/2/3/4/", {"1", "2", "3", "4", ""}},
        {"/a/b/c/d", {"", "a", "b", "c", "d"}}};
    for (auto& c : cases) {
        std::vector<std::string> result = string_split(c.first, '/');
        ASSERT_EQ(c.second.size(), result.size());
        for (size_t i = 0; i < result.size(); i++) {
            ASSERT_STREQ(result[i].c_str(), (c.second)[i].c_str());
        }
    }
}

TEST_F(UtilsTest, StrRegexMatchTest) {
    std::unordered_map<std::string, std::string> matched_cases = {{"abcdefg", "[a-z]+"},
                                                                  {"abcdefgABCDEFG", "[a-zA-Z]+"},
                                                                  {"1234567", "[1-7]+"},
                                                                  {".-~_", "[.\\-~_]+"}};
    for (auto& c : matched_cases) {
        bool result = is_str_match_regex(c.first, c.second);
        ASSERT_TRUE(result);
    }
}

TEST_F(UtilsTest, StringReplaceTest) {
    std::string s1 = "12212";
    std::string s11 = string_replace(s1, "12", "21");
    ASSERT_STREQ("21221", s11.c_str());

    std::string s2 = "12212";
    std::string s22 = string_replace_recursion(s2, "12", "21");
    ASSERT_STREQ("22211", s22.c_str());
}

TEST_F(UtilsTest, SlimPathTest) {
    std::unordered_map<std::string, std::string> matched_cases = {{"//abc", "/abc"},
                                                                  {"/", "/"},
                                                                  {"//", "/"},
                                                                  {"///", "/"},
                                                                  {"///abc", "/abc"},
                                                                  {"///a//b///c", "/a/b/c"},
                                                                  {"///a/b/c", "/a/b/c"},
                                                                  {"///a/b/c/", "/a/b/c/"},
                                                                  {"///ab/c", "/ab/c"}};
    for (auto& c : matched_cases) {
        std::string result = slim_path(c.first);
        ASSERT_STREQ(result.c_str(), c.second.c_str());
    }
}
