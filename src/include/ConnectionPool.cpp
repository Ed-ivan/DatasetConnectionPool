//
// Created by hdu on 2024/1/6.
//

#include "ConnectionPool.h"
#include <thread>
ConnectionPool *ConnectionPool::getConnectionPool() {
    static  ConnectionPool pool;
    return &pool;
    // 注意这里面 pool 的作用域是 {} 内， 但是它的声明周期可不是， 这么写的话 当第一次调用该函数 可以创建出来 pool
    // 之后进行调用就不用再创建一个 pool 了
    // TIPS ： c++ 11 Static 是线程安全的
}
ConnectionPool::~ConnectionPool(){
    while(!m_connectionQ.empty()){
        auto ptr= m_connectionQ.front();
        m_connectionQ.pop();
        delete ptr;
    }
}

void ConnectionPool::parseJsonFile() {
    //pass
}

ConnectionPool::ConnectionPool():m_ip("127.0.0.1"),m_user("root"),m_password("root"),m_timeout(1000),m_min_size(100),
m_max_size(1000),m_port(3306),m_maxIdleTime(2000),m_dbname("testdb")
{
    /*
     * 这里 其实可以使用json 进行解析 ， 时间关系就不弄了
     *
     */
    for(int i=0;i< m_min_size;i++){
        addConnection();
    }
    // there are  two  different threads that control destroy and create
    std::thread producer(&ConnectionPool::produceConnection,this);
    std::thread recycler(&ConnectionPool::recycleConnection,this);
    producer.detach();
    recycler.detach(); // 主线程其实是 connectionPool了 但是 如果 producer.join()的话 主线程就会阻塞并不行
}



//
void ConnectionPool::addConnection() {
    MysqlConn * conn = new MysqlConn;
    conn->connection(m_user,m_password,m_dbname,m_ip,m_port);
    conn->refreshAliveTime(); // 还有归还的时候也需要调用这个函数
    m_connectionQ.push(conn);
}


std::shared_ptr<MysqlConn> ConnectionPool::getConnection() {
    std::unique_lock<std::mutex>locker(m_mutex);
    while(m_connectionQ.empty()){
        if(std::cv_status::timeout==m_cond.wait_for(locker,milliseconds(m_timeout))){
            // 说明依旧为空
            if(!m_connectionQ.empty()){
                //return nullptr;
                continue;
            }
        }
    }
    // lambda 指定 this 说明可以使用当前(this) 的所有的 变量以及 函数
    // 这种写法真的秀 ， 重复利用 了c++ 的 类似Rall
    std::shared_ptr<MysqlConn>connptr(m_connectionQ.front(),[this](MysqlConn * conn){
        // 共享指针管理的 类型是一个 指针 ， 所以 对应的参数也得是指针
        std::unique_lock<std::mutex> locker(m_mutex);
        conn->refreshAliveTime();
        m_connectionQ.push(conn);
    });
    m_connectionQ.pop();
    m_cond.notify_all();
    // 这种方式其实还是有问题的  ， 因为 使用 cv 也会唤醒其他的 消费者 所以使用while
    return connptr;
}

//=========================
// producer and recycler
//=========================


//  当 连接池里面的线程是比较少的时候：
void ConnectionPool::produceConnection() {
    while(true){
        // 因为 生产者线程发现连接不够就应该继续创建新的连接
        std::unique_lock<std::mutex> locker(m_mutex);
        while(m_connectionQ.size()>=m_min_size) {
            // 这里一定是一个 while 循环  当含有多个生产者时候 会出错
            m_cond.wait(locker);
        }
        addConnection();
        m_cond.notify_all();
    }
}

void ConnectionPool::recycleConnection() {
    while(true){
        std::unique_lock<std::mutex> locker(m_mutex);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // 每隔 1secs 进行一次检测
        while(m_connectionQ.size()>m_max_size){
            MysqlConn * conn = m_connectionQ.front();
            //
            if(conn->getAliveTime()>=m_maxIdleTime){
                m_connectionQ.pop();
                delete conn;
            }
            else{
                break; // 使用队列先进进出
            }
        }
    }
}