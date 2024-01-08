cmake_minimum_required(VERSION 3.27)
project(DatesetPool)

set(CMAKE_CXX_STANDARD 11)
include_directories(C:\\Program\ Files\\MySQL\\MySQL\ Server\ 8.0\\include)
link_directories(C:\\Program\ Files\\MySQL\\MySQL\ Server\ 8.0\\lib)
link_libraries(libmysql)

add_executable(DatesetPool
        src/include/MysqlConn.cpp
        src/include/MysqlConn.h
        src/include/ConnectionPool.cpp
        src/include/ConnectionPool.h
        src/test/test.cpp)
target_link_libraries(DatesetPool  libmysql)