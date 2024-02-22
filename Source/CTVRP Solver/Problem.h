/*****************************************************************//**
 * \file   Problem.h
 * \brief  Class defined problem to solve for algorithms.
 * 
 * \author Jakub Kaniowski
 * \date   September 2023
 *********************************************************************/
#pragma once
#ifndef PROBLEM_H
#define PROBLEM_H

#include <iomanip>

#include "FileIO.h"
#include "Map.h"
#include "Solution.h"


typedef std::vector<Client*> ClientList;
typedef std::vector<Vehicle*> VehicleList;
class FileIO;
class Solution;
class Problem
{
public:
	Problem();							
	Problem(Problem& copyProblem);		///< Copy constructor
	Problem(std::string filepath, std::string filepathVehicle);

	~Problem();
	ClientList* getClientList();		///< Method used for return pointer to existing client list 
	VehicleList* getVehicleList();		///< Method used for return pointer to existing vehicle list 
	Map* getMap();						///< Method used for return pointer to map 

	Solution* solution;			///< Pointer to storage actual modified solution
	Solution* bestSolution;		///< Pointer used for storage the best found solution while algorithm is working.
	
	std::stringstream show();	///< Print solutions of problem. Returns stream to save to file.


protected:

	ClientList	clientList;		///< List of every client inserted into graph
	VehicleList	vehicleList;	///< List of every vehicle used in routing problem
	Map map;					///< Map used for problem

private:
	FileIO *files;			
	FileIO *filesVehicle;

};

#endif