///@file Directory.h
///@brief 为系统定义了所有输出文件的输出目录
///@author dushaofeng
#pragma once
#include "../Parameters/envconfig.h"
#include <ctime>
#include <cstdio>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string.hpp>
///Directory类为系统定义了所有输出文件的输出路径

class Directory {
private:
    boost::filesystem::path m_PrePath;

public:
    /// Get the full path name.
    boost::filesystem::path GetPath(const std::string& _filename);
    void static SetOutputPath(char* _casename[]);
    void Initialize();

private:
    static Directory* m_pDir;
    static char cOutputPath[260];
public:
    /// This function can be invoked to return a uniqe object of this class.
    /** The "Singleton Pattern" is applied to implement this class, The construction and
    destruction function is declaired as private members, so there is no other way to
    generate any object of this class. Users have to invoke this function to get the
    uniqe object of this class. */
    static Directory& Instance();

private:
    /// Constructor.
    Directory() = default;
    /// Copy constructor.
    Directory(const Directory&);
    /// Assignment.
    Directory & operator =(const Directory&);
    /// Destructor.
    ~Directory() = default;
};

