#include "Problem.h"

Problem::Problem()
{
	files = new FileIO();
	filesVehicle = new FileIO();
	solution = new Solution(this);
	bestSolution = new Solution(this);
}

Problem::Problem(Problem& copyProblem)
{
	files = copyProblem.files;  
	filesVehicle = copyProblem.filesVehicle;
	vehicleList = copyProblem.vehicleList;
	map = copyProblem.map;
	solution = new Solution(*copyProblem.solution);
	bestSolution = copyProblem.bestSolution;
}

Problem::Problem(std::string filepath, std::string filepathVehicle)
{
	files = new FileIO(filepath);
	filesVehicle = new FileIO(filepathVehicle);
	files->importToGraph(*this);
	filesVehicle->importToVehicles(vehicleList);
	solution = new Solution(this);
	bestSolution = new Solution(this);
}

Problem::~Problem()
{
	delete files;
	delete filesVehicle;
	delete solution;
	delete bestSolution;
}

ClientList* Problem::getClientList()
{
	return &clientList;
}

VehicleList* Problem::getVehicleList()
{
	return &vehicleList;
}

Map* Problem::getMap()
{
	return &map;
}

std::stringstream Problem::show()
{
	const char s = ' ';
	const char s2 = '*';
	std::stringstream temp;
	for (auto& vehicle : this->solution->locationsInSolution)
	{
		temp << std::setiosflags(std::ios::fixed) << std::left << std::setw(3) << std::setfill('0') << "V[" << vehicle.first->getId() << "]:\t";
		for (auto& Location : vehicle.second)
		{
			if (Location->client == nullptr)
				temp << std::setiosflags(std::ios::fixed) << std::right << std::setw(4) << std::setfill(s) << "0 ";
			else
				temp << std::setiosflags(std::ios::fixed) << std::right << std::setw(4) << std::setfill(s) << Location->client->getId() << " ";

		}
		//	temp  << "\t\b" << "Path cost: " << std::right << solution->getPathCost(*vehicle.first);
		temp << std::setw(80) << std::setfill(s) << std::endl;
	}

	return temp;
}

