/*****************************************************************//**
 * \file   FileIO.h
 * \brief  Class with import/export data operations on files.
 *
 * \author Jakub Kaniowski
 * \date   June 2023
 *********************************************************************/
#ifndef FILEIO_H
#define FILEIO_H
#include <iostream>
#include <fstream>
#include <filesystem>
#include <ctime>
#include <boost/tokenizer.hpp>

#include "Problem.h"
#include "Map.h"


enum Filetype { csv, txt };

using namespace boost;

class FileIO
{
public:
	friend class Problem;
	FileIO();					///< Create file to save
	FileIO(Filetype type);		///< Create file to save (with default name)
	FileIO(std::string prefix, std::string outCatalog, Filetype type);		///< Create file to save (with specified prefix and output catalog)
	FileIO(std::string filepath); ///< Open file to read
	~FileIO(); ///< Close file
	
	//bool importProblem(Problem& problem);
	bool importToGraph(Problem& problem);
	bool importToVehicles(std::vector<Vehicle*>& vehicles);
	bool exportToTextFile(std::stringstream text);
	bool exportGraphToFile(Graph g); 

private:
	std::time_t _time;
	std::string _filename;
	std::string _directory;
	std::ifstream* _inputFile;
	std::ofstream* _outputFile;
	
	Filetype _format;

	static std::string _fileDirectory(std::string prefix)				///< Vehicle identifier (counter)
	{
		std::time_t currentTime = time(0);
		static std::string _fileDirectory;
		struct tm ltm;
		localtime_s(&ltm, &currentTime);
		std::string folderName = std::to_string(1900 + ltm.tm_year) + "_" + std::to_string(1 + ltm.tm_mon) + "_" + std::to_string(ltm.tm_mday);
		if (!prefix.empty())
		{
			_fileDirectory = prefix + "_" + folderName;
		}
		else {
			_fileDirectory = folderName;
		}

		return _fileDirectory;
	}

	bool checkFileHeader(std::string& header, std::string referenceHeader);
};

#endif

