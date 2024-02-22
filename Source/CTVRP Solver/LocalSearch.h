/*****************************************************************//**
 * \file   LocalSearch.h
 * \brief  Class for Local search algorithm implementation.
 * 
 * \author Jakub Kaniowski
 * \date   September 2023
 *********************************************************************/
#pragma once
#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include "Algorithm.h"
class LocalSearch :
    public Algorithm
{
public:
    LocalSearch(Problem& problemToSolve);
    ~LocalSearch();

    void run(); 

};

#endif