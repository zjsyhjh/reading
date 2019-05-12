/*************************************************************************
 @Author: hujiahuan
 @Created Time : Sun May 12 20:39:01 2019
 @File Name: test_leveldb.cpp
 @Description:
 ************************************************************************/
#include <iostream>
#include "leveldb/db.h"

int main()
{
    leveldb::DB *db;
    leveldb::Options options;
    options.create_if_missing = true;
    
    // open 
    leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
    assert(status.ok());

    std::string key = "test_key"; 
    std::string value = "test_value";

    // write key value 
    status = db->Put(leveldb::WriteOptions(), key, value);
    assert(status.ok());

    // read
    status = db->Get(leveldb::ReadOptions(), key, &value);
    assert(status.ok());

    std::cout << "key=" << key << ", value=" << value << std::endl;

    // delete
    status = db->Delete(leveldb::WriteOptions(), key); 
    assert(status.ok());

    // read again
    status = db->Get(leveldb::ReadOptions(),key, &value);  
    if(!status.ok()) {
        std::cerr << status.ToString() << std::endl;
    } else {
        std::cout << "key=" << key << ", value=" << value << std::endl;    
    }   
    // close 
    delete db;

    return 0;
}
