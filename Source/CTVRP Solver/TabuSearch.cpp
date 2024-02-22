/*****************************************************************//**
 * \file   TabuSearch.cpp
 * \brief  Class which defines Tabu algorithm
 * 
 * \author Jakub Kaniowski
 * \date   September 2023
 *********************************************************************/

#include "TabuSearch.h"
#include <Windows.h>

#include "UtilsFcn.h"

TabuSearch::TabuSearch(Problem& problemToSolve, int tabuTenure, int maxIterations)
	:Algorithm(&problemToSolve), _tabuTenure((tabuTenure > 0) ? tabuTenure : 1)
{
	_maxIterations = (maxIterations > 0) ? maxIterations : 1;
	_tabuSize = tabuTenure; //(tabuSize > 0) ? tabuSize : 1;
	//_tabuList.push_back({ lastChange, 0 });	
}

TabuSearch::~TabuSearch()
{
}
/// @brief Main algorithm loop

void TabuSearch::run()
{
	const char s = ' ';
	unsigned int i = 0; ///< Iterator for main loop
	unsigned int overload = 0;
	unsigned int ok = 0;
	unsigned int non_profit = 0;
	unsigned int acc_non_profit = 0;
	unsigned int exception = 0;
	unsigned int valid_fail = 0;
	unsigned int tabu_conflicts = 0;
	unsigned int iterWithoutProgress = 0;
	double newSolutionCost = 0;
	double minFoundCost = INFINITY; // To finding best solution!
	float foundBestTime = 0;    // Timestamp when found minimal solution

	FileIO outputFile("result_", "TS", txt);
	std::stringstream tempStr;
	FileIO logResultsFile("TS_", "TS", csv);
	logResultsFile.exportToTextFile(std::stringstream() << "i;t;s;bs;\n"); // add header in csv: iteration, time, solution, best solution, temperature.

	
	boost::timer timer;
	timer.restart();
	if (!generateInitialSolution()) {
		std::cout << "\n (TS) ERROR - INITIAL SOLUTION NOT FOUND\n" << std::endl;
		return;
	}
	std::cout << "Generation time:\t" << timer.elapsed() << " s\n" << std::endl;

	Problem* initial = new Problem(*problemToSolve); // Copy to compare with minimum solution when algorithm end.
	initial->solution->calculateTotalCost();

	tempStr << "Tabu search algorithm\n--------------------------------\n"
		<< " Nodes: " << problemMap->getNumberOfLocations()
		<< " Edges: " << problemMap->getNumberOfRoutes()
		<< "\n Total demand of nodes: " << problemMap->getTotalDemand()
		<< "\n Vehicle max capacity:  " << actualVehicle->getMaximumCapacity()
		<< "\n--------------------------------\n"
		<< "Algorithm parameters: \n Tabu tenure: " << _tabuTenure << "\n Tabu list size: " << _tabuSize << "\n Iterations limit: " << _maxIterations
		<< "\n" << std::endl;

	tempStr << "\nInitial solution:\n" << std::endl;
	tempStr << initial->show().str() << std::endl;

	problemToSolve->solution = new Solution(*initial->solution);

	double oldCost = problemSolution->calculateTotalCost();
	problemSolution->calculatePathDemandForSolution(*problemSolution);
	std::cout << "\n-----------------------------------\nTotal solution cost:\t" << problemSolution->totalCost << "\n" << std::endl;

	Sleep(2000);
	system("cls");

	Solution* newSolution = nullptr;                    ///< Prepare pointer to generating new solution
	Solution lastSolution = *problemToSolve->solution;  ///< Prepare pointer to store last solution

	timer.restart();
	double costOldSolution = 0, costNewSolution = 0;

	// Tabu Search Algorithm

	while (i < _maxIterations)
	{

		if (iterWithoutProgress > (problemToSolve->getMap()->getNumberOfLocations() + 10)) { // END LOOP CONDITION
			std::cout << "\nPROGRESS IS NOT DETECTED! STOPING ALGORITHM!\n";
			break;
		}

		i++;
		updateTabu();
		newSolution = nullptr;


		// GENERATE NEIGHBOR S'


		newSolution = modSolution(*problemToSolve->solution);
		if (newSolution == nullptr)
		{
			continue; // SKIP IF SOLUTION NOT FOUND
		}
		newSolutionCost = newSolution->totalCost;


		// SHOW

		std::cout << "CTVRP Solver 1.0\n----------------------------------" << std::endl;
		std::cout << std::left << std::setw(10) << "TABU SEARCH\ni = " << std::setfill(s) << std::setiosflags(std::ios::fixed) << std::right << std::setw(6) << std::setprecision(0) << i
			<< std::setiosflags(std::ios::fixed) << std::right << std::setw(30) << "General progress: " << std::left << std::setw(6) << std::setprecision(2) << ((i * 100) / (_maxIterations)) << "%"
			<< std::setiosflags(std::ios::fixed) << std::right << std::setw(25) << "Tabu conflicts: " << std::left << std::setw(4) << std::setprecision(0) << std::setfill(s) << tabu_conflicts << std::setprecision(2) << "\n\n"
			<< std::setiosflags(std::ios::fixed) << std::right << std::setw(25) << "\nValidation fail: " << std::left << std::setw(4) << std::setprecision(0) << std::setfill(s) << valid_fail << std::setprecision(2) << "\n";
		std::cout << "\nOK: " << std::setiosflags(std::ios::fixed) << std::left << std::setw(10) << std::setfill(s) << ok
			<< "\tOver: " << std::setiosflags(std::ios::fixed) << std::left << std::setw(10) << std::setfill(s) << overload
			<< "\tNon-profit: " << std::setiosflags(std::ios::fixed) << std::left << std::setw(10) << std::setfill(s) << non_profit
			<< "\tAccepted Non-profit: " << std::setiosflags(std::ios::fixed) << std::left << std::setw(10) << std::setfill(s) << acc_non_profit
			<< "\tE: " << std::setiosflags(std::ios::fixed) << std::left << std::setw(10) << std::setfill(s) << exception << std::setw(20) << std::setfill(s) << std::endl
			<< newSolution->show().str() << std::endl
			<< "\nGeneration time:\t" << timer.elapsed() << " s" << std::endl
			<< "Init Total Cost:\t" << oldCost << std::endl
			<< "NEW SOLUTION COST:\t" << newSolutionCost << std::endl
			<< "Actual Total Cost:\t" << problemToSolve->solution->totalCost << std::endl
			<< "FOUND BEST:\t\t" << minFoundCost << "\t (t: " << foundBestTime << " s)" << std::endl
			<< "Best Profit: \t\t" << oldCost - minFoundCost << std::endl
			<< "Best Profit [%]:\t" << (oldCost - minFoundCost) * 100 / oldCost << "%\n" << std::endl;
		clearScreen();



		// Log into csv file: iteration, time, solution, best solution, temperature.  (But not logging new solution. Log only accepted solutions (in SA worse solution is also as accepted. In tabu - not)
		logResultsFile.exportToTextFile(std::stringstream() << i << ";" << timer.elapsed() << ";" << newSolution->totalCost << ";" << problemToSolve->bestSolution->totalCost << ";\n");


		std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
		std::cout << "\n\n\n\n\n\n\n\n\n" << showTabuList().str() << std::endl;
		std::cout << "\n\n\n\n\n\n\n";
		clearScreen();

		// CHECK S' WITH TABU LIST

		if (!checkTabu(lastChange)) {	// Check if operation after generation new solution exists in tabu list.
			delete newSolution;
			tabu_conflicts++;
			
			if (iterWithoutProgress > (problemToSolve->getMap()->getNumberOfLocations() * _tabuTenure)) { // END LOOP CONDITION
				std::cout << "\nPROGRESS IS NOT DETECTED! STOPING ALGORITHM!\n";
				break;
			}

			continue;

		}
		else {
			bool canBeBest = false;
			problemSolution->calculatePathDemandForSolution(*problemSolution);
			costOldSolution = problemSolution->calculateTotalCost();
			costNewSolution = newSolution->calculateTotalCost();

			switch (checkSolution(*problemToSolve->solution, *newSolution))
			{
			case UNVISITED_NODES:
			{
				valid_fail++;
				delete newSolution;
				continue;
			} break;

			case BETTER:    //    f(S') < f(S) ? if true -> f(S) = f(S')
			{
				canBeBest = true;
				ok++;
				
				lastSolution = *problemToSolve->solution;
				delete problemToSolve->solution;
				problemToSolve->solution = new Solution(*newSolution);
				delete newSolution;

			} break;

			case NON_PROFIT:
			{
				canBeBest = false;
				non_profit++;
				iterWithoutProgress++;
				lastSolution = *problemToSolve->solution;
				delete newSolution;
		
				//continue;

			} break;

			case V_OVERLOAD:
			{
				overload++;
				canBeBest = false;
				iterWithoutProgress++;
				lastSolution = *problemToSolve->solution;
				delete newSolution;
			
				//continue;

			} break;

			default:
			{
				std::cout << "\n\n\n\n\n!!!!UNSUPPORTED CASE!!!!" << std::endl;
				exception++;
			} break;

			}

			addTabu(lastChange);

			double newSolutionCost = problemToSolve->solution->totalCost;
			if ((newSolutionCost < minFoundCost) && canBeBest) {    // f(S') < f(S*) ? if true -> f(S*) = f(S')   (Best solution)
				iterWithoutProgress = 0;
				minFoundCost = problemToSolve->solution->totalCost;
				delete problemToSolve->bestSolution;
				problemToSolve->solution->calculatePathDemandForSolution(*problemToSolve->solution);
				problemToSolve->bestSolution = new Solution(*problemToSolve->solution);
				foundBestTime = timer.elapsed();
				//system("cls");
			}
			else {
				iterWithoutProgress++;
			}
			clearScreen();
		}

	}

	tempStr << "\n\nEnd solution:\n\n";
	tempStr << problemToSolve->show().str() << std::endl;
	tempStr << "INITIAL TOTAL COST: " << oldCost << std::endl;
	tempStr << "END TOTAL COST: " << problemToSolve->solution->totalCost << std::endl;
	tempStr << "\nProfit: " << oldCost - problemToSolve->solution->totalCost << std::endl;
	tempStr << "Profit [%]: " << (oldCost - problemToSolve->solution->totalCost) * 100 / oldCost << std::endl;
	tempStr << "\n\nSolutions finding history:\n";
	tempStr << "\nSolution OK:\t" << ok;
	tempStr << "\nNon_Profit:\t" << non_profit;
	tempStr << "\n  Accepted:\t" << acc_non_profit;
	tempStr << "\nOverload:\t" << overload;
	tempStr << "\nExceptions:\t" << exception;
	tempStr << "\nTabu conflicts:\t" << tabu_conflicts;

	if (ok == 0) {
		tempStr << "\n\n> NO BETTER SOLUTION FOUND <\n\n";
	}
	else {
		tempStr << "\n\nFOUND BEST::\n\n";
	}

	tempStr << problemToSolve->bestSolution->show().str() << std::endl;

	tempStr << "\n\nBEST SOLUTION COST:\t" << problemToSolve->bestSolution->totalCost << "\t found at: " << foundBestTime << " s" << std::endl;

	tempStr << "Best Profit: \t\t" << oldCost - minFoundCost << std::endl;
	tempStr << "Best Profit [%]:\t" << (oldCost - minFoundCost) * 100 / oldCost << "%" << std::endl;


	if (problemSolution->validateSolution(*problemToSolve->bestSolution))
	{
		problemToSolve->bestSolution->calculatePathDemandForSolution(*problemToSolve->bestSolution);
		tempStr << "\nValidation best solution:\tOK\n";
	}
	else {
		tempStr << "\nValidation best solution:\tFAILED\n";
	}
	tempStr << "Generation time: " << timer.elapsed() << " s\n" << std::endl;
	Sleep(3000);
	outputFile.exportToTextFile(std::stringstream() << tempStr.str());
	//outputFile.exportGraphToFile(problemMap->graph);

	std::cout << "\n\n\n\n\n\n\n\n\n";

	system("cls");
	std::cout << tempStr.str();

	minFoundCost = 0;

}

bool TabuSearch::checkTabu(MethodResult& lastModification)
{
	if (_tabuList.size() > 0)
	{
		for (auto& tabuElement : _tabuList) {
			if (lastModification == tabuElement.first)	// If found forbidden modification
			{
				if (tabuElement.second == 0)			// And if forbidden modification is allowed (iterations > tabu tenure)
				{
					return true;						// Return true. The action being checked is allowed.			
				}
				else {
					return false;						// Return false. Action is forbidden because tabu period is not expired.
				}
			}
		}
	}	
	return true;	// Return TRUE, The action was not found in the tabu list.
}

void TabuSearch::updateTabu()
{
	int i = 0;
	if (_tabuList.size() > 0) {
		for (auto& tabuElement : _tabuList) {
			if (_tabuList.size() <= 0) {
				break;
			}
			if (tabuElement.second != 0) {
				tabuElement.second--;				// Decrease the iteration period for any prohibited actions.

				if(tabuElement.second == 0) {

					if (_tabuList.size() <= 1) {	// Avoid erase last element via index (begin+i) - it generates crash
						_tabuList.clear();			// Just clear all list.
					}
					else{
						_tabuList.erase(_tabuList.begin() + i);
					}
					// If prohibition period is 0, remove element from tabu list.
					//system("cls");
				}
			}
			i++;
		}
	}
}

void TabuSearch::addTabu(MethodResult& lastModification)
{
	_tabuList.push_back({ lastChange, _tabuTenure });	// Add to tabu list forbidden action for _tabuTenure iterations of algorithm.

}

std::stringstream TabuSearch::showTabuList()
{
	const int ROW_LIMIT = 7;

	std::stringstream tempStr;
	tempStr << "|TABU LIST (Last " << ROW_LIMIT << " rows) |\n";
	tempStr << "---------------------------------------------------------\n";
	tempStr << "Result, L1, L2,   V1, V2, \tMethod,\t\tTabu\n";
	tempStr << "---------------------------------------------------------\n";
	int i = 0;
	
	for (auto& tabuElement : _tabuList) {
		if (i > ROW_LIMIT)
		{
			break;
		}
		tempStr << "" << ((tabuElement.first.result == TRUE) ? "OK  " : "FAIL") << "\t";

		std::string tempL1 = "";
		std::string tempL2 = "";
		if (tabuElement.first.L1 == nullptr)
			tempL1 = "0";
		else
			tempL1 = std::to_string(tabuElement.first.L1->client->getId());
		if (tabuElement.first.L2 == nullptr)
			tempL2 = "0";
		else
			tempL2 = std::to_string(tabuElement.first.L2->client->getId());
		tempStr << tempL1<< "<->" << tempL2 << "\t";
		tempStr << "  V[" << tabuElement.first.vehicle1->getId() << "]<->V[" << tabuElement.first.vehicle2->getId() << "]\t";
		std::string tempMethod = "";

		if (tabuElement.first.usedMethod == EXCHANGE)
			tempMethod = "| E |\t";
		else if (tabuElement.first.usedMethod == SWAP)
			tempMethod = "| S |\t";
		else if (tabuElement.first.usedMethod == MOVE)
			tempMethod = "| M |\t";
		else if (tabuElement.first.usedMethod == UNKNOWN)
			tempMethod = "|M-S|\t";
		else
			tempMethod = "| ? |\t";
		tempStr << tempMethod << "\t " << tabuElement.second << "  \n";
		i++;
	}
	return tempStr;
}

/// @brief Method to find best solution from generated neighborhood for Tabu Search.
/// @param solution 
/// @return 

Solution* TabuSearch::modSolution(Solution& solution)
{
	Solution* neighborBest = nullptr;
	Solution* modifiedSolution = nullptr;

	double bestCost = INFINITY;
	double locationDemand = 0;

	
	bool modificationIsPossible = false;
	GenerationType modificationType = UNKNOWN;
	MethodResult bestResult = {};
	int i = 0; // iterator for vehicles
	int j = 0; // iterator for locations

	std::pair<Vehicle*, Vehicle*> vehiclesToSwap = randomChooseVehicle(solution);
	if (vehiclesToSwap == std::pair<Vehicle*, Vehicle*>())
		return nullptr;

	std::pair<Location*, Location*> locationsToSwap = randomChooseLocation(*vehiclesToSwap.first, *vehiclesToSwap.second, solution);
	if (locationsToSwap == std::pair<Location*, Location*>())
		return nullptr;

	if (locationsToSwap.first->client == nullptr || locationsToSwap.second->client == nullptr)
		return nullptr;

	Vehicle& v1 = *vehiclesToSwap.first;
	Location& l1 = *locationsToSwap.first;
	Location& l2 = *locationsToSwap.second;


	for (auto& obj : solution.locationsInSolution) {	// LOOP SELECTOR VEHICLES
		Vehicle& v1 = *obj.first;

		for (auto& location : obj.second) {				// LOOP SELECTOR LOCATIONS
		Location& l1 = *location;
			if (l1.client==nullptr) { // Skip depot
			continue;
		}
			locationDemand = l1.getLocationDemand();

			for (auto& path : solution.locationsInSolution) {	// Iterate over path for vehicles
				j = 0;


				for (auto& locInPath : path.second) {	// Iterate over every location from path for specified vehicle.
					j++;
					if (j == 0) {	// Skip first depot! - cannot change between depot (0 <-> something) (source location == depot, cannot be changed)
						
						continue;
					}
					if (l1.client->getId() == locInPath->client->getId()) { // Skip self
						continue;
					}

					// MAIN MODIFICATION PROCEDURE
					// PROCEDURE 1: CHECK IF CHANGES ARE POSSIBLE! SELECT APPROPRIATE MODIFICATION METHOD
					modifiedSolution = new Solution(solution); // Start again from solution before any changes

					// CASE 1: Changes between the same vehicles (only Exchange is allowed)
					if (path.first == &v1) {

						modificationType = EXCHANGE;

						if (locInPath->client == nullptr) // EXCHANGE between "something" and depot is FORBIDDEN!
						{
							continue;
						}
						else {
							modificationIsPossible = true;
						}
					}

					// CASE 2: Changes between other vehicles (Move/Swap is allowed)
					else {

						double sourceLocDemand = l1.getLocationDemand();
						double targetLocDemand = (locInPath->client == nullptr) ? 0.0 : locInPath->getLocationDemand();

						// Check type of second location (if is depot, or not)

						if (locInPath->client == nullptr) { // if is depot, we can 'move' first location, in depot (remove it) and at the end of path for vehicles add depot

							modificationType = MOVE;

							// At first, check if vehicle has capacity to move locations
							if (Vehicle::reloadDemandBetween(sourceLocDemand, targetLocDemand, v1, *path.first))
								modificationIsPossible = true;
							else
								modificationIsPossible = false;
						}
						else { // if is NOT a depot, we can swap locations, ofcourse if it is possiblem (overload problems etc).

							modificationType = SWAP;

							// Check if change is possible
							if (Vehicle::reloadDemandBetween(sourceLocDemand, targetLocDemand, v1, *path.first))
								modificationIsPossible = true;
							else
								modificationIsPossible = false;
						}

					}

					// PROCEDURE 2: CHECK IF 'POSSIBLE' CHANGE EXISTS IN TABU LIST

					MethodResult plannedModification = { modificationIsPossible, &l1, locInPath, &v1, path.first, modificationType };	// Fill informations about planned operation.

					if (modificationIsPossible) {

					//	if (checkTabu(plannedModification)) { // Check if planned modification doesn't exist at tabu list! If not, do modification! (checkTabu returns true if operation is allowed)

							if (modificationType != MOVE) { // general procedure of swap between every location

								swap(v1, *path.first, l1, *locInPath, modificationType, *modifiedSolution);
							}
							else { // Specific procedure to swap between 'something' and depot.

								// Move procedure: -delete from first vehicle, add at the end of second vehicle, but don't remove depot. Depot has to be in last position.

								auto oneBeforeLast = std::distance(modifiedSolution->locationsInSolution[path.first].begin(), modifiedSolution->locationsInSolution[path.first].end()) - 1;
								auto positionAtVehicle = std::find(solution.locationsInSolution[&v1].begin(), solution.locationsInSolution[&v1].end(), &l1);
								int inV = std::distance(solution.locationsInSolution[&v1].begin(), positionAtVehicle);

								modifiedSolution->locationsInSolution[path.first].insert(modifiedSolution->locationsInSolution[path.first].begin() + oneBeforeLast, &l1);	// Add location to second vehicle in position "one before last" (last = depot)
								modifiedSolution->locationsInSolution[&v1].erase(modifiedSolution->locationsInSolution[&v1].begin() + inV);				// Remove location from source vehicle
								lastChange.log(
									true,
									&l1,
									locInPath,
									&v1,
									path.first,
									modificationType);
							}

							modifiedSolution->updateRoutesFromLocations();
							modifiedSolution->calculatePathDemandForSolution(*modifiedSolution);
							modifiedSolution->calculateTotalCost();
							if (!modifiedSolution->validateSolution(*modifiedSolution)) {
								std::cout << "(TS modSolution) - CORRUPTED SOLUTION\n" << std::endl;
								delete modifiedSolution;
								continue;
							}
							if (checkSolution(solution, *modifiedSolution) == V_OVERLOAD) {
								delete modifiedSolution;
								modificationIsPossible = false;
								modificationType = UNKNOWN;
								continue;
							}
							

						//}
					//	else {
					//		continue; // Selected location (locInPath) cannot be changed. Skip this location, go to next location in solution.
					//	}
					}

					// PROCEDURE 3: CHECK IF SOLUTION IS BETTER

					if (modifiedSolution->totalCost < bestCost && modificationIsPossible == true) {
						if (checkTabu(lastChange) && modifiedSolution->totalCost < problemToSolve->bestSolution->totalCost)
						{
							delete neighborBest;
							neighborBest = new Solution(*modifiedSolution);
							//		_bestTabu = _tabuList;		// ??
							bestResult = lastChange; // save operation ??
							bestCost = neighborBest->totalCost;
						}
						else {
							delete modifiedSolution;
							modificationIsPossible = false;
							modificationType = UNKNOWN;
							
							continue;
						}

						
					}
					delete modifiedSolution;
					modificationIsPossible = false;
					modificationType = UNKNOWN;
					//lastChange = bestResult; // Return last operation as best solution. Next it could provide 'addTabu' data.
				}
				i++;
			}

			




		} // END (FOR LOOP) LOCATION SELECTOR
	} // END (FOR LOOP) VEHICLE SELECTOR

	
			lastChange = bestResult;
	return neighborBest;
}
