//
// Created by ChTY on 2022/9/7.
//

#ifndef CHTY_SAFEUNORDERED_MAP_H
#define CHTY_SAFEUNORDERED_MAP_H
#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>
#include "WRITE_FIRST_RW_LOCK.h"
template<typename Key, typename Val>
class SafeUnordered_map{
    typedef size_t (*Hash)(const Key&);
    typedef typename std::unordered_map<Key, Val,std::function<size_t(const Key&)>>::iterator this_iterator;
    typedef typename std::unordered_map<Key, Val,std::function<size_t(const Key&)>>::const_iterator this_const_iterator;
public:
    Val& operator[](const Key& key) {
//        unique_writeguard<WfirstRWLock> l(*wrlock);
        std::lock_guard<std::mutex> lk(*mtx_);
        return unmap[key];
    }
    int erase(const Key& key) {
        //if(mtx_)
//        unique_writeguard<WfirstRWLock> l(*wrlock);
//        std::lock_guard<std::mutex> lk(*mtx_);
        return unmap.erase(key);
    }
    this_iterator find(const Key& key) {
        //if(mtx_)
//        unique_readguard<WfirstRWLock> l(*wrlock);
        std::lock_guard<std::mutex> lk(*mtx_);
        return unmap.find(key);
    }

    this_const_iterator find(const Key& key) const {
        //if(mtx_)

//        unique_readguard<WfirstRWLock> l(*wrlock);
//        std::lock_guard<std::mutex> lk(*mtx_);
        return unmap.find(key);
    }

    this_iterator end() {
        return unmap.end();
    }

    this_iterator begin() {
        return unmap.begin();
    }

    this_const_iterator end() const {
        return unmap.end();
    }
    void clear(){
//        unique_writeguard<WfirstRWLock> l(*wrlock);
        return unmap.clear();
    }
    bool empty(){
        return unmap.empty();
    }
    size_t size(){
      //  unique_readguard<WfirstRWLock> l(*wrlock);
//       std::lock_guard<std::mutex> lk(*mtx_);
        return unmap.size();
    }
    std::pair<this_iterator,bool> insert(const std::pair<Key,Val>& p){
//        unique_writeguard<WfirstRWLock> l(*wrlock);
//        std::lock_guard<std::mutex> lk(*mtx_);
        auto r= unmap.insert(p);
        return r;
    }

    size_t count(const Key& k){
//        unique_readguard<WfirstRWLock> l(*wrlock);
//        std::lock_guard<std::mutex> lk(*mtx_);
        return unmap.count(k);
    }
    SafeUnordered_map<Key, Val>(int n,Hash hash){
        mtx_=std::shared_ptr<std::mutex>(new std::mutex);
      //  wrlock=std::shared_ptr<WfirstRWLock>(new WfirstRWLock);
        unmap=std::unordered_map<Key, Val,std::function<size_t(const Key&)>>(n,hash);
    }
    SafeUnordered_map<Key, Val>(const SafeUnordered_map<Key, Val>& um){
//        std::lock_guard<std::mutex> lk(*um.mtx_);
        mtx_=std::shared_ptr<std::mutex>(new std::mutex);
       // wrlock=std::shared_ptr<WfirstRWLock>(new WfirstRWLock);
        unmap=um.unmap;
    }
//    SafeUnordered_map<Key, Val>(){
//        mtx_=std::shared_ptr<std::mutex>(new std::mutex);
//        unmap=std::unordered_map<Key,Val,std::hash<Key>>();
//    }
private:
    std::unordered_map<Key, Val,std::function<size_t(const Key&)>> unmap;
    std::shared_ptr<std::mutex> mtx_;
    std::shared_ptr<WfirstRWLock> wrlock;
};


bool operator==(const std::pair<int,int>& pos1,const std::pair<int,int>& pos2);
size_t pair_intHash(const std::pair<int,int>& pos);

#endif //CHTY_SAFEUNORDERED_MAP_H
