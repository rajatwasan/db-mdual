#pragma once
#include <bits/stdc++.h>
using namespace std;
#include "..//MDUAL/Utils.cpp"
/**
 * MDUAL is the implementaion of main algorithms
 */
class MDUAL
{
public:
    double min_R, max_R, minR_old, maxR_old;
    int gcdS, dim, subdim, nS, nW;
    bool sub_Dim_Flag, max_RChanged, min_RChanged;
    vector<double> minValues, dimLength, subDimLength;
    // double *dimLength, *subDimLength;

    map<vector<int>, Cell> slideIn, slideOut;
    map<vector<int>, int> slideDeltaCnt;
    map<vector<int>, globalCell> cardGrid, fullDimCardGrid;
    list<map<vector<int>, Cell>> slides;
    list<map<vector<int>, int>> fullDimCellSlidesCnt;
    set<Tuple> outliers;
    map<int, Query> querySet;

    bool sub_dim_Flag;

    MDUAL(int dim, int subDim, int nS, int gcdS, vector<double> minValues)
    {
        this->dim = dim;
        this->subdim = subDim;
        // cout << "subdim " << this->subdim << endl;
        this->sub_Dim_Flag = dim != subdim;
        this->min_R = std::numeric_limits<double>::max();
        this->max_R = std::numeric_limits<double>::lowest();
        this->gcdS = gcdS;
        this->nS = nS;
        this->minValues = minValues;
        this->sub_dim_Flag = true;
    }
    set<Tuple> findOutlier(vector<Tuple> newSlideTuples, map<int, Query> newQuerySet, int itr)
    {
        this->querySet = newQuerySet;
        // cout << itr;
        if (itr == 0)
        {
            this->updateBasisParams(itr);
            this->initCellSize();
            // cout << outliers.size() << endl;
        }
        this->clearPreviousOutliers();
        // cout << outliers.size() << endl;

        this->updateWindow(newSlideTuples, itr);
        // cout << outliers.size() << endl;

        this->updateBasisParams(itr);
        // cout << outliers.size() << endl;

        this->findOutlierMain(itr);
        // cout << outliers.size() << endl;

        return this->outliers;
    }
    void initCellSize()
    {
        dimLength.clear();
        dimLength.resize(dim);
        // cout << dim;
        // dimLength = new double[dim];
        for (int i = 0; i < dim; i++)
        {
            // cout << dim;
            dimLength[i] = sqrt(min_R * min_R / dim);
        }
        // cout << subdim << endl;
        // cout << sub_Dim_Flag << endl;
        if (sub_Dim_Flag)
        {
            // subDimLength = new double[subdim];
            subDimLength.clear();
            subDimLength.resize(subdim);
            for (int i = 0; i < subdim; i++)
            {
                // cout << subdim;
                subDimLength[i] = sqrt(min_R * min_R / subdim);
            }
        }
    }

    void clearPreviousOutliers()
    {
        for (auto i : outliers)
        {
            Tuple obj = i;
            obj.set_outlier_queryIds.clear();
            outliers.erase(i);
        }
    }

    // Initialize base variables
    void updateBasisParams(int itr)
    {
        max_RChanged = false;
        min_RChanged = false;
        minR_old = min_R;
        maxR_old = max_R;
        min_R = std::numeric_limits<double>::max();
        max_R = std::numeric_limits<double>::lowest();
        for (auto i : querySet)
        {
            if (max_R < i.second.R)
                max_R = i.second.R;
            if (min_R > i.second.R)
                min_R = i.second.R;
        }
        if (itr > 0 and min_R != minR_old)
            min_RChanged = true;
        if (itr > 0 and max_R != maxR_old)
            max_RChanged = true;
    }
    // Algorithm 1. Net change update
    // Update window size and net change
    void updateWindow(vector<Tuple> slideTuples, int itr)
    {
        map<vector<int>, int> fullDimCellSlideInCnt;
        for (auto tuple : slideTuples)
        {
            vector<int> fullDimCellIdx;
            vector<int> subDimCellIdx;
            for (int j = 0; j < dim; j++)
            {
                int dimIdx = (int)(tuple.vect_value[j] - minValues[j]) / dimLength[j];
                fullDimCellIdx.push_back(dimIdx);
            }
            if (sub_Dim_Flag)
            {
                int i = 0;
                while (i < subdim)
                {
                    int dimIdx = (int)((tuple.vect_value[i] - minValues[i]) / subDimLength[i]);
                    subDimCellIdx.push_back(dimIdx);
                    i++;
                }
            }
            else
            {
                subDimCellIdx = fullDimCellIdx;
            }

            tuple.vect_fullDim_CellIdx = fullDimCellIdx;
            tuple.vect_subDim_CellIdx = subDimCellIdx;

            if (slideIn.find(subDimCellIdx) == slideIn.end())
            {
                vector<double> cellCenter;
                cellCenter.resize(subdim);
                if (sub_Dim_Flag)
                {
                    for (int j = 0; j < subdim; j++)
                        cellCenter[j] = minValues[j] + subDimCellIdx[j] * subDimLength[j] + subDimLength[j] / 2;
                }
                else
                {
                    int j = 0;
                    while (j < dim)
                    {
                        cellCenter[j] = minValues[j] + fullDimCellIdx[j] * dimLength[j] + dimLength[j] / 2;
                        j++;
                    }
                }
                slideIn[subDimCellIdx] = *(new Cell(subDimCellIdx, cellCenter, sub_Dim_Flag));
            }

            if (sub_Dim_Flag)
            {
                // vector<double> temp;
                // for (size_t i = 0; i < dim; i++)
                // {
                //     /* code */
                //     temp.push_back(dimLength[i]);
                // }
                slideIn[subDimCellIdx].addTupleSubDim(tuple, dimLength, minValues);
                if (fullDimCellSlideInCnt.find(fullDimCellIdx) == fullDimCellSlideInCnt.end())
                {
                    fullDimCellSlideInCnt[fullDimCellIdx] = 0;
                }
                fullDimCellSlideInCnt[fullDimCellIdx] = fullDimCellSlideInCnt[fullDimCellIdx] + 1;
            }
            else
            {
                slideIn[subDimCellIdx].addTuple(tuple);
            }
        }

        slides.push_back(slideIn);
        if (sub_Dim_Flag)
            fullDimCellSlidesCnt.push_back(fullDimCellSlideInCnt);

        set<vector<int>> newCellIdices;
        // update slideInWindow
        for (auto i : slideIn)
        {
            int card = i.second.getNumTuples();
            if (cardGrid.find(i.first) == cardGrid.end())
            {
                cardGrid[i.first] = *(new globalCell(i.first));
                newCellIdices.insert(i.first);
            }
            cardGrid[i.first].card += card;
            cardGrid[i.first].mp_cardPerSlide[itr] = card;

            slideDeltaCnt[i.first] = card;
        }

        // Update full Dim cell window count - slideIn

        if (sub_Dim_Flag)
        {
            for (auto i : fullDimCellSlideInCnt)
            {
                int card = fullDimCellSlideInCnt[i.first];
                if (fullDimCardGrid.find(i.first) == fullDimCardGrid.end())
                {
                    fullDimCardGrid[i.first] = *(new globalCell(i.first));
                }
                fullDimCardGrid[i.first].card += card;
                fullDimCardGrid[i.first].mp_cardPerSlide[itr] = card;
            }
        }
        // SlideOut:
        getNeighCellMap(newCellIdices);

        if (itr > (nS - 1))
        {
            int slideOutID = itr - nS;
            slideOut = slides.front();
            slides.pop_front();
            for (auto i : slideOut)
            {
                int card = i.second.getNumTuples();
                cardGrid[i.first].card -= card;
                cardGrid[i.first].mp_cardPerSlide.erase(slideOutID);

                if (cardGrid[i.first].card < 1)
                {
                    for (auto j : cardGrid[i.first].mp_neighCellMap)
                    {
                        if (i.first == j.first)
                        {
                            continue;
                        }
                        cardGrid[j.first].mp_neighCellMap.erase(i.first);
                    }
                    cardGrid.erase(i.first);
                }
                if (slideDeltaCnt.find(i.first) != slideDeltaCnt.end())
                {
                    slideDeltaCnt[i.first] = slideDeltaCnt[i.first] - card;
                }
                else
                {
                    slideDeltaCnt[i.first] = card * (-1);
                }
            }

            // update full dim cell window count -slideout

            if (sub_Dim_Flag)
            {
                map<vector<int>, int> fullDimCellSlideOutCnt = fullDimCellSlidesCnt.front();
                fullDimCellSlidesCnt.pop_front();
                for (auto k : fullDimCellSlideOutCnt)
                {
                    int card = k.second;
                    fullDimCardGrid[k.first].card -= card;
                    fullDimCardGrid[k.first].mp_cardPerSlide.erase(slideOutID);

                    if (fullDimCardGrid[k.first].card < 1)
                        fullDimCardGrid.erase(k.first);
                }
            }
        }
    }

    void findOutlierMain(int itr)
    {
        if (min_RChanged)
        {
            initCellSize();
            reIndexCardGrid(itr);
        }
        if (max_RChanged or min_RChanged)
            this->reComputeNeighCellMap();

        list<Query> validQueryIDs;
        for (auto q : querySet)
        {
            if ((itr + 1) % (q.second.S / gcdS) == 0)
                validQueryIDs.push_back(q.second);
        }
        // TD:Sort
        // validQueryIDs.sort()
        // Algorithm 2: Group-wise coarse processing
        // Check boolean range for inlier and outlier cells
        for (auto cellIdx : cardGrid)
        {
            globalCell gCell = cellIdx.second;
            list<Query> ndQueryCands;
            ndQueryCands = validQueryIDs;
            vector<int> inlierCellQueryIDs;
            vector<int> outlierCellQueryIDs;
            list<Query> ndQueries;
            // check if ndQueryCands empty and then verify inliers/outliers
            while (!ndQueryCands.empty())
            {
                Query q = ndQueryCands.front();
                ndQueryCands.pop_front();
                // Inlier check
                if (find(gCell.vect_IndirectOutlierCellQueryIDs.begin(), gCell.vect_IndirectOutlierCellQueryIDs.end(), q.id) != gCell.vect_IndirectOutlierCellQueryIDs.end())
                {
                    outlierCellQueryIDs.push_back(q.id);
                    continue;
                }
                int firstSlideID = itr - q.W / gcdS + 1;
                int cardTotal = gCell.getCardTotal(firstSlideID);

                if (!sub_Dim_Flag and cardTotal > q.K)
                {
                    inlierCellQueryIDs.push_back(q.id);
                    list<Query>::iterator itr = ndQueryCands.begin();
                    while (itr != ndQueryCands.end())
                    {
                        Query q2 = *itr;
                        if (q2.W >= q.W and q2.R >= q.R and q2.K <= q.K)
                        {
                            ndQueryCands.erase(itr);
                        }
                    }
                }
                else
                {
                    int thredNeighCellCardTotal = 0;
                    for (auto neighCellIdx : gCell.getThredNeighCellsIn(q.R - min_R))
                    {
                        thredNeighCellCardTotal += cardGrid[neighCellIdx].getCardTotal(firstSlideID);
                        if (!sub_Dim_Flag and (thredNeighCellCardTotal + cardTotal) > q.K)
                        {
                            inlierCellQueryIDs.push_back(q.id);
                            list<Query>::iterator itr = ndQueryCands.begin();
                            while (itr != ndQueryCands.end())
                            {
                                Query q2 = *(itr);
                                if (q2.W >= q.W and q2.R >= q.R and q2.K <= q.K)
                                {
                                    ndQueries.erase(itr);
                                }
                            }
                        }
                        else
                        {
                            // Retrieve all the cards of neighbor cells
                            // if its not in sub dim, then add the cell card
                            int neighCellCardTotal = (sub_Dim_Flag ? 0 : cardTotal);
                            for (auto neighCellIdx : gCell.getThredNeighCellsOut(q.R + min_R))
                                neighCellCardTotal += cardGrid[neighCellIdx].getCardTotal(firstSlideID);

                            if (neighCellCardTotal <= q.K)
                            {
                                outlierCellQueryIDs.push_back(q.id);
                                list<Query>::iterator itrQuery = ndQueryCands.begin();
                                while (itrQuery != ndQueryCands.end())
                                {
                                    Query q2 = *(itrQuery);
                                    if (q2.W <= q.W and q2.R <= q.R and q2.K >= q.K)
                                    {
                                        ndQueryCands.erase(itrQuery);
                                        outlierCellQueryIDs.push_back(q2.id);
                                    }
                                }
                            }
                            else
                            {
                                ndQueries.push_back(q);
                            }
                        }
                    }
                }
            }
            for (auto qid : outlierCellQueryIDs)
            {
                // Push outlier tuple using outlier cell query ids
                int firstSlideID = itr - querySet[qid].W / gcdS + 1;
                int slideID = itr - slides.size();
                for (auto slide : slides)
                {
                    slideID++;
                    if (slideID < firstSlideID or !(slide.find(cellIdx.first) != slide.end()))
                        continue;
                    for (Tuple t : slide[cellIdx.first].set_tuples)
                    {
                        t.set_outlier_queryIds.insert(qid);
                        outliers.insert(t);
                    }
                }
            }
            gCell.vect_ndQueries = ndQueries;
        }

        // Algorithm 3: Group-wise fine processing for every cell
        // Checks is block is inlier or outlier

        for (auto cellIdx : cardGrid)
        {
            globalCell gcell = cellIdx.second;
            gcell.vect_IndirectOutlierCellQueryIDs.clear();
            int ndQueriesMinWin = INT_MAX;
            int ndQueriesMaxW = INT_MIN;
            int ndQueriesMaxK = INT_MIN;
            double ndQueriesMaxR = std::numeric_limits<double>::lowest();
            if (gcell.vect_ndQueries.empty())
            {
                continue;
            }
            else
            {
                for (Query q : gcell.vect_ndQueries)
                {
                    if (q.K > ndQueriesMaxK)
                        ndQueriesMaxK = q.K;
                    if (q.R > ndQueriesMaxR)
                        ndQueriesMaxR = q.R;
                    if (q.W < ndQueriesMinWin)
                        ndQueriesMinWin = q.W;
                    if (q.W > ndQueriesMaxW)
                        ndQueriesMaxW = q.W;
                }
            }

            set<Tuple> candOutlierTuple;
            int minWinfirstSlideID = itr - ndQueriesMinWin / gcdS + 1;
            int maxWinfirstSlideID = itr - ndQueriesMinWin / gcdS + 1;
            int slideID = itr - slides.size();
            for (map<vector<int>, Cell> slide : slides)
            {
                slideID++;
                if (slideID < maxWinfirstSlideID or !(slide.find(cellIdx.first) != slide.end()))
                    continue;

                if (sub_Dim_Flag)
                {
                    for (Tuple t : slide[cellIdx.first].set_tuples)
                    {
                        int numNeighInCellMinW = fullDimCardGrid[t.vect_fullDim_CellIdx].getCardTotal(minWinfirstSlideID) - 1;
                        if (numNeighInCellMinW < ndQueriesMaxK)
                            candOutlierTuple.insert(t);
                    }
                }
                else if (gcell.getCardTotal(minWinfirstSlideID) - 1 < (ndQueriesMaxK))
                {
                    for (auto tt : slide[cellIdx.first].set_tuples)
                    {
                        candOutlierTuple.insert(tt);
                    }
                }
            }
            for (auto tcand : candOutlierTuple)
            {
                list<Query> ndQueryTuple = gcell.vect_ndQueries;
                double ndQueriesMaxTuple = ndQueriesMaxR;

                while (ndQueryTuple.empty() == false)
                {
                    Query q = ndQueryTuple.front();
                    ndQueryTuple.pop_front();
                    int firstSlideID = itr - q.W / gcdS + 1;
                    if (tcand.silde_ID < firstSlideID)
                        continue;

                    int mn = (sub_Dim_Flag ? fullDimCardGrid[tcand.vect_fullDim_CellIdx].getCardTotal(firstSlideID) - 1 : gcell.getCardTotal(firstSlideID) - 1);
                    slideID = itr - slides.size();
                    for (map<vector<int>, Cell> slide : slides)
                    {
                        slideID++;
                        if (slideID < firstSlideID)
                            continue;
                        for (auto cellID : slide)
                        {
                            utils util;
                            if (gcell.mp_neighCellMap.find(cellID.first) == gcell.mp_neighCellMap.end() and gcell.mp_neighCellMap[cellID.first] < (min_R + ndQueriesMaxTuple) and util.isNeighborTupleCell(tcand.vect_value, slide[cellID.first].center, 0.5 * (min_R) + q.R))
                            {
                                for (Tuple tother : slide[cellID.first].set_tuples)
                                {
                                    if (sub_dim_Flag)
                                    {
                                        continue;
                                    }
                                    if (sub_Dim_Flag and tcand.vect_fullDim_CellIdx == tother.vect_fullDim_CellIdx)
                                        continue;
                                    if (util.distance(tcand, tother, q.R) <= q.R)
                                    {
                                        mn++;
                                        if (mn >= q.K)
                                        {
                                            list<Query>::iterator ndQueryTupleItr = ndQueryTuple.begin();
                                            ndQueriesMaxTuple = std::numeric_limits<double>::lowest();
                                            while (ndQueryTupleItr != ndQueryTuple.end())
                                            {
                                                Query q2 = *(ndQueryTupleItr);
                                                if (q2.W >= q.W and q2.R >= q.R and q2.K <= q.K)
                                                {
                                                    ndQueryTuple.erase(ndQueryTupleItr);
                                                }
                                                else if (q2.R > ndQueriesMaxTuple)
                                                {
                                                    ndQueriesMaxTuple = q2.R;
                                                }
                                            }
                                            continue;
                                        }
                                    }
                                }
                            }
                        }
                        if (mn < q.K)
                        {
                            tcand.set_outlier_queryIds.insert(q.id);
                            outliers.insert(tcand);

                            list<Query>::iterator ndQueryItr = ndQueryTuple.begin();
                            ndQueriesMaxTuple = std::numeric_limits<double>::lowest();
                            while (ndQueryItr != ndQueryTuple.end())
                            {
                                Query q2 = *(ndQueryItr);
                                if (q2.W <= q.W and q2.R <= q.R and q2.K >= q.K)
                                {
                                    ndQueryTuple.erase(ndQueryItr);
                                    tcand.set_outlier_queryIds.insert(q2.id);
                                }
                                else if (q2.R > ndQueriesMaxTuple)
                                {
                                    ndQueriesMaxTuple = q2.R;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void getNeighCellMap(set<vector<int>> newCellIdices)
    {
        utils u;
        for (vector<int> newCellIdx : newCellIdices)
        {
            globalCell newCell = cardGrid[newCellIdx];
            for (auto candCellIdx : cardGrid)
            {
                globalCell candCell = candCellIdx.second;
                double dist = u.getNeighborCellDist(newCellIdx, candCellIdx.first, min_R, min_R + max_R);
                if (dist < min_R + max_R and (sub_Dim_Flag or !(newCellIdx == candCellIdx.first)))
                {
                    newCell.mp_neighCellMap[candCellIdx.first] = dist;
                    if (newCellIdices.find(candCellIdx.first) == newCellIdices.end())
                    {
                        candCell.mp_neighCellMap[newCellIdx] = dist;
                    }
                }
            }
        }
    }

    void reComputeNeighCellMap()
    {
        utils u;
        for (auto cellIdx : cardGrid)
        {
            globalCell cell = cellIdx.second;
            cell.mp_neighCellMap.clear();
            for (auto otherCellIdx : cardGrid)
            {
                if (cell.mp_neighCellMap.find(otherCellIdx.first) != cell.mp_neighCellMap.end())
                {
                    continue;
                }
                double dist = u.getNeighborCellDist(cellIdx.first, otherCellIdx.first, min_R, min_R + max_R);
                if (dist < min_R + max_R and (sub_Dim_Flag or !(cellIdx.first == otherCellIdx.first)))
                {
                    cell.mp_neighCellMap[otherCellIdx.first] = dist;
                    cardGrid[otherCellIdx.first].mp_neighCellMap[cellIdx.first] = dist;
                }
            }
        }
    }
    void reIndexCardGrid(int itr)
    {
        list<map<vector<int>, Cell>> slidesNew;
        fullDimCellSlidesCnt.clear();
        cardGrid.clear();
        fullDimCardGrid.clear();
        int slideId = itr - slides.size();
        for (auto slide : slides)
        {
            slideId++;
            map<vector<int>, int> fullDimCellSlideInCnt;
            map<vector<int>, Cell> slideNew;

            for (auto c : slide)
            {
                for (auto t : c.second.set_tuples)
                {
                    vector<int> fullDimCellIdx;
                    vector<int> subDimCellIdx;
                    for (int j = 0; j < dim; j++)
                    {
                        int dimIdx = (int)((t.vect_value[j] - minValues[j] / dimLength[j]));
                        fullDimCellIdx.push_back(dimIdx);
                    }
                    if (sub_Dim_Flag)
                    {
                        int j = 0;
                        while (j < subdim)
                        {
                            int dimIdx = (int)((t.vect_value[j] - minValues[j] / subDimLength[j]));
                            subDimCellIdx.push_back(dimIdx);
                            j++;
                        }
                    }
                    else
                    {
                        subDimCellIdx = fullDimCellIdx;
                    }
                    t.vect_fullDim_CellIdx = fullDimCellIdx;
                    t.vect_subDim_CellIdx = subDimCellIdx;
                    if (slideNew.find(subDimCellIdx) == slideNew.end())
                    {
                        vector<double> cellCenter;
                        cellCenter.resize(subdim);
                        if (sub_Dim_Flag)
                        {
                            for (int j = 0; j < subdim; j++)
                            {
                                cellCenter[j] = minValues[j] = subDimCellIdx[j] * subDimLength[j] + subDimLength[j] / 2;
                            }
                        }
                        else
                        {
                            for (int j = 0; j < dim; j++)
                                cellCenter[j] = minValues[j] + fullDimCellIdx[j] * dimLength[j] + dimLength[j] / 2;
                        }
                        slideNew[subDimCellIdx] = *(new Cell(subDimCellIdx, cellCenter, sub_Dim_Flag));
                    }
                    if (sub_Dim_Flag)
                    {
                        // vector<double> temp;
                        // for (size_t i = 0; i < dim; i++)
                        // {
                        //     temp.push_back(dimLength[i]);
                        // }
                        slideNew[subDimCellIdx].addTupleSubDim(t, dimLength, minValues);
                        if (fullDimCellSlideInCnt.find(fullDimCellIdx) == fullDimCellSlideInCnt.end())
                            fullDimCellSlideInCnt[fullDimCellIdx] = 0;
                        fullDimCellSlideInCnt[fullDimCellIdx] = fullDimCellSlideInCnt[fullDimCellIdx] + 1;
                    }
                    else
                    {
                        slideNew[subDimCellIdx].addTuple(t);
                    }
                }
            }
            // update slideNew
            for (auto key : slideNew)
            {
                int card = key.second.getNumTuples();
                if (!(cardGrid.find(key.first) != cardGrid.end()))
                    cardGrid[key.first] = *(new globalCell(key.first));
                cardGrid[key.first].card += card;
                cardGrid[key.first].mp_cardPerSlide[slideId] = card;
            }
            // Update full Dim cell window count - slideIn
            if (sub_Dim_Flag)
            {
                for (auto key : fullDimCellSlideInCnt)
                {
                    int card = key.second;
                    if (!(fullDimCardGrid.find(key.first) != fullDimCardGrid.end()))
                    {
                        fullDimCardGrid[key.first] = *(new globalCell(key.first));
                    }
                    fullDimCardGrid[key.first].card += card;
                    fullDimCardGrid[key.first].mp_cardPerSlide[slideId] = card;
                }
            }
            slidesNew.push_back(slideNew);
            if (sub_Dim_Flag)
                fullDimCellSlidesCnt.push_back(fullDimCellSlideInCnt);
        }
        slides = slidesNew;
    }
};
