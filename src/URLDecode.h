#pragma once

#include <sstream>

std::string HEXCHARS = "1234567890ABCDEFabcdef";

static char _Decode(char fst, char snd) {
    unsigned int x;
    std::stringstream ss;
    std::string s{ fst, snd };
    ss << std::hex << s;
    ss >> x;
    return (char)x;
}
static bool _CheckInput(char x) {
    return HEXCHARS.find(x) < 0;
}
std::string URLDecode(std::string arg) {
    const size_t LEN = arg.length();
    std::string out = "";
    for (size_t i = 0; i < LEN; i++)
    {
        if (arg[i] != '%') {
            out += arg[i];
        }
        else {
            char fst, snd;
            if (_CheckInput(fst = arg[i + 1])) return out;
            if (_CheckInput(snd = arg[i + 2])) return out;
            out += _Decode(fst, snd);
            i += 2;
        }
    }
    return out;
}