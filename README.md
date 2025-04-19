# Banking System (C++ / Qt)

[![Build Backend](https://img.shields.io/badge/build-backend-green)](#backend)
[![Build CLI Client](https://img.shields.io/badge/build-cli--client-green)](#cli-client)
[![Build GUI Client](https://img.shields.io/badge/build-gui--client-green)](#gui-client)
[![License](https://img.shields.io/badge/license-MIT-blue)](#license)

> **Dynamic, live‑updating documentation** for your POSIX‑socket banking system with TLS, thread‑pool backend and Qt GUI client.

---

## 📑 Table of Contents

1. [Overview](#overview)
2. [Architecture](#architecture)
3. [Backend](#backend)
   - [Requirements](#backend-requirements)
   - [Directory Structure](#backend-directory-structure)
   - [Build & Run](#backend-build--run)
   - [Protocol & Commands](#protocol--commands)
4. [CLI Client](#cli-client)
   - [Build & Run](#cli-client-build--run)
   - [Usage](#cli-client-usage)
5. [GUI Client (Qt)](#gui-client)
   - [Requirements](#gui-client-requirements)
   - [Build & Run](#gui-client-build--run)
   - [Usage](#gui-client-usage)
6. [TLS Certificate](#tls-certificate)
7. [Contributing](#contributing)
8. [License](#license)

---

## 🔍 Overview

A simple **banking server** in C++ using POSIX sockets, a thread‑pool, file‑based account store, and TLS (OpenSSL).
Includes:

- **Backend** (`banking-server`): TCP/TLS server, text protocol
- **CLI client** (`banking-client-cli`): console tester using OpenSSL
- **GUI client** (`banking-client-qt`): Qt‑based TLS client with dynamic UI

---

## 🏗 Architecture

```
┌─────────┐       ┌───────────────┐       ┌───────────────┐
│  Client │◀ TLS ▶│ BankingServer │◀ Threads ▶│ AccountStore │
│ (Qt/CLI)│       │   (C++/TLS)   │       │  (File DB)    │
└─────────┘       └───────────────┘       └───────────────┘
```

- **BankingServer**
  - Listens on TCP port (default 5500)
  - TLS handshake via OpenSSL
  - Dispatches each connection to a `ThreadPool`
  - Commands parsed in `handleClientSSL()`
  - Data stored in `accounts.db`

- **Clients**
  - **CLI**: single‑command console client
  - **GUI**: Qt app with dynamic form and log

---

## 🖥 Backend

### Backend Requirements

- C++17 compiler (g++/clang++)
- CMake ≥ 3.15
- OpenSSL (e.g. `brew install openssl@3`)
- pthread (system‑provided)

### Backend Directory Structure

```
backend/
├── CMakeLists.txt
├── ssl/
│   ├── server.crt
│   └── server.key
└── src/
    ├── main.cpp
    ├── Server.hpp/.cpp
    ├── ThreadPool.hpp/.cpp
    ├── AccountStore.hpp/.cpp
    └── …
```

### Backend Build & Run

```bash
# 1. Generate SSL cert/key (once)
cd backend/ssl
openssl genrsa -out server.key 2048
openssl req -new -x509 -key server.key -out server.crt   -days 365 -subj "/C=AM/ST=Yerevan/L=Yerevan/O=MyBank/CN=localhost"

# 2. Build
cd backend
mkdir build && cd build
cmake ..   -DOPENSSL_ROOT_DIR="$(brew --prefix openssl@3)"   -DOPENSSL_INCLUDE_DIR="$(brew --prefix openssl@3)/include"   -DOPENSSL_LIB_DIR="$(brew --prefix openssl@3)/lib"
make

# 3. Run
./banking-server 5500 ../ssl/server.crt ../ssl/server.key
```

You should see:

> `Server listening on port 5500 (TLS)`

### Protocol & Commands

Each client connection sends **one** newline‑terminated command. Server replies then closes.

| Command                          | Description                                    | Reply                    |
|----------------------------------|------------------------------------------------|--------------------------|
| `CREATE <ID> <amount>`           | Create new account `<ID>` with balance `<amount>` | `OK
` or `ERROR
`      |
| `DEPOSIT <ID> <amount>`          | Add `<amount>` to account `<ID>`               | `OK
` or `ERROR
`      |
| `WITHDRAW <ID> <amount>`         | Subtract `<amount>` from account `<ID>`        | `OK
` or `ERROR
`      |
| `TRANSFER <fromID> <toID> <amt>` | Move `<amt>` from `<fromID>` to `<toID>`       | `OK
` or `ERROR
`      |
| `BALANCE <ID>`                   | Query balance of `<ID>`                        | `BALANCE <x>
`          |

---

## 🖥 CLI Client

### CLI Client Build & Run

```bash
cd backend/build
make banking-client-cli

./banking-client-cli 127.0.0.1 5500 ../ssl/server.crt ../ssl/server.key
```

### CLI Client Usage

```text
$ ./banking-client-cli 127.0.0.1 5500 server.crt server.key
Connected (TLSv1.3)
Enter command (e.g. CREATE Alice 1000): CREATE Alice 1000
Server response: OK
```

---

## 🎨 GUI Client (Qt)

### GUI Client Requirements

- Qt5 or Qt6 (Widgets, Network)
- CMake ≥ 3.15

### GUI Client Build & Run

```bash
cd banking-client-qt
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
make
./banking-client-qt
```

### GUI Client Usage

1. **Host/Port**: defaults to `localhost:5500`
2. **Command**: select operation
3. **ID**, **To ID**, **Amount**: fill as needed
4. **Send**: click to dispatch
5. **Log**: shows `>>` command and `<<` reply

---

## 🔐 TLS Certificate

- **Location**: `backend/ssl/server.crt` & `server.key`
- **Self‑signed**: clients disable verification or load `server.crt` as trusted CA.

---

## 🤝 Contributing

1. Fork the repo
2. Create feature branch
3. Submit PR against `main`
4. Ensure build & tests pass

---

## 📄 License

Distributed under the **MIT License**. See [`LICENSE`](LICENSE) for details.
