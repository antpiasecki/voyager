#include "gemini.h"

namespace Gemini {
    Url parse_url(std::string data) {
        // strip whitespace
        data = data.substr(0, data.find('\r'));

        // extract schema
        auto index = data.find("://");
        if (index == std::string::npos) {
            return Url{};
        }
        std::string schema = data.substr(0, index);
        std::string without_schema = data.substr(index + 3);

        // extract hostname
        index = without_schema.find('/');
        if (index == std::string::npos) {
            return Url{data, schema, without_schema, "/"};
        } else {
            return Url{data, schema, without_schema.substr(0, index), without_schema.substr(index)};
        }
    }

    Response make_response(const Url &url) {
        if (url.raw.length() > 1024) {
            return Response{59, "URL length cannot exceed 1024 characters", ""};
        }

        if (url.schema.empty()) {
            return Response{59, "URL schema cannot be empty", ""};
        } else if (url.schema != "gemini") {
            return Response{53, "Proxy request denied", ""};
        }

        if (url.path == "/") {
            return Response{20, "text/gemini", "# Hello Gemini!\r\n"};
        } else {
            return Response{51, "Not found", ""};
        }
    }
}