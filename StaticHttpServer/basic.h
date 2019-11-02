#pragma once
#include "pch.h"

namespace http {

// http��Ϣ��������
namespace mime_types {

using namespace std;

auto mime_map = unordered_map<string, string>({
    {"gif", "image/gif"},
    {"jpg", "image/jpeg"},
    {"htm", "text/htm"},
    {"html", "text/html"},
    {"png", "image/png"},
    {"pdf", "application/pdf"},
    {"zip", "application/zip"},
    {"js", "text/javascript"},
    {"css", "text/css"},
    // �����ɿ��������Ƶ��ʽ
});

string default_type("text/plain");

string GetType(const string &extension) {
    if (mime_map.find(extension) != mime_map.end()) {
        return mime_map[extension];
    }
    return default_type;
}

}// namespace mime_types
}// namespace http