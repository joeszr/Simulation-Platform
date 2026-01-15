//
// Created by ChTY on 2022/9/8.
//
#include "SafeUnordered_map.h"
bool operator==(const std::pair<int,int>& pos1,const std::pair<int,int>& pos2){
    return pos1.first==pos2.first&&pos1.second==pos2.second;
}
size_t pair_intHash(const std::pair<int,int>& pos){
    return (std::hash<int>()(pos.first)+std::hash<int>()(pos.second))/2;
}