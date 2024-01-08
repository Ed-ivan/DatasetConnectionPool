//
// Created by hdu on 2024/1/7.
//
#include <iostream>
#include <memory>
#include "../include/MysqlConn.h"
#include "../include/ConnectionPool.h"
int query(){
    MysqlConn conn;
    conn.connection("root","root","testdb","127.0.0.1");
    std::string sql ="insert into people value(007,'stanford','woman')";
    conn.update(sql);
    sql= "select * from people;";
    conn.query(sql);
    while(conn.next()){ //  next and next get value
        std::cout<<conn.value(0)<<" "<<conn.value(1)<<" "<<conn.value(2)<<std::endl;
    }
    return 0;
}

void op1(){

}

void op2(ConnectionPool * pool, int begin , int end){
    // use chron to time
    for(int i=begin; i<=end;i++){
        std::shared_ptr<MysqlConn> conn = pool->getConnection();
        char sql[1024] = {0};
        sprintf(sql,"insert into people value(%d,'stanford','woman')",i);
        conn->query(sql);
    }
}

void test2(){
    steady_clock::time_point  begin = steady_clock::now();
}

void test1() {
#if 0
    steady_clock::time_point begin = steady_clock::now();
    op1(0, 5000);
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "非连接池，单线程，用时：" << length.count() << "纳秒，" << length.count() / 1000000 << "毫秒" << endl;
#else
    ConnectionPool* pool = ConnectionPool::getConnectionPool();
    steady_clock::time_point begin = steady_clock::now();
    op2(pool, 0, 5);
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    std::cout << "connectionPool,single-threads,times:" << length.count() << "nano secs" << length.count() / 1000000 << "hao" << std::endl;
#endif
}


int main(){
    test1();
    return 0;
}