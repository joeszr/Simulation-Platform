/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/* 
 * File:   ChannelInfo.h
 * Author: djc
 *
 * Created on 2020年4月9日, 下午3:22
 */
#ifndef CHANNELINFO_H
#define CHANNELINFO_H
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <set>
#include <ctime>
using namespace std;

struct Point {
    int ID;
    bool Type; //true---Tx, false---Rx
    double X, Y, Height;
    int AntNum;

    Point() = default;

    explicit Point(bool type, int id = 0, double x = 0.0, double y = 0.0, double height = 25, int antNum = 2) : Type(type), ID(id), X(x), Y(y), Height(height), AntNum(antNum) {
    }

    //ostream不能设置为const，因为返回值不是const

    friend ostream& operator<<(ostream& out, const Point& point) {
        string name;
        if (point.Type) {//Tx
            name = "Tx";
        } else {//Rx
            name = "Rx";
        }
        out << "{\n\t\"name\":\"" << name << "\",\n"
                << "\t\"" + name + "ID\":" << point.ID << ",\n"
                << "\t\"X\":" << point.X << ",\n"
                << "\t\"Y\":" << point.Y << ",\n"
                << "\t\"Height\":" << point.Height << ",\n"
                << "\t\"" + name + "AntNum\":" << point.AntNum << "\n}";
        return out;
    }
};

struct Complex {
    double real;
    double imag;

    explicit Complex(double real = 0, double imag = 0) : real(real), imag(imag) {
    }

    friend ostream& operator<<(ostream& out, const Complex& c) {
        out << "[" << c.real << "," << c.imag << "]";
        return out;
    }
};

struct AntPair {
    int N; //径的数目 N_total_num_cluster
    int M; //子径的数目 M_total_num_ray
    int s; //Tx_ant_index_s
    int u; //Rx_ant_index_u
    //下标[i][j]，i是径的编号，j是子径的编号
    bool IsLOS;
    Complex C;
    Complex D;
    vector<vector<Complex> > A; // N*M
    vector<vector<Complex> > B; // N*M

    explicit AntPair(int N = 0, int M = 0, int u = 0, int s = 0) :
    N(N), M(M), u(u), s(s), IsLOS(false) {
        //A.resize(N, vector<Complex>(M));
        //B.resize(N, vector<Complex>(M));
    }

    friend ostream& operator<<(ostream& out, const AntPair& h) {
        out << "{\n\t\"name\":\"" << "AntPair" << "\",\n"
                << "\t\"N_total_num_cluster\":" << h.N << ",\n"
                << "\t\"M_total_num_ray\":" << h.M << ",\n"
                << "\t\"Tx_ant_index_s\":" << h.s << ",\n"
                << "\t\"Rx_ant_index_u\":" << h.u << ",\n"
                << "\t\"IsLOS\":" << h.IsLOS << ",\n"
                << "\t\"C\":" << h.C << ",\n"
                << "\t\"D\":" << h.D << ",\n"
                << "\t\"A\": [";
        for (int n = 0; n < h.N; n++) {
            for (int m = 0; m < h.M; m++) {
                out << h.A[n][m];
                if (n != h.N - 1 || m != h.M - 1) {
                    out << ",";
                }
            }
        }
        out << "],\n"
                << "\t\"B\": [";
        for (int n = 0; n < h.N; n++) {
            for (int m = 0; m < h.M; m++) {
                out << h.B[n][m];
                if (n != h.N - 1 || m != h.M - 1) {
                    out << ",";
                }
            }
        }
        out << "]\n}";
        return out;
    }
};

//通道对中的天线对

struct AntPair_List_per_TXRUPair {
    int Tx_TXRU_Index;
    int Rx_TXRU_Index;
    int Tx_Ant_Num;
    int Rx_Ant_Num;
    vector<vector<AntPair> > H_Ant; //A_or_C B_or_D

    explicit AntPair_List_per_TXRUPair(int Tx_TXRU_Index = 0, int Rx_TXRU_Index = 0, int Tx_Ant_Num = 2, int Rx_Ant_Num = 2) :
    Tx_TXRU_Index(Tx_TXRU_Index), Rx_TXRU_Index(Rx_TXRU_Index), Tx_Ant_Num(Tx_Ant_Num), Rx_Ant_Num(Rx_Ant_Num) {
        //H_Ant.resize(Tx_Ant_Num, vector<AntPair>(Rx_Ant_Num));
    }

    friend ostream& operator<<(ostream& out, const AntPair_List_per_TXRUPair& h) {

        out << "{\n"; //json对象的起始
        //中间的元素
        out << "\t\"name\":\"" << "AntPair_List_per_TXRUPair" << "\",\n"
                << "\t\"Tx_TXRU_Index\":" << h.Tx_TXRU_Index << ",\n"
                << "\t\"Rx_TXRU_Index\":" << h.Rx_TXRU_Index << ",\n"
                << "\t\"Tx_Ant_Num\":" << h.Tx_Ant_Num << ",\n"
                << "\t\"Rx_Ant_Num\":" << h.Rx_Ant_Num << ",\n"
                << "\t\"H_Ant\":[";
        for (int i = 0; i < h.Tx_Ant_Num; i++) {
            for (int j = 0; j < h.Rx_Ant_Num; j++) {
                out << h.H_Ant[i][j];
                if (i != h.Tx_Ant_Num - 1 || j != h.Rx_Ant_Num - 1) {
                    out << ",";
                }
            }
        }
        out << "]";
        //最后一个元素
        out << "\n}"; //json对象的结束

        return out;
    }
};


//收发端之间的信道矩阵

struct TxRx_Pair {//TxRx_Pair +PL
    //ID从0开始计算
    int Tx_ID;
    int Rx_ID;
    int Tx_TXRU_Num;
    int Rx_TXRU_Num;

    vector<vector<AntPair_List_per_TXRUPair> > H_TXRU_List; //H_TXRU_list

    bool IsLOS;
    double PL;
    double K_R;
    int N; //径的数目 N_total_num_cluster
    vector<double> path_delay_list; // N
    vector<double> path_powerPerRay_list; // N

    explicit TxRx_Pair(int Tx_ID = 0, int Rx_ID = 0, int Tx_TXRU_Num = 2, int Rx_TXRU_Num = 1, bool IsLos = false, double PL = 0.0, int N = 1) :
    Tx_ID(Tx_ID), Rx_ID(Rx_ID), Tx_TXRU_Num(Tx_TXRU_Num), Rx_TXRU_Num(Rx_TXRU_Num), IsLOS(IsLos), PL(PL) {
        //H_TXRU_List.resize(Tx_TXRU_Num, vector<AntPair_List_per_TXRUPair>(Rx_TXRU_Num));
        //path_delay_list.resize(N);
        //path_power_list.resize(N);

    }

    friend ostream& operator<<(ostream& out, const TxRx_Pair& h) {//必须是const

        out << "{\n"; //json对象的起始
        //中间的元素
        out << "\t\"name\":\"" << "TxRx_Pair" << "\",\n"
                << "\t\"Tx_ID\":" << h.Tx_ID << ",\n"
                << "\t\"Rx_ID\":" << h.Rx_ID << ",\n"
                << "\t\"Tx_TXRU_Num\":" << h.Tx_TXRU_Num << ",\n"
                << "\t\"Rx_TXRU_Num\":" << h.Rx_TXRU_Num << ",\n"
                << "\t\"IsLOS\":" << h.IsLOS << ",\n"
                << "\t\"PathLoss\":" << h.PL << ",\n"
                << "\t\"K_R\":" << h.K_R << ",\n"
                << "\t\"N_total_num_cluster\":" << h.N << ",\n"
                << "\t\"path_delay_list\": [";
        for (int n = 0; n < h.N; n++) {
            out << h.path_delay_list[n];
            if (n != h.N - 1) {
                out << ",";
            }
        }
        out << "],\n"
                << "\t\"path_powerPerRay_list\": [";
        for (int n = 0; n < h.N; n++) {
            out << h.path_powerPerRay_list[n];
            if (n != h.N - 1) {
                out << ",";
            }
        }
        out << "],\n"
                << "\t\"H_TXRU\":[";
        for (int i = 0; i < h.Tx_TXRU_Num; i++) {
            for (int j = 0; j < h.Rx_TXRU_Num; j++) {
                //索引修正
                //h.H_TXRU[i][j].Tx_TXRU_Index = i;
                //h.H_TXRU[i][j].Rx_TXRU_Index = j;
                out << h.H_TXRU_List[i][j];
                if (i != h.Tx_TXRU_Num - 1 || j != h.Rx_TXRU_Num - 1) {
                    out << ",";
                }
            }
        }
        out << "]";
        //最后一个元素
        out << "\n}"; //json对象的结束

        return out;
    }
};

//+verify func t -> H_u_s

//所有收发端的信道信息统计

class ChannelInfo {
private:
    ChannelInfo(int, int);
    ChannelInfo(const ChannelInfo& orig) = default;
    virtual ~ChannelInfo() = default;
public:
    int Tx_Num;
    int Rx_Num;
    //发射端信息统计
    vector<Point> Txs;
    //接收端信息统计
    vector<Point> Rxs;
    //收发端信道矩阵信息统计
    vector<vector<TxRx_Pair> > H_TX_RX;
    //采用单例设计模式
    static ChannelInfo& Instance();

    static bool isOver;

    void resize();

    friend ostream& operator<<(ostream&, const ChannelInfo&);

};
#endif /* CHANNELINFO_H */