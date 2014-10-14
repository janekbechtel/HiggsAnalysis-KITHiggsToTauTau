
#include "HiggsAnalysis/KITHiggsToTauTau/interface/Producers/HttValidJetsProducer.h"


bool HttValidJetsProducer::AdditionalCriteria(KDataPFJet* jet,
                                              event_type const& event, product_type& product,
                                              setting_type const& settings) const
{
	bool validJet = ValidJetsProducer::AdditionalCriteria(jet, event, product, settings);
	
	return validJet;
}


bool HttValidTaggedJetsProducer::AdditionalCriteria(KDataPFTaggedJet* jet,
                                                    event_type const& event, product_type& product,
                                                    setting_type const& settings) const
{
	bool validJet = ValidTaggedJetsProducer::AdditionalCriteria(jet, event, product, settings);
	
	HttSettings const& specSettings = static_cast<HttSettings const&>(settings);
	HttProduct const& specProduct = static_cast<HttProduct const&>(product);
	
	// remove taus from list of jets via simple DeltaR isolation
	// (targeted at ttH analysis, harmless if m_validTTHTaus is not filled)
	for (std::vector<KDataPFTau*>::const_iterator tau = specProduct.m_validTTHTaus.begin();
		validJet && tau != specProduct.m_validTTHTaus.end(); ++tau)
		{
			validJet = validJet && ROOT::Math::VectorUtil::DeltaR(jet->p4, (*tau)->p4) > specSettings.GetJetTauLowerDeltaRCut();
		}

	return validJet;
}
