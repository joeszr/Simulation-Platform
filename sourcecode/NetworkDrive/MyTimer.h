//
// Created by ChTY on 2022/10/10.
//

#ifndef CHTY_MYTIMER_H
#define CHTY_MYTIMER_H


class MyTimer {
private:
    int max;
    int current;
    int timeout_count;
public:
    void add();
    void add(const int& );
    void reset();
    void setmax(const int& );
    int CheckTimeout ()const;
    MyTimer();
    explicit MyTimer(const int& i);
};


#endif //CHTY_MYTIMER_H
