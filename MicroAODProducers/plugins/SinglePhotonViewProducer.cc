#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDMException.h"

#include "flashgg/MicroAODFormats/interface/SinglePhotonView.h"

#include <vector>
#include <algorithm>

using namespace std;
using namespace edm;

namespace flashgg {

	class SinglePhotonViewProducer : public EDProducer {
		
	public:
		SinglePhotonViewProducer( const ParameterSet & );
	private:
		void produce( Event &, const EventSetup & ) override;
		
		EDGetTokenT<View<DiPhotonCandidate> > diPhotonToken_;
		int maxCandidates_;
		
	};
	
	SinglePhotonViewProducer::SinglePhotonViewProducer(const ParameterSet & iConfig) :
		diPhotonToken_(consumes<View<flashgg::DiPhotonCandidate> >(iConfig.getUntrackedParameter<InputTag> ("DiPhotonTag", InputTag("flashggDiPhotons")))),
		maxCandidates_(iConfig.getUntrackedParameter("maxCandidates",1))
	{
		produces<vector<SinglePhotonView> >();
	}
	
	void SinglePhotonViewProducer::produce( Event & evt, const EventSetup & ) {
		
		Handle<View<flashgg::DiPhotonCandidate> > diPhotons;
		evt.getByToken(diPhotonToken_,diPhotons);
		const PtrVector<flashgg::DiPhotonCandidate>& diPhotonPointers = diPhotons->ptrVector();
		
		std::auto_ptr<vector<SinglePhotonView> > photonViews(new vector<SinglePhotonView>); 
		
		int nCand = maxCandidates_;
		for(auto & dipho : diPhotonPointers) {
			
			SinglePhotonView leg0(dipho,0);
			SinglePhotonView leg1(dipho,1);
			
			if( leg0->pt() < leg1->pt() ) { std::swap(leg0,leg1); }
			
			photonViews->push_back(leg0);
			photonViews->push_back(leg1);
			
			if( --nCand == 0 ) { break; }
		}
		
		//// if( photonViews->size() != 0 ) { 
		//// 	cout << photonViews->size() << endl;
		//// }
		evt.put(photonViews);
		
	}
}

typedef flashgg::SinglePhotonViewProducer FlashggSinglePhotonViewProducer;
DEFINE_FWK_MODULE(FlashggSinglePhotonViewProducer);