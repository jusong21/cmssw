/*!
\file
\brief Body file with definitions of IRPCHit functions.
\authors Shchablo Konstantin (shchablo@gmail.com)
\version 1.0
\copyright Copyright 2019 Shchablo Konstantin.
\license This file is released under the GNU General Public License v3.0.
\date May 2019

\modified by Juhee Song (Hanyang Univ, Vrije Universiteit Brussel), Feb. 2025
*/
#include "RecoLocalMuon/RPCRecHit/interface/IRPCHit.h"
#include <cmath>


IRPCHit::IRPCHit() { _channel = -1; _strip = -1; _time = -1; _isHR = false; _isLR = false; }

// *** I think we don't need channels
IRPCHit::IRPCHit(int strip, float time, int bunchx) :
				_strip(strip), _time(time), _bunchx(bunchx) {_isHR = false; _isLR = false;}
IRPCHit::IRPCHit(int strip, float time) :
				_strip(strip), _time(time) {_isHR = false; _isLR = false;}


IRPCHit::IRPCHit(int channel, int strip, float time) :
                 _channel(channel), _strip(strip), _time(time) { _isHR = false; _isLR = false; }
IRPCHit::IRPCHit(int channel, int strip, float time, int bunchx) :
                 _channel(channel), _strip(strip), _time(time), _bunchx(bunchx) { _isHR = false; _isLR = false; }
IRPCHit::IRPCHit(int channel, int strip, float time, int bunchx, float digiY) :
                 _channel(channel), _strip(strip), _time(time), _bunchx(bunchx), _digiY{digiY} { _isHR = false; _isLR = false; }
//IRPCHit::~IRPCHit() {}

int IRPCHit::bx() const { return _bunchx; }
int IRPCHit::strip() const { return _strip; }
//int IRPCHit::channel() { return _channel; }
float IRPCHit::time() const { return _time; }
//float IRPCHit::digiY() { return _digiY; }

void IRPCHit::setHR(bool is) { _isHR = is; };
void IRPCHit::setLR(bool is) { _isLR = is; };

bool IRPCHit::isHR() const { return _isHR; };
bool IRPCHit::isLR() const { return _isLR; };

bool IRPCHit::isAdjacentStrip( IRPCHit& refHit, int maxStripJump ) {
	return std::abs( this->strip() - refHit.strip() ) <= 1+maxStripJump;
}
bool IRPCHit::isAdjacentTime( IRPCHit& refHit, float thrTime ) {
	return std::abs( this->time() - refHit.time() ) < thrTime;
}
