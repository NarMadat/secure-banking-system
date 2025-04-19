

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc != 5) {
        std::cerr << "Использование: " << argv[0]
                  << " <host> <port> <cert.pem> <key.pem>\n";
        return 1;
    }
    const char* host     = argv[1];
    int port             = std::stoi(argv[2]);
    const char* certFile = argv[3];
    const char* keyFile  = argv[4];

    
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        return 1;
    }
    
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);

    
    SSL* ssl = SSL_new(ctx);
    if (!ssl) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return 1;
    }

    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return 1;
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    inet_pton(AF_INET, host, &addr.sin_addr);

    if (connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        perror("connect");
        close(sock);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return 1;
    }

    
    SSL_set_fd(ssl, sock);
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        close(sock);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return 1;
    }
    std::cout << "Connected (TLS " << SSL_get_version(ssl) << ")\n";

    
    std::cout << "Введите команду (например, CREATE Alice 1000): ";
    std::string cmd;
    std::getline(std::cin, cmd);
    cmd += "\n";

    
    SSL_write(ssl, cmd.c_str(), cmd.size());

    char buf[1024];
    int n = SSL_read(ssl, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        std::cout << "Ответ сервера: " << buf;
    } else {
        std::cerr << "Ошибка чтения ответа\n";
    }

    
    SSL_shutdown(ssl);
    close(sock);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    return 0;
}
