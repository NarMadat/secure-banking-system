cmake_minimum_required(VERSION 3.15)
project(BankingClientQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC  ON)
set(CMAKE_AUTORCC  ON)
set(CMAKE_AUTOUIC  ON)


set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt")

find_package(Qt6 COMPONENTS Widgets Network REQUIRED)

add_executable(banking-client-qt
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.hpp
)

target_include_directories(banking-client-qt
    PRIVATE
      src
)

target_link_libraries(banking-client-qt
    PRIVATE
      Qt6::Widgets
      Qt6::Network
)
