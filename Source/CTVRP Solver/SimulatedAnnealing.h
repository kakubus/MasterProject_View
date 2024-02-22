/*****************************************************************//**
 * \file   SimulatedAnnealing.h
 * \brief  Class for Simulated annealing algorithm implementation.
 * 
 * \author Jakub Kaniowski
 * \date   September 2023
 *********************************************************************/
#pragma once
#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

#include <random>

#include "Algorithm.h"

class SimulatedAnnealing :
    public Algorithm
{
public:
	SimulatedAnnealing(Problem& problemToSolve, float tMax, float tEnd, float alpha, int maxIterations);
	~SimulatedAnnealing();

	void run();

private:

	int			_maxIterations;		///< Max attempts limit
	float		_tMax;				///< Starting temperature
	float		_tEnd;				///< Stop temperature
	float		_T;					///< Internal, actual temperature
	const float _alpha;				///< Constant cooling parameter
	double		_vCapacity;			///< Maxiumum capacity of vehicle


};

#endif