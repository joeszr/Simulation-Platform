//
// Created by ChTY on 2022/10/7.
//

#ifndef CHTY_MIXEDTRAFFIC_H
#define CHTY_MIXEDTRAFFIC_H
#include "TrafficModel.h"
#include "MixedTraffic_MS.h"

class MixedTraffic: public TrafficModel {
private:
    unordered_map<int, MixedTraffic_MS> MSID2Traffic;
    bool xrSingleOn, xrMultiOn, ftp3On, DatabaseOn, fullbufferOn;  //业务模型开关
public:
    void WorkSlot() override;
public:
    void OutputTrafficInfo() override;
    explicit MixedTraffic() =default;
    MixedTraffic(bool _xrSingleOn, bool _xrMultiOn, bool _ftp3On, bool _fullbufferOn, bool _DatabaseOn):
            xrSingleOn(_xrSingleOn), xrMultiOn(_xrMultiOn), ftp3On(_ftp3On), fullbufferOn(_fullbufferOn), DatabaseOn(_DatabaseOn){}
};


#endif //CHTY_MIXEDTRAFFIC_H
