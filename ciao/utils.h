#pragma once
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// get current date/time, format is YYYY-MM-DD HH:mm:ss
const std::string get_current_datetime(time_t ts = 0);
int get_hour(time_t ts = 0);
std::string gen_random_str(int start = 0, int end = 100000);
std::vector<std::string> string_split(const std::string& str, char delim);
std::string string_to_lower(std::string src);
std::string string_to_upper(std::string src);
std::string trim_path_spaces(std::string pattern);
std::string trim_prefix_slash(std::string pattern);
bool is_str_match_regex(std::string str, std::string regex);
std::unordered_map<std::string, std::string> mixin(std::unordered_map<std::string, std::string> a,
                                                   std::unordered_map<std::string, std::string> b);
std::string& string_replace(std::string& str, const std::string& old_value,
                            const std::string& new_value);
std::string& string_replace_recursion(std::string& str, const std::string& old_value,
                                      const std::string& new_value);
std::string slim_path(std::string path);

bool get_avaliable_port(uint32_t& port);