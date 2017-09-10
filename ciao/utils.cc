#include "ciao/utils.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

const std::string get_current_datetime(time_t ts) {
    if (ts == 0) {
        ts = time(nullptr);
    }
    struct tm tm;
    localtime_r(&ts, &tm);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return buf;
}

int get_hour(time_t ts) {
    if (ts == 0) {
        ts = time(nullptr);
    }
    struct tm tm;
    localtime_r(&ts, &tm);
    return tm.tm_hour;
}

std::string gen_random_str(int start, int end) {
    srand((unsigned)time(NULL));
    int rand_int = (rand() % (end - start)) + start;
    return std::to_string(rand_int);
}

std::vector<std::string> string_split(const std::string& str, char delim) {
    std::vector<std::string> result;
    size_t start = std::string::npos;
    for (size_t i = 0; i <= str.length(); ++i) {
        if (start == std::string::npos) {
            start = i;
        }
        if (i == str.length() || str[i] == delim) {
            // skip if the start point itself is a delimiter
            result.emplace_back(str.c_str() + start, str.c_str() + i);
            start = std::string::npos;
        }
    }
    return result;
}

std::string string_to_lower(std::string src) {
    if (src.empty()) return "";
    std::string lower;
    lower.resize(src.size());
    transform(src.begin(), src.end(), lower.begin(), ::tolower);
    return lower;
}

std::string string_to_upper(std::string src) {
    if (src.empty()) return "";
    std::string upper;
    upper.resize(src.size());
    transform(src.begin(), src.end(), upper.begin(), ::toupper);
    return upper;
}

std::string trim_path_spaces(std::string s) {
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    s.erase(
        std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
        s.end());
    return s;
}

std::string trim_prefix_slash(std::string s) {
    std::function<bool(char)> is_slash = [](char x) { return x == '/'; };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(is_slash)));
    return s;
}

// TODO: optimize regex matche
bool is_str_match_regex(std::string str, std::string regex) {
    if (regex.empty()) {
        return false;
    }

    auto pattern = std::regex(regex);
    bool matched = std::regex_match(str, pattern);
    return matched;
}

std::unordered_map<std::string, std::string> mixin(std::unordered_map<std::string, std::string> a,
                                                   std::unordered_map<std::string, std::string> b) {
    for (auto& it : b) {
        a[it.first] = it.second;
    }
    return a;
}

std::string& string_replace_recursion(std::string& str, const std::string& old_value,
                                      const std::string& new_value) {
    while (true) {
        std::string::size_type pos(0);
        if ((pos = str.find(old_value)) != std::string::npos)
            str.replace(pos, old_value.length(), new_value);
        else
            break;
    }
    return str;
}

std::string& string_replace(std::string& str, const std::string& old_value,
                            const std::string& new_value) {
    for (std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length()) {
        if ((pos = str.find(old_value, pos)) != std::string::npos)
            str.replace(pos, old_value.length(), new_value);
        else
            break;
    }
    return str;
}

std::string slim_path(std::string path) {
    if (path.empty()) return path;
    std::string result = string_replace_recursion(path, "//", "/");
    return result;
}

bool get_avaliable_port(uint32_t& port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = 0;

    int ret = ::bind(sock, (struct sockaddr*)&addr, sizeof addr);
    if (0 != ret) {
        close(sock);
        return false;
    }

    struct sockaddr_in connAddr;
    socklen_t len = sizeof connAddr;
    ret = getsockname(sock, (struct sockaddr*)&connAddr, &len);

    if (0 != ret) {
        close(sock);
        return false;
    }

    port = ntohs(connAddr.sin_port);
    return true;
}