/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file GaussianMap.h
///@brief  GaussianMap类声明
///
///包括构造函数，析构函数，创建Shadowing Map，获得Map大小，计算ShadowLoss，读取和输出Map数据的函数声明
///
///@author wangfei
#pragma once
#include <itpp/itbase.h>
#include <complex>

using namespace std;
using namespace itpp;

namespace cm {
    ///GaussianMap class
    class GaussianMap {
    private:
        //the lognormal map size
        int map_size; 
        //the shadowing variance
        double sigma; 
        //the filter num
        int fil_num; 
        //correlation factor between the filternum
        double alpha; 
        //correlation distance
        double corrdist; 
        //intercell correlation
        double raa; 
        //the lognormal map data
        double* Lognmap_data; 

    public:
        ///构造函数
        explicit GaussianMap(double _dCorreDistM);
        ///析构函数
        virtual ~GaussianMap();

        ///reading data from the lognmap
        double ReadMap(complex<double> position) const;

        ///cac the value
        double Calculate(complex<double> ant_mtpos, complex<double> mt_pos, complex<double> zoffset) const;

        ///Get the mapsize
        int Get_mapsize() const;

        ///Output the map data
        void Out_mapdata();
    private:
        ///Generate the shadowing map
        void Create_Map();        
        double eslgq(double x_pos, double y_pos) const;
    };
}