//
// Created by hdu on 2024/1/6.
//

#ifndef DATESETPOOL_MYSQLCONN_H
#define DATESETPOOL_MYSQLCONN_H

#include <iostream>
#include <string>
#include <mysql.h>
#include <chrono>
using namespace  std::chrono;
class MysqlConn {
public:
    // 既然 c++ 是面向 对象 就应该想想怎么进行封装， 一个比较合理的
    // 确当这个类的功能
    MysqlConn();
    // init_connection
    ~MysqlConn();
    // distroy_connection

    // update datasets
    bool connection(std::string user , std::string password , std::string dbname,std::string ip ,unsigned short port=3306);
    //
    bool update(std::string sql);

    bool next();
    bool query(std::string sql);

    std::string value(int index);

    bool transaction();

    bool commit();

    bool rollback();
    // 计算 刷新起始 空闲时间
    void refreshAliveTime();
    // 计算连接时常
    long long getAliveTime();




private:
    void freeResult();
    MYSQL * m_conn =nullptr;
    MYSQL_RES * m_result= nullptr;
    MYSQL_ROW  m_row = nullptr;
    steady_clock::time_point m_alive_time; // 绝对时钟
};


#endif //DATESETPOOL_MYSQLCONN_H
