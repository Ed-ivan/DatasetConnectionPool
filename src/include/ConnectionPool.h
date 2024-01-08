//
// Created by hdu on 2024/1/6.
//

#ifndef DATESETPOOL_CONNECTIONPOOL_H
#define DATESETPOOL_CONNECTIONPOOL_H
#include <queue>
#include "MysqlConn.h"
#include <mutex>
#include <condition_variable>
// 单例模式 懒汉模式 ： （1） 使用 mutex 实现  （2） 使用 c++11 的call_once()
class ConnectionPool {
public:
    // 通过类名访问实例的话 一定是一个静态的方法  为了保证单例  delete 下面的函数 ：
    static ConnectionPool * getConnectionPool();
    ConnectionPool(const ConnectionPool & obj) =delete;
    ConnectionPool & operator=(const ConnectionPool & obj) =delete;
    std::shared_ptr<MysqlConn>getConnection();
    ~ConnectionPool();
private:
    ConnectionPool();
    void parseJsonFile();
    void produceConnection();
    void recycleConnection();
    void addConnection();
    //

private:
    std::string m_ip;
    std::string m_user;
    std::string m_dbname;
    std::string m_password;
    unsigned short m_port;
    int m_min_size;
    int m_max_size;
    int m_timeout;
    int m_maxIdleTime;
    // when  connectionPool  is destroyed  above is not exited !
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::queue<MysqlConn *> m_connectionQ;
};


#endif //DATESETPOOL_CONNECTIONPOOL_H
