
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"

#include "HiggsAnalysis/KITHiggsToTauTau/interface/HttTypes.h"
#include "HiggsAnalysis/KITHiggsToTauTau/interface/Utility/RecoilCorrector.h"
#include "HiggsAnalysis/KITHiggsToTauTau/interface/Utility/MEtSys.h"

#include <boost/regex.hpp>


/**
   \brief Corrects the MET created by the MET producer
   
   Run this producer after the Valid(Tagged)JetsProducer, since it relies on the number of
   jets in the event.
*/


template<class TMet>
class MetCorrectorBase: public ProducerBase<HttTypes>
{
public:

	typedef typename HttTypes::event_type event_type;
	typedef typename HttTypes::product_type product_type;
	typedef typename HttTypes::setting_type setting_type;
	enum CorrectionMethod { NONE=0, QUANTILE_MAPPING=1, MEAN_RESOLUTION=2};
	
	MetCorrectorBase(TMet* product_type::*metMemberUncorrected,
			 TMet product_type::*metMemberCorrected,
			 std::vector<float> product_type::*metCorrections,
			 std::string (setting_type::*GetRecoilCorrectorFile)(void) const,
			 std::string (setting_type::*GetMetShiftCorrectorFile)(void) const
	) :
		ProducerBase<HttTypes>(),
		m_metMemberUncorrected(metMemberUncorrected),
		m_metMemberCorrected(metMemberCorrected),
		m_metCorrections(metCorrections),
		GetRecoilCorrectorFile(GetRecoilCorrectorFile),
		GetMetShiftCorrectorFile(GetMetShiftCorrectorFile)
	{
	}

	virtual void Init(setting_type const& settings) override
	{
		ProducerBase<HttTypes>::Init(settings);
		
		m_recoilCorrector = new RecoilCorrector((settings.*GetRecoilCorrectorFile)());
		
		if ((settings.GetMetSysType() != 0) || (settings.GetMetSysShift() != 0))
		{
			m_metShiftCorrector = new MEtSys((settings.*GetMetShiftCorrectorFile)());

			if (settings.GetMetSysType() == 1)
			{
				m_sysType = MEtSys::SysType::Response;
			}
			else if (settings.GetMetSysType() == 2)
			{
				m_sysType = MEtSys::SysType::Resolution;
			}
			else
			{
				m_sysType = MEtSys::SysType::NoType;
				LOG(FATAL) << "Invalid HttSettings::MetSysType option";
			}
			
			if (settings.GetMetSysShift() > 0)
			{
				m_sysShift = MEtSys::SysShift::Up;
			}
			else
			{
				m_sysShift = MEtSys::SysShift::Down;
			}
		}

		// determine process type, trigger several decisions later
		if (boost::regex_search(settings.GetNickname(), boost::regex("DY.?JetsToLL|W.?JetsToLNu|HToTauTau", boost::regex::extended)))
		{
			m_processType = MEtSys::ProcessType::BOSON;
		}
		else if (boost::regex_search(settings.GetNickname(), boost::regex("TT", boost::regex::extended)))
		{
			m_processType = MEtSys::ProcessType::TOP;
		}
		else
		{
			m_processType = MEtSys::ProcessType::EWK;
		}
		m_isWJets = boost::regex_search(settings.GetNickname(), boost::regex("W.?JetsToLNu", boost::regex::icase | boost::regex::extended));
		
		m_doMetSys = ((settings.GetMetSysType() != 0) || (settings.GetMetSysShift() != 0));

		if(settings.GetMetCorrectionMethod() == "quantileMapping")
			m_correctionMethod = MetCorrectorBase::CorrectionMethod::QUANTILE_MAPPING;
		else if(settings.GetMetCorrectionMethod() == "meanResolution")
			m_correctionMethod = MetCorrectorBase::CorrectionMethod::MEAN_RESOLUTION;
		else
		{
			m_correctionMethod = MetCorrectorBase::CorrectionMethod::NONE;
			LOG(FATAL) << "Invalid MetCorrectionMethod option. Available are 'quantileMapping' and 'meanResolution'";
		}
	}

	virtual void Produce(event_type const& event, product_type & product, 
	                     setting_type const& settings) const override
	{
		assert(m_metMemberUncorrected != nullptr);
		
		// Retrieve the needed informations from the event content
		float metX = (product.*m_metMemberUncorrected)->p4.Px();
		float metY = (product.*m_metMemberUncorrected)->p4.Py();
		float metEnergy = (product.*m_metMemberUncorrected)->p4.energy();
		float metResolution = std::sqrt(metEnergy * metEnergy - metX * metX - metY * metY);
		int nJets30 = product_type::GetNJetsAbovePtThreshold(product.m_validJets, 30.0);
		
		// In selected W+Jets events one of the leptons is faked by hadronic jet and this 
		// jet should be counted as a part of hadronic recoil to the W boson
		if(m_isWJets)
		{
			nJets30 += 1;
		}
		
		float genPx = 0.;  // generator Z(W) px
		float genPy = 0.;  // generator Z(W) py
		float visPx = 0.;  // visible (generator) Z(W) px
		float visPy = 0.;  // visible (generator) Z(W) py
		
		for (KGenParticles::const_iterator genParticle = event.m_genParticles->begin();
		 genParticle != event.m_genParticles->end(); ++genParticle)
		{
			int pdgId = std::abs(genParticle->pdgId);     
			
			if ( (pdgId >= DefaultValues::pdgIdElectron && pdgId <= DefaultValues::pdgIdNuTau && genParticle->fromHardProcessFinalState()) ||
			     (genParticle->isDirectHardProcessTauDecayProduct()) )
			{
				genPx += genParticle->p4.Px();
				genPy += genParticle->p4.Py();
				
				if ( !(pdgId == DefaultValues::pdgIdNuE || pdgId == DefaultValues::pdgIdNuMu || pdgId == DefaultValues::pdgIdNuTau) )
				{
					visPx += genParticle->p4.Px();
					visPy += genParticle->p4.Py();
				}
			}
		}
		
		// Save the ingredients for the correction
		(product.*m_metCorrections).push_back(genPx);
		(product.*m_metCorrections).push_back(genPy);
		(product.*m_metCorrections).push_back(visPx);
		(product.*m_metCorrections).push_back(visPy);
		
		float correctedMetX, correctedMetY;
		
		if(m_correctionMethod == MetCorrectorBase::CorrectionMethod::QUANTILE_MAPPING)
			m_recoilCorrector->Correct(
				metX,
				metY,
				genPx,
				genPy,
				visPx,
				visPy,
				nJets30,
				correctedMetX,
				correctedMetY);
		else if(m_correctionMethod == MetCorrectorBase::CorrectionMethod::MEAN_RESOLUTION)
			m_recoilCorrector->CorrectByMeanResolution(
				metX,
				metY,
				genPx,
				genPy,
				visPx,
				visPy,
				nJets30,
				correctedMetX,
				correctedMetY);
		
		(product.*m_metMemberCorrected) = *(product.*m_metMemberUncorrected);
		
		// Apply the correction to the MET object (only for DY, W and Higgs samples)
		if (m_processType == MEtSys::ProcessType::BOSON)
		{
			(product.*m_metMemberCorrected).p4.SetPxPyPzE(
				correctedMetX,
				correctedMetY,
				0.,
				std::sqrt(metResolution * metResolution + correctedMetX * correctedMetX + correctedMetY * correctedMetY));
			if (m_correctGlobalMet)
			{
				product.m_met = product.*m_metMemberCorrected;
			}
		}
		
		// Apply the correction to the MET object, if required (done for all the samples)
		if (m_doMetSys)
		{
			float correctedMetShiftX, correctedMetShiftY;
			
			m_metShiftCorrector->ApplyMEtSys(
				(product.*m_metMemberCorrected).p4.Px(), (product.*m_metMemberCorrected).p4.Py(),
				genPx, genPy,
				visPx, visPy,
				nJets30,
				m_processType,
				m_sysType,
				m_sysShift,
				correctedMetShiftX,
				correctedMetShiftY
			);
			
			(product.*m_metMemberCorrected).p4.SetPxPyPzE(
				correctedMetShiftX,
				correctedMetShiftY,
				0.,
				std::sqrt(metResolution * metResolution + correctedMetShiftX * correctedMetShiftX + correctedMetShiftY * correctedMetShiftY));
                        if (m_correctGlobalMet)
                        {
                                product.m_met = product.*m_metMemberCorrected;
                        }
		}
	}

protected:
	TMet* product_type::*m_metMemberUncorrected;
	TMet product_type::*m_metMemberCorrected;
	std::vector<float> product_type::*m_metCorrections;
	std::string (setting_type::*GetRecoilCorrectorFile)(void) const;
	std::string (setting_type::*GetMetShiftCorrectorFile)(void) const;
	RecoilCorrector* m_recoilCorrector;
	MEtSys* m_metShiftCorrector;
	MEtSys::ProcessType m_processType;
	MEtSys::SysType m_sysType;
	MEtSys::SysShift m_sysShift;
	bool m_isWJets;
	bool m_doMetSys;
	CorrectionMethod m_correctionMethod;
	bool m_correctGlobalMet;
};



/**
   \brief Corrector for (PF) MET
*/
class MetCorrector: public MetCorrectorBase<KMET>
{
public:
	MetCorrector();
	virtual void Init(setting_type const& settings) override;
	virtual std::string GetProducerId() const override;
};

/**
   \brief Corrector for MVAMET
*/
class MvaMetCorrector: public MetCorrectorBase<KMET>
{
public:
	MvaMetCorrector();
	virtual void Init(setting_type const& settings) override;
	virtual std::string GetProducerId() const override;
};
