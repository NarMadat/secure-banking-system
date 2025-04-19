#include "AccountStore.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

AccountStore::AccountStore(const std::string& filename)
  : filename_(filename)
{
    load();
}

AccountStore::~AccountStore() {
    save();
}

void AccountStore::load() {
    std::ifstream in(filename_);
    if (!in.is_open()) return;  

    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string id;
        double balance;
        if (iss >> id >> balance) {
            accounts_[id] = balance;
        }
    }
}

void AccountStore::save() {
    std::ofstream out(filename_, std::ios::trunc);
    for (auto& [id, balance] : accounts_) {
        out << id << " " << balance << "\n";
    }
}

bool AccountStore::createAccount(const std::string& id, double initialBalance) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (accounts_.count(id)) return false; 
    accounts_[id] = initialBalance;
    save();
    return true;
}

bool AccountStore::deposit(const std::string& id, double amount) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = accounts_.find(id);
    if (it == accounts_.end() || amount < 0) return false;
    it->second += amount;
    save();
    return true;
}

bool AccountStore::withdraw(const std::string& id, double amount) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = accounts_.find(id);
    if (it == accounts_.end() || amount < 0 || it->second < amount)
        return false;
    it->second -= amount;
    save();
    return true;
}

bool AccountStore::transfer(const std::string& fromId, const std::string& toId, double amount) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto fromIt = accounts_.find(fromId);
    auto toIt   = accounts_.find(toId);
    if (fromIt==accounts_.end() || toIt==accounts_.end() || amount<0 || fromIt->second<amount)
        return false;
    fromIt->second -= amount;
    toIt->second   += amount;
    save();
    return true;
}

double AccountStore::getBalance(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = accounts_.find(id);
    return (it != accounts_.end()) ? it->second : 0.0;
}
