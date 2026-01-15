/// COPRIGHT NOTICE    
/// Copyright (c) 2010,
/// All right reserved.
///
///@file envconfig.h
///@brief  包含了windows和linux下参数类需要的一些系统库文件
///
///
///
///@author wangxiaozhou
#pragma once
#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#endif
