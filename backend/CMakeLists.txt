cmake_minimum_required(VERSION 3.15)
project(BankingServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1) Указываем, где Homebrew положил OpenSSL
set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl@3")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_LIB_DIR     "${OPENSSL_ROOT_DIR}/lib")

# 2) Ищем OpenSSL
find_package(OpenSSL REQUIRED)

add_executable(banking-server
    src/main.cpp
    src/Server.cpp
    src/ThreadPool.cpp
    src/AccountStore.cpp
)

# 3) Пути к нашим хедерам и к OpenSSL
target_include_directories(banking-server
    PRIVATE
      src
      ${OPENSSL_INCLUDE_DIR}
)

# 4) Линкуем OpenSSL
target_link_directories(banking-server PRIVATE ${OPENSSL_LIB_DIR})
target_link_libraries(banking-server
    PRIVATE
      OpenSSL::SSL
      OpenSSL::Crypto
)

add_executable(banking-client-cli
    src/Client.cpp
)
target_include_directories(banking-client-cli
    PRIVATE
      src
      ${OPENSSL_INCLUDE_DIR}
)
target_link_directories(banking-client-cli PRIVATE ${OPENSSL_LIB_DIR})
target_link_libraries(banking-client-cli
    PRIVATE
      OpenSSL::SSL
      OpenSSL::Crypto
)