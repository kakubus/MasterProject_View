/*****************************************************************//**
 * \file   Algorithm.h
 * \brief  General class contains methods used in any implemented algorithm.
 * 
 * \author Jakub Kaniowski
 * \date   June 2023
 *********************************************************************/

#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <boost/timer.hpp>
#include <random>

#include "Problem.h"
class Solution;

enum SolutionQuality{BETTER, V_OVERLOAD, NON_PROFIT, UNVISITED_NODES, UNDEFINED_SOLUTION_ERROR, EMPTY_PATH, EDGE_NOT_FOUND};

class Algorithm {

public:

	Algorithm(Problem* problemToSolve);		///< Algorithm can solve a problem
	~Algorithm();

	/// @brief Method used for generate new solution in LS and TS algorithms
	/// @param genIter 
	/// @param solution
	/// @return 
	Solution* bestGenerator(int genIter, Solution& solution);

	/// @brief Randomized generation of new solution
	/// @param solution 
	/// @return 
	Solution* rndGenerator(Solution& solution);

	/// @brief Method to generate initial solution
	/// @return false if fail, true if generation was successfull
	bool generateInitialSolution();			


	/// @brief Main method of execution algorithm
	virtual void run() = 0;					///< Virtual method for any defined algorithm

	/// @brief Method used for comparing actual solution with new solution
	/// @param oldSolution 
	/// @param newSolution 
	/// @return enum defined in SolutionQuality 
	SolutionQuality checkSolution(Solution& oldSolution, Solution& newSolution);	

	

protected:
	Vehicle* actualVehicle;			///< Pointer to actual used vehicle
	Problem* problemToSolve;		///< Pointer to solved problem
	Solution* problemSolution;		///< Pointer to solution returned by algorithm
	Map* problemMap;				///< Shortcut to problemToSolve->getMap();

	MethodResult lastChange;		///< Latest change in solution

	bool swap(Vehicle& v1, Vehicle& v2, Location& l1, Location& l2, GenerationType methodType, Solution& solution);	///< Method used for exchange vertices in solution

	std::pair<Vehicle*, Vehicle*> randomChooseVehicle(Solution& solution);	///< Function used for exchange, swap and move methods

	std::pair<Location*, Location*> randomChooseLocation(		///< Function used for swap method
		Vehicle& sourceVehiclePath,
		Vehicle& targetVehiclePath,
		Solution& solution
	);
	std::pair<Location*, Location*> randomChooseLocation(	///< Function used for move method - it has additional variable which modifying after get random location iterator.
		Vehicle& sourceVehiclePath,
		Vehicle& targetVehiclePath,
		Solution& solution,
		int& isMoveReturn
	);



private:

	
	bool exchange(Solution& solution);		///< Method used for exchange vertices in solution
	bool swap(Solution& solution);			///< Method used for swap vertices in solution
	bool move(Solution& solution);			///< Method used for move vertices in solution

	bool modify(Solution& solution);					///< Modify solution. - used in SA
	bool modify(int i, int l, Solution& solution);		///< Modify solution. - used in TS

	Vertex getRandomNeighborVertexFrom(Vertex v);							///< Function to retrieve a random vertex from the neighborhood of a given vertex
	std::vector<Location*> toLocation(std::vector<Route*>& fromRoutePath);  ///< Function to converting routePaths to Locations paths
	

};

#endif
