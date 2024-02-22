

#include "LocalSearch.h"
#include "UtilsFcn.h"

LocalSearch::LocalSearch(Problem& problemToSolve) : Algorithm(&problemToSolve)
{
}

LocalSearch::~LocalSearch()
{
}

void LocalSearch::run() {
    unsigned int i = 0; ///< Iterator for main loop
    unsigned int overload = 0;
    unsigned int ok = 0;
    unsigned int non_profit = 0;
    unsigned int acc_non_profit = 0;
    unsigned int exception = 0;
    unsigned int valid_fail = 0;
    double newSolutionCost = 0;
    double minFoundCost = INFINITY; // To finding best solution!
    float foundBestTime = 0;    // Timestamp when found minimal solution
    const char s = ' ';

    FileIO outputFile("result_","LS",txt);
    std::stringstream tempStr;  ///< Temporary string stream to console output and logging
    FileIO logResultsFile("LS_", "LS", csv);
    logResultsFile.exportToTextFile(std::stringstream() << "i;t;s;bs;\n"); // add header in csv: iteration, time, solution, best solution, temperature.
    
    system("cls");

    boost::timer timer;
    timer.restart();

    if (!generateInitialSolution()) {
        std::cout << "\n (LS) ERROR - INITIAL SOLUTION NOT FOUND\n" << std::endl;
        return;
    }

    std::cout << "Generation time:\t" << timer.elapsed() << " s\n" << std::endl;

	Problem* initial = new Problem(*problemToSolve);    ///< Save initial solution to compare at the end
    initial->solution->calculateTotalCost();
    

    tempStr << "Local Search algorithm\n--------------------------------\n"
        << " Nodes: " << problemMap->getNumberOfLocations()
        << " Edges: " << problemMap->getNumberOfRoutes()
        << "\n Total demand of nodes: " << problemMap->getTotalDemand()
        << "\n Vehicle max capacity:  " << actualVehicle->getMaximumCapacity()
        << "\n--------------------------------\n"
        << "\n" << std::endl;

    tempStr << "\nInitial solution:\n" << std::endl;
    tempStr << initial->show().str() << std::endl;

    problemToSolve->solution = new Solution(*initial->solution);

    double oldCost = problemSolution->calculateTotalCost();
    problemSolution->calculatePathDemandForSolution(*problemSolution);
    std::cout << "\n-----------------------------------\nTotal solution cost:\t" << problemSolution->totalCost << "\n" << std::endl;

    boost::detail::Sleep(2000);
    system("cls");

    Solution* newSolution = nullptr;                    ///< Prepare pointer to generating new solution

    timer.restart();
    double costOldSolution = 0, costNewSolution = 0;

    bool stopLoop = false;
    bool canBeBest = false;

    int iterWithoutProgress = 0;

    // Local Search Algorithm

    do {
        double newSolutionCost = problemToSolve->solution->totalCost;
        newSolution = nullptr;
        
        if (iterWithoutProgress > 20) { // End condition
            break;
        }
       
        // Generate neighbor best:


        newSolution = bestGenerator(1, *problemToSolve->solution);
        if (newSolution == nullptr)
        {
            continue; // TEMPORARY SKIP IF SOLUTION NOT FOUND
        }
        newSolutionCost = newSolution->totalCost;


        // SHOW

        std::cout << "CTVRP Solver 1.0\n----------------------------------" << std::endl;
        std::cout << std::left << std::setw(10) << "LOCAL SEARCH\ni = " << std::setfill(s) << std::setiosflags(std::ios::fixed) << std::left << std::setw(6) << std::setprecision(0) << i
      
            << std::setiosflags(std::ios::fixed) << std::right << std::setw(25) << "Validation fail: " << std::left << std::setw(4) << std::setprecision(0) << std::setfill(s) << valid_fail << std::setprecision(2) << "\n";
        std::cout << "\nOK: " << std::setiosflags(std::ios::fixed) << std::left << std::setw(10) << std::setfill(s) << ok
            << "\tOver: " << std::setiosflags(std::ios::fixed) << std::left << std::setw(10) << std::setfill(s) << overload
            << "\tNon-profit: " << std::setiosflags(std::ios::fixed) << std::left << std::setw(10) << std::setfill(s) << non_profit
            << "\tAccepted Non-profit: " << std::setiosflags(std::ios::fixed) << std::left << std::setw(10) << std::setfill(s) << acc_non_profit
            << "\tE: " << std::setiosflags(std::ios::fixed) << std::left << std::setw(10) << std::setfill(s) << exception << std::setw(20) << std::setfill(s) << std::endl
            << newSolution->show().str() << std::endl
            << "\nGeneration time:\t" << timer.elapsed() << " s" << std::endl
            << "Init Total Cost:\t" << oldCost << std::endl
            << "NEW SOLUTION COST:\t" << newSolutionCost << std::endl
            << "Actual Total Cost:\t" << problemToSolve->solution->totalCost << std::endl
            << "FOUND BEST:\t\t" << minFoundCost << "\t (t: " << foundBestTime << " s)" << std::endl
            << "Best Profit: \t\t" << oldCost - minFoundCost << std::endl
            << "Best Profit [%]:\t" << (oldCost - minFoundCost) * 100 / oldCost << "%\n" << std::endl;
        clearScreen();

        logResultsFile.exportToTextFile(std::stringstream() << i << ";" << timer.elapsed() << ";" << newSolution->totalCost << ";" << problemToSolve->bestSolution->totalCost << ";\n");


        newSolution->updateRoutesFromLocations();
        newSolution->calculatePathDemandForSolution(*newSolution);
        problemSolution->calculatePathDemandForSolution(*problemSolution);
        costOldSolution = problemSolution->calculateTotalCost();
        costNewSolution = newSolution->calculateTotalCost();
        canBeBest = false;
        switch (checkSolution(*problemToSolve->solution, *newSolution))
        {
        case UNVISITED_NODES:
        {
            valid_fail++;
            delete newSolution;
            continue;
        } break;

        case BETTER:    //    f(S') < f(S) ? if true -> f(S) = f(S')
        {
            //iterWithoutProgress = 0;
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
            delete newSolution;
            stopLoop = true;
            iterWithoutProgress++;
           // continue;

        } break;

        case V_OVERLOAD:
        {
            iterWithoutProgress++;
            overload++;
            canBeBest = false;
            delete newSolution;
            clearScreen();
            i++;
            iterWithoutProgress++;
            continue;
          //  continue;

        } break;

        default:
        {
            std::cout << "\n\n\n\n\n!!!!UNSUPPORTED CASE!!!!" << std::endl;
            exception++;
        } break;
        }
        newSolutionCost = problemToSolve->solution->totalCost;
        if ((newSolutionCost < minFoundCost) && canBeBest) {    // f(S') < f(S*) ? if true -> f(S*) = f(S')   (Best solution)
            minFoundCost = problemToSolve->solution->totalCost;
            delete problemToSolve->bestSolution;
            problemToSolve->solution->calculatePathDemandForSolution(*problemToSolve->solution);
            problemToSolve->bestSolution = new Solution(*problemToSolve->solution);
            foundBestTime = timer.elapsed();
            //system("cls");
        }
        else {
            iterWithoutProgress++;
        }
        std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
        clearScreen();
        i++;
    } while (((costNewSolution - costOldSolution) <= 0) && !stopLoop );    ///< Work until there is  progress



    std::cout << "Generation time:\t" << timer.elapsed() << " s\n" << std::endl;

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
        tempStr << "\n\nFOUND BEST:\n\n";
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

    outputFile.exportToTextFile(std::stringstream() << tempStr.str());
    // outputFile.exportGraphToFile(problemMap->graph);

    std::cout << "\n\n\n\n\n\n\n\n\n";

    system("cls");
    std::cout << tempStr.str();
}
