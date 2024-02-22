/*****************************************************************//**
 * \file   Vehicle.h
 * \brief  Class with definition vehicles. Contains capacities, fuel consumption etc.
 *
 * In case of master thesis, all vehicles are this same type!
 *
 * \author Jakub Kaniowski
 * \date   June 2023
 *********************************************************************/
#ifndef VEHICLE_H
#define VEHICLE_H
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
 /* Vehicle types:
 * LIGHT_CARGO < 900 kg of load
 * MEDIUM_CARGO < 3500 kg of load
 * HEAVY_CARGO < 40000 kg of load
 * 0- VW Caddy, Transporter, 1-Iveco Daily, 2-MB Across, Volvo FH16
 */
enum VehicleType {					///< Vehicle type (unused in this moment)
	LIGHT_CARGO,
	MEDIUM_CARGO,
	HEAVY_CARGO
};

class Vehicle
{
public:
	Vehicle(std::string mark, std::string type, double maxCapacity, double tankCapacity, double vehicleMass, double fuelConsumption);
	Vehicle(const Vehicle& toCopy);
	~Vehicle();

	bool	takeLoad(double amount);		///< Try take a load into vehicle (if it's impossible, return false)	
	void	unload();						///< Make free vehicle capacity (capacity = 0)
	double	unload(double amount);			///< After unload return actual capacity

	int		getId();
	double	calculateCostPerKilometer();
	double  getCapacity();					///< Return actual capacity of vehicle
	double	setCapacity(double demand);		///< Set defined value of actual vehicle capacity
	double	getMaximumCapacity();			///< Return property of vehicle maximum capacity
	std::string show();						///< Show info about vehicle

	static bool	reloadDemandBetween(		///< Check if is possible to change nodes between vehicles (vehicles cannot be overloaded)
		double& capacitySource,
		double& capacityTarget, 
		Vehicle& source, 
		Vehicle& target
	);

	bool operator==(Vehicle& compared);

private:
	static int _vid()				///< Vehicle identifier (counter)
	{
		static int _vid = 0;
		_vid++;
		return _vid;
	}

	int		_id;				///< Vehicle identifier		
	double	_capacity;			///< Actual cargo capacity of vehicle
	double	_maxCapacity;		///< Maxiumum cargo capacity of vehicle
	double	_fuel;				///< Actual fuel [Default: Full]
	double	_fuelConsumption;	///< Calculated fuel consumption [L/100km]
	double	_tankCapacity;		///< Tank capacity [Litres]
	double	_mass;				///< Actual mass of vehicle - Vehicle + mass of load (Not GVW)
	double	_vehicleMass;		///< GVW - gross vehicle weight
	double	_fuelCost;			///< Pobierane z definicji problemu (TO DO) - na razie na sztywno 6.35PLN/l
	double	_costPerKilometer;

	VehicleType _vehicleType;
	std::string _mark;
	std::string _type;

};

#endif