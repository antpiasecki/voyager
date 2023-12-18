#include <regex>
#include <filesystem>
#include <fstream>
#include "gemini.h"
#include "util.h"

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

        if (url.hostname.contains(":")) {
            auto port = url.hostname.substr(url.hostname.find(':') + 1);
            if (port != "1965") {
                return Response{53, "Proxy request denied", ""};
            }
        }

        if (url.schema.empty()) {
            return Response{59, "URL schema cannot be empty", ""};
        } else if (url.schema != "gemini") {
            return Response{53, "Proxy request denied", ""};
        }

        auto clean_path = std::regex_replace(url.path, std::regex("\\.\\."), "");
        if (clean_path == "/")
            clean_path = "/index.gmi";

        // TODO: check if it's actually secure
        clean_path = strip_slashes(clean_path);

        auto absolute_path = std::filesystem::current_path() / "data" / "public" / clean_path;

        if (std::filesystem::exists(absolute_path)) {
            if (std::filesystem::is_regular_file(absolute_path)) {
                std::ifstream file(absolute_path);
                std::stringstream buffer;
                buffer << file.rdbuf();
                file.close();

                // TODO: meta
                return Response{20, "text/gemini", buffer.str()};
            } else {
                // TODO
                return Response{51, "Not found", ""};
            }
        } else {
            return Response{51, "Not found", ""};
        }
    }
}