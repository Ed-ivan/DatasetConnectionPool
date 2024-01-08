//
// Created by hdu on 2024/1/6.
//

#include "MysqlConn.h"
MysqlConn::MysqlConn() {
    m_conn = mysql_init(nullptr);
    mysql_set_character_set(m_conn,"utf8");
}

MysqlConn::~MysqlConn(){
    if(!m_conn){
        mysql_close(m_conn);
    }
    freeResult();
}

bool MysqlConn::connection(std::string user, std::string password, std::string dbname, std::string ip,
                           unsigned short port) {
    MYSQL * ptr= mysql_real_connect(m_conn,ip.c_str(),user.c_str(),password.c_str(),dbname.c_str(),port , nullptr,0);
    // if connection , result should be same as m_conn
    return ptr!= nullptr;
}

bool MysqlConn::query(std::string sql) {
    // pre times  results are different
    freeResult();
    if(mysql_query(m_conn,sql.c_str())){
        return false;
    }
    m_result = mysql_store_result(m_conn);
    return true;
}

bool MysqlConn::update(std::string sql) {
    if(mysql_query(m_conn,sql.c_str())){
        return false;
    }
    return true;
}

// get  data like two dim matrix
bool MysqlConn::next() {
    if(m_result!=nullptr){
       m_row =  mysql_fetch_row(m_result);
       return m_row !=nullptr;
    }
    return false;
}


// now row index column
std::string MysqlConn::value(int index) {
    int count= mysql_num_fields(m_result);
    if(index>= count  || index <0){
        return std::string();
    }
    char * val = m_row[index];
    // 如果 字段值 中间位置有  \0 那么需要得到string 的长度
    unsigned long length=mysql_fetch_lengths(m_result)[index];
    return std::string(val,length);

}


//=====================================
//  transactions related
//=====================================

bool MysqlConn::transaction(){
    return mysql_autocommit(m_conn,false);
}

bool MysqlConn::commit() {
    return mysql_commit(m_conn);
}
bool MysqlConn::rollback() {
    return mysql_rollback(m_conn);
}
void MysqlConn::freeResult() {
    if(m_result){
        mysql_free_result(m_result);
        m_result =nullptr;
    }
}

void MysqlConn::refreshAliveTime() {
    m_alive_time =steady_clock::now();
}

long long MysqlConn::getAliveTime() {
    nanoseconds res= steady_clock::now() - m_alive_time;
    milliseconds millsec = duration_cast<milliseconds >(res);
    return millsec.count();
}