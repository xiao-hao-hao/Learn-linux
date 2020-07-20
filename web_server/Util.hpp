#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>

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
        std::string key = sub.substr(0, pos);
        std::string value = sub.substr(pos+2);
        header_kv.insert({key, value});
    }

    static int StringToInt(std::string &str)
    {
        std::stringstream ss(str);
        int len = 0;
        ss >> len;
        return len;
    }
    static std::string IntToString(int num)
    {
        std::stringstream ss;
        ss << num;
        return ss.str();
    }
    static std::string SuffixToType(std::string &suffix)
    {
        if (suffix == ".html" || suffix == ".htm")
        {
            return "text/html";
        }
        else if (suffix == ".js")
        {
            return "application/x-javascript";
        }
        else if (suffix == ".css")
        {
            return "text/css";
        }
        else if (suffix == ".jpg")
        {
            return "application/x-jpg";
        }
        else
        {
            return "text/html";
        }
    }
};
