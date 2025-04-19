// backend/src/Server.hpp
#pragma once

#include "ThreadPool.hpp"
#include "AccountStore.hpp"

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <string>

class Server
{
public:

    // port — порт для прослушивания
    // certFile, keyFile — пути к .crt и .key
    Server(int port,
           const std::string &certFile,
           const std::string &keyFile);
    ~Server();

    // Запустить цикл приёма клиентов
    void run();

private:
    int server_fd_;      // файловый дескриптор слушающего сокета
    ThreadPool pool_;    // пул рабочих потоков
    AccountStore store_; // хранилище аккаунтов
    SSL_CTX *sslCtx_;    // контекст OpenSSL

    // Инициализация OpenSSL
    void initSSL(const std::string &certFile,
                 const std::string &keyFile);

    // Цикл приёма и распределения соединений
    void acceptClients();

    // Обработка одного клиента по SSL
    void handleClientSSL(SSL *ssl);
};
