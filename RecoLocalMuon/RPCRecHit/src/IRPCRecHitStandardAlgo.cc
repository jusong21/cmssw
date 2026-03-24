/*
 *  See header file for a description of this class.
 *
 *  \author original version: M. Maggi -- INFN
 *  \adated by Juhee Song (Hanyang Univ, Vrije Universiteit Brussel)
 */

#include "RecoLocalMuon/RPCRecHit/interface/IRPCCluster.h"
#include "RecoLocalMuon/RPCRecHit/interface/IRPCRecHitStandardAlgo.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "Geometry/CommonTopologies/interface/TrapezoidalStripTopology.h"

// First Step
bool IRPCRecHitStandardAlgo::compute(const RPCRoll& roll,
                                     const IRPCCluster& cluster,
                                     LocalPoint& Point,
                                     LocalError& error,
                                     float& timeHR,
                                     float& timeHRErr,
									 float& timeLR,
									 float& timeLRErr) const {

    // Get Average Strip position
    const float fstrip = (roll.centreOfStrip(cluster.firstStrip())).x();
    const float lstrip = (roll.centreOfStrip(cluster.lastStrip())).x();
    const float centreOfCluster = (fstrip + lstrip) / 2;
    const double y = cluster.hasY() ? cluster.y() : 0;
    Point = LocalPoint(centreOfCluster, y, 0);

    if (!cluster.hasY()) {
        error = LocalError(roll.localError((cluster.firstStrip() + cluster.lastStrip()) / 2.));
    } else {
        // Use the default one for local x error
        float ex2 = roll.localError((cluster.firstStrip() + cluster.lastStrip()) / 2.).xx();
        // Maximum estimate of local y error, (distance to the boundary)/sqrt(3)
        // which gives consistent error to the default one at y=0
        const float stripLen = roll.specificTopology().stripLength();
        const float maxDy = stripLen / 2 - std::abs(cluster.y());

        // Apply x-position correction for the endcap
        if (roll.id().region() != 0) {
            const auto& topo = dynamic_cast<const TrapezoidalStripTopology&>(roll.topology());
            const double angle = topo.stripAngle((cluster.firstStrip() + cluster.lastStrip()) / 2.);
            const double x = centreOfCluster - y * std::tan(angle);
            Point = LocalPoint(x, y, 0);

            // rescale x-error by the change of local pitch
            const double scale = topo.localPitch(Point) / topo.pitch();
            ex2 *= scale * scale;
        }

        error = LocalError(ex2, 0, maxDy * maxDy / 3.);
    }

//  if (cluster.hasTime()) {
//    time = cluster.time();
//    timeErr = cluster.timeRMS();
//  } else {
//    time = 0;
//    timeErr = -1;
//  }

    // Keep HR/LR times separately in the rechit.
    if (cluster.hasHighTime() && cluster.hasLowTime()) {
        timeHR = cluster.highTime();
        timeHRErr = cluster.highTimeRMS();
        timeLR = cluster.lowTime();
        timeLRErr = cluster.lowTimeRMS();
    } else {
        timeHR = 0;
        timeHRErr = -1;
        timeLR = 0;
        timeLRErr = -1;
    }

    return true;
}

bool IRPCRecHitStandardAlgo::compute(const RPCRoll& roll,
                                     const IRPCCluster& cl,
                                     const float& angle,
                                     const GlobalPoint& globPos,
                                     LocalPoint& Point,
                                     LocalError& error,
                                     float& timeHR,
                                     float& timeHRErr,
									 float& timeLR,
									 float& timeLRErr) const {
    this->compute(roll, cl, Point, error, timeHR, timeHRErr, timeLR, timeLRErr);
    return true;
}
