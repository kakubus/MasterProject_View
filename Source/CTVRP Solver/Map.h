/*****************************************************************//**
 * \file   Map.h
 * \brief  Class describes graphs and connecting with Locations,
 * Routes and graph from boost::adjacency_list 
 * 
 * \author Jakub Kaniowski
 * \date   June 2023
 *********************************************************************/
#ifndef MAP_H
#define MAP_H
#include <utility>
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

#include "Structures.h"


using namespace boost;

using Graph = adjacency_list<vecS, vecS, bidirectionalS, Location, Route*>;	///< Use adjacency_list as directed graph with Location and Routes.

typedef graph_traits<Graph>::vertex_descriptor Vertex;
typedef graph_traits<Graph>::edge_descriptor Edge;
typedef graph_traits<Graph>::vertex_iterator vi;
typedef graph_traits<Graph>::edge_iterator ei;

class Map
{
public:

	Map();
	Map(Map& copyMap);
	~Map();

	int visitedLocations;			///< Number of visited locations
	Graph graph;					///< Contains main Map Graph

	void addEdgeWithRouteToMap(Vertex v1, Vertex v2, Route& route);
	void addToMap(Location& L1, Location& L2, Route& route);
	void addLocation(Location* location);

	std::stringstream showMap();					///< Returns information about map
	std::stringstream showMap(bool printToScreen);	///< Returns and print information about map on console screen.

	Vertex findVertex(Location& location);
	Vertex getSourceVertex(const Edge& edge);		///< Returns source vertex which from edge (A <-> B, where A is source vertex)
	Vertex getTargetVertex(const Edge& edge);		///< Returns target vertex which from edge (A <-> B, where B is target vertex)
	Edge&  getEdge(Vertex v1, Vertex v2);			///< Returns edge if exists between Vertex1 and Vertex2.

	const Location* getSourceLocation(const Edge& edge);
	const Location* getTargetLocation(const Edge& edge);
	Route*	getRouteFromEdge(const Edge& edge);

	int getNumberOfLocations();
	int getNumberOfRoutes();
	double updateTotalDemand();
	double getTotalDemand();

	bool findEdgeByVertices(const Graph& g, Graph::vertex_descriptor v1, Graph::vertex_descriptor v2, Graph::edge_descriptor& FOUNDge);

private:
	int	_numPoints;						///< Number of locations (nodes)
	double totalDemand;					///< Total node demand
};

#endif

