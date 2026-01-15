/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SafeDeque.h
 * Author: chty
 *
 * Created on 2022年9月2日, 上午9:51
 */

#ifndef SAFEDEQUE_H
#define SAFEDEQUE_H

#include <deque>
#include <mutex>
#include <condition_variable>

template<typename item>
class SafeDeque {
public:
    typedef typename std::deque<item>::iterator this_iterator;
    typedef typename std::deque<item>::const_iterator this_const_iterator;

    void push_back(const item& i) {
        //if (mtx_ == nullptr)cout << "nullptr1111" << endl;
        std::lock_guard<std::mutex> lk(*mtx_);
        return dataDeque_.push_back(i);
    }
    void emplace_back(const item& i) {
        std::lock_guard<std::mutex> lk(*mtx_);
         dataDeque_.emplace_back(i);
    }
    void push_front(const item& i){
        std::lock_guard<std::mutex> lk(*mtx_);
        return dataDeque_.push_front(i);
    }
    void emplace_front(item& i) {
        std::lock_guard<std::mutex> lk(*mtx_);
        return dataDeque_.emplace_front(i);
    }
    void pop_front() {
       std::lock_guard<std::mutex> lk(*mtx_);
        return dataDeque_.pop_front();
    }
    void pop_back() {
        std::lock_guard<std::mutex> lk(*mtx_);
        return dataDeque_.pop_back();
    }
    item& operator[](const int&  i){
        std::lock_guard<std::mutex> lk(*mtx_);
        return dataDeque_[i];
    }
    
    
    
    item& front(){
        return dataDeque_.front();
    }
    this_iterator erase(this_const_iterator it) {
        std::lock_guard<std::mutex> lk(*mtx_);
        return dataDeque_.erase(it);
    }
    

    this_const_iterator cbegin()const{
        return dataDeque_.cbegin();
    }
    this_const_iterator cend() const{
        return dataDeque_.cend();
    }

    this_iterator end() {
        return dataDeque_.end();
    }

    this_iterator begin() {
        return dataDeque_.begin();
    }

    this_const_iterator end() const {
        return dataDeque_.end();
    }

    void clear() {
        std::lock_guard<std::mutex> lk(*mtx_);
        return dataDeque_.clear();
    }

    int size() {
        //std::lock_guard<std::mutex> lk(*mtx_);
        return dataDeque_.size();
    }
    bool empty() {
        //std::lock_guard<std::mutex> lk(*mtx_);
        return dataDeque_.empty();
    }

    SafeDeque<item>() {
        mtx_ = std::shared_ptr<std::mutex>(new std::mutex);
        if(mtx_==nullptr)cout<<"nullptr"<<endl;
    }
    SafeDeque<item>(const SafeDeque<item>&) {
        mtx_ = std::shared_ptr<std::mutex>(new std::mutex);
    }

private:
    std::deque<item> dataDeque_;
    std::shared_ptr<std::mutex> mtx_;
    std::condition_variable m_cond;
};

#endif /* SAFEDEQUE_H */

