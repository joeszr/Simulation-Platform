///@file Observer.h
///@brief 用来输出MS，BS,BTS,字符串对应数据流的函数
///@author dushaofeng
#include "../NetworkDrive/Clock.h"
#include "../Utility/IDType.h"
#include "../MobileStation/MSID.h"
#include"../BaseStation/BSID.h"
#include"../BaseStation/BTSID.h"
#include "Directory.h"
#include "Observer.h"
///<msid,输出流>
std::unordered_map<int, std::shared_ptr< boost::filesystem::ofstream> > Observer::m_mOfms;
///<bsid,输出流>
std::unordered_map<int, std::shared_ptr< boost::filesystem::ofstream> > Observer::m_mOfbs;
///<btsid,输出流>
std::unordered_map<int, std::shared_ptr< boost::filesystem::ofstream> > Observer::m_mOfbts;
///<字符串，输出流>
std::unordered_map<string, std::shared_ptr< boost::filesystem::ofstream> > Observer::m_mOfstr(100);

bool Observer::m_bIsEnable = false;

ofstream Observer::ofsnull;

///@brief 建立每个MS（用MS的ID标识）到输出流的对应函数
///@param _MSID MS的ID标识
///@return MS对应的输出流

ofstream& Observer::Print(const MSID& _MSID) {
    //    if (m_bIsEnable) {
    //        if (m_mOfms.count(_MSID))
    //            return *m_mOfms[_MSID];
    //        else {
    //            int imsid = _MSID.ToInt();
    //            ///将MS的ID转换成字符串
    //            string smsid = boost::lexical_cast<string > (imsid);
    //            boost::filesystem::path filename;
    //            filename = Directory::Instance().GetPath("MSID_" + smsid + ".txt"); //.directory_string();
    //            m_mOfms[_MSID] = std::shared_ptr< boost::filesystem::ofstream > (new boost::filesystem::ofstream(filename));
    //            //(*m_pOfms)<<"msid"<<endl;
    //            return *m_mOfms[_MSID];
    //        }
    //    } else
    //        return ofsnull;


    if (m_bIsEnable) {
        if (!m_mOfms.count(_MSID.ToInt())) {
            int imsid = _MSID.ToInt();
            ///将MS的ID转换成字符串
            string smsid = boost::lexical_cast<string > (imsid);
            boost::filesystem::path filename;
            filename = Directory::Instance().GetPath("MSID_" + smsid + ".txt"); //.directory_string();
            m_mOfms[_MSID.ToInt()] = std::make_shared< boost::filesystem::ofstream > (filename);
        }
        if (m_mOfms[_MSID.ToInt()]) {
            return *m_mOfms[_MSID.ToInt()];
        } else {
            assert(false);
            static ofstream null_stream;
            return null_stream;
        }

    } else {
        static ofstream null_stream;
        return null_stream;
    }
}

///@brief 建立每个BS（用BS的ID标识）到输出流的对应函数
///@param _BSID BS的ID标识
///@return BS对应的输出流

ofstream& Observer::Print(const BSID& _BSID) {
    //    if (m_bIsEnable) {
    //        if (m_mOfbs.count(_BSID))
    //            return *m_mOfbs[_BSID];
    //        else {
    //            int ibsid = _BSID.ToInt();
    //            ///将BS的ID转换成字符串
    //            string sbsid = boost::lexical_cast<string > (ibsid);
    //            boost::filesystem::path filename;
    //            filename = Directory::Instance().GetPath("BSID_" + sbsid + ".txt"); //.directory_string();
    //            m_mOfbs[_BSID] = std::shared_ptr<boost::filesystem::ofstream > (new boost::filesystem::ofstream(filename));
    //            // (*m_pOfbs)<<"bsid"<<endl;
    //            return *m_mOfbs[_BSID];
    //        }
    //    } else
    //        return ofsnull;

    if (m_bIsEnable) {
        if (!m_mOfbs.count(_BSID.ToInt())) {
            int ibsid = _BSID.ToInt();
            ///将BS的ID转换成字符串
            string sbsid = boost::lexical_cast<string > (ibsid);
            boost::filesystem::path filename;
            filename = Directory::Instance().GetPath("BSID_" + sbsid + ".txt"); //.directory_string();
            m_mOfbs[_BSID.ToInt()] = std::make_shared< boost::filesystem::ofstream > (filename);
        }
        if (m_mOfbts[_BSID.ToInt()]) {
            return *m_mOfbts[_BSID.ToInt()];
        } else {
            assert(false);
            static ofstream null_stream;
            return null_stream;
        }

    } else {
        static ofstream null_stream;
        return null_stream;
    }

}

///@brief 建立每个BTS（用BTS的ID标识）到输出流的对应函数
///@param _BTSID BTS的ID标识
///@return BTS对应的输出流

ofstream& Observer::Print(const BTSID& _BTSID) {
    //    if (m_bIsEnable) {
    //        if (m_mOfbts.count(_BTSID))
    //            return *m_mOfbts[_BTSID];
    //        else {
    //            BSID bsid = _BTSID.GetBSID();
    //            int ibsid = bsid.ToInt();
    //            int ibtsid = _BTSID.m_iBTSID;
    //            ///将BTS的ID转换成字符串
    //            string sbtsid = boost::lexical_cast<string > (ibtsid);
    //            string sbsid = boost::lexical_cast<string > (ibsid);
    //            boost::filesystem::path filename;
    //            filename = Directory::Instance().GetPath("BTSID<" + sbsid + "," + sbtsid + ">.txt"); //.directory_string();
    //            m_mOfbts[_BTSID] = std::shared_ptr<boost::filesystem::ofstream > (new boost::filesystem::ofstream(filename));
    //            //(*m_pOfbts)<<"btsid"<<endl;
    //            return *m_mOfbts[_BTSID];
    //        }
    //    } else
    //        return ofsnull;

    if (m_bIsEnable) {
        if (!m_mOfbts.count(_BTSID.ToInt())) {
            BSID bsid = _BTSID.GetBSID();
            int ibsid = bsid.ToInt();
            int ibtsid = _BTSID.m_iBTSID;
            ///将BTS的ID转换成字符串
            string sbtsid = boost::lexical_cast<string > (ibtsid);
            string sbsid = boost::lexical_cast<string > (ibsid);
            boost::filesystem::path filename;
            filename = Directory::Instance().GetPath("BTSID<" + sbsid + "," + sbtsid + ">.txt"); //.directory_string();
            m_mOfbts[_BTSID.ToInt()] = std::make_shared< boost::filesystem::ofstream > (filename);
        }
        if (m_mOfbts[_BTSID.ToInt()]) {
            return *m_mOfbts[_BTSID.ToInt()];
        } else {
            assert(false);
            static ofstream null_stream;
            return null_stream;
        }
    } else {
        static ofstream null_stream;
        return null_stream;
    }

}

///@brief 建立一个字符串到输出流的对应函数
///@param _str 字符串
///@return 字符串对应的输出流

ofstream& Observer::Print(const string& _str) {
    //    if (m_bIsEnable) {
    //        if (m_mOfstr.find(_str) != m_mOfstr.end())
    //            return *m_mOfstr[_str];
    //        boost::filesystem::path filename;
    //        filename = Directory::Instance().GetPath(_str + ".txt"); //.directory_string();
    //        m_mOfstr[_str] = std::shared_ptr<boost::filesystem::ofstream > (new boost::filesystem::ofstream(filename));
    //        return *m_mOfstr[_str];
    //    } else
    //        return ofsnull;

    if (m_bIsEnable) {
        if (m_mOfstr.find(_str) == m_mOfstr.end()) {
            boost::filesystem::path filename;
            filename = Directory::Instance().GetPath(_str + ".txt"); //.directory_string();   
            std::shared_ptr < boost::filesystem::ofstream> p = std::make_shared< boost::filesystem::ofstream > (filename);
            
            if(p) {
                m_mOfstr[_str] = p;            
            } else {
                assert(false);
            }
            return *p;            
        } else {
            return *m_mOfstr[_str];
        }
    } else {
        // 使用函数内静态变量确保 ofsnull 在使用前被初始化
        static ofstream null_stream;
        return null_stream;
    }
}

///设置m_bIsEnable的值的函数
///@param _bIsEnable  bool型变量true或者false

void Observer::SetIsEnable(bool _bIsEnable) {
    m_bIsEnable = _bIsEnable;
}