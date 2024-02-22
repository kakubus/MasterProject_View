#include "FileIO.h"
#include "UtilsFcn.h"

#define NODE 0
#define X 1
#define Y 2
#define DEMAND 3
#define RISK 4

FileIO::FileIO(Filetype type)
{
    _format = type;
    _time = time(0);
    struct tm ltm;
    localtime_s(&ltm, &_time);
    std::string fileFormat;
    switch (_format) {
    case csv:
        fileFormat = ".csv";
        break;

    case txt:
        fileFormat = ".txt";
        break;

    default:
        fileFormat = ".csv";
        break;
    }
    _directory = _fileDirectory("out");
    try {
        std::filesystem::create_directory(std::filesystem::path("Results") / _directory);
        // std::cout << "Created directory: " << _directory << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error FileIO: " << ex.what() << std::endl;
        system("PAUSE");
    }
    _filename = "results_" + std::to_string(1900 + ltm.tm_year) + "_" + std::to_string(1 + ltm.tm_mon) + "_" + std::to_string(ltm.tm_mday) + "_" + std::to_string(ltm.tm_hour) + "_" + std::to_string(ltm.tm_min) + "_" + std::to_string(ltm.tm_sec) + "" + fileFormat;
    std::cout << "Creating output file: " << std::filesystem::path(_directory) / _filename << std::endl;
    _outputFile = new std::ofstream(std::filesystem::path("Results") / std::filesystem::path(_directory) / _filename, std::ios::out);
    _inputFile = nullptr;
    
}

FileIO::FileIO(std::string prefix, std::string outCatalog, Filetype type)
{
    _format = type;
    _time = time(0);
    struct tm ltm;
    localtime_s(&ltm, &_time);
    std::string fileFormat;
    switch (_format) {
    case csv:
        fileFormat = ".csv";
        break;

    case txt:
        fileFormat = ".txt";
        break;

    default:
        fileFormat = ".csv";
        break;
    }
    _directory = _fileDirectory("out");
    try {
        std::filesystem::create_directory(std::filesystem::path("Results") / _directory);
        // std::cout << "Utworzono katalog: " << _directory << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error FileIO: " << ex.what() << std::endl;
        system("PAUSE");
    }
    _directory = _fileDirectory("out");
    std::string subDirectory = _fileDirectory(outCatalog);
    try {
        std::filesystem::create_directory(std::filesystem::path("Results") / _directory / subDirectory);
        // std::cout << "Utworzono katalog: " << _directory << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error FileIO: " << ex.what() << std::endl;
        system("PAUSE");
    }


    _filename = prefix + std::to_string(1900 + ltm.tm_year) + "_" + std::to_string(1 + ltm.tm_mon) + "_" + std::to_string(ltm.tm_mday) + "_" + std::to_string(ltm.tm_hour) + "_" + std::to_string(ltm.tm_min) + "_" + std::to_string(ltm.tm_sec) + "" + fileFormat;
    std::cout << "Creating output file: " << std::filesystem::path(_directory)/ std::filesystem::path(subDirectory) / _filename << std::endl;
    _outputFile = new std::ofstream(std::filesystem::path("Results") / std::filesystem::path(_directory) / std::filesystem::path(subDirectory) / _filename, std::ios::out);
    _inputFile = nullptr;
}

FileIO::FileIO()
{
    _format = csv;
    _time = time(0);
    struct tm ltm;
    localtime_s(&ltm, &_time);
    std::string fileFormat;
    switch (_format) {
    case csv:
        fileFormat = ".csv";
        break;

    case txt:
        fileFormat = ".txt";
        break;

    default:
        fileFormat = ".csv";
        break;
    }

    _directory = _fileDirectory("out");
    try {
        std::filesystem::create_directory(std::filesystem::path("Results") / _directory);
        // std::cout << "Created directory: " << _directory << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error FileIO: " << ex.what() << std::endl;
        system("PAUSE");
    }
    _filename = "results_" + std::to_string(1900 + ltm.tm_year) + "_" + std::to_string(1 + ltm.tm_mon) + "_" + std::to_string(ltm.tm_mday) + "_" + std::to_string(ltm.tm_hour) + "_" + std::to_string(ltm.tm_min) + "_" + std::to_string(ltm.tm_sec) + "" + fileFormat;
    std::cout << "Creating output file: " << std::filesystem::path(_directory) / _filename << std::endl;
    _outputFile = new std::ofstream(std::filesystem::path("Results") / std::filesystem::path(_directory) / _filename, std::ios::out);
    _inputFile = nullptr;
}


FileIO::FileIO(std::string filepath)
{
    _time = time(0);
    _format = csv;
    _inputFile = new std::ifstream(filepath, std::ios::in);
    if (!_inputFile->is_open()) {
        std::cout << "(!) ERROR OPENING FILE: " + filepath << std::endl;
        _inputFile->close();
        _inputFile = nullptr;
    }
    else {
        std::cout << "Input file (" + filepath + ") is open" << std::endl;
    }
    _outputFile = nullptr;

}

FileIO::~FileIO()
{
    if (_inputFile != nullptr) {
        _inputFile->close();
        std::cout << "Input file closed." << std::endl;
    }
    if (_outputFile != nullptr) {
        _outputFile->close();
        std::cout << "Output File: " + _filename + " closed." << std::endl;
    }

    delete _outputFile;
    delete _inputFile;
}

bool FileIO::checkFileHeader(std::string& header, std::string referenceHeader)
{
    if (std::strcmp(header.c_str(), referenceHeader.c_str()) == 0) {
        std::cout << "\tFile recognized correctly!";
        return true;
    }
    else {
        std::cout << "\t(!) File is corrupted!\n";
        return false;
    }
}

bool FileIO::importToGraph(Problem& problem)
{
    Map& map = *problem.getMap();
    std::string line;           ///< Row line readed from file
    std::map<int, double> row;  ///< Temporary row for parsed file

    if (_inputFile->is_open()) {
        int it = 0;                 ///< Iterator for elements in row
        int lineNumber = 0;          ///< Iterator for number of rows (0 - is table header)
        system("cls");
        
        while (!_inputFile->eof()) {
            row.clear();
            *_inputFile >> line;

            char_separator<char> sep(";");
            tokenizer<char_separator<char> > tok(line, sep);
            it = 0;
            for (tokenizer<char_separator<char> >::iterator beg = tok.begin(); beg != tok.end(); ++beg) {

                std::cout << *beg << "\t|";
                if (*beg == "") {
                    continue;
                }
                if (lineNumber == 0) {
                    row[it] = NULL; ///< Do nothing
                }
                else {
                    row[it] = std::stod(*beg);
                }
                it++;

                if (lineNumber == 0) { ///< Checking table header
                    static std::string compare;
                    compare += *beg + ";";
                    if (it == 5) { ///< Iterator should be equal 4, because we have 5 parameters (0->4)

                        if (!checkFileHeader(compare, "Node;X;Y;Demand;Risk;"))
                        {
                            return false;
                        }

                        std::cout << "\n-----------------------------------------";
                    }
                }
            }

            if (row[X] == NULL && row[Y] == NULL && row[DEMAND] == NULL && row[RISK] == NULL)
            {
                lineNumber++;
                std::cout << std::endl;
                continue;
            }
            else {
                Location tempL;
                Client* tempC;
                if (lineNumber == 0) {
                    std::cout << "\n-----------------------------------------";
                }
                else if (lineNumber == 1) { ///< First element is depot!
                    tempL = { (int)row[X], (int)row[Y], false, nullptr };
                    map.addLocation(&tempL);
                }
                else { ///< Each other element is a client destination
                   
                    problem.getClientList()->push_back(new Client(row[DEMAND], row[RISK]));
                    tempC = problem.getClientList()->back();
                    tempL = { (int)row[X], (int)row[Y], false, tempC };
                    map.addLocation(&tempL);
                }
                std::cout << "Creating a graph\n" << std::endl;
                std::cout << "Import vertices.. " << std::endl;
                clearScreen();
            }

            lineNumber++;
            std::cout << std::endl;
        }
        system("cls");
        
        for (const auto& idx : make_iterator_range(vertices(map.graph))) {
            static int iterator = 0;
            iterator++;
            for (const auto& idx2 : make_iterator_range(vertices(map.graph))) {
                if (map.graph[idx] == map.graph[idx2]) {
                    continue;                                                           
                }
                Route* tempR = new Route(map.graph[idx], map.graph[idx2]);
                //      boost::list_edge<size_t, Route> temp;
                //      temp.m_source = vertex(idx, map.graph);
                //      temp.m_target = vertex(idx2, map.graph);
                //      temp.m_property = *tempR;
                
                //      map.graph.m_edges.push_back(temp);
                //      temp.push_back(sizeof(tempR));
                
                //      problem.getMap()->graph.m_edges.push_back(temp);
                
                //      boost::put(edge_weight_t(), map.graph, map.graph.m_edges.back(), tempR->cost);

                map.addEdgeWithRouteToMap(vertex(idx, map.graph), vertex(idx2, map.graph), *tempR);

                std::cout << "Building graph\n" << std::endl;
                std::cout << "Import vertices.. " << std::endl;
                std::cout << "Import edges... (" << iterator << "/" << map.graph.m_vertices.size() * (map.graph.m_vertices.size() - 1) << ")" << std::endl;
                clearScreen();
            }
        }
        map.updateTotalDemand();
        return true;
    }

    return false;
}

bool FileIO::importToVehicles(std::vector<Vehicle*>& vehicles)
{
    std::string line;                   ///< Row line readed from file
    std::map<int, std::string> row;     ///< Temporary row for parsed file

    if (_inputFile->is_open()) {
        int it = 0;                     ///< Iterator for elements in row
        int lineNumber = 0;             ///< Iterator for number of rows (0 - is table header)

        while (!_inputFile->eof()) {
            row.clear();
            *_inputFile >> line;

            char_separator<char> sep(";");
            tokenizer<char_separator<char> > tok(line, sep);
            it = 0;
            for (tokenizer<char_separator<char> >::iterator beg = tok.begin(); beg != tok.end(); ++beg) {

                std::cout << *beg << " |";
                if (*beg == "") {
                    continue;
                }
                if (lineNumber == 0) {
                    row[it] = ""; ///< Do nothing
                }
                else {
                    row[it] = *beg;
                }
                it++;

                if (lineNumber == 0) {      ///< Checking table header
                    static std::string compare;
                    compare += *beg + ";";
                    if (it == 6) {          ///< Iterator should be equal 5, because we have 5 parameters (0->4)

                        if (!checkFileHeader(compare, "Mark;Type;MaxCapacity;TankCapacity;VehicleMass;FuelConsumption;"))
                        {
                            return false;
                        }

                        std::cout << "\n-----------------------------------------";
                    }
                }
            }

            if (row[0] == "" && row[1] == "" && row[2] == "" && row[3] == "" && row[4] == "" && row[5] == "")
            {
                lineNumber++;
                std::cout << std::endl;
                continue;
            }
            else {
                if (lineNumber == 0) {
                    std::cout << "\n-----------------------------------------";
                }
                else { ///< Each other element is vehicle
                    vehicles.emplace_back(new Vehicle(row[0], row[1], std::stod(row[2]), std::stod(row[3]), std::stod(row[4]), std::stod(row[5])));
                    std::cout << "\nImport vehicles...." << std::endl;
                    clearScreen();
                }
            }

            lineNumber++;
            std::cout << std::endl;
        }
        return true;
    }
    return false;
}

bool FileIO::exportToTextFile(std::stringstream text)
{
    if (_outputFile->is_open()) {
        *_outputFile << text.str();
        return true;
    }
    else {
        return false;
    }
}

bool FileIO::exportGraphToFile(Graph g) {
    _time = time(0);
    struct tm ltm;
    localtime_s(&ltm, &_time);
    try {
        std::filesystem::create_directory(std::filesystem::path("Results") / std::filesystem::path(_directory) / "graph");
        // std::cout << "Created directory: " << _directory << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error FileIO Graph: " << ex.what() << std::endl;
        boost::detail::Sleep(3000);
    }
    std::string graphFilename = "graph_" + std::to_string(1900 + ltm.tm_year) + "_" + std::to_string(1 + ltm.tm_mon) + "_" + std::to_string(ltm.tm_mday) + "_" + std::to_string(ltm.tm_hour) + "_" + std::to_string(ltm.tm_min) + "_" + std::to_string(ltm.tm_sec);
    std::string graphFilenameExt = graphFilename + ".dot";
    std::ofstream dotFile(std::filesystem::path("Results") / std::filesystem::path(_directory) / "graph" / graphFilenameExt);
    boost::write_graphviz(dotFile, g);
    dotFile.close();
    std::string dotFilePath = "Results/"+ _directory +"/graph/"+ graphFilenameExt;
    std::string jpgGraph = "Results/" + _directory + "/graph/" + graphFilename+".jpg";


    std::string command = "dot -Tjpg " + dotFilePath + " -o " + jpgGraph;

    
    int result = system(command.c_str());

    if (result == 0) {
        system("cls");
        std::cout << "\nGraph to image converted succesfully." << std::endl;
        boost::detail::Sleep(3000);
    }
    else {
        system("cls");
        std::cerr << "\nConvert graph to image error. Do you have installed dot?" << std::endl;
        boost::detail::Sleep(3000);
    }
    return true;
}
