/*****************************************************************//**
 * \file   Client.h
 * \brief  Class with definitions of verticles - Clients. Contains cargo demand for VRP problem.
 *
 *
 * \author Jakub Kaniowski
 * \date   June 2023
 *********************************************************************/

#pragma once
#ifndef CLIENT_H
#define CLIENT_H
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <iomanip>



class Client
{
private:
	static int _cid()			/// Client identifier (counter)
	{
		/**
		 * Client ID is only for debug in this moment.
		 */
		static int _cid = 1; // In benchmark, 1 is depot...
		return _cid++;
	}

	int _id;
	std::string _companyName;
	bool _visited;

public:
	Client(double capacityDemand);
	Client(double capacityDemand, double risk);
	Client(std::string companyName, double capacityDemand, double risk);
	Client(const Client& toCopy);
	~Client();

	std::stringstream	show();
	std::string			showBase();

	int getId();

	double capacityDemand;
	double risk;

	bool makeVisited();
	bool makeUnvisited();
	bool isVisited();


};

#endif