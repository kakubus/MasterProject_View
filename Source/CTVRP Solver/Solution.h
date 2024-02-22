/*****************************************************************//**
 * \file   Solution.h
 * \brief  Class describes solution returned by algorithm.
 * Class contains definition of paths for the vehicles.
 * 
 * \author Jakub Kaniowski
 * \date   September 2023
 *********************************************************************/

#pragma once
#ifndef SOLUTION_H
#define SOLUTION_H

#include <iostream>
#include <map>
#include <vector>

#include "Problem.h"

typedef std::map<Vehicle*, std::vector<Location*>> LocationPaths;
typedef std::map<Vehicle*, std::vector<Route*>> RoutePaths;
typedef std::map<Vehicle*, double> DemandPaths;
class Problem;

class Solution
{

public:
	Solution();
	Solution(Problem *p);
	~Solution();

	/// @brief Copy constructor Solution
	/// @param toCopy 
	Solution(const Solution& toCopy);

	/// @brief Container for fast list of locations in purposed solution
	/// @param map: (Vehicle*, vector: Location*)
	LocationPaths locationsInSolution;	

	/// @brief  Container for detailed data of routes via all locations indicated in locationsInSolution
	/// @param map: (Vehicle*, vector: Route*)
	RoutePaths	routesInSolution;

	/// @brief Container for every demand in every locations indicated in locationsInSolution
	/// @param map: (Vehicle*, double)
	DemandPaths demandPaths;
	double totalCost;					///< Total cost of solution
	
	
	void updateRoutesFromLocations();	///< Method to recalculate totalCost and change all routesInSolutions based on locationsInSolution
	void updateRoutesFromLocations(Solution& selectedSolution);
	void updateRoutesForSpecificLocations(Vehicle& specifiedPath);
	void updateRoutesForSpecificLocations(Solution& selectedSolution, Vehicle& specifiedPath); ///< Method and change specified routesInSolutions based on specified locationsInSolution which are indicated as external Solution object



	double calculateTotalCost();
	double calculateTotalCost(Solution& selectedSolution);
	double calculatePathDemandForSolution(Solution& selectedSolution);
	double getPathCost(Vehicle& specifiedPath);
	double getPathCost(Solution& selectedSolution, Vehicle& specifiedPath);
	double getPathDemand(Solution& selectedSolution, Vehicle& specifiedPath);

	double getRemainingCapacity(Solution& selectedSolution, Vehicle& specifiedPath); // used for return remaining capacity at vehicle (max capacity - locations demand)
	Vehicle* findVehicleByNeededCapacity(Solution& selectedSolution, double capacity);

#define NO_PRINTABLE_DEBUG
	bool validateSolution();
	bool validateSolution(Solution& selectedSolution);

	std::stringstream show();
	std::stringstream showRaw();

	void operator=(const Solution& toCopy);
	
private:
	Problem* _problem;
};

#endif
