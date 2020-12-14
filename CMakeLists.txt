cmake_minimum_required(VERSION 3.15)
project(final)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /Users/macbookpro/Qt/5.15.2/clang_64/lib/cmake)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Charts REQUIRED)


add_executable(final main.cpp filereadthread.cpp mainwindow.cpp databasecheckthread.cpp FileSelectDialog.cpp graphDrawThread.cpp freeQueryThread.cpp predictionThread.cpp predictionThread.h)

target_link_libraries(final Qt5::Widgets Qt5::Sql Qt5::Charts)