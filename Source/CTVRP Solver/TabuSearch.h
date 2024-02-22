/*****************************************************************//**
 * \file   TabuSearch.h
 * \brief  Class for Tabu Search algorithm implemetation.
 * 
 * \author Jakub Kaniowski
 * \date   October 2023
 *********************************************************************/
#pragma once
#ifndef TABU_SEARCH_H
#define TABU_SEARCH_H

#include "Algorithm.h"

typedef  std::pair<MethodResult, unsigned int> TabuElements;     ///< 'History' data of changes. Tabu list. Method result has informations about changes, unsigned int is counter for forbidden iterations.
typedef std::vector<TabuElements> ChangeLogList;    

class TabuSearch :
    public Algorithm
{
public:
    TabuSearch(Problem& problemToSolve, int tabuTenure, int maxIterations);
    ~TabuSearch();

    void run();

private:
    const int _tabuTenure; ///< Forbidden iterations
    int _tabuSize;         ///< Tabu size
    int _maxIterations;    ///< Maximum iterations for algorithm
    ChangeLogList _tabuList;        ///< Tabu list
    ChangeLogList _bestTabu;

    /// @brief Method used for checking if indicated modification is allowed or not.
    /// @param lastModification 
    /// @return true if last modification is not exist, or element in tabu list has tabu prohibition = 0 (tenure == 0).
    bool checkTabu(MethodResult &lastModification); 

    /// @brief Update tabu elements after every iteration of algorithm. The expiry time of forbidden actions is decreased.
    void updateTabu();

    /// @brief Add indicated modifictation to tabu list. The operation clears prohibitions so that the length of the list is constant.
    /// @param lastModification 
    void addTabu(MethodResult& lastModification); 

    /// @brief Show tabu list
    /// @return Output as txt stream
    std::stringstream showTabuList();      


    Solution* modSolution(Solution& solution);

};

#endif