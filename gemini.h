#pragma once

#include <string>

namespace Gemini {
    struct Url {
        std::string raw;
        std::string schema;
        std::string hostname;
        std::string path;
    };

    struct Response {
        int status;
        std::string meta;
        std::string body;
    };

    Url parse_url(std::string data);

    Response make_response(const Url &url);
}