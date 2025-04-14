/*
 *  See header file for a description of this class.
 *
 *  \author original version: M. Maggi -- INFN
 *  \adated by Juhee Song (Hanyang Univ, Vrije Universiteit Brussel)
 */

#include "RecoLocalMuon/IRPCRecHit/interface/IRPCCluster.h"
#include "RecoLocalMuon/IRPCRecHit/plugins/IRPCRecHitStandardAlgo.h"
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

	std::cout << std::endl;
	std::cout << " ******************** HitStandardAlgo ******************** " << std::endl;
	std::cout << " - cluster.firstStrip(): " << cluster.firstStrip() << std::endl;
	std::cout << " - roll.centreOfStrip(cluster.firstStrip()): " << roll.centreOfStrip(cluster.firstStrip()) << std::endl;
	std::cout << " - cluster.lastStrip(): " << cluster.lastStrip() << std::endl;
	std::cout << " - roll.centreOfStrip(cluster.lastStrip()): " << roll.centreOfStrip(cluster.lastStrip()) << std::endl;

    // Get Average Strip position
    const float fstrip = (roll.centreOfStrip(cluster.firstStrip())).x();
    const float lstrip = (roll.centreOfStrip(cluster.lastStrip())).x();
    const float centreOfCluster = (fstrip + lstrip) / 2;
    const double y = cluster.hasY() ? cluster.y() : 0;
    Point = LocalPoint(centreOfCluster, y, 0);

	std::cout << " - Strip position" << std::endl;
	std::cout << "   fs: " << fstrip << " ls: " << lstrip << " cen: " << centreOfCluster << " y: " << y << std::endl;

    if (!cluster.hasY()) {
        error = LocalError(roll.localError((cluster.firstStrip() + cluster.lastStrip()) / 2.));
		std::cout << " - (!hasY) error: " << error << std::endl;
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
		std::cout << " - ex2: " << ex2 << " error: " << error << std::endl;
    }

//  if (cluster.hasTime()) {
//    time = cluster.time();
//    timeErr = cluster.timeRMS();
//  } else {
//    time = 0;
//    timeErr = -1;
//  }

    // FIXME iRPC has two times...
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

	std::cout << " - timeHR: " << timeHR << " timeHRErr: " << timeHRErr << std::endl;
	std::cout << " - timeLR: " << timeLR << " timeLRErr: " << timeLRErr << std::endl;

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
