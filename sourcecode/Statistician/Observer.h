///@file Observer.h
///@brief 用来输出MS，BS,BTS，字符串对应数据流的函数
///@author dushaofeng
#pragma once
//#include "../Utility/Include.h"
#include <map>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "../SafeUnordered_map.h"
#include <memory>
using namespace std;


class MSID;
class BSID;
class BTSID;

/// @brief Observe类用来实现输出数据,主要用来支持平台调试
///
/// 可以调用本类的Print函数得到输出流，并使用该输出流来输出调试信息，使用方法为
/// Observer::Print(移动台ID / 基站ID / 基站发射器ID / 字符串) << 被输出内容
class Observer {
private:
    /// MSID到文件输出流的映射 <msid,输出流>
    static std::unordered_map<int, std::shared_ptr< boost::filesystem::ofstream> > m_mOfms;
    /// BSID到文件输出流的映射 <bsid,输出流>
    static std::unordered_map<int, std::shared_ptr< boost::filesystem::ofstream> > m_mOfbs;
    /// BTSID到文件输出流的映射 <btsid,输出流>
    static std::unordered_map<int, std::shared_ptr< boost::filesystem::ofstream> > m_mOfbts;
    /// 字符串到文件输出流的映射 <字符串，输出流>
    static std::unordered_map<string, std::shared_ptr< boost::filesystem::ofstream> > m_mOfstr;
    /// 表示本类是否处于工作状态的变量；
    static bool m_bIsEnable;
    /// 空输出流，当本类不工作时，将所有的输出操作输出到空输出流
    static ofstream ofsnull;
public:
    /// @brief 返回输入MSID对应的输出流，如果不存在则新建一个输出流与MSID对应
    static ofstream& Print(const MSID& _MSID);
    /// @brief 返回输入BSID对应的输出流，如果不存在则新建一个输出流与BSID对应
    static ofstream& Print(const BSID& _BSID);
    /// @brief 返回输入BTSID对应的输出流，如果不存在则新建一个输出流与BTSID对应
    static ofstream& Print(const BTSID& _BTSID);
    /// @brief 返回输入字符串对应的输出流，如果不存在则新建一个输出流与字符串对应
    static ofstream& Print(const string& _str);
    /// @brief 设置Observer是否处于工作状态的函数
    /// @param _bIsEnable true表示正常工作，false表示不工作
    static void SetIsEnable(bool _bIsEnable);
};
