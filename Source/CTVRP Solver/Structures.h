/*****************************************************************//**
 * \file   Structures.h
 * \brief  Header file storages definition of Location and Route struct.
 * 
 * \author Jakub Kaniowski
 * \date   September 2023
 *********************************************************************/
#pragma once
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "Client.h"
#include "Vehicle.h"


///< V - verticles. Definition of graph nodes.
struct Location {

	int			x;				///< X coords
	int			y;				///< Y coords
	bool		visited;		///< If location was visited - set true.
	Client*		client;			///< If node(V) is NOT a client (is depot) = nullptr

	double getLocationDemand() {
		if (client != nullptr)
		{
			return client->capacityDemand;
		}
		else {
			return NULL;
		}
	}

	void makeVisited() {
		if (client != nullptr) {
			visited = true;
			client->makeVisited();
		}
	}

	void makeUnvisited()
	{
		if (client != nullptr) {
			visited = false;
			client->makeUnvisited();
		}
	}

	bool isVisited() {
		if (client != nullptr) {
			return client->isVisited();
		}
		else {
			return true; // Depot is also visited, because program wanted always return to 0.. 
		}
	}

	std::stringstream show() {
		std::stringstream s;
		if (client == nullptr) {
			s << "V -> [0" << "]\t(x: " << x << "\ty: " << y << ")";
		}
		else {
			s << "V -> [" << client->getId() << "]\t(x: " << x << "\ty: " << y << ")";
		}
		return s;
	}

	bool operator==(const Location& compared) const {
		return (x == compared.x) && (y == compared.y) && (client == compared.client);
	}
};

///< E - edges. Definition of graph edges.
struct Route {

	double		distance;		///< Distance between A and B nodes.
	double		risk;			///< Risk of theft between A and B route.
	double		fuelCost;		///< Fuel cost between A nad B nodes.
	double		cost;			///< Calculated total transportation cost between A and B nodes.

	Location	*L1;				///< Location of 'A' node
	Location	*L2;				///< Location of 'B' node

	Route() {							///< Generic route
		L1 = new Location();
		L2 = new Location();

		distance = 0;
		risk = 0;
		fuelCost = 0;
		cost = (distance + this->risk) / 2;
	}

	Route(Location& A, Location& B) {		///< Auto calculate distance between nodes.
		L1 = &A;
		L2 = &B;
		double dx = B.x - A.x;
		double dy = B.y - A.y;

		distance = std::sqrt(dx * dx + dy * dy);
		fuelCost =0;					///< It should be updated in addToMap() method in Map.h

		if (L1->client == nullptr && L2->client == nullptr) {
			risk = 0;
		}
		else if (L1->client == nullptr) {
			risk = (0 + L2->client->risk) / 2;
		}
		else if (L2->client == nullptr) {
			risk = (L1->client->risk + 0) / 2;
		}
		else {
			risk = (L1->client->risk + L2->client->risk) / 2;
		}
		 cost = (distance + risk);
	}

	std::stringstream show() {
		std::stringstream s;
		if (L1->client == nullptr) {
			s << "[0] <-> " << "[" << L2->client->getId() << "]";
		}
		else if (L2->client == nullptr) {
			s << "[" << L1->client->getId() << "] <-> [0]";
		}
		else {
			s << "[" << L1->client->getId() << "] <-> [" << L2->client->getId() << "]";
		}

		return s;
	}

	
};

enum GenerationType{ EXCHANGE, SWAP, MOVE, UNUSED, UNKNOWN };

/// @brief MethodResult is structure used for log history of change solution.
struct MethodResult {
	bool result;				///< Returned success of usage. (True - success, False - failed)
	Location* L1;				///< Source vertex used to exchange, swap, move
	Location* L2;				///< Target vertex used to exchange, swap, move
	Vehicle* vehicle1;			///< Source vehicle used in procedure.
	Vehicle* vehicle2;			///< Target vehicle used in procedure.
	GenerationType usedMethod;	///< Indicator - which method was used for generation new solution.

	void log(bool result, Location* L1, Location* L2, Vehicle* vehicle1, Vehicle* vehicle2, GenerationType usedMethod) {
		this->result = result;
		this->L1 = L1;
		this->L2 = L2;
		this->vehicle1 = vehicle1;
		this->vehicle2 = vehicle2;
		this->usedMethod = usedMethod;
	}

	bool operator==(const MethodResult& compared) const {
		// Disabled comparing result (if true, or not).. (result == compared.result) &&  ?? - choæ z logicznego punktu widzenia, jeœli operacja siê niepowiod³a, to czemu ma byc zakazana? Moze teraz (tzn. w innym czasie) ona sie powiedzie i nawet znajdzie lepsze rozwiazanie?
		return (result == compared.result) &&
			(L1 == compared.L1) && 
			(L2 == compared.L2) && 
			(vehicle1 == compared.vehicle1) && 
			(vehicle2 == compared.vehicle2) && 
			(usedMethod == compared.usedMethod);
	}
};

#endif
