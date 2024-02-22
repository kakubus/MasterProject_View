#include "Map.h"
#include "boost/graph/lookup_edge.hpp"

Map::Map()
{
	_numPoints = 0;
	totalDemand = 0;
	visitedLocations = 0;
}

Map::Map(Map& copyMap)
{
	_numPoints = copyMap._numPoints;
	visitedLocations = copyMap.visitedLocations;
	graph = copyMap.graph;
	updateTotalDemand();
}

Map::~Map()
{
}

std::stringstream Map::showMap()
{
	int v = num_vertices(graph);
	int e = num_edges(graph);
	std::stringstream outputStream;


	outputStream << std::fixed << std::setprecision(2)
		<< "============================================\n"
		<< "MAP Info\n"
		<< "============================================\n"
		<< "Locations (Verticles):\t" << v << "\n"
		<< "Routes (Edges):\t" << e << "\n"
		<< "--------------------------------------------\n"
		<< std::endl
		<< "Location\tCoords (x,y)\tCompany Info\n"
		<< "-----------------------------------------------------------------\n";

	for (const auto& vertex : make_iterator_range(vertices(graph))) {
		const Location& location = graph[vertex];
		std::string companyInfo;
		if (location.client == nullptr) {
			companyInfo = "======= COMPANY DEPOT =======";
		}
		else {
			companyInfo = location.client->showBase();
		}
		outputStream << "[" << vertex << "]\t\t(" << location.x << "," << location.y << ")\t\t" << companyInfo << "\n";
	}
	outputStream << std::endl
		<< "Route\t\tDistance\tRisk\tTotal cost\n"
		"-----------------------------------------------------------------\n";

	for (const auto& edge : make_iterator_range(edges(graph))) {
		Vertex vertexL1 = source(edge, graph);
		Vertex vertexL2 = target(edge, graph);
		const Route& route = *graph[edge];
		outputStream << std::fixed << std::setprecision(2) << "[" << vertexL1 << "] -> [" << vertexL2 << "]" << "\t" << (route.distance) << "\t\t" << (route.risk) << "\t" << (route.cost) << "\n";
	}

	// std::cout << outputStream.str() << std::endl;

	return outputStream;
}

std::stringstream Map::showMap(bool printToScreen)
{
	std::stringstream stream = showMap();
	if (printToScreen == true) {
		std::cout << stream.str() << std::endl;
	}
	return stream;
}



Vertex Map::findVertex(Location& location)
{
	vi it_begin, it_end;
	for (boost::tie(it_begin, it_end) = vertices(graph); it_begin != it_end; ++it_begin) {
		if (graph[*it_begin] == location)
		{
			return *it_begin;
		}
	}
	return Vertex();
}

Vertex Map::getSourceVertex(const Edge& edge)
{
	return boost::source(edge, graph);
}

Vertex Map::getTargetVertex(const Edge& edge)
{
	return boost::target(edge, graph);
}

bool Map::findEdgeByVertices(const Graph& g, Graph::vertex_descriptor v1, Graph::vertex_descriptor v2, Graph::edge_descriptor& FOUNDge) {
	boost::graph_traits<Graph>::edge_iterator ei, ei_end;
	for (boost::tie(ei, ei_end) = boost::edges(g); ei != ei_end; ++ei) {
		Graph::vertex_descriptor sourceVertex = boost::source(*ei, g);
		Graph::vertex_descriptor targetVertex = boost::target(*ei, g);

		if ((sourceVertex == v1 && targetVertex == v2) || (sourceVertex == v2 && targetVertex == v1)) {
			FOUNDge = *ei;
			return true; // Found edge
		}
	}

	return false; // Edge is not found
}



Edge& Map::getEdge(Vertex v1, Vertex v2)
{
	bool exists;
	Edge edge;
	
	std::tie(edge, exists) = boost::edge(v1, v2, this->graph);	// boost::edge return TRUE if specific edge exist.
	if (exists) {
		return edge;
	}
	else {
		throw std::runtime_error("Edge doesn't exist!");
	}
}

const Location* Map::getSourceLocation(const Edge& edge)
{
	return graph[edge]->L1;
}

const Location* Map::getTargetLocation(const Edge& edge)
{
	return graph[edge]->L2;
}

Route* Map::getRouteFromEdge(const Edge& edge)
{
	return graph[edge];
}

void Map::addEdgeWithRouteToMap(Vertex v1, Vertex v2, Route& route)
{
	//routesList.push_back(route);
	add_edge(v1, v2, &route, this->graph).first;
}

void Map::addToMap(Location& L1, Location& L2, Route& route) {
	Vertex vertexL1 = add_vertex(L1, graph);
	Vertex vertexL2 = add_vertex(L2, graph);
	add_edge(vertexL1, vertexL2, &route, graph);
}

void Map::addLocation(Location* location)
{
	add_vertex(*location, this->graph);
	_numPoints++;
}

double Map::updateTotalDemand()
{
	double demand = 0;
	for (auto v : make_iterator_range(vertices(graph))) {
		if (graph[v].client != nullptr) {
			demand += graph[v].client->capacityDemand;
		}
	}
	totalDemand = demand;
	return demand;
}

double Map::getTotalDemand()
{
	return totalDemand;
}

int Map::getNumberOfLocations() {
	_numPoints = num_vertices(graph);
	return _numPoints;
}

int Map::getNumberOfRoutes() {
	return num_edges(graph);
}
