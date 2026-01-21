//
// Created by ChTY on 2022/9/21.
//

#include "Random.h"
#include <random>
#include <mutex>
#include <cassert>
using namespace std;

double Random::xUniform(double _dmin, double _dmax) {
    //static std::mutex lock;
    //std::lock_guard<std::mutex> l(lock);
    assert(_dmax >= _dmin);
    unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xUniform(seed);
    uniform_real_distribution<double> _xUnifrom(_dmin, _dmax);
    return _xUnifrom(engine_xUniform);
}

double Random::xUniform_channel(double _dmin, double _dmax) {
    //static std::mutex lock;
    //std::lock_guard<std::mutex> l(lock);
    assert(_dmax >= _dmin);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xUniform_channel(seed);
    uniform_real_distribution<double> _xUniform_channel(_dmin, _dmax);
    return _xUniform_channel(engine_xUniform_channel);
}

double Random::xUniform_distributems(double _dmin, double _dmax) {
//    static std::mutex lock;
//    std::lock_guard<std::mutex> l(lock);
    assert(_dmax >= _dmin);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xUniform_distributems(seed);
    uniform_real_distribution<double> _xUniform_distributems(_dmin, _dmax);
    return _xUniform_distributems(engine_xUniform_distributems);
}

double Random::xUniform_detection(double _dmin, double _dmax) {
//    static std::mutex lock;
//    std::lock_guard<std::mutex> l(lock);
    assert(_dmax >= _dmin);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xUniform_detection(seed);
    uniform_real_distribution<double> _xUniform_detection(_dmin, _dmax);
    return _xUniform_detection(engine_xUniform_detection);
}

double Random::xUniform_AntCalibrationError(double _dmin, double _dmax) {
//    static std::mutex lock;
//    std::lock_guard<std::mutex> l(lock);
    assert(_dmax >= _dmin);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xUniform_AntCalibrationError(seed);

    uniform_real_distribution<double> _xUniform_AntCalibrationError(_dmin, _dmax);
    return _xUniform_AntCalibrationError(engine_xUniform_AntCalibrationError);
}

double Random::xUniform_distributepico(double _dmin, double _dmax) {
//    static std::mutex lock;
//    std::lock_guard<std::mutex> l(lock);
    assert(_dmax >= _dmin);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xUniform_distributepico(seed);
    uniform_real_distribution<double> _xUniform_distributepico(_dmin, _dmax);
    return _xUniform_distributepico(engine_xUniform_distributepico);
}

double Random::xUniform_msconstruct(double _dmin, double _dmax) {
    //static std::mutex lock;
    //std::lock_guard<std::mutex> l(lock);
    assert(_dmax >= _dmin);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xUniform_msconstruct(seed);
    uniform_real_distribution<double> _xUniform_msconstruct(_dmin, _dmax);
    return _xUniform_msconstruct(engine_xUniform_msconstruct);
}

//UL_begin

double Random::xUniform_Detection(double _dmin, double _dmax) {
    //static std::mutex lock;
    //std::lock_guard<std::mutex> l(lock);
    assert(_dmax >= _dmin);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xUniform_Detection(seed);
    uniform_real_distribution<double> _xUniform_Detection(_dmin, _dmax);
    return _xUniform_Detection(engine_xUniform_Detection);
}

double Random::xUniform_DiffLoad(double _dmin, double _dmax) {
    //static std::mutex lock;
    //std::lock_guard<std::mutex> l(lock);
    assert(_dmax >= _dmin);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xUniform_differentLoad(seed);
    uniform_real_distribution<double> _xUniform_differentLoad(_dmin, _dmax);
    return _xUniform_differentLoad(engine_xUniform_differentLoad);
}

double Random::xNormal_error(double _dave, double _dstd) {
    //static std::mutex lock;
    //std::lock_guard<std::mutex> l(lock);
    assert(_dstd >= 0);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xNormal_error(seed);
    std::normal_distribution<double> _xNormal_error(_dave, _dstd);
    return _xNormal_error(engine_xNormal_error);
}
//UL_end

double Random::xNormal(double _dave, double _dstd) {
    //static std::mutex lock;
    //std::lock_guard<std::mutex> l(lock);
    assert(_dstd >= 0);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xNormal(seed);
    std::normal_distribution<double> _xNormal(_dave, _dstd);
    return _xNormal(engine_xNormal);
}

double Random::xNormal_channel(double _dave, double _dstd) {
    //static std::mutex lock;
    //std::lock_guard<std::mutex> l(lock);
    assert(_dstd >= 0);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xNormal_channel(seed);
    std::normal_distribution<double> _xNormal_channel(_dave, _dstd);
    return _xNormal_channel(engine_xNormal_channel);
}

double Random::xNormal_SRSError(double _dave, double _dstd) {
    //static std::mutex lock;
    //std::lock_guard<std::mutex> l(lock);
    assert(_dstd >= 0);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xNormal_SRSError(seed);
    std::normal_distribution<double> _xNormal_SRSError(_dave, _dstd);
    return _xNormal_SRSError(engine_xNormal_SRSError);
}

double Random::xNormal_DMRSError(double _dave, double _dstd) {
    //static std::mutex lock;
    //std::lock_guard<std::mutex> l(lock);
    assert(_dstd >= 0);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xNormal_DMRSError(seed);
    std::normal_distribution<double> _xNormal_DMRSError(_dave, _dstd);
    return _xNormal_DMRSError(engine_xNormal_DMRSError);
}

double Random::xNormal_AntCalibrationError(double _dave, double _dstd) {
    //static std::mutex lock;
    //std::lock_guard<std::mutex> l(lock);
    assert(_dstd >= 0);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xNormal_AntCalibrationError(seed);
    std::normal_distribution<double> _xNormal_AntCalibrationError(_dave, _dstd);
    return _xNormal_AntCalibrationError(engine_xNormal_AntCalibrationError);
}

double Random::xNormal_Other(double _dave, double _dstd) {
    //static std::mutex lock;
    //std::lock_guard<std::mutex> l(lock);
    assert(_dstd >= 0);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xNormal_Other(seed);
    std::normal_distribution<double> _xNormal_Other(_dave, _dstd);
    return _xNormal_Other(engine_xNormal_Other);
}

double Random::xNormal_msconstruct(double _dave, double _dstd) {
    //static std::mutex lock;
    //std::lock_guard<std::mutex> l(lock);
    assert(_dstd >= 0);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xNormal_msconstruct(seed);
    std::normal_distribution<double> _xNormal_msconstruct(_dave, _dstd);
    return _xNormal_msconstruct(engine_xNormal_msconstruct);
}

int Random::xUniformInt(const int _imin,const int _imax) {
    //static std::mutex lock;
    //std::lock_guard<std::mutex> l(lock);
    assert(_imax >= _imin);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xUniformInt(seed);
    uniform_int_distribution<int> _xUniformInt(_imin, _imax);
    return _xUniformInt(engine_xUniformInt);
}

double Random::xExponent(double _dlamda) {
    //static std::mutex lock;
    //std::lock_guard<std::mutex> l(lock);
    assert(_dlamda >= 0);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_newExponent(seed);
    std::exponential_distribution<double> _newExponent(_dlamda);
    return _newExponent(engine_newExponent);
}

int Random::xPossion_DL(double _ave) {
    //static std::mutex lock;
    //std::lock_guard<std::mutex> l(lock);
    assert(_ave >= 0);
        unsigned seed;
    if(_on){
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    else{
        seed = pseudo_seed++;
    }
    default_random_engine engine_xPossion_DL(seed);

    std::poisson_distribution<int> _xPossion_DL(_ave);
    return _xPossion_DL(engine_xPossion_DL);
}