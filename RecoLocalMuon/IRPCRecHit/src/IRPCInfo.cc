/*!
\file
\brief Body file with definitions of IRPCInfo functions.
\authors Shchablo Konstantin (shchablo@gmail.com)
\version 1.0
\copyright Copyright 2019 Shchablo Konstantin.
\license This file is released under the GNU General Public License v3.0.
\date May 2019
*/
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCInfo.h"

/* std */
#include <limits>

IRPCInfo::IRPCInfo()
{
    _isAND = true;
    _isUse = true;
    _isHR = false; _isLR = false;
    _speed = 19.786302;
//    _thrTimeHR = std::numeric_limits<float>::max();
//    _thrTimeLR = std::numeric_limits<float>::max();
    _thrTimeHR = 0.3;
    _thrTimeLR = 0.3;
    _thrDeltaTimeMin = std::numeric_limits<float>::min();
    _thrDeltaTimeMax = std::numeric_limits<float>::max();
    _count = 0;
}

IRPCInfo::~IRPCInfo()
{
}

bool IRPCInfo::isHR(int channel, int &strip)
{
    auto it = _HR.find(channel);
    if(it == _HR.end()) return false;
    strip = it->second;
    return true;
}
bool IRPCInfo::isLR(int channel, int &strip)
{
    auto it = _LR.find(channel);
    if(it == _LR.end()) return false;
    strip = it->second;
    return true;
}

void IRPCInfo::setMapHR(std::map<int, int> &map) { _HR = map; }
void IRPCInfo::setMapLR(std::map<int, int> &map) { _LR = map; }

std::map<int, int> IRPCInfo::getMapHR() { return _HR; }
std::map<int, int> IRPCInfo::getMapLR() {return _LR; }

float IRPCInfo::thrTimeHR() { return _thrTimeHR; }
float IRPCInfo::thrTimeLR() { return _thrTimeLR; }

void IRPCInfo::setThrTimeHR(float thrTime) { _thrTimeHR = thrTime; }
void IRPCInfo::setThrTimeLR(float thrTime) { _thrTimeLR = thrTime; }

void IRPCInfo::setThrDeltaTimeMin(float thrTime) { _thrDeltaTimeMin = thrTime; }
void IRPCInfo::setThrDeltaTimeMax(float thrTime) { _thrDeltaTimeMax = thrTime; }
void IRPCInfo::setThrDeltaTimeY(float thrTime) { _thrDeltaTimeY = thrTime; }

float IRPCInfo::thrDeltaTimeMin() { return _thrDeltaTimeMin; }
float IRPCInfo::thrDeltaTimeMax() { return _thrDeltaTimeMax; }
float IRPCInfo::thrDeltaTimeY() { return _thrDeltaTimeY; }

float IRPCInfo::speed() { return _speed; }
void IRPCInfo::setSpeed(float speed) { _speed = speed; }

bool IRPCInfo::isReturnOnlyAND() { return _isAND; }
void IRPCInfo::isReturnOnlyAND(bool is) { _isAND = is; };

bool IRPCInfo::isUseIRPCAlgorithm() { return _isUse; }
void IRPCInfo::isUseIRPCAlgorithm(bool is) { _isUse = is; };

bool IRPCInfo::isOnlyHR() { return _isHR; }
void IRPCInfo::isOnlyHR(bool is) { _isHR = is; };

bool IRPCInfo::isOnlyLR() { return _isLR; }
void IRPCInfo::isOnlyLR(bool is) { _isLR = is; };

int IRPCInfo::counter() { _count = _count + 1; return _count; };
