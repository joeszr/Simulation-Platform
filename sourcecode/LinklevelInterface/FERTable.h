///@file FERTable.h
///@brief 链路级FER表类声明
///@author dushaofeng
#pragma once
#include "../Utility/Include.h"
#include "../Statistician/Observer.h"

///通用的链路级FER表类

template <typename KEY>
class FERTABLE {
protected:
    ///链路级曲线类型，由两个数组组成，第一个数组存储Eb2N0的点值，也就是横轴的点值；
    ///第二个数组存储FER点值，也就是纵轴的点值。横轴和纵轴的点值描述了一条曲线。
    typedef pair<vector<double>, vector<double> > CURVE;
    ///从键值到链路级曲线的映射表
    map<KEY, CURVE> m_FERTable;
    ///FER表文件名
    char * m_pFileName;
protected:
    ///从文件读取并建立FER曲线族的函数
    void ReadTable(const char * _pFileName);
    ///从一个包含KEY值的字符串读取键值的函数
    virtual KEY ReadKEY(const char _cBuffer[500]);
public:
    ///根据键值和业务信道的Eb/Nt计算FER
    virtual double FER(const KEY & _rKey, const double& _dEb2Nt_dB);
public:
    ///FERTABLE类的构造函数
    FERTABLE();
    ///FERTABLE类的虚析构函数
    virtual ~FERTABLE() = 0;
};

template <typename KEY>
FERTABLE<KEY>::FERTABLE()  = default;

template <typename KEY>
FERTABLE<KEY>::~FERTABLE() = default;

///@brief 从文件读取并建立FER曲线表。
///本函数在第一次查FER时调用。
///@param _pFileName 要读取的文件名

template <typename KEY>
void FERTABLE<KEY>::ReadTable(const char * _pFileName) {
    assert(_pFileName);
    ifstream FER_FILE(_pFileName);
    assert(FER_FILE);
    char buffer[2000];
    KEY key;
    string temp;
    while (!FER_FILE.eof()) {
        FER_FILE >> temp;
        ///找到键值标志“KEY”字符串，并且读取键值
        if (temp == "KEY") {
            ///执行完本行后buffer是包含键值的字符串
            FER_FILE.getline(buffer, 2000);
            ///（虚函数）从字符串读取键值。（模板方法设计模式）
            key = ReadKEY(buffer);
        } else continue;
        /////在FER表中插入该键值代表的链路FER曲线（现在曲线还是空的）
        m_FERTable.insert(make_pair(key, CURVE()));

        ///将下一行作为整个字符串读入
        FER_FILE.getline(buffer, 2000);
        ///用字符串构造一个字符串输入流
        istringstream isstr1(buffer);
        istream_iterator<double> start, end;
        //  while (temp != "RBIR") isstr1 >> temp;
        while (temp != "Eb/Nt") isstr1 >> temp;
        ///找到数据起始的位置
        while (temp != "(") isstr1 >> temp;
        start = istream_iterator<double>(isstr1);
        ///使用STL的copy算法读入Eb/Nt序列
        copy(start, end, back_inserter(m_FERTable[key].first));

        ///将下一行作为整个字符串读入
        FER_FILE.getline(buffer, 2000);
        ///用字符串构造一个字符串输入流
        istringstream isstr2(buffer);
        while (temp != "FER") isstr2 >> temp;
        ///找到数据起始的位置
        while (temp != "(") isstr2 >> temp;
        ///流对像去初始化一个流迭代器
        start = istream_iterator<double>(isstr2);
        ///使用STL的copy算法读入FER序列
        copy(start, end, back_inserter(m_FERTable[key].second));
    }
}


///@brief 从字符串中读出key值
///本函数是虚函数，将由具体派生类决定如何读取key值。所以本函数在这里为空（模板方法设计模式）
///@param _cBuffer[500] 一个包含KEY键值的字符串
///@return 键值

template <typename KEY>
KEY FERTABLE<KEY>::ReadKEY(const char _cBuffer[500]) {
    return KEY();
}

///@brief 预测FER函数
///接收端算出业务信道一帧的平均Eb/Nt,然后根据链路 级曲线查出该Eb/Nt对应的FER。接收端根据该FER随机地决定帧的对错。
///@param _rKey  用来在曲线族中查找曲线的key值
///@param _dEb2Nt_dB  用来在曲线中查找FER的Eb/Nt。

template <typename KEY>
double FERTABLE<KEY>::FER(const KEY & _rKey, const double& _dEb2Nt_dB) {
    assert(m_FERTable.find(_rKey) != m_FERTable.end());
    ///取Eb/Nt序列(已排序，递增)
    vector<double> & eb2nt = m_FERTable[_rKey].first;
    ///取FER序列
    vector<double> & ferve = m_FERTable[_rKey].second;

    typedef vector<double>::iterator POS;
    ///left代表输入Eb/Nt的左侧的位置，right代表右侧的位置
    POS left, right;
    right = find_if(eb2nt.begin(), eb2nt.end(), bind2nd(greater<double>(), _dEb2Nt_dB));
    ///如果要查的值不在序列范围内，则返回边缘值
    if (right == eb2nt.begin()) return ferve[0];
    if (right == eb2nt.end()) return 0;
    left = right - 1;
    int ileft = static_cast<int> (left - eb2nt.begin()); //将pos类型转换成int表示
    int iright = ileft + 1;
    ///如果右侧点的FER等于0，则返回0。
    if (ferve[iright] == 0) return 0;
    double ebnt1 = eb2nt[ileft];
    ///将FER值取对数（FER曲线在对数域是直线）
    double fer1 = log10(ferve[ileft]);
    double ebnt2 = eb2nt[iright];
    double fer2 = log10(ferve[iright]);
    ///计算直线的斜率
    double k = (fer2 - fer1) / (ebnt2 - ebnt1);
    ///计算要查的Eb/Nt对应的对数域FER
    double result = fer1 + k * (_dEb2Nt_dB - ebnt1);
    ///将该FER转换到线性域
    result = pow(10.0, result);
    return result;
}
