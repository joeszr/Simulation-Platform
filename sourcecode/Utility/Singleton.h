/**
 * @file	BasePrj\sourcecode\Utility\Singleton.h
 * @brief	Declares the singleton class.
 */

#pragma once


/**
 * @class	singleton
 * @brief	singleton pattern for C++ 11.
 * @author	keting
 * @date	2017/2/4
 * @tparam	T	Generic type parameter.
 */
template<class T>
class singleton {
public:
        static T& Instance()
        {
                static T* _instance = new T();
                return *_instance;
        }
        singleton(singleton const&) = delete;
        singleton& operator=(singleton const&) = delete;
protected:
        singleton() = default;
        ~singleton() = default;
};
