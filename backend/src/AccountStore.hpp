#pragma once
#include <string>
#include <unordered_map>
#include <mutex>

class AccountStore {
public:
    explicit AccountStore(const std::string& filename);
    ~AccountStore();

    // Операции
    bool createAccount(const std::string& id, double initialBalance);
    bool deposit(const std::string& id, double amount);
    bool withdraw(const std::string& id, double amount);
    bool transfer(const std::string& fromId, const std::string& toId, double amount);
    double getBalance(const std::string& id);

private:
    void load();   // загрузка из файла
    void save();   // сохранение в файл

    std::unordered_map<std::string, double> accounts_;
    std::mutex mutex_;
    std::string filename_;
};
