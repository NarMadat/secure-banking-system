cmake_minimum_required(VERSION 3.15)
project(BankingClientQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем автоматический moc/uic/rcc для Qt‑классов с Q_OBJECT
set(CMAKE_AUTOMOC  ON)
set(CMAKE_AUTORCC  ON)
set(CMAKE_AUTOUIC  ON)

# Если Qt установлен через Homebrew, укажите его префикс:
# (замените на вывод `brew --prefix qt`, если он иной)
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt")

# Ищем необходимые модули Qt6
find_package(Qt6 COMPONENTS Widgets Network REQUIRED)

# Собираем исполняемый файл
add_executable(banking-client-qt
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.hpp
)

# Пути к заголовочным файлам (только ваш src)
target_include_directories(banking-client-qt
    PRIVATE
      src
)

# Линкуем модули Qt
target_link_libraries(banking-client-qt
    PRIVATE
      Qt6::Widgets
      Qt6::Network
)
