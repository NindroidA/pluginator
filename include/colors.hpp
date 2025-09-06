#pragma once
#include <string>

using namespace std;

struct Colors {
    static inline const string NC = "\033[0m";
    static inline const string CRIMSON = "\033[38;5;160m";
    static inline const string RED = "\033[0;31m";
    static inline const string ORANGE = "\033[38;5;208m";
    static inline const string YELLOW = "\033[0;33m";
    static inline const string GREEN = "\033[0;32m";
    static inline const string SOFT_GREEN = "\033[38;5;114m";
    static inline const string CYAN = "\033[0;36m";
    static inline const string BRIGHT_BLUE = "\033[38;5;81m";
    static inline const string BLUE = "\033[38;5;27m";
    static inline const string LIGHT_PURPLE = "\033[0;34m";
    static inline const string PURPLE = "\033[38;5;93m";
    static inline const string PINK = "\033[0;35m";
    static inline const string BRIGHT_WHITE = "\033[1;37m";
    static inline const string DIM_WHITE = "\033[2;37m";
    static inline const string LIGHT_GRAY = "\033[38;5;248m";
};