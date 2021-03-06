
#include "Artus/Utility/interface/DefaultValues.h"

#include "HiggsAnalysis/KITHiggsToTauTau/interface/Producers/HttValidElectronsProducer.h"
#include "HiggsAnalysis/KITHiggsToTauTau/interface/Utility/ParticleIsolation.h"


HttValidElectronsProducer::HttValidElectronsProducer(std::vector<KElectron*> product_type::*validElectrons,
                                                     std::vector<KElectron*> product_type::*invalidElectrons,
                                                     std::string (setting_type::*GetElectronID)(void) const,
                                                     std::string (setting_type::*GetElectronIDType)(void) const,
                                                     std::string (setting_type::*GetElectronIDName)(void) const,
                                                     float (setting_type::*GetElectronMvaIDCutEB1)(void) const,
                                                     float (setting_type::*GetElectronMvaIDCutEB2)(void) const,
                                                     float (setting_type::*GetElectronMvaIDCutEE)(void) const,
                                                     std::string (setting_type::*GetElectronIsoType)(void) const,
                                                     std::string (setting_type::*GetElectronIso)(void) const,
                                                     std::string (setting_type::*GetElectronReco)(void) const,
                                                     std::vector<std::string>& (setting_type::*GetLowerPtCuts)(void) const,
                                                     std::vector<std::string>& (setting_type::*GetUpperAbsEtaCuts)(void) const,
                                                     float (setting_type::*GetElectronChargedIsoVetoConeSizeEB)(void) const,
                                                     float (setting_type::*GetElectronChargedIsoVetoConeSizeEE)(void) const,
                                                     float (setting_type::*GetElectronNeutralIsoVetoConeSize)(void) const,
                                                     float (setting_type::*GetElectronPhotonIsoVetoConeSizeEB)(void) const,
                                                     float (setting_type::*GetElectronPhotonIsoVetoConeSizeEE)(void) const,
                                                     float (setting_type::*GetElectronDeltaBetaIsoVetoConeSize)(void) const,
                                                     float (setting_type::*GetElectronChargedIsoPtThreshold)(void) const,
                                                     float (setting_type::*GetElectronNeutralIsoPtThreshold)(void) const,
                                                     float (setting_type::*GetElectronPhotonIsoPtThreshold)(void) const,
                                                     float (setting_type::*GetElectronDeltaBetaIsoPtThreshold)(void) const,
                                                     float (setting_type::*GetElectronIsoSignalConeSize)(void) const,
                                                     float (setting_type::*GetElectronDeltaBetaCorrectionFactor)(void) const,
                                                     float (setting_type::*GetElectronIsoPtSumOverPtLowerThresholdEB)(void) const,
                                                     float (setting_type::*GetElectronIsoPtSumOverPtLowerThresholdEE)(void) const,
                                                     float (setting_type::*GetElectronIsoPtSumOverPtUpperThresholdEB)(void) const,
                                                     float (setting_type::*GetElectronIsoPtSumOverPtUpperThresholdEE)(void) const,
                                                     float (setting_type::*GetElectronTrackDxyCut)(void) const,
                                                     float (setting_type::*GetElectronTrackDzCut)(void) const,
                                                     std::vector<std::string>& (setting_type::*GetElectronIDList)(void) const) :
	ValidElectronsProducer(validElectrons, invalidElectrons,
	                       GetElectronID, GetElectronIsoType, GetElectronIso, GetElectronReco,
	                       GetLowerPtCuts, GetUpperAbsEtaCuts),
	GetElectronIDType(GetElectronIDType),
	GetElectronIDName(GetElectronIDName),
	GetElectronMvaIDCutEB1(GetElectronMvaIDCutEB1),
	GetElectronMvaIDCutEB2(GetElectronMvaIDCutEB2),
	GetElectronMvaIDCutEE(GetElectronMvaIDCutEE),
	GetElectronChargedIsoVetoConeSizeEB(GetElectronChargedIsoVetoConeSizeEB),
	GetElectronChargedIsoVetoConeSizeEE(GetElectronChargedIsoVetoConeSizeEE),
	GetElectronNeutralIsoVetoConeSize(GetElectronNeutralIsoVetoConeSize),
	GetElectronPhotonIsoVetoConeSizeEB(GetElectronPhotonIsoVetoConeSizeEB),
	GetElectronPhotonIsoVetoConeSizeEE(GetElectronPhotonIsoVetoConeSizeEE),
	GetElectronDeltaBetaIsoVetoConeSize(GetElectronDeltaBetaIsoVetoConeSize),
	GetElectronChargedIsoPtThreshold(GetElectronChargedIsoPtThreshold),
	GetElectronNeutralIsoPtThreshold(GetElectronNeutralIsoPtThreshold),
	GetElectronPhotonIsoPtThreshold(GetElectronPhotonIsoPtThreshold),
	GetElectronDeltaBetaIsoPtThreshold(GetElectronDeltaBetaIsoPtThreshold),
	GetElectronIsoSignalConeSize(GetElectronIsoSignalConeSize),
	GetElectronDeltaBetaCorrectionFactor(GetElectronDeltaBetaCorrectionFactor),
	GetElectronIsoPtSumOverPtLowerThresholdEB(GetElectronIsoPtSumOverPtLowerThresholdEB),
	GetElectronIsoPtSumOverPtLowerThresholdEE(GetElectronIsoPtSumOverPtLowerThresholdEE),
	GetElectronIsoPtSumOverPtUpperThresholdEB(GetElectronIsoPtSumOverPtUpperThresholdEB),
	GetElectronIsoPtSumOverPtUpperThresholdEE(GetElectronIsoPtSumOverPtUpperThresholdEE),
	GetElectronTrackDxyCut(GetElectronTrackDxyCut),
	GetElectronTrackDzCut(GetElectronTrackDzCut),
	GetElectronIDList(GetElectronIDList)
{
}

void HttValidElectronsProducer::Init(setting_type const& settings)
{
	ValidElectronsProducer<HttTypes>::Init(settings);
	
	electronIDType = ToElectronIDType(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy((settings.*GetElectronIDType)())));

	electronIDInMetadata = false;
	electronIDListInMetadata = false;

	electronIDName = (settings.*GetElectronIDName)();
	electronIDList = (settings.*GetElectronIDList)();

	electronMvaIDCutEB1 = (settings.*GetElectronMvaIDCutEB1)();
	electronMvaIDCutEB2 = (settings.*GetElectronMvaIDCutEB2)();
	electronMvaIDCutEE = (settings.*GetElectronMvaIDCutEE)();

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<HttTypes>::AddFloatQuantity("leadingEleIso", [this](event_type const& event, product_type const& product) {
		return product.m_validElectrons.size() >= 1 ? SafeMap::GetWithDefault(product.m_electronIsolation, product.m_validElectrons[0], DefaultValues::UndefinedDouble) : DefaultValues::UndefinedDouble;
	});
	LambdaNtupleConsumer<HttTypes>::AddFloatQuantity("leadingEleIsoOverPt", [this](event_type const& event, product_type const& product) {
		return product.m_validElectrons.size() >= 1 ? SafeMap::GetWithDefault(product.m_electronIsolationOverPt, product.m_validElectrons[0], DefaultValues::UndefinedDouble) : DefaultValues::UndefinedDouble;
	});
	LambdaNtupleConsumer<HttTypes>::AddFloatQuantity("id_e_mva_nt_loose_1", [this](event_type const& event, product_type const& product)
	{
		return (product.m_validElectrons.size() >= 1 && electronIDType != ElectronIDType::NONE) ? product.m_validElectrons[0]->getId(electronIDList.at(0), event.m_electronMetadata) : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<HttTypes>::AddFloatQuantity("id_e_cut_veto_1", [this](event_type const& event, product_type const& product)
	{
		return (product.m_validElectrons.size() >= 1 && electronIDType != ElectronIDType::NONE) ? product.m_validElectrons[0]->getId(electronIDList.at(1), event.m_electronMetadata) : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<HttTypes>::AddFloatQuantity("id_e_cut_loose_1", [this](event_type const& event, product_type const& product)
	{
		return (product.m_validElectrons.size() >= 1 && electronIDType != ElectronIDType::NONE) ? product.m_validElectrons[0]->getId(electronIDList.at(2), event.m_electronMetadata) : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<HttTypes>::AddFloatQuantity("id_e_cut_medium_1", [this](event_type const& event, product_type const& product)
	{
		return (product.m_validElectrons.size() >= 1 && electronIDType != ElectronIDType::NONE) ? product.m_validElectrons[0]->getId(electronIDList.at(3), event.m_electronMetadata) : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<HttTypes>::AddFloatQuantity("id_e_cut_tight_1", [this](event_type const& event, product_type const& product)
	{
		return (product.m_validElectrons.size() >= 1 && electronIDType != ElectronIDType::NONE) ? product.m_validElectrons[0]->getId(electronIDList.at(4), event.m_electronMetadata) : DefaultValues::UndefinedFloat;
	});
}

bool HttValidElectronsProducer::AdditionalCriteria(KElectron* electron,
                                                   event_type const& event, product_type& product,
                                                   setting_type const& settings) const
{
	assert(event.m_vertexSummary);
	
	bool validElectron = ValidElectronsProducer<HttTypes>::AdditionalCriteria(electron, event, product, settings);
	
	double isolationPtSum = DefaultValues::UndefinedDouble;
	
	// require no missing inner hits
	if (validElectron && electronReco == ElectronReco::USER) {
		validElectron = validElectron && (electron->track.nInnerHits == 0);
	}

	// custom WPs for electron ID
	// https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauWorkingSummer2013#Electron_ID
	if (validElectron && electronID == ElectronID::USER) {
		if (electronIDType == ElectronIDType::SUMMER2013LOOSE)
		{
			validElectron = validElectron && IsMVANonTrigElectronHttSummer2013(&(*electron), event, false);
		}
		else if (electronIDType == ElectronIDType::SUMMER2013TIGHT)
		{
			validElectron = validElectron && IsMVANonTrigElectronHttSummer2013(&(*electron), event, true);
		}
		else if (electronIDType == ElectronIDType::SUMMER2013TTHLOOSE)
		{
			validElectron = validElectron && IsMVATrigElectronTTHSummer2013(&(*electron), event, false);
		}
		else if (electronIDType == ElectronIDType::SUMMER2013TTHTIGHT)
		{
			validElectron = validElectron && IsMVATrigElectronTTHSummer2013(&(*electron), event, true);
		}
		else if (electronIDType == ElectronIDType::CUTBASED2015ANDLATER)
		{
			assert(CheckElectronMetadata(event.m_electronMetadata, electronIDName, electronIDInMetadata));
			assert(CheckElectronMetadata(event.m_electronMetadata, electronIDList, electronIDListInMetadata));
			validElectron = validElectron && IsCutBased(&(*electron), event, electronIDName);
		}
		else if (electronIDType == ElectronIDType::MVABASED2015ANDLATER)
		{
			assert(CheckElectronMetadata(event.m_electronMetadata, electronIDName, electronIDInMetadata));
			assert(CheckElectronMetadata(event.m_electronMetadata, electronIDList, electronIDListInMetadata));
			validElectron = validElectron && IsMVABased(&(*electron), event, electronIDName);
		}
		else if (electronIDType != ElectronIDType::NONE)
			LOG(FATAL) << "Electron ID type of type " << Utility::ToUnderlyingValue(electronIDType) << " not yet implemented!";
	}

	// custom electron isolation with delta beta correction
	// https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauWorkingSummer2013#Electron_Muon_Isolation
	if (validElectron && electronIsoType == ElectronIsoType::USER) {
		if (product.m_pfChargedHadronsFromFirstPV.size() > 0 &&
		    product.m_pfNeutralHadronsFromFirstPV.size() > 0 &&
		    product.m_pfPhotonsFromFirstPV.size() > 0 &&
		    product.m_pfChargedHadronsNotFromFirstPV.size() > 0)
		{
			isolationPtSum = ParticleIsolation::IsolationPtSum(
					electron->p4, product,
					(settings.*GetElectronIsoSignalConeSize)(),
					(settings.*GetElectronDeltaBetaCorrectionFactor)(),
					(settings.*GetElectronChargedIsoVetoConeSizeEB)(),
					(settings.*GetElectronChargedIsoVetoConeSizeEE)(),
					(settings.*GetElectronNeutralIsoVetoConeSize)(),
					(settings.*GetElectronPhotonIsoVetoConeSizeEB)(),
					(settings.*GetElectronPhotonIsoVetoConeSizeEE)(),
					(settings.*GetElectronDeltaBetaIsoVetoConeSize)(),
					(settings.*GetElectronChargedIsoPtThreshold)(),
					(settings.*GetElectronNeutralIsoPtThreshold)(),
					(settings.*GetElectronPhotonIsoPtThreshold)(),
					(settings.*GetElectronDeltaBetaIsoPtThreshold)()
			);
		}
		else {
			isolationPtSum = electron->pfIso((settings.*GetElectronDeltaBetaCorrectionFactor)());
		}
		
		double isolationPtSumOverPt = isolationPtSum / electron->p4.Pt();
		
		product.m_leptonIsolation[electron] = isolationPtSum;
		product.m_leptonIsolationOverPt[electron] = isolationPtSumOverPt;
		product.m_electronIsolation[electron] = isolationPtSum;
		product.m_electronIsolationOverPt[electron] = isolationPtSumOverPt;
		
		if (std::abs(electron->p4.Eta()) < DefaultValues::EtaBorderEB)
		{
			if ((isolationPtSumOverPt > (settings.*GetElectronIsoPtSumOverPtLowerThresholdEB)()) &&
			    (isolationPtSumOverPt < (settings.*GetElectronIsoPtSumOverPtUpperThresholdEB)()))
			{
				validElectron = settings.GetDirectIso();
			}
			else
			{
				validElectron = (! settings.GetDirectIso());
			}
		}
		else
		{
			if ((isolationPtSumOverPt > (settings.*GetElectronIsoPtSumOverPtLowerThresholdEE)()) &&
			    (isolationPtSumOverPt < (settings.*GetElectronIsoPtSumOverPtUpperThresholdEE)()))
			{
				validElectron = settings.GetDirectIso();
			}
			else
			{
				validElectron = (! settings.GetDirectIso());
			}
		}
	}
	
	// (tighter) cut on impact parameters of track
	validElectron = validElectron
	                && ((settings.*GetElectronTrackDxyCut)() <= 0.0 || std::abs(electron->track.getDxy(&event.m_vertexSummary->pv)) < (settings.*GetElectronTrackDxyCut)())
	                && ((settings.*GetElectronTrackDzCut)() <= 0.0 || std::abs(electron->track.getDz(&event.m_vertexSummary->pv)) < (settings.*GetElectronTrackDzCut)());

	return validElectron;
}

bool HttValidElectronsProducer::IsMVATrigElectronTTHSummer2013(KElectron* electron, event_type const& event, bool tightID) const
{
	bool validElectron = true;
	
	validElectron = validElectron && electron->getId("mvaTrigV0", event.m_electronMetadata) > (tightID ? 0.5 : 0.5);

	return validElectron;
}

bool HttValidElectronsProducer::IsMVANonTrigElectronHttSummer2013(KElectron* electron, event_type const& event, bool tightID) const
{
	bool validElectron = true;
	
	// https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauWorkingSummer2013#Electron_ID
	validElectron = validElectron &&
		(
			(
				(electron->p4.Pt() < 20.0)
				&&
				(
					(std::abs(electron->superclusterPosition.Eta()) < 0.8 && electron->getId("mvaNonTrigV0", event.m_electronMetadata) > 0.925)
					|| (std::abs(electron->superclusterPosition.Eta()) > 0.8 && std::abs(electron->superclusterPosition.Eta()) < DefaultValues::EtaBorderEB && electron->getId("mvaNonTrigV0", event.m_electronMetadata) > 0.915)
					|| (std::abs(electron->superclusterPosition.Eta()) > DefaultValues::EtaBorderEB && electron->getId("mvaNonTrigV0", event.m_electronMetadata) > 0.965)
				)
			)
			||
			(
				(electron->p4.Pt() >= 20.0) &&
				(
					(std::abs(electron->superclusterPosition.Eta()) < 0.8 && electron->getId("mvaNonTrigV0", event.m_electronMetadata) > (tightID ? 0.925 : 0.905))
					|| (std::abs(electron->superclusterPosition.Eta()) > 0.8 && std::abs(electron->superclusterPosition.Eta()) < DefaultValues::EtaBorderEB && electron->getId("mvaNonTrigV0", event.m_electronMetadata) > (tightID ? 0.975 : 0.955))
					|| (std::abs(electron->superclusterPosition.Eta()) > DefaultValues::EtaBorderEB && electron->getId("mvaNonTrigV0", event.m_electronMetadata) > (tightID ? 0.985 : 0.975))
				)
			)
		);

	return validElectron;
}

bool HttValidElectronsProducer::IsCutBased(KElectron* electron, event_type const& event, const std::string &idName) const
{
	bool validElectron = true;

	validElectron = validElectron
			&& electron->getId(idName, event.m_electronMetadata);

	return validElectron;
}

bool HttValidElectronsProducer::IsMVABased(KElectron* electron, event_type const& event, const std::string &idName) const
{
	bool validElectron = true;

	// https://twiki.cern.ch/twiki/bin/view/CMS/MultivariateElectronIdentificationRun2#General_Purpose_MVA_training_det
	// pT always greater than 10 GeV
	validElectron = validElectron &&
		(
			(std::abs(electron->superclusterPosition.Eta()) < 0.8 && electron->getId(idName, event.m_electronMetadata) > electronMvaIDCutEB1)
			||
			(std::abs(electron->superclusterPosition.Eta()) > 0.8 && std::abs(electron->superclusterPosition.Eta()) < DefaultValues::EtaBorderEB && electron->getId(idName, event.m_electronMetadata) > electronMvaIDCutEB2)
			||
			(std::abs(electron->superclusterPosition.Eta()) > DefaultValues::EtaBorderEB && electron->getId(idName, event.m_electronMetadata) > electronMvaIDCutEE)
		);

	return validElectron;
}

bool HttValidElectronsProducer::CheckElectronMetadata(const KElectronMetadata *meta, std::string idName, bool &checkedAlready) const
{
	if(!checkedAlready && !Utility::Contains(meta->idNames, idName))
		LOG(FATAL) << "HttValidElectronsProducer::CheckElectronMetadata: could not find following Id in electron metadata. " << idName << std::endl;

	checkedAlready = true;

	return checkedAlready;
}

bool HttValidElectronsProducer::CheckElectronMetadata(const KElectronMetadata *meta, std::vector<std::string> idNames, bool &checkedAlready) const
{
	if(!checkedAlready)
	{
		for(auto idName : idNames)
		{
			if(!Utility::Contains(meta->idNames, idName))
				LOG(FATAL) << "HttValidElectronsProducer::CheckElectronMetadata: could not find following Id in electron metadata. " << idName << std::endl;
		}
	}

	checkedAlready = true;

	return checkedAlready;
}

HttValidLooseElectronsProducer::HttValidLooseElectronsProducer(
		std::vector<KElectron*> product_type::*validElectrons,
		std::vector<KElectron*> product_type::*invalidElectrons,
		std::string (setting_type::*GetElectronID)(void) const,
		std::string (setting_type::*GetElectronIDType)(void) const,
		std::string (setting_type::*GetElectronIDName)(void) const,
		float (setting_type::*GetElectronMvaIDCutEB1)(void) const,
		float (setting_type::*GetElectronMvaIDCutEB2)(void) const,
		float (setting_type::*GetElectronMvaIDCutEE)(void) const,
		std::string (setting_type::*GetElectronIsoType)(void) const,
		std::string (setting_type::*GetElectronIso)(void) const,
		std::string (setting_type::*GetElectronReco)(void) const,
		std::vector<std::string>& (setting_type::*GetLowerPtCuts)(void) const,
		std::vector<std::string>& (setting_type::*GetUpperAbsEtaCuts)(void) const,
		float (setting_type::*GetElectronChargedIsoVetoConeSizeEB)(void) const,
		float (setting_type::*GetElectronChargedIsoVetoConeSizeEE)(void) const,
		float (setting_type::*GetElectronNeutralIsoVetoConeSize)(void) const,
		float (setting_type::*GetElectronPhotonIsoVetoConeSizeEB)(void) const,
		float (setting_type::*GetElectronPhotonIsoVetoConeSizeEE)(void) const,
		float (setting_type::*GetElectronDeltaBetaIsoVetoConeSize)(void) const,
		float (setting_type::*GetElectronChargedIsoPtThreshold)(void) const,
		float (setting_type::*GetElectronNeutralIsoPtThreshold)(void) const,
		float (setting_type::*GetElectronPhotonIsoPtThreshold)(void) const,
		float (setting_type::*GetElectronDeltaBetaIsoPtThreshold)(void) const,
		float (setting_type::*GetElectronIsoSignalConeSize)(void) const,
		float (setting_type::*GetElectronDeltaBetaCorrectionFactor)(void) const,
		float (setting_type::*GetElectronIsoPtSumOverPtLowerThresholdEB)(void) const,
		float (setting_type::*GetElectronIsoPtSumOverPtLowerThresholdEE)(void) const,
		float (setting_type::*GetElectronIsoPtSumOverPtUpperThresholdEB)(void) const,
		float (setting_type::*GetElectronIsoPtSumOverPtUpperThresholdEE)(void) const,
		float (setting_type::*GetElectronTrackDxyCut)(void) const,
		float (setting_type::*GetElectronTrackDzCut)(void) const
) :
	HttValidElectronsProducer(validElectrons,
	                          invalidElectrons,
	                          GetElectronID,
	                          GetElectronIDType,
	                          GetElectronIDName,
	                          GetElectronMvaIDCutEB1,
	                          GetElectronMvaIDCutEB2,
	                          GetElectronMvaIDCutEE,
	                          GetElectronIsoType,
	                          GetElectronIso,
	                          GetElectronReco,
	                          GetLowerPtCuts,
	                          GetUpperAbsEtaCuts,
	                          GetElectronChargedIsoVetoConeSizeEB,
	                          GetElectronChargedIsoVetoConeSizeEE,
	                          GetElectronNeutralIsoVetoConeSize,
	                          GetElectronPhotonIsoVetoConeSizeEB,
	                          GetElectronPhotonIsoVetoConeSizeEE,
	                          GetElectronDeltaBetaIsoVetoConeSize,
	                          GetElectronChargedIsoPtThreshold,
	                          GetElectronNeutralIsoPtThreshold,
	                          GetElectronPhotonIsoPtThreshold,
	                          GetElectronDeltaBetaIsoPtThreshold,
	                          GetElectronIsoSignalConeSize,
	                          GetElectronDeltaBetaCorrectionFactor,
	                          GetElectronIsoPtSumOverPtLowerThresholdEB,
	                          GetElectronIsoPtSumOverPtLowerThresholdEE,
	                          GetElectronIsoPtSumOverPtUpperThresholdEB,
	                          GetElectronIsoPtSumOverPtUpperThresholdEE,
	                          GetElectronTrackDxyCut,
	                          GetElectronTrackDzCut)
{
}



HttValidVetoElectronsProducer::HttValidVetoElectronsProducer(
		std::vector<KElectron*> product_type::*validElectrons,
		std::vector<KElectron*> product_type::*invalidElectrons,
		std::string (setting_type::*GetElectronID)(void) const,
		std::string (setting_type::*GetElectronIDType)(void) const,
		std::string (setting_type::*GetElectronIDName)(void) const,
		float (setting_type::*GetElectronMvaIDCutEB1)(void) const,
		float (setting_type::*GetElectronMvaIDCutEB2)(void) const,
		float (setting_type::*GetElectronMvaIDCutEE)(void) const,
		std::string (setting_type::*GetElectronIsoType)(void) const,
		std::string (setting_type::*GetElectronIso)(void) const,
		std::string (setting_type::*GetElectronReco)(void) const,
		std::vector<std::string>& (setting_type::*GetLowerPtCuts)(void) const,
		std::vector<std::string>& (setting_type::*GetUpperAbsEtaCuts)(void) const,
		float (setting_type::*GetElectronChargedIsoVetoConeSizeEB)(void) const,
		float (setting_type::*GetElectronChargedIsoVetoConeSizeEE)(void) const,
		float (setting_type::*GetElectronNeutralIsoVetoConeSize)(void) const,
		float (setting_type::*GetElectronPhotonIsoVetoConeSizeEB)(void) const,
		float (setting_type::*GetElectronPhotonIsoVetoConeSizeEE)(void) const,
		float (setting_type::*GetElectronDeltaBetaIsoVetoConeSize)(void) const,
		float (setting_type::*GetElectronChargedIsoPtThreshold)(void) const,
		float (setting_type::*GetElectronNeutralIsoPtThreshold)(void) const,
		float (setting_type::*GetElectronPhotonIsoPtThreshold)(void) const,
		float (setting_type::*GetElectronDeltaBetaIsoPtThreshold)(void) const,
		float (setting_type::*GetElectronIsoSignalConeSize)(void) const,
		float (setting_type::*GetElectronDeltaBetaCorrectionFactor)(void) const,
		float (setting_type::*GetElectronIsoPtSumOverPtLowerThresholdEB)(void) const,
		float (setting_type::*GetElectronIsoPtSumOverPtLowerThresholdEE)(void) const,
		float (setting_type::*GetElectronIsoPtSumOverPtUpperThresholdEB)(void) const,
		float (setting_type::*GetElectronIsoPtSumOverPtUpperThresholdEE)(void) const,
		float (setting_type::*GetElectronTrackDxyCut)(void) const,
		float (setting_type::*GetElectronTrackDzCut)(void) const
) :
	HttValidElectronsProducer(validElectrons,
	                          invalidElectrons,
	                          GetElectronID,
	                          GetElectronIDType,
	                          GetElectronIDName,
	                          GetElectronMvaIDCutEB1,
	                          GetElectronMvaIDCutEB2,
	                          GetElectronMvaIDCutEE,
	                          GetElectronIsoType,
	                          GetElectronIso,
	                          GetElectronReco,
	                          GetLowerPtCuts,
	                          GetUpperAbsEtaCuts,
	                          GetElectronChargedIsoVetoConeSizeEB,
	                          GetElectronChargedIsoVetoConeSizeEE,
	                          GetElectronNeutralIsoVetoConeSize,
	                          GetElectronPhotonIsoVetoConeSizeEB,
	                          GetElectronPhotonIsoVetoConeSizeEE,
	                          GetElectronDeltaBetaIsoVetoConeSize,
	                          GetElectronChargedIsoPtThreshold,
	                          GetElectronNeutralIsoPtThreshold,
	                          GetElectronPhotonIsoPtThreshold,
	                          GetElectronDeltaBetaIsoPtThreshold,
	                          GetElectronIsoSignalConeSize,
	                          GetElectronDeltaBetaCorrectionFactor,
	                          GetElectronIsoPtSumOverPtLowerThresholdEB,
	                          GetElectronIsoPtSumOverPtLowerThresholdEE,
	                          GetElectronIsoPtSumOverPtUpperThresholdEB,
	                          GetElectronIsoPtSumOverPtUpperThresholdEE,
	                          GetElectronTrackDxyCut,
	                          GetElectronTrackDzCut)
{
}

