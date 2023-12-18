#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netinet/in.h>
#include <unistd.h>
#include <bits/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <csignal>
#include <sstream>
#include <thread>
#include <vector>
#include "util.h"
#include "gemini.h"

SSL_CTX *init_ssl() {
    signal(SIGPIPE, SIG_IGN);

    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    if (SSL_CTX_use_certificate_file(ctx, "data/cert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "data/key.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }
    return ctx;
}

std::string receive_request(SSL *ssl) {
    std::string req;

    while (!req.ends_with("\r\n")) {
        char buffer[CHUNK_SIZE] = {0};
        int n = SSL_read(ssl, buffer, CHUNK_SIZE);
        if (n == -1) {
            break;
        }
        req += std::string(buffer, n);
    }

    return req;
}

void handle_connection(SSL_CTX *ctx, int c, struct sockaddr_in addr) {
    std::cout << "Connection from " << inet_ntoa(addr.sin_addr) << "!" << std::endl;

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, c);

    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
    } else {
        auto url = Gemini::parse_url(receive_request(ssl));
        auto response = Gemini::make_response(url);

        std::stringstream resp_stream;
        resp_stream << response.status << " " << response.meta << "\r\n";
        resp_stream << response.body;
        auto resp_data = resp_stream.str();

        SSL_write(ssl, resp_data.c_str(), (int) resp_data.length());
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(c);
}

int main() {
    auto *ctx = init_ssl();

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1965);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(s, 1) < 0) {
        perror("listen");
        exit(1);
    }

    std::vector<std::thread> threads;
    std::cout << "Listening..." << std::endl;
    while (true) {
        struct sockaddr_in client_addr{};
        unsigned int len = sizeof(client_addr);

        int client = accept(s, (struct sockaddr *) &client_addr, &len);
        if (client < 0) {
            perror("accept");
            exit(1);
        }

        std::thread t(handle_connection, ctx, client, client_addr);
        threads.push_back(std::move(t));
    }
}