#pragma once

#include <iostream>
#include <string>
#include <unorder_map>

class Util
{
public:
    static void MakeKV(std::unordered_map<std::string, std::string> &header_kv, std::string &sub)
    {
        std::size_t pos = sub.find(": ");
        if (std::string::npos == pos)
        {
            return;
        }
        string key = sub.substr(0, pos);
        string value = sub.substr(pos+2);
        header_kv.insert({key, value});
    }
}
