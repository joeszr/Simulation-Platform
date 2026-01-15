/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SafeMap.h
 * Author: chty
 *
 * Created on 2022年8月29日, 上午10:22
 */

#ifndef SAFEMAP_H
#define SAFEMAP_H
#include <unordered_map>
#include <map>
#include <mutex>
template<typename Key, typename Val>
class SafeMap {
public:
    typedef typename std::map<Key, Val>::iterator this_iterator;
    typedef typename std::map<Key, Val>::const_iterator this_const_iterator;

    Val& operator[](const Key& key) {
       // std::lock_guard<std::mutex> lk(*mtx_);
        return dataMap_[key];
    }

    int erase(const Key& key) {
 //       std::lock_guard<std::mutex> lk(*mtx_);
        return dataMap_.erase(key);
    }

    this_iterator find(const Key& key) {
//        std::lock_guard<std::mutex> lk(*mtx_);
        return dataMap_.find(key);
    }

    this_const_iterator find(const Key& key) const {
//        std::lock_guard<std::mutex> lk(*mtx_);
        return dataMap_.find(key);
    }

    this_iterator end() {
        return dataMap_.end();
    }
    
    this_iterator begin() {
        return dataMap_.begin();
    }

    this_const_iterator end() const {
        return dataMap_.end();
    }
    void clear(){
        return dataMap_.clear();
    }
    SafeMap<Key, Val>(){
        mtx_=std::shared_ptr<std::mutex>(new std::mutex);
    }

private:
    std::map<Key, Val> dataMap_;
    std::shared_ptr<std::mutex> mtx_;
};


#endif /* SAFEMAP_H */

