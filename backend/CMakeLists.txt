cmake_minimum_required(VERSION 3.15)
project(BankingServer LANGUAGES CXX)

find_program(CLANG_TIDY_EXE NAMES clang-tidy)

if(CLANG_TIDY_EXE)
  message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
  set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
else()
  message(WARNING "clang-tidy not found, skipping static analysis")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl@3")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_LIB_DIR     "${OPENSSL_ROOT_DIR}/lib")

find_package(OpenSSL REQUIRED)

add_executable(banking-server
    src/main.cpp
    src/Server.cpp
    src/ThreadPool.cpp
    src/AccountStore.cpp
)

target_include_directories(banking-server
    PRIVATE
      src
      ${OPENSSL_INCLUDE_DIR}
)

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

add_custom_target(
    tidy
    COMMAND ${CLANG_TIDY_EXE}
            -p ${CMAKE_BINARY_DIR}
            ${PROJECT_SOURCE_DIR}/src/*.cpp
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-tidy..."
    VERBATIM
)
