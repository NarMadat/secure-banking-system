#include "Server.hpp"
#include <iostream>

int main() {
    try {
        Server server(
            5500,
            "../ssl/server.crt",
            "../ssl/server.key"
        );
        std::cout << "Сервер запущен на порту 5500 (TLS)\n";
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
