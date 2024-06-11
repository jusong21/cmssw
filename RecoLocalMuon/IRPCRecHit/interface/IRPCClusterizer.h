/*!
\file
\brief File with definitions IRPCClusterizer class.
\authors Shchablo Konstantin (IPNL-France) (shchablo@gmail.com)
\version 1.0
\copyright Copyright 2019 Shchablo Konstantin.
\license This file is released under the GNU General Public License v3.0.
\date May 2019
*/

#ifndef RecoLocalMuon_IRPCClusterizer_h
#define RecoLocalMuon_IRPCClusterizer_h

/*!
    \brief In this class defined functions to fulfillment clustering.
    \author Shchablo (IPNL-France)
    \version 1.0
    \date May 2019
*/
/* IRPC */
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCClusterContainer.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCCluster.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCInfo.h"
/* CMSSW */
#include "DataFormats/IRPCDigi/interface/IRPCDigiCollection.h"
#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Geometry/CommonTopologies/interface/TrapezoidalStripTopology.h"
/* std */
#include <vector>
#include <utility>

class IRPCClusterizer 
{
    public:

        /* \brief Constructor.*/
        IRPCClusterizer();
        /*! \brief Destructor. */
        ~IRPCClusterizer();

        /*!
            \brief The function of clustering hits from a single side of the chamber.
            \param[in] thrTime - The threshold for time clustering between two hits (Chain checking of hits).
            \param[out] clusters - Container of output clusters.
            \return Fulfillment status.
        */
        bool clustering(float thrTime, IRPCHitContainer &hits, IRPCClusterContainer &clusters);

        /*!
            \brief The function of the association of clusters from high radius and low radius radius.
            \param[in] info - paramitors for clustering..
            \param[in] hr - Container of clusters correspond high radius of the chamber.
            \param[in] lr - Container of clusters correspond low radius of the chamber.
            \return Container of clusters.
        */
        IRPCClusterContainer association(IRPCInfo &info, IRPCClusterContainer hr, IRPCClusterContainer lr);

		//IRPCClusterContainer oneSideCluster(IRPCHitContainer &hitsOneSide, float limit);
		bool oneSideCluster(float limit, IRPCHitContainer &hitsOneSide, IRPCClusterContainer &clusters);
		IRPCClusterContainer finalCluster(IRPCClusterContainer LR, IRPCClusterContainer HR);


        /* CMSSW */
        /*!
            \brief The action function.
            \param[in] digiRange - simulated data.
            \return Container of clusters.
        */
        //IRPCClusterContainer doAction(const RPCDigiCollection::Range& digiRange, IRPCInfo& info);
        IRPCClusterContainer doAction(const RPCRoll& roll, const IRPCDigiCollection::Range& digiRange, IRPCInfo& info);
};

#endif // RecoLocalMuon_IRPCClusterClusterizer_h
