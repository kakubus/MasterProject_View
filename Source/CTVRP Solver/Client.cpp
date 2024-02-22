#include "Client.h"

Client::Client(double capacityDemand)
{
	_id = _cid();
	this->_companyName = "GenericCompany_" + std::to_string(_id);
	this->capacityDemand = (capacityDemand > 0) ? capacityDemand : 1000;
	this->risk = 0;
	this->_visited = false;
}

Client::Client(double capacityDemand, double risk)
{
	_id = _cid();
	this->_companyName = "GenericCompany_" + std::to_string(_id);
	this->capacityDemand = (capacityDemand > 0) ? capacityDemand : 1000;
	this->risk = (risk > 0) ? risk : 100;
	_visited = false;
}

Client::Client(std::string companyName, double capacityDemand, double risk)
{
	_id = _cid();
	if (companyName.empty() == true)
		this->_companyName = "GenericCompany" + std::to_string(_id);
	else
		this->_companyName = companyName;

	this->capacityDemand = (capacityDemand > 0) ? capacityDemand : 1000;
	this->risk = (risk > 0) ? risk : 100;
	this->_visited = false;
}

Client::Client(const Client& toCopy)
{
	_id = _cid();
	this->_companyName = "Copy_GenericCompany_" + std::to_string(toCopy._id) + "#" + std::to_string(_id);
	this->capacityDemand = toCopy.capacityDemand;
	this->risk = toCopy.risk;
	this->_visited = false;
}

Client::~Client()
{
	//std::cout << "Client [ID: " << std::to_string(_id) << " " << _companyName << "] has been deleted" << std::endl;
}


std::stringstream Client::show()
{
	std::stringstream outputStream;
	outputStream << std::fixed << std::setprecision(2) << "\n================================================"
		<< "\nCompany Name: " << _companyName
		<< "\nCompany ID: " << _id
		<< "\n--------------------------------------"
		<< "\nCapacity Demand : " << (capacityDemand)
		<< "\nRisk : " << (risk) << "%"
		<< "\n";

	std::cout << outputStream.str() << std::endl;
	return outputStream;
}

std::string Client::showBase()
{
	std::stringstream outputStream;
	outputStream << std::fixed << std::setprecision(2) << _companyName << "\t| Demand: " << capacityDemand << "\t| Risk: " << risk;
	return outputStream.str();
}

int Client::getId()
{
	if (this == nullptr) {
		return NULL;
	}
	else {
		return _id;
	}

}

bool Client::makeVisited()
{
	_visited = true;
	return _visited;
}

bool Client::makeUnvisited() {
	_visited = false;
	return _visited;
}

bool Client::isVisited()
{
	return _visited;
}
