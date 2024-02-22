#include "Solution.h"
#include "boost/graph/lookup_edge.hpp"
Solution::Solution()
{
	_problem = nullptr;
	totalCost = -1;
}

Solution::Solution(Problem *p)
{
	_problem = p;
	totalCost = INFINITY;
}

Solution::~Solution()
{
//	delete _problem;
}

Solution::Solution(const Solution& toCopy)
{
	locationsInSolution = toCopy.locationsInSolution;
	routesInSolution = toCopy.routesInSolution;
	totalCost = toCopy.totalCost;
	this->calculatePathDemandForSolution(*this);
	_problem = toCopy._problem;
}

void Solution::operator=(const Solution& toCopy) {
	this->locationsInSolution = toCopy.locationsInSolution;
	this->routesInSolution = toCopy.routesInSolution;
	this->demandPaths = toCopy.demandPaths;
	this->totalCost = toCopy.totalCost;
	this->_problem = toCopy._problem;
}

void Solution::updateRoutesForSpecificLocations(Vehicle& specifiedPath)
{
	updateRoutesForSpecificLocations(*this->_problem->solution, specifiedPath);
}

void Solution::updateRoutesForSpecificLocations(Solution& selectedSolution, Vehicle& specifiedPath)
{

	Map& m = *selectedSolution._problem->getMap();				///< Shortcut to graph in Map.h
	selectedSolution.routesInSolution[&specifiedPath].clear();

	int i = 0;
	std::pair<Location*, Location*> temp;						///< Container for source location (L1 in route) and target location (L2) in route
	if (selectedSolution.locationsInSolution[&specifiedPath].size() == 2)
	{
		Location* depotL1 = selectedSolution.locationsInSolution[&specifiedPath].at(0); // At 0 because we have only one location to delivery.
		Location* depotL2 = selectedSolution.locationsInSolution[&specifiedPath].at(1); // At 1 because we have only one location to delivery.
		Route* stayInDepot = new Route(*depotL1, *depotL2);
		selectedSolution.routesInSolution[&specifiedPath].push_back(stayInDepot);		///< Insert to routesInSolution routes
	}
	else if (selectedSolution.locationsInSolution[&specifiedPath].size() == 3) { // Specific case, when in vehicle is only one location, so final route should be Depot -> location -> Depot
		
		int v_id = selectedSolution.locationsInSolution[&specifiedPath].at(1)->client->getId(); // At 0 because we have only one location to delivery.

		Vertex v1 = vertex(v_id, m.graph);

		Edge foundEdge = boost::edge(0, v1, m.graph).first;	// Depot -> location
		Route* found = m.graph[foundEdge];
		selectedSolution.routesInSolution[&specifiedPath].push_back(found);

		foundEdge = boost::edge(v1, 0, m.graph).first;		// location -> Depot
		found = m.graph[foundEdge];
		selectedSolution.routesInSolution[&specifiedPath].push_back(found);
	}
	else {	// Standard case with minimum 2 locations in subSolution
		for (auto& locations : selectedSolution.locationsInSolution[&specifiedPath])		///< Iterate over every location points for current instance of vehicle
		{
			if (i > 0)
			{
				temp.second = locations;				///< L2 (in route)
				
				// FOR DEBUG -> //	std::cout << "DEBUG: U_R_F_L: <" << specifiedPath.getId() << "> First: " << temp.first->client->getId() << "\tSecond: " << temp.second->client->getId() << std::endl;

				try
				{
					std::pair<int, int> v_id = [](std::pair<Location*, Location*> pairSourceToTarget) {  // Return id of clients - because id of client == iterator in graph
						std::pair<int, int> i;
						i.first = (int)(pairSourceToTarget.first->client->getId());
						i.second = (int)(pairSourceToTarget.second->client->getId());
						return i;
					}(temp);

					Vertex v1 = vertex(v_id.first, m.graph);					///< Get vertex v1 from the property
					Vertex v2 = vertex(v_id.second, m.graph);					///< Get vertex v2 from the property
					Edge foundEdge;
					Route* found = nullptr;
					//Edge FOUNDge= m.getEdge(v1, v2);							///< Try find Edge in Graph between two vertices
					//	if (v1 != 0 && v2 != 0)
					//	{
					foundEdge = boost::edge(v1, v2, m.graph).first;
					found = m.graph[foundEdge];
					//	}
					
					//m.graph.m_edges[FOUNDge];
					//	if (m.findEdgeByVertices(m.graph, v1, v2, FOUNDge)) {
					//			std::cout << "Znaleziono krawedz!!!" << std::endl;
					//	}

					//Route* foundRoute = m.getRouteFromEdge(FOUNDge);						///< Try found route corelated with FOUND edge
					selectedSolution.routesInSolution[&specifiedPath].push_back(found);		///< Insert to routesInSolution routes
				}
				catch (const std::runtime_error& e)
				{
					std::cout << "Edge not found!" << std::endl;
					std::cout << e.what() << '\n';
					continue;
				}										///< Search edge between two locations L1->L2
				std::swap(temp.first, temp.second);		///< Swap second locations to L1.
			}
			temp.first = locations;						///< L1 (in route)
			i++;
		}

	}
}

void Solution::updateRoutesFromLocations()
{
	updateRoutesFromLocations(*this);
}

void Solution::updateRoutesFromLocations(Solution& selectedSolution)
{
	for (auto& locationsForVehicle : locationsInSolution) {
		Vehicle& actualVehicle = *locationsForVehicle.first;	///< Get actual vehicle instance
		updateRoutesForSpecificLocations(selectedSolution, actualVehicle);
	}
}

double Solution::calculateTotalCost()
{
	return calculateTotalCost(*this);
 }

double Solution::calculateTotalCost(Solution& selectedSolution)
{
	double cost = 0;
	for (auto& subSolution : selectedSolution.routesInSolution) {
		Vehicle& actualVehicle = *subSolution.first;	///< Get actual vehicle instance
		cost += getPathCost(selectedSolution, actualVehicle);
	}
	totalCost = cost;
	return cost;
}

double Solution::calculatePathDemandForSolution(Solution& selectedSolution)
{
	double totalDemand = 0;
	for (auto& path : selectedSolution.locationsInSolution) {
		totalDemand += getPathDemand(selectedSolution, *path.first);
	}
	return totalDemand;
}

double Solution::getPathCost(Vehicle& specifiedPath)
{
	return getPathCost(*this, specifiedPath);
}

double Solution::getPathCost(Solution& selectedSolution, Vehicle& specifiedPath)
{
	double cost = 0;
	
	for (auto& route : selectedSolution.routesInSolution[&specifiedPath])		///< Iterate over every location points for current instance of vehicle
	{
		if (route == nullptr)
		{
			continue;
		}
		//	if (route->L1->client == nullptr || route->L2->client == nullptr) {	/// TEST WITHOUT ROUTES FROM DEPOT!!
		//		cost += 0;
		//		continue;
		//	}
		cost += route->cost;
	}
	return cost;
}
double Solution::getPathDemand(Solution& selectedSolution, Vehicle& specifiedPath)
{
	double demand = 0;
	for (auto& location : selectedSolution.locationsInSolution[&specifiedPath])		///< Iterate over every location points for current instance of vehicle
	{
		//specifiedPath.setCapacity(demand);
		if(location->client != nullptr)
			demand += location->client->capacityDemand;
	}
	demandPaths[&specifiedPath] = demand;
	return demand;
}

double Solution::getRemainingCapacity(Solution& selectedSolution, Vehicle& specifiedPath)
{
	double demand = selectedSolution.demandPaths[&specifiedPath];	
	return (_problem->getVehicleList()->at(0)->getMaximumCapacity() - demand);
}

Vehicle* Solution::findVehicleByNeededCapacity(Solution& selectedSolution, double capacity)
{
	for (auto& vehicle : selectedSolution.locationsInSolution)		
	{
		double vehicleReaminingCapacity = getRemainingCapacity(selectedSolution, *vehicle.first);
		if (vehicleReaminingCapacity >= capacity) {
			return vehicle.first;
		}
	}
	return nullptr;
	
}

bool Solution::validateSolution()
{
	return validateSolution(*this); //nonsens
}

bool Solution::validateSolution(Solution& selectedSolution)
{
	bool result = true;
	int i = 0;
	int expectedNodes = selectedSolution._problem->getMap()->getNumberOfLocations();

	std::map<int, int> occurrency;

	std::vector<Client*>* clientList = selectedSolution._problem->getClientList();
	for (auto& client : *clientList)
	{
		std::vector<int> temp;
		for (auto& subSolution : selectedSolution.locationsInSolution) {
			for (auto& clientFromSubSolution : subSolution.second) {
				temp.push_back(clientFromSubSolution->client->getId());
			}
			i = std::count(temp.begin(), temp.end(), client->getId());

		}
		occurrency.insert({ client->getId(), i });
		temp.clear();

		if (i != 1)
		{
			result = false;
		}

#ifndef NO_PRINTABLE_DEBUG
		std::cout << "Client: " << client->getId() << " Occurrency: " << i << std::endl;
#endif // !NO_PRINTABLE_DEBUG

	}

	//std::cout << "\nVALIDATION RESULT:\t" << ((result == true) ? "OK" : "FAILED") << std::endl;;
	return result;
}

std::stringstream Solution::show()
{
	const char s = ' ';
	const char s2 = '*';
	std::stringstream temp;
	for (auto& vehicle : this->locationsInSolution)
	{
		temp << std::setiosflags(std::ios::fixed) << std::left << std::setw(3) << std::setfill('0') << "V[" << vehicle.first->getId() << "]:\t";
		for (auto& Location : vehicle.second)
		{
			if (Location->client == nullptr)
				temp << std::setiosflags(std::ios::fixed) << std::right << std::setw(3) << std::setfill(s) << " 0 ";
			else
				temp << std::setiosflags(std::ios::fixed) << std::right << std::setw(3) << std::setfill(s) << Location->client->getId() << " ";
		}
		//temp  << "\t\b" << "Path cost: " << std::right << solution->getPathCost(*vehicle.first);
		temp << std::setiosflags(std::ios::fixed) << std::right << std::setw(80) << std::setfill(' ') << std::endl;
	}

	return temp;
}

std::stringstream Solution::showRaw()
{
	std::stringstream temp;
	for (auto& vehicle : this->locationsInSolution)
	{
		for (auto& Location : vehicle.second)
		{
			if (Location->client == nullptr)
			{
			}
			else {
				temp <<  Location->client->getId() << "\t";
			}
		}
		//temp  << "\t\b" << "Path cost: " << std::right << solution->getPathCost(*vehicle.first);
	}

	return temp;
}
