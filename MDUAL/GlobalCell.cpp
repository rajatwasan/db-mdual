#pragma once
#include <bits/stdc++.h>

using namespace std;
#include "../Loader/Query.cpp"
/**
 * Global Cell data structure for block groups
 */
class globalCell
{
public:
    map<vector<int>, double> mp_neighCellMap;
    vector<int> vect_cellIdx;
    list<Query> vect_ndQueries;

    vector<int> vect_IndirectOutlierCellQueryIDs;
    map<int, int> mp_cardPerSlide;
    bool last_Updated;
    int card;

    globalCell(vector<int> vect_cellIdx)
    {
        this->vect_cellIdx = vect_cellIdx;
        card = 0;
        last_Updated = false;
    }
    globalCell(){};

    int getsize()
    {
        return vect_cellIdx.size();
    }

    int getCardTotal(int firstSlideID)
    {
        int total = 0;
        for (auto i : mp_cardPerSlide)
        {
            // cout << i.first << "i.first" << endl;
            if (i.first >= firstSlideID)
            {
                total += i.second;
            }
        }
        return total;
    }

    vector<vector<int>> getThredNeighCellsOut(double distThred)
    {
        vector<vector<int>> thredNeighCells;
        for (auto i : mp_neighCellMap)
        {
            if (i.second < distThred)
            {
                thredNeighCells.push_back(i.first);
            }
        }
        return thredNeighCells;
    }

    vector<vector<int>> getThredNeighCellsIn(double distThred)
    {
        vector<vector<int>> thredNeighCells;
        for (auto i : mp_neighCellMap)
        {
            if (i.second <= distThred)
            {
                thredNeighCells.push_back(i.first);
            }
        }
        return thredNeighCells;
    }

    vector<vector<int>> getThredNeighCell(double distThred)
    {
        vector<vector<int>> thredNeighCells;
        for (auto i : mp_neighCellMap)
        {
            if (i.second != distThred)
            {
                thredNeighCells.push_back(i.first);
            }
        }
        return thredNeighCells;
    }
};
