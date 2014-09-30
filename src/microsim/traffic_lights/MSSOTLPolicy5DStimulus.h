/****************************************************************************/
/// @file    MSSOTLPolicyStimulus.h
/// @author  Riccardo Belletti
/// @author  Simone Bacchilega
/// @date    Aug 2014
/// @version $Id: MSSOTLPolicyDesirability.h 0  $
///
// The class the low-level policy stimulus
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#ifndef MSSOTLPOLICY5DSTIMULUS_H_
#define MSSOTLPOLICY5DSTIMULUS_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#define SWARM_DEBUG
#include <utils/common/SwarmDebug.h>
#include <utils/common/MsgHandler.h>
#include <sstream>
#include <math.h>
#include "MSSOTLPolicyDesirability.h"

using namespace std;
/**
 * \class MSSOTLPolicyStimulus
 * \brief This class determines the stimulus of a MSSOTLPolicy when
 * used in combination with a high level policy.\n
 * The stimulus function is calculated as follows:\n
 * stimulus = cox * exp(-pow(pheroIn - offsetIn, 2)/divisor -pow(pheroOut - offsetOut, 2)/divisor)
 */
class MSSOTLPolicy5DStimulus: public MSSOTLPolicyDesirability {

private:

	double stimCoxDVal,
	stimOffsetInDVal, stimOffsetOutDVal, stimOffsetDispersionInDVal, stimOffsetDispersionOutDVal,
	stimDivInDVal, stimDivOutDVal, stimDivDispersionInDVal, stimDivDispersionOutDVal,
	stimCoxExpInDVal, stimCoxExpOutDVal, stimCoxExpDispersionInDVal, stimCoxExpDispersionOutDVal;

public:

	MSSOTLPolicy5DStimulus(string keyPrefix,const std::map<std::string, std::string>& parameters);

	double getStimCox() {
		string key = getKeyPrefix() + "_STIM_COX";
		return readParameter(key, stimCoxDVal);
	}
	void setStimCoxDefVal(double defVal) {
		stimCoxDVal = defVal;
	}
	double getStimOffsetIn() {
		string key = getKeyPrefix() + "_STIM_OFFSET_IN";
		return readParameter(key, stimOffsetInDVal);
	}
	void setStimOffsetInDefVal(double defVal) {
		stimOffsetInDVal = defVal;
	}
	double getStimOffsetOut() {
		string key = getKeyPrefix() + "_STIM_OFFSET_OUT";
		return readParameter(key, stimOffsetOutDVal);
	}

	void setStimOffsetOutDefVal(double defVal) {
		stimOffsetOutDVal = defVal;
	}

	double getStimOffsetDispersionIn() {
		string key = getKeyPrefix() + "_STIM_OFFSET_DISPERSION_IN";
		return readParameter(key, stimOffsetDispersionInDVal);
	}
	void setStimOffsetDispersionInDefVal(double defVal) {
		stimOffsetDispersionInDVal = defVal;
	}
	double getStimOffsetDispersionOut() {
		string key = getKeyPrefix() + "_STIM_OFFSET_DISPERSION_OUT";
		return readParameter(key, stimOffsetDispersionOutDVal);
	}
	void setStimOffsetDispersionOutDefVal(double defVal) {
		stimOffsetDispersionOutDVal = defVal;
	}
	double getStimDivisorIn() {
		string key = getKeyPrefix() + "_STIM_DIVISOR_IN";
		return readParameter(key, stimDivInDVal);
	}

	void setStimDivisorInDefVal(double defVal) {
		stimDivInDVal = defVal;
	}

	double getStimDivisorOut() {
		string key = getKeyPrefix() + "_STIM_DIVISOR_OUT";
		return readParameter(key, stimDivOutDVal);
	}

	void setStimDivisorOutDefVal(double defVal) {
		stimDivOutDVal = defVal;
	}

	double getStimDivisorDispersionIn() {
		string key = getKeyPrefix() + "_STIM_DIVISOR_DISPERSION_IN";
		return readParameter(key, stimDivDispersionInDVal);
	}

	void setStimDivisorDispersionInDefVal(double defVal) {
		stimDivDispersionInDVal = defVal;
	}
	double getStimDivisorDispersionOut() {
		string key = getKeyPrefix() + "_STIM_DIVISOR_DISPERSION_OUT";
		return readParameter(key, stimDivDispersionOutDVal);
	}

	void setStimDivisorDispersionOutDefVal(double defVal) {
		stimDivDispersionOutDVal = defVal;
	}
	double getStimCoxExpIn() {
		string key = getKeyPrefix() + "_STIM_COX_EXP_IN";
		return readParameter(key, stimCoxExpInDVal);
	}
	void setStimCoxExpInDefVal(double defVal) {
		stimCoxExpInDVal = defVal;
	}
	double getStimCoxExpOut() {
		string key = getKeyPrefix() + "_STIM_COX_EXP_OUT";
		return readParameter(key, stimCoxExpOutDVal);
	}
	void setStimCoxExpOutDefVal(double defVal) {
		stimCoxExpOutDVal = defVal;
	}

	double getStimCoxExpDispersionIn() {
		string key = getKeyPrefix() + "_STIM_COX_EXP_DISPERSION_IN";
		return readParameter(key, stimCoxExpDispersionInDVal);
	}
	void setStimCoxExpDispersionInDefVal(double defVal) {
		stimCoxExpDispersionInDVal = defVal;
	}
	double getStimCoxExpDispersionOut() {
		string key = getKeyPrefix() + "_STIM_COX_EXP_DISPERSION_OUT";
		return readParameter(key, stimCoxExpDispersionOutDVal);
	}
	void setStimCoxExpDispersionOutDefVal(double defVal) {
		stimCoxExpDispersionOutDVal = defVal;
	}
	string getMessage();
	/**
	 *	@brief Computes stimulus function
	 *  stimulus = cox * exp(-pow(pheroIn - offsetIn, 2)/divisor -pow(pheroOut - offsetOut, 2)/divisor);
	 */
	virtual double computeDesirability(double vehInMeasure,
			double vehOutMeasure);

	virtual double computeDesirability(double vehInMeasure, double vehOutMeasure,double vehInDispersionMeasure,	double vehOutDispersionMeasure);
};

#endif /* MSSOTLPOLICYSTIMULUS_H_ */
