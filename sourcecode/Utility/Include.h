///==========================================================
/// COPYRIGHT NOTICE
/// Copyright (c) 2010,CMCC
/// All right reserved.
///
///@file Include.h
///@brief   Including some important header files.
///
///It contains header files of STL,header files of BOOST and header files defined by the user
///
///@author wangxiaozhou


#pragma	once				  ///pre-compling command which make sure that the header files below can be only compiled once

//#include <stdlib.h>

///Begin STL Header Files
//#include <cmath>			  //use the basic math function
#include <complex>  		  ///use the basic complex class
#include <iostream> 		  ///use input output istream
#include <iomanip>  		  ///使用输入输出流控制库
#include <sstream>  		  ///使用输入字符串流库
#include <functional>   	  ///使用STL定义的函数对象和配接器
#include <algorithm>		  ///使用STL定义的算法
#include <iterator> 		  ///使用STL定义的迭代器
#include <numeric>  		  ///使用STL定义的数值特性及算法
#include <limits>   		  ///使用STL定义的标量算术特征
#include <string>   		  ///使用STL定义的字符串类
#include <cassert>  		  ///使用C标准库定义的断言
#include <valarray> 		  ///使用数值数组类
#include <map>  	          ///使用STL定义的映射容器
#include <queue>	          ///使用STL定义的队列
#include <cmath>
#include <memory>
#include <iostream>
#include <istream>
#include <streambuf>
#include <fstream>
#include <sstream>
using namespace std;              ///有关STL的相关信息可以查阅MSDN和《泛型编程与STL》侯捷
///End STL Header Files

#include <itpp/itbase.h>
using namespace itpp;

///Begin Boost Header Files
//#include <boost/bind.hpp>         ///使用boost定义的函数对象绑定库
//#include <boost/smart_ptr.hpp>	  ///使用boost定义的智能指针
#include <boost/array.hpp>        ///使用boost定义的数组
//#include <boost/progress.hpp>     ///使用boost定义的时间类
#include <boost/format.hpp> 	  ///使用boost定义的格式类
#include <boost/any.hpp>          ///使用boost定义的任意容器类
#include <boost/lexical_cast.hpp> ///使用boost定义的类型转换类
#include <boost/date_time.hpp>    ///使用boost定义的日期时间类
#include <boost/thread.hpp>       ///使用boost定义的多线程
#include <boost/foreach.hpp>
//#include <boost/make_shared.hpp>
using namespace boost;            ///有关boost的相关信息可以查阅http://www.boost.org
///End Boost Header Files

#include "../Utility/functions.h"
#include "../Parameters/Parameters.h"
#include <unordered_map>
const double M_SQRT3 = sqrt(3.0);