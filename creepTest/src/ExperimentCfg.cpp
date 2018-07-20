/*
 * ExperimentCfg.cpp
 *
 *  Created on: 27.11.2008
 *      Author: lexa
 */

#include "ExperimentCfg.h"

ExperimentCfg::~ExperimentCfg()
{
	// TODO Auto-generated destructor stub
}

ExperimentCfg::ExperimentCfg()
{
	// TODO Auto-generated constructor stub

}

ExperimentCfg* ExperimentCfg::getInstance()
{
	static ExperimentCfg expCfg;
	return &expCfg;
}
