/*!
 * \file
 * \brief Frequency scale for generating filter banks and their frequencies
 *
 * \author Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2006/09/28
 * \version \$Id: Scale.cpp 459 2007-11-08 11:50:04Z tom $
 */
/* (c) 2006, University of Cambridge, Medical Research Council
 * http://www.pdn.cam.ac.uk/groups/cnbh/aimmanual
 */
#include "Support/common.h"
#include "Support/util.h"
#include "Modules/Scale/Scale.h"
#include "Modules/Scale/ScaleLinear.h"
#include "Modules/Scale/ScaleERB.h"
#include "Modules/Scale/ScaleLog.h"
#include "Modules/Scale/ScaleLogScaled.h"

Scale *Scale::Create(ScaleType iType, unsigned int min, unsigned int max, float density)
{
	switch(iType) {
	case SCALE_LINEAR:
		return static_cast<Scale*>(new ScaleLinear(min, max, density));
	case SCALE_ERB:
		return static_cast<Scale*>(new ScaleERB(min, max, density));
	case SCALE_LOG:
		return static_cast<Scale*>(new ScaleLog(min, max, density));
	case SCALE_LOGSCALED:
		return static_cast<Scale*>(new ScaleLogScaled(min, max, density));
	default:
		aimASSERT(0);
		break;
	}
	// Unreachable code
	aimASSERT(0);
	return NULL;
}

Scale *Scale::Create(ScaleType iType)
{
	return Create(iType, 0, 0, 0);
}

Scale *Scale::Clone()
{
	Scale *pScale = Create(m_iType, m_iMin, m_iMax, m_fDensity);
	aimASSERT(pScale);
	pScale->m_fScaledCurHalfSum = m_fScaledCurHalfSum;
	pScale->m_fScaledCurDiff = m_fScaledCurDiff;
	return pScale;
}

float Scale::FromLinearScaled(float fVal)
{
	/*! This function returns
	 *    ( FromLinear(fVal) - (fMinScaled+fMaxScaled)/2 ) / (fMaxScaled-fMinScaled)
	 */
	float fValScaled = FromLinear(fVal);
	return (fValScaled - m_fScaledCurHalfSum) / m_fScaledCurDiff;
}
void Scale::FromLinearScaledExtrema(float fMin, float fMax)
{
	float fMinScaled = FromLinear(fMin);
	float fMaxScaled = FromLinear(fMax);
	m_fScaledCurHalfSum = (fMinScaled+fMaxScaled)/2;
	m_fScaledCurDiff = fMaxScaled-fMinScaled;
	m_fMin = fMin;
	m_fMax = fMax;
}
void Scale::FromLinearScaledExtrema(Scale *pScale)
{
	aimASSERT(pScale);
	FromLinearScaledExtrema(pScale->m_fMin, pScale->m_fMax);
}

SignalBank* Scale::CreateSignalBank(unsigned int iChannels, unsigned int iBufferlength, unsigned int iSamplerate)
{
	SignalBank *pBank;
	double intpart, fracpart;
	fracpart = modf((m_iMax-m_iMin)*m_fDensity, &intpart);
	unsigned int nBankChan = (unsigned int) intpart;
	if (fracpart >= 0.5f)
        nBankChan++;

	pBank = new SignalBank(iChannels, iBufferlength, iSamplerate, nBankChan);
	aimASSERT(pBank);

	float scaleDelta = ( FromLinear(m_iMax) - FromLinear(m_iMin) ) / (nBankChan-1);
	float scaleCur = FromLinear(m_iMin);

	for (unsigned int i=0; i<nBankChan; i++) {
		pBank->setCentreFrequency(i, ToLinear(scaleCur));
		scaleCur+=scaleDelta;
	}

	return pBank;
}



SignalBank* Scale::CreateSignalBank(Signal* pSig) {
	aimASSERT(pSig);
	return CreateSignalBank(pSig->getAudioChannels(), pSig->getBufferlength(), pSig->getSamplerate());
}

