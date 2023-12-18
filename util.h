#pragma once

#include <string>

#define CHUNK_SIZE 1024

inline std::string strip_slashes(std::string s) {
    while (s.starts_with("/")) {
        s = s.substr(1);
    }
    return s;
}