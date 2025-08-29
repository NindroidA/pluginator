#pragma once
#include <string>

using namespace std;

struct Colors {
    static inline const string RED = "\033[0;31m";
    static inline const string GREEN = "\033[0;32m";
    static inline const string YELLOW = "\033[0;33m";
    static inline const string BLUE = "\033[0;34m";
    static inline const string PURPLE = "\033[0;35m";
    static inline const string CYAN = "\033[0;36m";
    static inline const string NC = "\033[0m";
};