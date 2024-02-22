#include "Algorithm.h"
#include "UtilsFcn.h"
#include <algorithm>


Algorithm::Algorithm(Problem* problemToSolve)
{
	this->problemToSolve = problemToSolve;
	actualVehicle = this->problemToSolve->getVehicleList()->at(0);         ///< Always starting from first vehicle.
	problemSolution = this->problemToSolve->solution;
	problemMap = this->problemToSolve->getMap();
	lastChange = { false, 0, 0, nullptr, nullptr, UNUSED };
}

Algorithm::~Algorithm()
{
	//delete problemToSolve;
   // FileIO graphOut;
   // graphOut.exportGraphToFile(problemToSolve->getMap()->graph);
	std::cout << "Ending algorithm." << std::endl;
}

std::vector<Location*> Algorithm::toLocation(std::vector<Route*>& fromRoutePath)
{
	std::vector<Location*> temp;
	for (Route* element : fromRoutePath) {
		if (element->L1->client == nullptr)
		{
			temp.push_back(element->L1);
		}
		else {
			temp.push_back(element->L2);
		}
	}
	return temp;
}

Vertex Algorithm::getRandomNeighborVertexFrom(Vertex v)
{
	int min = 1;                                                            ///< Minimal number for random.
	std::random_device          rd;
	std::default_random_engine  gen(rd());

	size_t numNeighbors = boost::out_degree(v, problemMap->graph) - 1;      ///< Return amount of neighbors in actual vertices.
	std::uniform_int_distribution<> dist(min, numNeighbors);                ///< Random index of neighbor.

	int     randomVertexIndex = dist(gen);
	auto    edges = boost::out_edges(v, problemMap->graph);
	auto    targetEdge = edges.first;

	std::advance(targetEdge, randomVertexIndex);                            ///< Change verticle iterator to random index.

	return target(*targetEdge, problemMap->graph);                          ///< Return random iterator to neighbor verticles.
}

bool Algorithm::modify(Solution& solution)
{
	bool operationResult = false;

	int idxToRemove = 0;
	std::pair<Vehicle*, Vehicle*> vehiclesToSwap = randomChooseVehicle(solution);
	if (vehiclesToSwap == std::pair<Vehicle*, Vehicle*>())
		return operationResult;
	std::pair<Location*, Location*> locationsToSwap = randomChooseLocation(*vehiclesToSwap.first, *vehiclesToSwap.second, solution, idxToRemove); // Second location in move is unused.
	if (locationsToSwap == std::pair<Location*, Location*>())
		return operationResult;

	if (locationsToSwap.first->client == nullptr)   // First element to move cannot be moved if is depot! (depot: client == nulptr)
		return operationResult;

	// Get vehicle and location to change
	Vehicle* v1 = vehiclesToSwap.first;



	// Find proper vehicle for change selected first location
	for (auto& path : solution.locationsInSolution) {
		Vehicle* v2 = path.first;
		if (v1->getId() == v2->getId()) { // skip self (vehicle) 
			continue;
		}

		double remainingCapacity = solution.getRemainingCapacity(solution, *path.first); // calculate remainng capacity for vehicle
		double locationDemand = locationsToSwap.first->getLocationDemand();
		if (locationDemand <= remainingCapacity) {
			solution.locationsInSolution[v2].pop_back(); // delete depot (from actual last position)
			solution.locationsInSolution[v2].push_back(locationsToSwap.first); // add point to back
			solution.locationsInSolution[v2].push_back(solution.locationsInSolution[v2].at(0)); // add depot to end
			solution.locationsInSolution[v1].erase(solution.locationsInSolution[v1].begin() + idxToRemove);
			operationResult = true;
			lastChange.log(
				operationResult,
				locationsToSwap.first,
				locationsToSwap.first,
				v1,
				v2,
				MOVE);
			break;
		}
		else {
			operationResult = false;
		}
	}

	if (!operationResult) // if move operation is still wrong, try swap locations
	{
		for (auto& path : solution.locationsInSolution) {
			Vehicle* v2 = path.first;
			bool swapIsPossible = false;
			if (v1->getId() == v2->getId()) { // skip self (vehicle) - a może nie skipować, tylko po prostu tam robić specyficznego exchange, pomiędzy punktami.
				continue;
			}
			double remainingCapacityTarget = solution.getRemainingCapacity(solution, *v2); // calculate remainng capacity for vehicle
			double remainingCapacitySource = solution.getRemainingCapacity(solution, *v1);

			for (auto& location : path.second) // iterate over every location in specific vehicle
			{
				if (location->client == nullptr) // Depot cannot be changed
				{
					continue;
				}
				double swapCapacitySource = v2->getCapacity() - locationsToSwap.first->getLocationDemand();  // Vehicle capacity - demand of location to swap = vehicle possible capacity to accept swap from other demand location
				double locationDemand = location->getLocationDemand();
				if (locationDemand <= swapCapacitySource) {
					swapIsPossible = true;

					std::swap(*locationsToSwap.first, *location);

				}
				else { // Swap is impossible, try find other locations in vehicle
					swapIsPossible = false;
				}

				if (swapIsPossible) {
					break;
				}

			}
			if (swapIsPossible) {// if swap operation is done
				operationResult = true;
				lastChange.log(
					operationResult,
					locationsToSwap.first,
					locationsToSwap.second,
					v1,
					v2,
					SWAP);
				break;
			}   // if not, try find other vehicle and location to change.

		}
	}
	/*    if (!operationResult) {
			exchange(solution); // random exchange
			lastChange.log(
				operationResult,
				locationsToSwap.first,
				locationsToSwap.second,
				vehiclesToSwap.first,
				vehiclesToSwap.second,
				EXCHANGE);
		}   */
	if (!operationResult) // if in this place operation result is still wrong, log fail as unknown operation
	{
		lastChange.log(
			operationResult,
			locationsToSwap.first,
			locationsToSwap.second,
			vehiclesToSwap.first,
			vehiclesToSwap.second,
			UNKNOWN);
	}




	return operationResult;
}

bool Algorithm::modify(int i, int l, Solution& solution)
{
	i = (i > problemToSolve->getVehicleList()->size() - 1) ? 1 : i;

	bool operationResult = false;

	int idxToRemove = 0;
	std::pair<Vehicle*, Vehicle*> vehiclesToSwap = randomChooseVehicle(solution);
	if (vehiclesToSwap == std::pair<Vehicle*, Vehicle*>())
		return operationResult;
	std::pair<Location*, Location*> locationsToSwap = randomChooseLocation(*vehiclesToSwap.first, *vehiclesToSwap.second, solution, idxToRemove); // Second location in move is unused.
	if (locationsToSwap == std::pair<Location*, Location*>())
		return operationResult;

	if (locationsToSwap.first->client == nullptr)   // First element to move cannot be moved if is depot! (depot: client == nulptr)
		return operationResult;

	// Get vehicle and location to change
	Vehicle* v1 = problemToSolve->getVehicleList()->at(i);
	Location* l1 = problemToSolve->solution->locationsInSolution[v1].at(l);


	// Find proper vehicle for change selected first location
	for (auto& path : solution.locationsInSolution) {
		Vehicle* v2 = path.first;
		if (v1->getId() == v2->getId()) { // skip self (vehicle) 
			continue;
		}

		double remainingCapacity = solution.getRemainingCapacity(solution, *path.first); // calculate remainng capacity for vehicle
		double locationDemand = l1->getLocationDemand();
		if (locationDemand <= remainingCapacity) {
			double temp = 0.0;
			if (Vehicle::reloadDemandBetween(locationDemand, temp, *v1, *v2))
			{
				auto index = std::find(problemToSolve->solution->locationsInSolution[v1].begin(), problemToSolve->solution->locationsInSolution[v1].end(), l1);
				int idx = std::distance(problemToSolve->solution->locationsInSolution[v1].begin(), index);

				solution.locationsInSolution[v2].pop_back(); // delete depot (from actual last position)
				solution.locationsInSolution[v2].push_back(l1); // add point to back
				solution.locationsInSolution[v2].push_back(solution.locationsInSolution[v2].at(0)); // add depot to end
				solution.locationsInSolution[v1].erase(solution.locationsInSolution[v1].begin() + idx);
				operationResult = true;
				lastChange.log(
					operationResult,
					l1,
					locationsToSwap.first,
					v1,
					v2,
					MOVE);
				break;
			}
			else {
				operationResult = false;
			}
		}
		else {
			operationResult = false;
		}
	}

	if (!operationResult) // if move operation is still wrong, try swap locations
	{
		for (auto& path : solution.locationsInSolution) {
			Vehicle* v2 = path.first;
			bool swapIsPossible = false;
			if (v1->getId() == v2->getId()) { // skip self (vehicle) 
				continue;
			}
			double remainingCapacityTarget = solution.getRemainingCapacity(solution, *v2); // calculate remainng capacity for vehicle
			double remainingCapacitySource = solution.getRemainingCapacity(solution, *v1);

			for (auto& location : path.second) // iterate over every location in specific vehicle
			{
				if (location->client == nullptr) // Depot cannot be changed
				{
					continue;
				}
				if (location == l1) {
					continue;
				}
				double swapCapacitySource = v2->getCapacity() - l1->getLocationDemand();  // Vehicle capacity - demand of location to swap = vehicle possible capacity to accept swap from other demand location
				double locationDemand = location->getLocationDemand();
				double sourceDemand = l1->getLocationDemand();
				if (locationDemand <= swapCapacitySource) {
					if (Vehicle::reloadDemandBetween(sourceDemand, locationDemand, *v1, *v2))
					{
						swapIsPossible = true;

						std::swap(*l1, *location);
					}
					else {
						swapIsPossible = false;
					}
				}
				else { // Swap is impossible, try find other locations in vehicle
					swapIsPossible = false;
				}

				if (swapIsPossible) {
					break;
				}

			}
			if (swapIsPossible) {// if swap operation is done
				operationResult = true;
				lastChange.log(
					operationResult,
					l1,
					locationsToSwap.second,
					v1,
					v2,
					SWAP);
				break;
			}   // if not, try find other vehicle and location to change.

		}
	}

	if (!operationResult) // if in this place operation result is still wrong, log fail as unknown operation
	{
		exchange(solution);
		lastChange.log(
			operationResult,
			locationsToSwap.first,
			locationsToSwap.second,
			vehiclesToSwap.first,
			vehiclesToSwap.second,
			EXCHANGE);
	}

	if (!operationResult) // if in this place operation result is still wrong, log fail as unknown operation
	{
		lastChange.log(
			operationResult,
			locationsToSwap.first,
			locationsToSwap.second,
			vehiclesToSwap.first,
			vehiclesToSwap.second,
			UNKNOWN);
	}
	return operationResult;
}

std::pair<Vehicle*, Vehicle*> Algorithm::randomChooseVehicle(Solution& solution) {
	const int LOOP_LIMIT = 100;

	int numVehicles = problemToSolve->getVehicleList()->size();     // Get number of vehicles
	int rdVehicleIterator1 = 0, rdVehicleIterator2 = 0;
	int loop = 0;

	while (rdVehicleIterator1 == rdVehicleIterator2 && loop < LOOP_LIMIT) { // Draw random iterator - must be different!               // Believe that vehicles more than 1!
		rdVehicleIterator1 = randomInt(0, numVehicles - 1);
		rdVehicleIterator2 = randomInt(0, numVehicles - 1);
		loop++;
	}

	if (loop >= LOOP_LIMIT)
	{
		return std::pair<Vehicle*, Vehicle*>();
	}

	Vehicle* v1 = problemToSolve->getVehicleList()->at(rdVehicleIterator1);
	Vehicle* v2 = problemToSolve->getVehicleList()->at(rdVehicleIterator2);

	return std::make_pair(v1, v2);
}

std::pair<Location*, Location*> Algorithm::randomChooseLocation(
	Vehicle& sourceVehiclePath,
	Vehicle& targetVehiclePath,
	Solution& solution
)
{
	int i = -1;
	return randomChooseLocation(sourceVehiclePath, targetVehiclePath, solution, i);
}

std::pair<Location*, Location*> Algorithm::randomChooseLocation(
	Vehicle& sourceVehiclePath,
	Vehicle& targetVehiclePath,
	Solution& solution,
	int& isMoveReturn
)
{
	const int PATH_WITH_ONE_LOCATION = 3;
	const int PATH_WITH_ONLY_DEPOTS = 2;
	const int EMPTY_LOCATION_PATH = 1;  // Empty location path has 2 elements in vector<Location*> which means: Demand -> Demand (0 -> 0)

	Location* l1 = nullptr;
	Location* l2 = nullptr;

	int sizeLocSrc = solution.locationsInSolution[&sourceVehiclePath].size();        // Get number of locations stored at path for vehicle source
	int sizeLocDest = solution.locationsInSolution[&targetVehiclePath].size();        // Get number of locations stored at path for vehicle target/destination
	int rdLocIterator1 = 0, rdLocIterator2 = 0;

	if (isMoveReturn == -1) {                        // Check if function is calling by MOVE (!= -1) method or SWAP(==-1) method
		if (sizeLocSrc <= PATH_WITH_ONLY_DEPOTS || sizeLocDest <= PATH_WITH_ONLY_DEPOTS)        // For SWAP cannot be depot in source and target.
			return std::pair<Location*, Location*>();
	}
	else {
		if (sizeLocSrc <= PATH_WITH_ONLY_DEPOTS)    // For MOVE cannot be depot only in source (it is possible to append to empty (0->0) location paths  
			return std::pair<Location*, Location*>();
	}

	if (sizeLocSrc <= EMPTY_LOCATION_PATH || sizeLocDest <= EMPTY_LOCATION_PATH)
		return std::pair<Location*, Location*>();

	rdLocIterator1 = (sizeLocSrc != PATH_WITH_ONE_LOCATION) ? randomInt(1, sizeLocSrc - 2) : 1;     // If number of location for source vehicle has only one element, return 1. If not, return any element, but except first (depot) and last (also depot)

	if (isMoveReturn == -1) {   // If SWAP method is called
		rdLocIterator2 = (sizeLocDest != PATH_WITH_ONE_LOCATION) ? randomInt(1, sizeLocDest - 2) : 1;     // If number of location for target vehicle has only one element, return 1. If not, return any element, but except first (depot) and last (also depot)
		l2 = solution.locationsInSolution[&targetVehiclePath].at(rdLocIterator2);
	}

	l1 = solution.locationsInSolution[&sourceVehiclePath].at(rdLocIterator1);

	if (isMoveReturn != -1) {
		int i = rdLocIterator1;
		isMoveReturn = i;
	}

	return std::make_pair(l1, l2);
}

bool Algorithm::generateInitialSolution() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::vector<Location*> pathForVehicle;

	bool routeIsFound = false;
	int createdVehicles = 0;
	int expectedVehicles = (int)(ceil(problemMap->getTotalDemand() / actualVehicle->getMaximumCapacity()));
	double nonDeliveredDemand = problemMap->getTotalDemand();

	auto copyVertices = problemToSolve->getMap()->graph.m_vertices;
	copyVertices.erase(copyVertices.begin()); // delete depot from list of clients

	actualVehicle = problemToSolve->getVehicleList()->at(0);
	actualVehicle->setCapacity(0);
	createdVehicles++;

	while (!copyVertices.empty()) // Work until every location will be visited
	{
		const int SELECT_LOCATION_FAIL = -1;
		static int infiniteProtection = 0;
		static int unsuccessfullAttempts = 0;
		int infiniteLimit = problemToSolve->getClientList()->size() + 100000;
		int selectedLocation = 0;
		double remainingVehicleCapacity = actualVehicle->getMaximumCapacity() - actualVehicle->getCapacity();

		std::shuffle(copyVertices.begin(), copyVertices.end(), gen);

		while (!actualVehicle->takeLoad(copyVertices[selectedLocation].m_property.getLocationDemand())) {  // If vehicle will be overloaded after add location demand.
			if (selectedLocation < copyVertices.size() - 1) {  // Try add other location from the list to vehicle, if selectedLocation will be last element, set -1, and then break a loop.
				selectedLocation++;
			}
			else {
				selectedLocation = SELECT_LOCATION_FAIL;
				break;
			}
		}

		if (selectedLocation != SELECT_LOCATION_FAIL) {
			Vertex v1 = problemMap->findVertex(copyVertices[selectedLocation].m_property);

			if (!problemMap->graph[v1].isVisited()) {

				problemMap->graph[v1].makeVisited();
				nonDeliveredDemand -= problemMap->graph[v1].getLocationDemand();
				remainingVehicleCapacity = actualVehicle->getMaximumCapacity() - actualVehicle->getCapacity();
				//problemToSolve->getMap()->graph.m_vertices.at(selectedLocation).m_property.makeVisited();
				copyVertices[selectedLocation].m_property.makeVisited();

				pathForVehicle.push_back(&problemToSolve->getMap()->graph.m_vertices[v1].m_property);               // Set the ORIGINAL location as visited

				copyVertices.erase(copyVertices.begin() + selectedLocation);

				if (copyVertices.empty()) // case for last vehicle
				{
					pathForVehicle.insert(pathForVehicle.begin(), &problemMap->graph.m_vertices[0].m_property);         // Add depot as first!
					pathForVehicle.insert(pathForVehicle.end(), &problemMap->graph.m_vertices[0].m_property);           // Add depot as last!
					problemSolution->locationsInSolution[actualVehicle] = pathForVehicle;                               // Add existing "full" solution for vehicle to locationsInSolution
					problemSolution->updateRoutesForSpecificLocations(*actualVehicle);                                  // Convert new locationsInSolution to routesInSolution
					problemSolution->calculatePathDemandForSolution(*problemSolution);
				}
			}
			else {
				continue;
			}
		}
		else if (remainingVehicleCapacity <= 0 || selectedLocation == SELECT_LOCATION_FAIL) {  // Locations are still unvisited, but cannot add new location to vehicle, because it will be overloaded, so make new vehicle!
			pathForVehicle.insert(pathForVehicle.begin(), &problemMap->graph.m_vertices[0].m_property);         // Add depot as first!
			pathForVehicle.insert(pathForVehicle.end(), &problemMap->graph.m_vertices[0].m_property);           // Add depot as last!

			problemSolution->locationsInSolution[actualVehicle] = pathForVehicle;                               // Add existing "full" solution for vehicle to locationsInSolution
			problemSolution->updateRoutesForSpecificLocations(*actualVehicle);                                  // Convert new locationsInSolution to routesInSolution
			problemSolution->calculatePathDemandForSolution(*problemSolution);

			problemToSolve->getVehicleList()->emplace_back(new Vehicle(*actualVehicle));                        //Create copy of actual vehicle, but with empty capacity (capacity = 0)     
			actualVehicle = problemToSolve->getVehicleList()->back();                                           // Set pointer to new vehicle
			createdVehicles++;
			pathForVehicle.clear();                                                                             // Clear temporary path for vehicle
			unsuccessfullAttempts = 0;
		}
		infiniteProtection++;
		if (infiniteProtection > infiniteLimit)
		{
			std::cout << "\nINFINITY PROTECTION! > INITIAL SOLUTION < BREAK! \n" << std::endl;
			infiniteProtection = 0;
			break;
		}
	}

	std::cout << "Preparing initial solution..\nNumber of nodes:\t" << problemMap->getNumberOfLocations() << "\nTotal nodes demand:\t" << problemMap->getTotalDemand() << "\nExpected vehicles:\t" << expectedVehicles << "\nCreated vehicles:\t" << createdVehicles << std::endl;
	std::cout << problemSolution->show().str();
	if (problemSolution->validateSolution()) {
		return true;
	}
	else {
		return false;
	}

}





bool Algorithm::swap(Vehicle& v1, Vehicle& v2, Location& l1, Location& l2, GenerationType methodType, Solution& solution)  // Danger operation, use only when you know what do you want to do!
{
	bool operationResult = false;

	if (l1.client == nullptr || l2.client == nullptr)
		return operationResult;

	auto l1_positionAtVehicle1 = std::find(solution.locationsInSolution[&v1].begin(), solution.locationsInSolution[&v1].end(), &l1); // find l1 in v1
	auto l2_positionAtVehicle2 = std::find(solution.locationsInSolution[&v2].begin(), solution.locationsInSolution[&v2].end(), &l2); // find l2 in v2

	int inV1 = std::distance(solution.locationsInSolution[&v1].begin(), l1_positionAtVehicle1); // return index of element l1
	int inV2 = std::distance(solution.locationsInSolution[&v2].begin(), l2_positionAtVehicle2); // return index of element l2

	Location* temp = solution.locationsInSolution[&v1].at(inV1);                                // save first location in v1
	solution.locationsInSolution[&v1].at(inV1) = solution.locationsInSolution[&v2].at(inV2);    // swap l2 to vehicle1
	solution.locationsInSolution[&v2].at(inV2) = temp;                                          // swap saved l1 to vehicle2


	//   std::swap(l1, l2);// > czy na pewno? :|
	operationResult = true;

	// Add history log:
	lastChange.log(
		operationResult,
		&l1,
		&l2,
		&v1,
		&v2,
		methodType);

	return operationResult;
}

bool Algorithm::exchange(Solution& solution)
{
	bool operationResult = false;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::vector<Location*> pathForVehicle;

	std::pair<Vehicle*, Vehicle*> vehiclesToSwap = randomChooseVehicle(solution);
	if (vehiclesToSwap == std::pair<Vehicle*, Vehicle*>())
		return operationResult;

	std::pair<Location*, Location*> locationsToSwap = randomChooseLocation(*vehiclesToSwap.first, *vehiclesToSwap.first, solution);
	if (locationsToSwap == std::pair<Location*, Location*>())
		return operationResult;

	if (locationsToSwap.first->client == nullptr || locationsToSwap.second->client == nullptr)
		return operationResult;

	double demandSource = locationsToSwap.first->getLocationDemand();
	double demandTarget = locationsToSwap.second->getLocationDemand();


	std::swap(*locationsToSwap.first, *locationsToSwap.second);
	operationResult = true;

	// Add history log:
	lastChange.log(
		operationResult,
		locationsToSwap.first,
		locationsToSwap.second,
		vehiclesToSwap.first,
		vehiclesToSwap.second,
		EXCHANGE);

	return operationResult;
}



bool Algorithm::swap(Solution& solution)
{
	bool operationResult = false;
	std::pair<Vehicle*, Vehicle*> vehiclesToSwap = randomChooseVehicle(solution);
	if (vehiclesToSwap == std::pair<Vehicle*, Vehicle*>())
		return operationResult;

	std::pair<Location*, Location*> locationsToSwap = randomChooseLocation(*vehiclesToSwap.first, *vehiclesToSwap.second, solution);
	if (locationsToSwap == std::pair<Location*, Location*>())
		return operationResult;

	if (locationsToSwap.first->client == nullptr || locationsToSwap.second->client == nullptr)
		return operationResult;

	double demandSource = locationsToSwap.first->getLocationDemand();
	double demandTarget = locationsToSwap.second->getLocationDemand();

	if (Vehicle::reloadDemandBetween(demandSource, demandTarget, *vehiclesToSwap.first, *vehiclesToSwap.second))
	{
		std::swap(*locationsToSwap.first, *locationsToSwap.second);
		operationResult = true;
	}
	else {
		operationResult = false;
	}

	// Add history log:
	lastChange.log(
		operationResult,
		locationsToSwap.first,
		locationsToSwap.second,
		vehiclesToSwap.first,
		vehiclesToSwap.second,
		SWAP);

	return operationResult;

}

bool Algorithm::move(Solution& solution)
{
	bool operationResult = false;
	int idxToRemove = 0;
	std::pair<Vehicle*, Vehicle*> vehiclesToSwap = randomChooseVehicle(solution);
	if (vehiclesToSwap == std::pair<Vehicle*, Vehicle*>())
		return operationResult;

	std::pair<Location*, Location*> locationsToSwap = randomChooseLocation(*vehiclesToSwap.first, *vehiclesToSwap.second, solution, idxToRemove); // Second location in move is unused.
	if (locationsToSwap == std::pair<Location*, Location*>())
		return operationResult;

	if (locationsToSwap.first->client == nullptr)   // First element to move cannot be moved if is depot! (depot: client == nulptr)
		return operationResult;

	double demandSource = locationsToSwap.first->getLocationDemand();
	double demandTarget = 0.0;

	if (Vehicle::reloadDemandBetween(demandSource, demandTarget, *vehiclesToSwap.first, *vehiclesToSwap.second))
	{
		solution.locationsInSolution[vehiclesToSwap.second].pop_back(); // delete depot (from actual last position)
		solution.locationsInSolution[vehiclesToSwap.second].push_back(locationsToSwap.first); // add point to back
		solution.locationsInSolution[vehiclesToSwap.second].push_back(solution.locationsInSolution[vehiclesToSwap.second].at(0)); // add depot to end
		solution.locationsInSolution[vehiclesToSwap.first].erase(solution.locationsInSolution[vehiclesToSwap.first].begin() + idxToRemove);
		operationResult = true;
	}
	else {
		operationResult = false;
	}

	// Add history log:
	lastChange.log(
		operationResult,
		locationsToSwap.first,
		locationsToSwap.second,
		vehiclesToSwap.first,
		vehiclesToSwap.second,
		MOVE);

	return operationResult;
}

SolutionQuality Algorithm::checkSolution(Solution& oldSolution, Solution& newSolution) //nwm czy uzywac
{
	//Vehicle cannot be overloaded. Check if every demand in path is not higher than capacity of vehicle.
	oldSolution.calculatePathDemandForSolution(oldSolution);
	newSolution.calculatePathDemandForSolution(newSolution);
	oldSolution.calculateTotalCost();
	newSolution.calculateTotalCost();

	for (auto& demand : newSolution.demandPaths) {
		if (demand.second > demand.first->getMaximumCapacity()) {
			// std::cout << "\n\t\t\t\tVehicle <" << demand.first->getId() << "> is overloaded! Cap:" << demand.first->getMaximumCapacity() << " Current: " << demand.second;
			return V_OVERLOAD;
		}
	}

	if (!newSolution.validateSolution(newSolution)) {
		char s = ' ';
		//  std::cout << std::setiosflags(std::ios::fixed) << std::setfill(s) << std::right << std::setw(100) << "\n\t\t";
		return UNVISITED_NODES;
	}
	else if (newSolution.totalCost <= oldSolution.totalCost) {
		char s = ' ';
		//   std::cout << std::setiosflags(std::ios::fixed) << std::setfill(s) << std::right << std::setw(100) << "\n\t\t";
		return BETTER;
	}
	else {
		char s = ' ';
		//    std::cout << std::setiosflags(std::ios::fixed) << std::setfill(s) << std::right << std::setw(100) << "\n\t\t";
		return NON_PROFIT;
	}

	return UNDEFINED_SOLUTION_ERROR;
}

Solution* Algorithm::rndGenerator(Solution& solution) {
	Solution* neighborBest = nullptr;
	Solution* modifiedSolution = nullptr;

	double bestCost = INFINITY;
	double locationDemand = 0;

	Vehicle* v1 = nullptr;
	Location* l1 = nullptr;


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

	v1 = vehiclesToSwap.first;
	l1 = locationsToSwap.first;


	locationDemand = l1->getLocationDemand();

	for (auto& path : solution.locationsInSolution) {	// Iterate over path for vehicles
		j = 0;


		for (auto& locInPath : path.second) {	// Iterate over every location from path for specified vehicle.
			j++;
			if (j == 0) {	// Skip first depot! - cannot change between depot (0 <-> something) (source location == depot, cannot be changed)

				continue;
			}
			if (l1->client->getId() == locInPath->client->getId()) { // Skip self
				continue;
			}

			// MAIN MODIFICATION PROCEDURE
			// PROCEDURE 1: CHECK IF CHANGES ARE POSSIBLE! SELECT APPROPRIATE MODIFICATION METHOD
			modifiedSolution = new Solution(solution); // Start again from solution before any changes

			// CASE 1: Changes between the same vehicles (only Exchange is allowed)
			if (path.first == v1) {

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

				double sourceLocDemand = l1->getLocationDemand();
				double targetLocDemand = (locInPath->client == nullptr) ? 0.0 : locInPath->getLocationDemand();

				// Check type of second location (if is depot, or not)

				if (locInPath->client == nullptr) { // if is depot, we can 'move' first location, in depot (remove it) and at the end of path for vehicles add depot

					modificationType = MOVE;

					// At first, check if vehicle has capacity to move locations
					if (Vehicle::reloadDemandBetween(sourceLocDemand, targetLocDemand, *v1, *path.first))
						modificationIsPossible = true;
					else
						modificationIsPossible = false;
				}
				else { // if is NOT a depot, we can swap locations, ofcourse if it is possiblem (overload problems etc).

					modificationType = SWAP;

					// Check if change is possible
					if (Vehicle::reloadDemandBetween(sourceLocDemand, targetLocDemand, *v1, *path.first))
						modificationIsPossible = true;
					else
						modificationIsPossible = false;
				}

			}

			// PROCEDURE 2: CHECK IF 'POSSIBLE' CHANGE EXISTS IN TABU LIST

			MethodResult plannedModification = { modificationIsPossible, l1, locInPath, v1, path.first, modificationType };	// Fill informations about planned operation.

			if (modificationIsPossible) {

				//	if (checkTabu(plannedModification)) { // Check if planned modification doesn't exist at tabu list! If not, do modification! (checkTabu returns true if operation is allowed)

				if (modificationType != MOVE) { // general procedure of swap between every location

					swap(*v1, *path.first, *l1, *locInPath, modificationType, *modifiedSolution);
				}
				else { // Specific procedure to swap between 'something' and depot.

					// Move procedure: -delete from first vehicle, add at the end of second vehicle, but don't remove depot. Depot has to be in last position.

					auto oneBeforeLast = std::distance(modifiedSolution->locationsInSolution[path.first].begin(), modifiedSolution->locationsInSolution[path.first].end()) - 1;
					auto positionAtVehicle = std::find(solution.locationsInSolution[v1].begin(), solution.locationsInSolution[v1].end(), l1);
					int inV = std::distance(solution.locationsInSolution[v1].begin(), positionAtVehicle);

					modifiedSolution->locationsInSolution[path.first].insert(modifiedSolution->locationsInSolution[path.first].begin() + oneBeforeLast, l1);	// Add location to second vehicle in position "one before last" (last = depot)
					modifiedSolution->locationsInSolution[v1].erase(modifiedSolution->locationsInSolution[v1].begin() + inV);				// Remove location from source vehicle
					lastChange.log(
						true,
						l1,
						l1,
						v1,
						path.first,
						modificationType);
				}

				modifiedSolution->updateRoutesFromLocations();
				modifiedSolution->calculatePathDemandForSolution(*modifiedSolution);
				modifiedSolution->calculateTotalCost();
				if (!modifiedSolution->validateSolution(*modifiedSolution)) {
					std::cout << "(TS modSolution) - CORRUPTED SOLUTION\n" << std::endl;
					continue;
				}

				//}
			//	else {
			//		continue; // Selected location (locInPath) cannot be changed. Skip this location, go to next location in solution.
			//	}
			}

			// PROCEDURE 3: CHECK IF SOLUTION IS BETTER

			if (modifiedSolution->totalCost < bestCost && modificationIsPossible == true) {
				delete neighborBest;
				neighborBest = new Solution(*modifiedSolution);

				bestResult = lastChange; // save operation ??
				bestCost = neighborBest->totalCost;
			}
			delete modifiedSolution;
			modificationIsPossible = false;
			modificationType = UNKNOWN;
			//lastChange = bestResult; // Return last operation as best solution. Next it could provide 'addTabu' data.
		}
		i++;
	}
	lastChange = bestResult;



	return neighborBest;
}

Solution* Algorithm::bestGenerator(int genIter, Solution& solution)
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

	for (auto& obj : solution.locationsInSolution) {	// LOOP SELECTOR VEHICLES
		Vehicle& v1 = *obj.first;

		for (auto& location : obj.second) {				// LOOP SELECTOR LOCATIONS
			Location& l1 = *location;
			if (l1.client == nullptr) { // Skip depot
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
								&l1,
								&v1,
								path.first,
								modificationType);
						}

						modifiedSolution->updateRoutesFromLocations();
						modifiedSolution->calculatePathDemandForSolution(*modifiedSolution);
						modifiedSolution->calculateTotalCost();
						if (!modifiedSolution->validateSolution(*modifiedSolution)) {
							std::cout << "(LS modSolution) - CORRUPTED SOLUTION\n" << std::endl;
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
						delete neighborBest;
						neighborBest = new Solution(*modifiedSolution);
						//		_bestTabu = _tabuList;		// ??
						bestResult = lastChange; // save operation ??
						bestCost = neighborBest->totalCost;
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

