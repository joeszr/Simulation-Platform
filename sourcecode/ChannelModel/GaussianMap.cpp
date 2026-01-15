///@file GaussianMap.cpp
///@brief  GaussianMap类定义
///@author wangfei
#include "GaussianMap.h"

using namespace cm;
///构造函数
GaussianMap::GaussianMap(double _dCorreDistM) {
    sigma = 1.0;
    map_size = 100;
    fil_num = 7;
    alpha = 0.5;
    corrdist = _dCorreDistM; //m
    raa = 0.5;
    Lognmap_data = new double [map_size * map_size];
    Create_Map();
}
///析构函数
GaussianMap::~GaussianMap() {
    delete[] Lognmap_data;
}

void GaussianMap::Create_Map() {
    double average_temp;
    const int temp_num = fil_num / 2;
    int m, n;

    double* temp_h;
    double* r_matrix;

    temp_h = new double [fil_num * fil_num];
    r_matrix = new double [map_size * map_size];

    average_temp = 0;
    for (int i = 0; i < fil_num; i++) {
        for (int j = 0; j < fil_num; j++) {
            temp_h[i * fil_num + j] = exp(-alpha * (sqrt((double) ((i - temp_num) * (i - temp_num) + (j - temp_num) * (j - temp_num)))));
            average_temp = average_temp + pow(temp_h[i * fil_num + j], 2);
        }
    }
    average_temp = std::sqrt(average_temp);
    for (int i = 0; i < fil_num * fil_num; i++) {
        temp_h[i] = temp_h[i] / average_temp;
    }


    //Debug
    Mat<double> normal_shadow;
    normal_shadow = randn(1, map_size * map_size);

    for (int i = 0; i < map_size * map_size; i++) {
        r_matrix[i] = sigma * normal_shadow(0, i);
    }

    for (int i = 0; i < map_size; i++) {
        for (int j = 0; j < map_size; j++) {
            Lognmap_data[i * map_size + j] = 0;

            for (int l = 0; l < fil_num; l++) {
                for (int k = 0; k < fil_num; k++) {
                    m = (i + l - temp_num);
                    if (m < 0) {
                        m = m + map_size;
                    }
                    if (m >= map_size) {
                        m = m - map_size;
                    }

                    n = (j + k - temp_num);
                    if (n < 0) {
                        n = n + map_size;
                    }
                    if (n >= map_size) {
                        n = n - map_size;
                    }

                    Lognmap_data[i * map_size + j] = Lognmap_data[i * map_size + j] + temp_h[l * fil_num + k] * r_matrix[m * map_size + n];
                }
            }
        }
    }

    delete[] temp_h;
    delete[] r_matrix;
}

int GaussianMap::Get_mapsize() const{
    return map_size;
}

double GaussianMap::ReadMap(complex<double> position) const {
    complex <double> distance;

    double x_pos;

    double y_pos;

    distance = 4.5 * position / corrdist;

    x_pos = fabs(distance.real());

    y_pos = fabs(distance.imag());

    x_pos = x_pos - (int) (x_pos / map_size) * map_size;

    y_pos = y_pos - (int) (y_pos / map_size) * map_size;

    return eslgq(x_pos, y_pos);
}

double GaussianMap::eslgq(double x_pos, double y_pos) const {
    int ip, ipp, i, j, l, iq, iqq, k;

    double h, w, b[10];

    // x_pos
    {
        i = 1;
        j = map_size;
        while (((i - j) != 1) && ((i - j) != -1)) {
            l = (i + j) / 2;
            if (x_pos < l - 1) {
                j = l;
            } else {
                i = l;
            }
        }
        ip = i - 3;
        ipp = i + 4;
    }

    // y_pos
    {
        i = 1;
        j = map_size;

        while (((i - j) != 1) && ((i - j) != -1)) {
            l = (i + j) / 2;
            if (y_pos < l - 1) {
                j = l;
            } else {
                i = l;
            }
        }
        iq = i - 3;
        iqq = i + 4;
    }

    for (i = ip - 1; i <= ipp - 1; i++) {
        b[i - ip + 1] = 0.0;
        for (j = iq - 1; j <= iqq - 1; j++) {
            h = Lognmap_data[map_size * ((i + (i < 0) * map_size) % (map_size)) + (j + (j < 0) * map_size) % (map_size)];
            for (k = iq - 1; k <= iqq - 1; k++) {
                if (k != j) {
                    h = h * (y_pos - k + 1) / (j - k);
                }
            }
            b[i - ip + 1] = b[i - ip + 1] + h;
        }
    }

    w = 0.0;

    for (i = ip - 1; i <= ipp - 1; i++) {
        h = b[i - ip + 1];
        for (j = ip - 1; j <= ipp - 1; j++) {
            if (j != i) {
                h = h * (x_pos - j + 1) / (i - j);
            }
        }
        w = w + h;
    }

    return w;
}

double GaussianMap::Calculate(complex<double> ant_mtpos, complex<double> mt_pos, complex<double> zoffset) const {
    double shadow_loss;

    complex<double> zoffset_real = corrdist* zoffset;

    shadow_loss = std::sqrt(1 - raa) * ReadMap(ant_mtpos + zoffset_real) + std::sqrt(raa) * ReadMap(mt_pos);

    return shadow_loss;
}

void GaussianMap::Out_mapdata() {
    std::ofstream OUT("result/lognmap_data.m", ios::out | ios::app);

    OUT << " lognmap_data = [ ";

    for (int i = 0; i < map_size; i++) {
        for (int j = 0; j < map_size; j++) {
            OUT << Lognmap_data[i * map_size + j] << " ";
        }
        OUT << endl;
    }

    OUT << " ];" << endl;
}