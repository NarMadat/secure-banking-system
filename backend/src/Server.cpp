#include "Server.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <algorithm>

#include <openssl/ssl.h>
#include <openssl/err.h>

Server::Server(int port,
               const std::string& certFile,
               const std::string& keyFile)
  : pool_(4),
    store_("accounts.db"),
    sslCtx_(nullptr)
{

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    initSSL(certFile, keyFile);


    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0)
        throw std::runtime_error(std::string("socket() failed: ") + std::strerror(errno));


    int opt = 1;
    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error(std::string("setsockopt() failed: ") + std::strerror(errno));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
        throw std::runtime_error(std::string("bind() failed: ") + std::strerror(errno));

    if (listen(server_fd_, 10) < 0)
        throw std::runtime_error(std::string("listen() failed: ") + std::strerror(errno));

    std::cout << "Server listening on port " << port << " (TLS)\n";
}

Server::~Server() {
    close(server_fd_);
    if (sslCtx_) SSL_CTX_free(sslCtx_);
}

void Server::initSSL(const std::string& certFile,
                     const std::string& keyFile)
{
    sslCtx_ = SSL_CTX_new(TLS_server_method());
    if (!sslCtx_)
        throw std::runtime_error("SSL_CTX_new failed");

    if (SSL_CTX_use_certificate_file(sslCtx_,
                                     certFile.c_str(),
                                     SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Loading certificate failed");
    }

    if (SSL_CTX_use_PrivateKey_file(sslCtx_,
                                    keyFile.c_str(),
                                    SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Loading private key failed");
    }
}

void Server::run() {
    acceptClients();
}

void Server::acceptClients() {
    while (true) {
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd < 0) {
            std::cerr << "accept() failed: " << std::strerror(errno) << "\n";
            continue;
        }


        SSL* ssl = SSL_new(sslCtx_);
        SSL_set_fd(ssl, client_fd);


        pool_.enqueue([this, ssl, client_fd]() {
            if (SSL_accept(ssl) <= 0) {
                ERR_print_errors_fp(stderr);
            } else {
                handleClientSSL(ssl);
            }
            SSL_shutdown(ssl);
            SSL_free(ssl);
            close(client_fd);
        });
    }
}

void Server::handleClientSSL(SSL* ssl) {
    char tmp[1024];
    int n = SSL_read(ssl, tmp, sizeof(tmp) - 1);
    if (n <= 0) return;
    tmp[n] = '\0';


    std::string buf(tmp);
    buf.erase(std::remove(buf.begin(), buf.end(), '\r'), buf.end());
    if (!buf.empty() && buf.back() == '\n')
        buf.pop_back();

    std::cout << ">> Received raw command: [" << buf << "]\n";


    std::istringstream iss(buf);
    std::string cmd, id, toId;
    double amount;
    std::string response;

    iss >> cmd;
    if (cmd == "CREATE") {
        iss >> id >> amount;
        response = store_.createAccount(id, amount) ? "OK\n" : "ERROR\n";
    }
    else if (cmd == "DEPOSIT") {
        iss >> id >> amount;
        response = store_.deposit(id, amount) ? "OK\n" : "ERROR\n";
    }
    else if (cmd == "WITHDRAW") {
        iss >> id >> amount;
        response = store_.withdraw(id, amount) ? "OK\n" : "ERROR\n";
    }
    else if (cmd == "TRANSFER") {
        iss >> id >> toId >> amount;
        response = store_.transfer(id, toId, amount) ? "OK\n" : "ERROR\n";
    }
    else if (cmd == "BALANCE") {
        iss >> id;
        double bal = store_.getBalance(id);
        response = "BALANCE " + std::to_string(bal) + "\n";
    }
    else {
        response = "UNKNOWN COMMAND\n";
    }

    SSL_write(ssl, response.c_str(), response.size());
}
