#pragma once

#include "ThreadPool.hpp"
#include "AccountStore.hpp"

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <string>

class Server
{
public:

    
    
    Server(int port,
           const std::string &certFile,
           const std::string &keyFile);
    ~Server();

    
    void run();

private:
    int server_fd_;      
    ThreadPool pool_;    
    AccountStore store_; 
    SSL_CTX *sslCtx_;    

    
    void initSSL(const std::string &certFile,
                 const std::string &keyFile);

    
    void acceptClients();

    
    void handleClientSSL(SSL *ssl);
};
