#pragma once

#include "Artus/Utility/interface/Utility.h"


#include "Kappa/DataFormats/interface/Kappa.h"
#include "HiggsAnalysis/KITHiggsToTauTau/interface/HttTypes.h"
#include "Artus/Utility/interface/DefaultValues.h"

/**
   \brief
*/

class RecoTauCPProducer : public ProducerBase<HttTypes> {
public:

	typedef typename HttTypes::event_type event_type;
	typedef typename HttTypes::product_type product_type;
	typedef typename HttTypes::setting_type setting_type;
	
	virtual std::string GetProducerId() const override;
	
	virtual void Init(setting_type const& settings)  override;

	virtual void Produce(event_type const& event, product_type& product, setting_type const& settings) const override;
};
