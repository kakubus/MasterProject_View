/*****************************************************************//**
 * \file   main.cpp
 * \brief  Main program file. Contains cmd interpreter, and call 
 * Algorithm methods.
 * 
 * \author Jakub Kaniowski
 * \date   October 2023
 *********************************************************************/
#include <iostream>

#include "Problem.h"
#include "SimulatedAnnealing.h"
#include "LocalSearch.h"
#include "TabuSearch.h"


int main(int argc, char* argv[]) {
    bool v_option = false;
    bool p_option = false;
    std::vector<float> sa_values; 
    std::vector<int> ls_values; 
    std::vector<int> ts_values;

    std::string problemPath = "";
    std::string vehiclePath = "";
    system("cls");
    std::cout << "CTVRP Solver 1.0\n----------------------------------" << std::endl;

    // TESTS
    
    
 //   Problem* vrpProblem = new Problem("Benchmarks/Benchmark_03_661.csv", "Benchmarks/Vehicles_List.csv");
 //   SimulatedAnnealing SA = { *vrpProblem, 100, 0.1, 0.90, 100 };							//	SimulatedAnnealing SA = { *vrpProblem, 2000, 0.1, 0.97, 1200 };
 //   SA.run();

   // Problem* vrpProblemTS = new Problem("Benchmarks/Benchmark_02_375.csv", "Benchmarks/Vehicles_List6000.csv");
  //  TabuSearch TS = { *vrpProblemTS, 100, 1000 };
  //  TS.run();
    
    
  //  Problem* vrpProblemLS = new Problem("Benchmarks/Benchmark_03_661.csv", "Benchmarks/Vehicles_List.csv");
 //   LocalSearch LS = { *vrpProblemLS };
 //   LS.run();
    


    // END TESTS

    for (int i = 1; i < argc; i++) {
        std::string option = argv[i];

        if (option == "--help") {
            std::cout << "Program help:\n\n" << "To ensure that the program runs correctly, it must be invoked with the following list of arguments\nin the specified order:\n"
                << "\t-p <path to problem files>\t This is a relative path to the problem definition file.\n"
                << "\t-v <path to vehicles files>\t This is a relative path to the vehicle definition file used for optimization.\n"
                << "\nAlgorithms:\nYou can use three algorithms with three options each:\n\n"
                << "\t-SA <param1: Tmax> <param2: Tend> <param3: alpha> <param4: i_epochs> -Run the Simulated Annealing\n\talgorithm with parameters Tmax, Tend, Alpha, and Epoch.\n\n"
                << "\t-LS -Run the Local Search algorithm.\n\n"
                << "\t-TS <param1: tabu tenure> <param2: iteration limit> -Run the Tabu Search \n\talgorithm with parameters Tabu tenure and iteration limit.\n\n"
                << "\t(!) Please remember to provide this data in the specified order!\n\n"
                << "Usage example:\n\tProgram.exe -p Benchmarks\\Benchmark.csv -v Benchmarks\\Vehicles.csv -SA 1200 0.01 0.95 1500\n\n----------------------------------\nCopyright (C) 2023 Jakub Kaniowski"
                << "" << std::endl;
            v_option = "NULL";
            p_option = "NULL";
            return 0;
        }
        else if (option == "-v") { // Import vehicles
            v_option = true;
            if (i + 1 < argc) {
                std::string value = argv[i + 1];
                i++;
                std::cout << "Import vehicles from: " << value << std::endl;
                vehiclePath = value;

                boost::detail::Sleep(500);
            }
            else {
                std::cout << "Empty -v parameter. Program closed." << std::endl;
                boost::detail::Sleep(1000);
                return -1;

            }
        }
        else if (option == "-p") { // Import problem
            p_option = true;
            if (i + 1 < argc) {
                std::string value = argv[i + 1];
                i++;
                std::cout << "Import problem definition from: " << value << std::endl;
                problemPath = value;

                boost::detail::Sleep(500);
            }
            else {
                std::cout << "Empty -p parameter. Program closed." << std::endl;
                boost::detail::Sleep(1000);
                return -1;
            }
        }
        else if (option == "-sa" || option == "-SA") { // Simulated annealing case
            if (i + 4 < argc) {
                std::string param1 = argv[i + 1];
                std::string param2 = argv[i + 2];
                std::string param3 = argv[i + 3];
                std::string param4 = argv[i + 4];
                i += 4;
                sa_values.push_back(std::stof(param1));    // Tmax
                sa_values.push_back(std::stof(param2));    // Tend    
                sa_values.push_back(std::stof(param3));    // Alpha
                sa_values.push_back(std::stof(param4));    // Iterations per epochs
                std::cout << "\nSimulated annealing:\nStart temperature:\t" << sa_values.at(0) << "\nEnd temperature:\t" << sa_values.at(1) << "\nAlpha:\t\t\t" << sa_values.at(2) << "\nIterations per epoch:\t" << sa_values.at(3) << std::endl;
                boost::detail::Sleep(1000);

                Problem* vrpProblem = new Problem(problemPath, vehiclePath);
                SimulatedAnnealing SA = { *vrpProblem, sa_values.at(0), sa_values.at(1), sa_values.at(2), (int)(sa_values.at(3)) };							//	SimulatedAnnealing SA = { *vrpProblem, 2000, 0.1, 0.97, 1200 };
                SA.run();
                
                return 0;
            }
            else {
                std::cout << "Simulated annealing: No enough input parameters! Program closed." << std::endl;
                boost::detail::Sleep(1000);
                return -2;
            }
        }
        else if (option == "-ts" || option == "-TS") {  // Tabu Search case
            if (i + 2 < argc) {
                std::string param1 = argv[i + 1];
                std::string param2 = argv[i + 2];

                i += 2;
            
                ts_values.push_back(std::stoi(param1));    // Tabu tenure
                ts_values.push_back(std::stoi(param2));     // Max iterations   
 
                
                std::cout << "\nTabu Search:\nTabu tenure:\t" << ts_values.at(0) << "\nMax iterations:\t" << ts_values.at(1) << std::endl;
                boost::detail::Sleep(1000);

                Problem* vrpProblemTS = new Problem(problemPath, vehiclePath);
                TabuSearch TS = { *vrpProblemTS, (int)ts_values.at(0), (int)ts_values.at(1) };
                TS.run();

                return 0;
            }
            else {
                std::cout << "Tabu search: No enough input parameters! Program closed." << std::endl;
                boost::detail::Sleep(1000);
                return -2;
            }
        }
        else if (option == "-ls" || option == "-LS") {  // Local search case

            Problem* vrpProblemLS = new Problem(problemPath, vehiclePath);
            LocalSearch LS = { *vrpProblemLS };
            LS.run();
                
            return 0;
    
      
        }
        else { // Wrong parameters
            std::cout << "Unknown input parameters. Program closed." << std::endl;
            boost::detail::Sleep(1000);
            return -1;
        }
    }

    if (!v_option || !p_option) { // Wrong parameters
        std::cout << "No input paths! Program closed." << std::endl;
        boost::detail::Sleep(1000);
        return -1;
    }

	system("PAUSE");
 	return 0;
}