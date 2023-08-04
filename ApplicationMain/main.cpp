#include <iostream>
#include <vector>
#include <array>
#include <memory>
#include <thread>
#include <filesystem>
#include <fstream>
#include <unordered_map>

#include "range_tree/range_tree.h"

#include "SimpleWebServer/server_http.hpp"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
namespace fs = std::filesystem;

inline std::string ui_path = "../../build";


std::unordered_map<std::string, std::string> mime_types = {
        {"css", "text/css"},
        {"html", "text/html"},
        {"js", "application/javascript"},
        {"svg", "image/svg+xml"},
        {"ico", "image/vnd.microsoft.icon"},
        {"json", "application/json"},
        {"txt", "text/plain"},
        {"png", "image/png"},
};


bool is_valid_file(const std::string& p)
{
    std::string concat = ui_path + p;

    return fs::exists(concat) and fs::is_regular_file(concat);
}


std::string get_content(const std::string& p)
{
    if (not is_valid_file(p))
    {
        std::cout << "file not found" << std::endl;
        return "HTTP/1.1 404 NOT FOUND\r\nContent-Length: 0\r\n\r\n";
    }

    std::string concat = ui_path + p;

    // Read the file into a string
    std::ifstream file_stream(concat);
    if (!file_stream)
        exit(1);

    std::string file_contents((std::istreambuf_iterator<char>(file_stream)),
            std::istreambuf_iterator<char>());

    // Close the file
    file_stream.close();

    int last = 0;

    for (int i = p.size() - 1; i >= 0; --i)
    {
        if (p[i] == '.')
        {
            last = i;
            break;
        }
    }

    std::string file_extension = p.substr(last + 1);

    std::string file_mime_type = mime_types[file_extension];

    return "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(file_contents.length()) + "\r\nContent-Type: " + file_mime_type + "\r\n\r\n" + file_contents;
}


int main()
{
    HttpServer server;
    server.config.port = 8080;

    server.resource["^/static/css/[^\\/]+$"]["GET"] = [] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string p = request->path;

        *response << get_content(p);
    };

    server.resource["^/static/js/[^\\/]+$"]["GET"] = [] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string p = request->path;

        *response << get_content(p);
    };

    server.resource["^/static/media/[^\\/]+$"]["GET"] = [] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string p = request->path;

        *response << get_content(p);
    };

    server.resource["^/[^\\/]+$"]["GET"] = [] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string p = request->path;

        std::cout << "HI" << std::endl;

        *response << get_content(p);
    };

    server.resource["^/data/[^\\/]+$"]["GET"] = [] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string p = request->path;

        std::cout << "HI" << std::endl;

        *response << get_content(p);
    };

    std::thread server_thread([&server]() {
        server.start();
    });

    server_thread.join();

    return 0;
}
