//
// Created by ChTY on 2022/10/10.
//

#include "MyTimer.h"

void MyTimer::add() {
    current++;
    if(current==max)timeout_count++;
}

void MyTimer::add(const int &i) {
    timeout_count+=i/max;
    current=i%max;
}

void MyTimer::reset() {
    current=0;
    timeout_count=0;
}

void MyTimer::setmax(const int &m) {
    max=m;
}

MyTimer::MyTimer() {
    max=-1;
    current=0;
    timeout_count=0;
}

MyTimer::MyTimer(const int &i) {
    max=i;
    current=0;
    timeout_count=0;
}

int MyTimer::CheckTimeout() const{
    return timeout_count;
}
