/*
 * See header file for a description of this class.
 *
 * \author J. Shin -- Kyung Hee University
 */

#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "Geometry/CommonTopologies/interface/TrapezoidalStripTopology.h"

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
  const float midStrip = clusterMiddleStrip(cluster.firstStrip(), cluster.lastStrip());
  const float x0 = clusterCenterX(roll, cluster.firstStrip(), cluster.lastStrip());
  const float y0 = cluster.hasY() ? cluster.y() : 0.f;

  float x = x0;
  float y = y0;
  float ex2 = roll.localError(midStrip).xx();

  // Endcap: y0 is along-strip distance from timing; y = y0 * cos(theta) is roll local y.
  // x = x0 - y * tan(theta) = x0 - y0 * sin(theta).
  if (roll.id().region() != 0) {
    const auto& topo = dynamic_cast<const TrapezoidalStripTopology&>(roll.topology());
    const double angle = topo.stripAngle(midStrip);
    const float cosAngle = static_cast<float>(std::cos(angle));
    y = y0 * cosAngle;
    x = x0 - y * static_cast<float>(std::tan(angle));

    const double scale = topo.localPitch(LocalPoint(x, y, 0.f)) / topo.pitch();
    ex2 *= static_cast<float>(scale * scale);
  }

  point = LocalPoint(x, y, 0.f);

  // y-error: distance to strip end along strip axis (use y0, not projected y).
  const float stripLen = roll.specificTopology().stripLength();
  const float maxDy = stripLen / 2.f - std::abs(y0);
  const float ey2 = maxDy * maxDy / 3.f;

  error = LocalError(ex2, 0.f, ey2);

  if (cluster.hasTime()) {
    time = cluster.time();
    timeErr = cluster.timeRMS();
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