/*
 * See header file for a description of this class.
 *
 * \author J. Shin -- Kyung Hee University
 */

#include "Geometry/CommonTopologies/interface/TrapezoidalStripTopology.h"
#include "Geometry/RPCGeometry/interface/RPCRoll.h"

#include "RPCRecHitPhase2Algo.h"

#include <cmath>

namespace {
inline float clusterCenterX(const RPCRoll& roll, int firstStrip, int lastStrip) {
  const float x1 = roll.centreOfStrip(firstStrip).x();
  const float x2 = roll.centreOfStrip(lastStrip).x();
  return 0.5f * (x1 + x2);
}

inline float clusterMiddleStrip(int firstStrip, int lastStrip) {
  return 0.5f * (static_cast<float>(firstStrip) + static_cast<float>(lastStrip));
}
}  // namespace

bool RPCRecHitPhase2Algo::compute(const RPCRoll& roll,
                                  const RPCClusterPhase2& cluster,
                                  LocalPoint& point,
                                  LocalError& error,
                                  float& time,
                                  float& timeErr) const {
  const float x = clusterCenterX(roll, cluster.firstStrip(), cluster.lastStrip());

  point = LocalPoint(x, 0.f, 0.f);
  error = roll.localError(clusterMiddleStrip(cluster.firstStrip(), cluster.lastStrip()));

  if (cluster.hasTime()) {
    time = cluster.time();
    timeErr = cluster.timeRMS();
  } else {
    time = 0.f;
    timeErr = -1.f;
  }

  return true;
}

bool RPCRecHitPhase2Algo::compute(const RPCRoll& roll,
                                  const IRPCCluster& cluster,
                                  LocalPoint& point,
                                  LocalError& error,
                                  float& time,
                                  float& timeErr) const {
  const float fstrip = roll.centreOfStrip(cluster.firstStrip()).x();
  const float lstrip = roll.centreOfStrip(cluster.lastStrip()).x();
  const float centreOfCluster = 0.5f * (fstrip + lstrip);
  const double y = cluster.hasY() ? static_cast<double>(cluster.y()) : 0.;

  point = LocalPoint(centreOfCluster, static_cast<float>(y), 0.f);

  if (!cluster.hasY()) {
    error = roll.localError((cluster.firstStrip() + cluster.lastStrip()) / 2);
  } else {
    float ex2 = roll.localError((cluster.firstStrip() + cluster.lastStrip()) / 2).xx();
    const float stripLen = roll.specificTopology().stripLength();
    const float maxDy = stripLen / 2.f - std::abs(cluster.y());

    if (roll.id().region() != 0) {
      const auto& topo = dynamic_cast<const TrapezoidalStripTopology&>(roll.topology());
      const double angle = topo.stripAngle((cluster.firstStrip() + cluster.lastStrip()) / 2);
      const double x = static_cast<double>(centreOfCluster) - y * std::tan(angle);
      point = LocalPoint(static_cast<float>(x), static_cast<float>(y), 0.f);

      const double scale = topo.localPitch(point) / topo.pitch();
      ex2 *= static_cast<float>(scale * scale);
    }

    error = LocalError(ex2, 0.f, maxDy * maxDy / 3.f);
  }

  if (cluster.hasHighTime() && cluster.hasLowTime()) {
    time = 0.5f * (cluster.highTime() + cluster.lowTime());
    timeErr = 0.5f * (std::max(0.f, cluster.highTimeRMS()) + std::max(0.f, cluster.lowTimeRMS()));
  } else if (cluster.hasHighTime()) {
    time = cluster.highTime();
    timeErr = cluster.highTimeRMS();
  } else if (cluster.hasLowTime()) {
    time = cluster.lowTime();
    timeErr = cluster.lowTimeRMS();
  } else {
    time = 0.f;
    timeErr = -1.f;
  }

  return true;
}

RPCRecHit RPCRecHitPhase2Algo::build(const RPCRoll& roll, const RPCDetId& rpcId, const RPCClusterPhase2& cluster) const {
  LocalPoint point;
  LocalError error;
  float time = 0.f;
  float timeErr = -1.f;

  compute(roll, cluster, point, error, time, timeErr);

  RPCRecHit recHit(rpcId, cluster.bx(), cluster.firstStrip(), cluster.clusterSize(), point, error);
  recHit.setTimeAndError(time, timeErr);

  return recHit;
}

RPCRecHit RPCRecHitPhase2Algo::build(const RPCRoll& roll, const RPCDetId& rpcId, const IRPCCluster& cluster) const {
  LocalPoint point;
  LocalError error;
  float time = 0.f;
  float timeErr = -1.f;

  compute(roll, cluster, point, error, time, timeErr);

  RPCRecHit recHit(rpcId, cluster.bx(), cluster.firstStrip(), cluster.clusterSize(), point, error);
  recHit.setTimeAndError(time, timeErr);

  return recHit;
}
