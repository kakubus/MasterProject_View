#include "Vehicle.h"

Vehicle::Vehicle(std::string mark, std::string type, double maxCapacity, double tankCapacity, double vehicleMass, double fuelConsumption)
{
	_id = _vid();
	_mark = (mark.empty() == true) ? "UnknownMark" : mark;
	_type = (type.empty() == true) ? "UnknownType" : type;

	if (maxCapacity > 0 && maxCapacity <= 900)		// <900 kg = LIGHT_CARGO
	{
		this->_vehicleType = LIGHT_CARGO;
		this->_maxCapacity = maxCapacity;
		this->_fuel = this->_tankCapacity = (tankCapacity > 0) ? tankCapacity : 50;
		this->_fuelConsumption = (fuelConsumption > 0) ? fuelConsumption : 6;
		this->_vehicleMass = (vehicleMass > 0) ? vehicleMass : 1200;
	}
	if (maxCapacity > 900 && maxCapacity <= 3500)	// 901-3500 kg = MEDIUM_CARGO
	{
		this->_vehicleType = MEDIUM_CARGO;
		this->_maxCapacity = maxCapacity;
		this->_fuel = this->_tankCapacity = (tankCapacity > 0) ? tankCapacity : 100;
		this->_fuelConsumption = (fuelConsumption > 0) ? fuelConsumption : 10;
		this->_vehicleMass = (vehicleMass > 0) ? vehicleMass : 3000;
	}
	if (maxCapacity > 3500 && maxCapacity <= 40000) // 3501 - 40000 kg = HEAVY_CARGO
	{
		this->_vehicleType = HEAVY_CARGO;
		this->_maxCapacity = maxCapacity;
		this->_fuel = this->_tankCapacity = (tankCapacity > 0) ? tankCapacity : 1500;
		this->_fuelConsumption = (fuelConsumption > 0) ? fuelConsumption : 25;
		this->_vehicleMass = (vehicleMass > 0) ? vehicleMass : 9200;
	}
	if (maxCapacity <= 0)
	{
		this->_vehicleType = LIGHT_CARGO;
		this->_maxCapacity = 900;
	}
	this->_capacity = 0;
	this->_mass = _vehicleMass - _maxCapacity + _capacity;
	this->_fuelCost = 6.35;
	calculateCostPerKilometer();
}


Vehicle::Vehicle(const Vehicle& toCopy)	///< Attention! Copy constructor is setting capacity to zero!
{
	//	std::cout << "USING COPY CONSTRUCTOR!" << std::endl;		// FOR DEBUG
	this->_id = _vid();
	this->_mark = toCopy._mark;
	this->_type = toCopy._type;
	this->_vehicleType = toCopy._vehicleType;
	this->_maxCapacity = toCopy._maxCapacity;
	this->_vehicleMass = toCopy._vehicleMass;
	this->_mass = toCopy._mass;
	this->_fuel = toCopy._fuel;
	this->_fuelConsumption = toCopy._fuelConsumption;
	this->_fuelCost = toCopy._fuelCost;

	this->_capacity = 0;

	calculateCostPerKilometer();
}

Vehicle::~Vehicle()
{

//	std::cout << "Vehicle [ID: " << std::to_string(_id) << " " << _mark << " " << _type << "] has been deleted" << std::endl;
}


bool Vehicle::takeLoad(double amount)
{
	if (amount > _maxCapacity - this->_capacity) // Check if it's possible to take load (calculate free load on vehicle);
	{
		//	std::cout << "Vehicle [ID: " << std::to_string(_id) << "] is overloaded! (Free capacity: " << _maxCapacity - _capacity << " Requested load: " << amount << ")" << std::endl;			// FOR DEBUG
		return false;
	}
	this->_capacity += (amount > 0) ? amount : 0;
	return true;
}


double Vehicle::unload(double amount)
{
	this->_capacity -= (amount <= _capacity) ? amount : _capacity;
	//std::cout << "Vehicle [ID: " << std::to_string(_id) << " " << _mark << " " << _type << "] is unloaded! [Capacity to unload: " << amount << ". Remaining capacity: " << _capacity << "]" << std::endl;
	if (_capacity <= 0)
	{
	//	delete this;
	}
	return _capacity;
}

void Vehicle::unload()
{
	//	std::cout << "Vehicle [ID: " << std::to_string(_id) << " " << _mark << " " << _type << "] is unloaded! [Capacity from: " << _capacity << " to: 0]" << std::endl;
	this->_capacity = 0;
	this->_mass = _vehicleMass - _maxCapacity;
}

bool Vehicle::reloadDemandBetween(double& capacitySource, double& capacityTarget, Vehicle& source, Vehicle& target) {

	bool operationSuccess = true;

	Vehicle vCopySource = source;						///< Make a temporary copy of source vehicle
	Vehicle vCopyTarget = target;						///<  Make a temporary copy of target vehicle
	vCopySource.setCapacity(source.getCapacity());		///<  Copy original capacity to copy
	vCopyTarget.setCapacity(target.getCapacity());		///<  Copy original capacity to copy

	double remainingCapSource = (vCopySource.getMaximumCapacity() - vCopySource.getCapacity());
	double remainingCapTarget = (vCopyTarget.getMaximumCapacity() - vCopyTarget.getCapacity());

	if (capacitySource > vCopySource.getCapacity())     ///<  Check if unload makes vehicle capacity < 0 - It is forbidden!
		return false;

	if (capacityTarget > vCopyTarget.getCapacity())     ///<  Check if unload makes vehicle capacity < 0 - It is forbidden!
		return false;

	vCopySource.unload(capacitySource);					///<  UNLOAD SOURCE VEHICLE (with selected location)
	vCopyTarget.unload(capacityTarget);					///<  UNLOAD TARGET VEHICLE (with selected location for target)

	if (!vCopySource.takeLoad(capacityTarget)) {		///<  TRY LOAD TARGET VEHICLE WITH LOCATION FROM SOURCE VEHICLE
		operationSuccess = false;
	}

	if (!vCopyTarget.takeLoad(capacitySource)) {		///<  TRY LOAD SOURCE VEHICLE WITH LOCATION FROM TARGET VEHICLE
		operationSuccess = false;
	}

	if (operationSuccess) {								///<  IF OPERATION FINISHED WITHOUT ERRORS - add changes to original vehicles 
		source.setCapacity(vCopySource.getCapacity());
		target.setCapacity(vCopyTarget.getCapacity());
	}
	return operationSuccess;
}

bool Vehicle::operator==(Vehicle& compared)
{
	return (_id == compared._id);
}


std::string Vehicle::show()
{
	std::string typeString = "";
	switch (_vehicleType) {
	case LIGHT_CARGO:
		typeString = "LIGHT_CARGO";
		break;
	case MEDIUM_CARGO:
		typeString = "MEDIUM_CARGO";
		break;
	case HEAVY_CARGO:
		typeString = "HEAVY_CARGO";
		break;
	}
	std::stringstream outputStream;
	outputStream << std::fixed << std::setprecision(2)
		<< "\n================================================"
		<< "\nMark: " << _mark
		<< "\nType: " << _type
		<< "\n--------------------------------------"
		<< "\nVehicle ID : " << (_id)
		<< "\nVehicle type: " << typeString

		<< "\n\n--[Fuel and costs]--------------------"

		<< "\n\tActual fuel: " << (_fuel) << "l "
		<< "\n\tFuel tank Capacity: " << (_tankCapacity) << "l "
		<< "\n\tFuel consumption: " << (_fuelConsumption) << " l/100km "
		<< "\n\tCost per kilometer: " << (_costPerKilometer) << " PLN "

		<< "\n--[Capacity and weight]---------------"

		<< "\n\tActual capacity: " << (_capacity) << " kg "
		<< "\n\tMax capacity: " << (_maxCapacity) << " kg "
		<< "\n\tActual vehicle weight: " << (_mass) << " kg "
		<< "\n\tGross vehicle weight: " << (_vehicleMass) << " kg \n";

	std::cout << outputStream.str() << std::endl;
	return outputStream.str();
}

int Vehicle::getId()
{
	return _id;
}

double Vehicle::calculateCostPerKilometer()
{
	double cost = (_tankCapacity * _fuelCost) / (100 * _tankCapacity / _fuelConsumption);
	this->_costPerKilometer = cost;
	return cost;
}

double Vehicle::getCapacity()
{
	return _capacity;
}

double Vehicle::setCapacity(double demand)
{
	_capacity = (demand > _maxCapacity) ? 100.0 : demand;
	return _capacity;
}

double Vehicle::getMaximumCapacity()
{
	return _maxCapacity;
}
