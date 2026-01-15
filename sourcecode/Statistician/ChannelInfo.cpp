/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ChannelInfo.cpp 
 * Author: djc
 * 
 * Created on 2020年4月9日, 下午3:22
 */

#include "ChannelInfo.h"

ChannelInfo::ChannelInfo(int Tx_Num=0,int Rx_Num=0):Tx_Num(Tx_Num),Rx_Num(Rx_Num) {
    //Txs.resize(Tx_Num,Point(true));
    //Rxs.resize(Rx_Num,Point(false));
    //H_TX_RX.resize(Tx_Num,vector<TxRx_Pair>(Rx_Num));
}

ostream& operator<<(ostream& out,const ChannelInfo& h){
    
    out<<"{\n";//json对象的起始
    //中间的元素
    out << "\t\"name\":\"" << "ChannelInfo" << "\",\n"
        << "\t\"Tx_Num\":" << h.Tx_Num << ",\n"
        << "\t\"Rx_Num\":" << h.Rx_Num << ",\n"
        << "\t\"Txs\":["; 
    for(int i=0;i<h.Tx_Num;i++){
        out<<h.Txs[i];
        if(i!=h.Tx_Num-1){
            out<<",";
        }
    }
    out<<"]"<< ",\n"
       << "\t\"Rxs\":["; 
    for(int i=0;i<h.Rx_Num;i++){
        out<<h.Rxs[i];
        if(i!=h.Rx_Num-1){
            out<<",";
        }
    }
    out<<"]"<< ",\n"
       << "\t\"H_TX_RX\":["; 
    for(int i=0;i<h.Tx_Num;i++){
        for(int j=0;j<h.Rx_Num;j++){
            out<<h.H_TX_RX[i][j];
            if(i!=h.Tx_Num-1||j!=h.Rx_Num-1){
                out<<",";
            }
        }
    }
    out<<"]";
    //最后一个元素
    out<<"\n}";//json对象的结束
    
    return out;
}

void ChannelInfo::resize(){
    Txs.clear();
    Txs.resize(Tx_Num,Point(true));
    Rxs.clear();
    Rxs.resize(Rx_Num,Point(false));
    H_TX_RX.clear();
    H_TX_RX.resize(Tx_Num,vector<TxRx_Pair>(Rx_Num));
}

ChannelInfo& ChannelInfo::Instance(){
        static auto* instance = new ChannelInfo;
        return *instance;
}

bool ChannelInfo::isOver = true;//true表示关闭信道信息的统计
