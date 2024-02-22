/*****************************************************************//**
 * \file   SimulatedAnnealing.cpp
 * \brief  Class which defines simulated annealing algorithm
 * 
 * \author Jakub Kaniowski
 * \date   June 2023
 *********************************************************************/

#include "SimulatedAnnealing.h"
#include <Windows.h>
#include <string>
#include <chrono>

#include "UtilsFcn.h"

SimulatedAnnealing::SimulatedAnnealing(Problem& problemToSolve, float tMax, float tEnd, float alpha, int maxIterations) 
	: Algorithm(&problemToSolve), _alpha(alpha)
{
	_maxIterations = (maxIterations > 0) ? maxIterations : 1;
	_tMax = tMax;
	_tEnd = tEnd;
	_T = tMax;
	_vCapacity = problemToSolve.getVehicleList()->at(0)->getMaximumCapacity();
}

SimulatedAnnealing::~SimulatedAnnealing() 
{
}

void SimulatedAnnealing::run()
{
    _T = _tMax;                             // Set initial temperature
    FileIO outputFile("result_", "SA", txt);
    FileIO logResultsFile("SA_", "SA", csv);
    logResultsFile.exportToTextFile(std::stringstream() << "i;t;s;bs;T;\n"); // add header in csv: iteration, time, solution, best solution, temperature.
    std::stringstream tempStr;

    int i = 0;
    int global_i = 0;
    int i_initial = 0;
    float foundBestTime = 0;    // Timestamp when found minimal solution

    boost::timer timer;

    timer.restart();

    if (!generateInitialSolution()) {
        std::cout << "\n (SA) ERROR - INITIAL SOLUTION NOT FOUND\n" << std::endl;
        return;
    }

    std::cout << "Generation time:\t" << timer.elapsed() << " s\n" << std::endl;
    
    // To refactor!

    Problem* initial = new Problem(*problemToSolve); // Copy to compare with minimum solution when algorithm end.
    initial->solution->calculateTotalCost();

    tempStr << "Simulated annealing algorithm\n--------------------------------\n"
        << " Nodes: " << problemMap->getNumberOfLocations()
        << " Edges: " << problemMap->getNumberOfRoutes()
        << "\n Total demand of nodes: " << problemMap->getTotalDemand()
        << "\n Vehicle max capacity:  " << _vCapacity
        << "\n--------------------------------\n"
        << "Algorithm parameters: \n Alpha: " << _alpha << " tMax: " << _tMax << " tEnd: " << _tEnd << "\n Iterations limit: " << _maxIterations
        << "\n" << std::endl;

    tempStr << "\nInitial solution:\n" << std::endl;
    tempStr << initial->show().str() << std::endl;
   

    double oldCost = problemSolution->calculateTotalCost();
    problemSolution->calculatePathDemandForSolution(*problemSolution);
    std::cout << "\n-----------------------------------\nTotal solution cost:\t" << problemSolution->totalCost << "\n" << std::endl;
    
    Sleep(2000);
    system("cls");
    
    
  //  std::cout << "\n-" << std::endl;

    // FOR TEST

    unsigned int overload = 0;
    unsigned int ok = 0;
    unsigned int non_profit = 0;
    unsigned int acc_non_profit = 0;
    unsigned int exception = 0;
    unsigned int valid_fail = 0;

    double newSolutionCost = 0;
    double probability = 0;     // Acceptation probability
    double eProbability = 0;  // Acceptation probability

    double minFoundCost = INFINITY; // To finding best solution!

    timer.restart();

    // Simulated Annealing Algorithm

    while (_T > _tEnd) {
        int i = 0;
    //    std::cout << "\nSA -" << i << std::endl;
        while (i < _maxIterations)
        {
            global_i++;
            const char s = ' ';
            
          

            Solution* newSolution = nullptr;
            bool canBeBest = false; // FLAG FOR CHECKING BEST SOLUTION
          

            //  GENERATE NEIGHBOR S'
            
            
            newSolution = rndGenerator(*problemToSolve->solution);
            if (newSolution == nullptr)
            {
                continue; // TEMPORARY SKIP IF SOLUTION NOT FOUND
            }
            newSolutionCost = newSolution->totalCost;

            
            // OLD CONCEPT
          //  newSolution = modifySolution(newSolution);
            
          //  int differenceMake = round(randomFloat(1, ((_T <= 1) ? 1 : _T)));
           // for (int j = 0; j < differenceMake; j++) {
               //  - - - - - - newSolution = modifySolution(newSolution);
                //newSolution = generateNeighborSolution(newSolution);
          //  }
            

            if (newSolution == nullptr) {   ///< If generateNewSolution returns fail, delete failed new solution and continue loop.
                delete newSolution;
                //i++;
                continue; // skip if problem
            }
            newSolutionCost = newSolution->calculateTotalCost();




            // SHOW

            std::cout << "CTVRP Solver 1.0\n----------------------------------" << std::endl;
            std::cout << std::left << std::setw(20) << "SIMULATED ANNEALING\nT = " << std::setfill(s) << std::setiosflags(std::ios::fixed) << std::left << std::setw(6) << std::setprecision(6) << _T
                << std::setiosflags(std::ios::fixed) << std::right << std::setw(30) << "General progress [T]: " << std::left << std::setw(6) << std::setprecision(3) << ((_tMax - _T) * 100 / (_tMax - _tEnd)) << "%"
                << std::setiosflags(std::ios::fixed) << std::right << std::setw(25) << "Iteration progress [i]: " << std::left << std::setw(4) << std::setprecision(2) << std::setfill(s) << (i * 100 / _maxIterations) << "%"
                << std::setiosflags(std::ios::fixed) << std::right << std::setw(25) << "Validation fail: " << std::left << std::setw(4) << std::setprecision(0) << std::setfill(s) << valid_fail << std::setprecision(3) << "";

            std::cout << "\nOK: " << std::setiosflags(std::ios::fixed) << std::left << std::setw(10) << std::setfill(s) << ok
                << "\tOver: " << std::setiosflags(std::ios::fixed) << std::left << std::setw(10) << std::setfill(s) << overload
                << "\tNon-profit: " << std::setiosflags(std::ios::fixed) << std::left << std::setw(10) << std::setfill(s) << non_profit
                << "\tAccepted Non-profit: " << std::setiosflags(std::ios::fixed) << std::left << std::setw(10) << std::setfill(s) << acc_non_profit
                << "\tE: " << std::setiosflags(std::ios::fixed) << std::left << std::setw(10) << std::setfill(s) << exception << std::setw(20) << std::setfill(s) << std::endl
                << "\nNP Probability:\t" << probability << "\tExp:\t" << std::setw(5) << eProbability << std::setw(10) << std::setfill(s) << "\n\n" << std::endl
                << newSolution->show().str() << std::endl
                << "\nGeneration time:\t" << timer.elapsed() << " s" << std::endl
                << "Init Total Cost:\t" << oldCost << std::endl
                << "NEW SOLUTION COST:\t" << newSolutionCost << std::endl
                << "Actual Total Cost:\t" << problemToSolve->solution->totalCost << std::endl
                << "FOUND BEST:\t\t" << minFoundCost << "\t (t: " << foundBestTime << " s)" << std::endl
                << "Best Profit: \t\t" << oldCost - minFoundCost << std::endl
                << "Best Profit [%]:\t" << (oldCost - minFoundCost) * 100 / oldCost << "%\n" << std::endl;
            // << "Dynamic Iteration limiter:\t" << _maxIterations << "\n" << std::endl;

            clearScreen();






            // Log into csv file: iteration, time, solution, best solution, temperature.
            logResultsFile.exportToTextFile(std::stringstream() << global_i << ";" << timer.elapsed() << ";" << problemToSolve->solution->totalCost << ";" << problemToSolve->bestSolution->totalCost << ";" << _T << ";\n");



            switch (checkSolution(*problemToSolve->solution, *newSolution))
            {
            case UNVISITED_NODES:
            {
                i++;
                valid_fail++;
                delete newSolution;
                continue;
            } break;

            case BETTER:    //    f(S') <= f(S) ? if true -> f(S) = f(S') 
            {
                canBeBest = true;
                ok++;
                delete problemToSolve->solution;
                problemToSolve->solution = new Solution(*newSolution);
                delete newSolution;
            } break;
                
            case NON_PROFIT:
            {
                canBeBest = false;
                non_profit++;

                problemToSolve->solution->calculateTotalCost();

                //  PROBABILITY ACCEPTATION
                probability = randomFloat(0.0, 1.0);    // Random probability value
                eProbability = exp((problemToSolve->solution->totalCost - newSolution->totalCost) / _T);   // e( (f(S')-f(S)) /T )

                if (probability < eProbability) {       // Accept worse solution
                    acc_non_profit++;

                    delete problemToSolve->solution;
                    problemToSolve->solution = new Solution(*newSolution);
                }

                delete newSolution;
                i++;
               // continue;

            } break;

            case V_OVERLOAD:
            {
                overload++;
                canBeBest = false;
                delete newSolution;
                i++;
             //   continue;

            } break;

            default:
            {
                std::cout << "\n\n\n\n\n!!!!UNSUPPORTED CASE!!!!" << std::endl;
                exception++;
            } break;
            }

            double newSolutionCost = problemToSolve->solution->totalCost;

            if ((newSolutionCost < minFoundCost) && canBeBest) {    // f(S') < f(S*) ? if true -> f(S*) = f(S')   (Best solution)
                minFoundCost = problemToSolve->solution->totalCost;
                delete problemToSolve->bestSolution;
                problemToSolve->solution->calculatePathDemandForSolution(*problemToSolve->solution);
                problemToSolve->bestSolution = new Solution(*problemToSolve->solution);
                foundBestTime = timer.elapsed();
                system("cls");
               //  break; // ?? ?? ?? ?? NOPE, CANNOT BE BREAK
            }

            i++;
            std::cout << "\n\n\n";
            clearScreen();
           

        } // END LOOP WHILE (i < SAmax)
   
        _T *= _alpha;
    }
    
    tempStr << "\n\nEnd solution:\n\n";
    tempStr << problemToSolve->show().str() << std::endl;
    tempStr << "INITIAL TOTAL COST: " << oldCost << std::endl;
    tempStr << "END TOTAL COST: " << problemToSolve->solution->totalCost << std::endl;
    tempStr << "\nProfit: " << oldCost - problemToSolve->solution->totalCost << std::endl;
    tempStr << "Profit [%]: " << (oldCost - problemToSolve->solution->totalCost) * 100 / oldCost << std::endl;
    tempStr << "\n\nSolutions finding history:\n";
    tempStr << "\nSolution OK:\t" << ok;
    tempStr << "\nNon_Profit:\t" << non_profit;
    tempStr << "\n  Accepted:\t" << acc_non_profit;
    tempStr << "\nOverload:\t" << overload;
    tempStr << "\nExceptions:\t" << exception;

    if (ok == 0) {
        tempStr << "\n\n> NO BETTER SOLUTION FOUND <\n\n";
    }
    else {
        tempStr << "\n\nFOUND BEST::\n\n";
    }
    
    tempStr << problemToSolve->bestSolution->show().str() << std::endl;

    tempStr << "\n\nBEST SOLUTION COST:\t" << problemToSolve->bestSolution->totalCost << "\t found at: " << foundBestTime << " s" << std::endl;
    
    tempStr << "Best Profit: \t\t" << oldCost - minFoundCost << std::endl;
    tempStr << "Best Profit [%]:\t" << (oldCost - minFoundCost) * 100 / oldCost << "%" << std::endl;


    if (problemSolution->validateSolution(*problemToSolve->bestSolution))
    {
        problemToSolve->bestSolution->calculatePathDemandForSolution(*problemToSolve->bestSolution);
        tempStr << "\nValidation best solution:\tOK\n";
    }
    else {
        tempStr << "\nValidation best solution:\tFAILED\n";
    }
    tempStr << "Generation time: " << timer.elapsed() << " s\n" << std::endl;
    Sleep(3000);
    outputFile.exportToTextFile(std::stringstream() << tempStr.str());
   // outputFile.exportGraphToFile(problemMap->graph);

    std::cout << "\n\n\n\n\n\n\n\n\n";
    
    system("cls");
    std::cout << tempStr.str();

    minFoundCost = 0;
 //   delete initial;

}

