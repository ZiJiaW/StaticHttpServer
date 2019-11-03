#include "pch.h"
#include "basic.h"

namespace http {

namespace mime_types {

using namespace std;

unordered_map<string, string> mime_map = unordered_map<string, string>({
    {"gif", "image/gif"},
    {"jpg", "image/jpeg"},
    {"htm", "text/htm"},
    {"html", "text/html"},
    {"png", "image/png"},
    {"pdf", "application/pdf"},
    {"zip", "application/zip"},
    {"js", "text/javascript"},
    {"css", "text/css"},
    // 后续可考虑添加视频格式
});

string GetType(const string &extension) {
    if (mime_map.find(extension) != mime_map.end()) {
        return mime_map[extension];
    }
    return default_type;
}

string default_type = string("text/plain");


}// namespace mimetypes
}// namespace http