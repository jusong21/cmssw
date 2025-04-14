#include <memory>
#include <string>
#include <iostream>

#include "DataFormats/IRPCRecHit/interface/IRPCRecHit.h"
#include "DataFormats/IRPCRecHit/interface/IRPCRecHitCollection.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "TH1D.h"

class IRPCRecHitReader : public edm::one::EDAnalyzer<> {

	public:
		explicit IRPCRecHitReader(const edm::ParameterSet & conf);
		virtual ~IRPCRecHitReader();
	
		virtual void analyze(const edm::Event& e, const edm::EventSetup& c) override;
	
	private:
		// member data
		edm::EDGetTokenT<IRPCRecHitCollection> irpcRecHitToken;
		TH1D* h_test;
	
	#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
		edm::ESGetToken<SetupData, SetupRecord> setupToken_;
	#endif
};

IRPCRecHitReader::IRPCRecHitReader(const edm::ParameterSet& iConfig) {
	#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
		setupDataToken_ = esConsumes<SetupData, SetupRecord>();
	#endif

	irpcRecHitToken = consumes<IRPCRecHitCollection>(iConfig.getParameter<edm::InputTag>("irpcRecHitTag"));
	//usesResource("TFileService");
	edm::Service<TFileService> fs;

	h_test = fs->make<TH1D>("h_test", "h_test", 20, 0, 20);
}

void IRPCRecHitReader::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	
	using namespace edm;

	edm::Handle<IRPCRecHitCollection> irpcRecHits;
	iEvent.getByToken(irpcRecHitToken, irpcRecHits);

	std::cout << "Events analyzed #Run " << iEvent.id().run() << "#Events: " << iEvent.id().event() << std::endl;

	int nHit = 0;

	//IRPCRecHitCollection::const_iterator recIt;

	for (auto recIt = irpcRecHits->begin(); recIt != irpcRecHits->end(); recIt++) {
		//RPCDetId id = (RPCDetId)(*recIt).rpcId();

		nHit++;
		//std::cout << "detId: " << id << std::endl;
	}
	std::cout << "nHits: " << nHit << std::endl;
	h_test->Fill(nHit);
}

IRPCRecHitReader::~IRPCRecHitReader() {
	std::cout << "done" << std::endl;
}

DEFINE_FWK_MODULE(IRPCRecHitReader);
	
