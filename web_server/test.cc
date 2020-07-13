#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

int main()
{
    string str = "Content-Length: 30";
    size_t pos = str.find(": ");
    string key = str.substr(0, pos);
    string value = str.substr(pos+2);
    cout << "key: " << key << endl;
    cout << "value: " << value << endl;
    unordered_map<string, string> m;
    m.insert(make_pair<string, string>(string(key), string(value)));
    return 0;
}
