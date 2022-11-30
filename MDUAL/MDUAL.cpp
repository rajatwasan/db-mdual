#pragma once
#include <bits/stdc++.h>
using namespace std;
#include "..//MDUAL/Utils.cpp"
#include "..//MDUAL/Naive.cpp"
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
    map<int, set<int>> final_outliers;
    set<Tuple> outlier_;
    map<int, Query> querySet;

    bool sub_dim_Flag;
    // NAIVE *naive;
    MDUAL(int dim, int subDim, int nS, int gcdS, vector<double> minValues)
    {
        this->dim = dim;
        this->subdim = subDim;
        // cout << "subdim " << this->subdim << endl;
        this->sub_Dim_Flag = dim != subdim;
        this->min_R = std::numeric_limits<double>::max();
        this->max_R = std::numeric_limits<double>::min();
        this->gcdS = gcdS;
        this->nS = nS;
        this->minValues = minValues;
        this->sub_dim_Flag = true;
        // naive = new NAIVE(nS, gcdS);
    }
    set<Tuple> findOutlier(vector<Tuple> newSlideTuples, map<int, Query> newQuerySet, int itr)
    {
        this->querySet = newQuerySet;
        if (itr == 0)
        {
            this->updateBasisParams(itr);
            this->initCellSize();
        }
        this->clearPreviousOutliers();
        this->outlier_ = naive->findOutlier(newSlideTuples, newQuerySet, itr);
        this->updateWindow(newSlideTuples, itr);
        this->updateBasisParams(itr);
        this->findOutlierMain(itr);
        // cout << outliers.size() << endl;
        return this->outliers;
    }
    void initCellSize()
    {
        dimLength.clear();
        dimLength.resize(dim);
        for (int i = 0; i < dim; i++)
        {
            // cout << dim;
            dimLength[i] = sqrt(min_R * min_R / dim);
        }
        if (sub_Dim_Flag)
        {
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
        max_R = std::numeric_limits<double>::min();
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
        // cout << slides.size() << "Slides";
        if (sub_Dim_Flag)
            fullDimCellSlidesCnt.push_back(fullDimCellSlideInCnt);

        set<vector<int>> newCellIdices;
        // update slideIn
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

        // update full Dim cell window count - slideIn

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
        // cout << fullDimCellSlideInCnt.size() << " fullDimCellSlideInCnt" << endl;
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
        vector<Query> arr;

        for (auto q : querySet)
        {
            if ((itr + 1) % (q.second.S / gcdS) == 0)
            {
                // validQueryIDs.push_back(q.second);
                arr.push_back(q.second);
            }
        }
        // Sort based on R, W and K
        sort(arr.begin(), arr.end(), [&](Query q1, Query q2)
             {
            if(q1.R>q2.R) {
					return 1;
				}else if(q1.R==q2.R){
					if(q1.W>q2.W) return 1;
					else if(q1.W==q2.W){
						if(q1.K<=q2.K) return 1;
						else return -1;
					}else return -1;
				}else return -1; });
        for (auto val : arr)
        {
            validQueryIDs.push_back(val);
        }
        // cout << validQueryIDs.size() << "validQueryIDs" << endl;

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
            // cout << ndQueryCands.size() << "ndQueryCands" << endl;
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
                int first_Slide_Id = itr - q.W / gcdS + 1;
                int cardTotal = gCell.getCardTotal(first_Slide_Id);
                // cout << q.id << " : QID" << endl;
                cout << cardTotal << " : CardTotal" << endl;

                if (!sub_Dim_Flag and cardTotal > q.K)
                {
                    inlierCellQueryIDs.push_back(q.id);
                    list<Query>::iterator queryIterator = ndQueryCands.begin();
                    while (queryIterator != ndQueryCands.end())
                    {
                        Query q2 = *queryIterator;
                        if (q2.W >= q.W and q2.R >= q.R and q2.K <= q.K)
                        {
                            ndQueryCands.erase(queryIterator++);
                        }
                        queryIterator++;
                    }
                }
                else
                {
                    // cout << "else" << endl;
                    int thredNeighCellCardTotal = 0;
                    for (auto neighCellIdx : gCell.getThredNeighCellsIn(q.R - min_R))
                    {
                        thredNeighCellCardTotal += cardGrid[neighCellIdx].getCardTotal(first_Slide_Id);
                        // cout << "for neigh" << endl;
                        if (!sub_Dim_Flag and (thredNeighCellCardTotal + cardTotal) > q.K)
                        {
                            // cout << "for neigh if" << endl;
                            inlierCellQueryIDs.push_back(q.id);
                            list<Query>::iterator queryIterator = ndQueryCands.begin();
                            while (queryIterator != ndQueryCands.end())
                            {
                                Query q2 = *queryIterator;
                                if (q2.W >= q.W and q2.R >= q.R and q2.K <= q.K)
                                {
                                    ndQueries.erase(queryIterator);
                                }
                                queryIterator++;
                            }
                        }
                        else
                        {
                            // Retrieve all the cards of neighbor cells
                            // if its not in sub dim, then add the cell card
                            int neighCellCardTotal = (sub_Dim_Flag ? 0 : cardTotal);
                            for (auto neighCellIdx : gCell.getThredNeighCellsOut(q.R + min_R))
                                neighCellCardTotal += cardGrid[neighCellIdx].getCardTotal(first_Slide_Id);
                            // cout << "for else" << endl;
                            if (neighCellCardTotal <= q.K)
                            {
                                // cout << "<q.k>" << endl;
                                outlierCellQueryIDs.push_back(q.id);
                                list<Query>::iterator itrQuery = ndQueryCands.begin();
                                while (itrQuery != ndQueryCands.end())
                                {
                                    // cout << "while" << endl;
                                    Query q2 = *(itrQuery);
                                    if (q2.W <= q.W and q2.R <= q.R and q2.K >= q.K)
                                    {
                                        // cout << "if" << endl;
                                        ndQueryCands.erase(itrQuery);
                                        outlierCellQueryIDs.push_back(q2.id);
                                    }
                                    itrQuery++;
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
            // cout << outlierCellQueryIDs.size() << "CellQuery" << endl;
            for (auto qid : outlierCellQueryIDs)
            {
                // Push outlier tuple using outlier cell query ids
                int first_Slide_Id = itr - querySet[qid].W / gcdS + 1;
                int slideID = itr - slides.size();
                for (auto slide : slides)
                {
                    // cout << "GWFP" << endl;
                    slideID++;
                    if (slideID < first_Slide_Id or !(slide.find(cellIdx.first) != slide.end()))
                        continue;
                    for (Tuple t : slide[cellIdx.first].set_tuples)
                    {
                        t.set_outlier_queryIds.insert(qid);
                        // cout << "tcand" << t.id << endl;
                        outliers.insert(t);
                        // fout_outlier.push_back(t);
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
                    switch (expression)
                    {
                    case q.K > ndQueriesMaxK:
                        ndQueriesMaxK = q.K;
                    case (q.R > ndQueriesMaxR):
                        ndQueriesMaxR = q.R;
                    case (q.W < ndQueriesMinWin):
                        ndQueriesMinWin = q.W;
                    case (q.W > ndQueriesMaxW):
                        ndQueriesMaxW = q.W;
                    default:
                        break;
                    }
                }
            }
            int slideID = itr - slides.size();

            set<Tuple> candOutlierTuple;
            int minWinfirstSlideID = itr - ndQueriesMinWin / gcdS + 1;
            int maxWinfirstSlideID = itr - ndQueriesMinWin / gcdS + 1;
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
                    int first_Slide_Id = itr - q.W / gcdS + 1;
                    if (tcand.silde_ID < first_Slide_Id)
                        continue;

                    int mn = (sub_Dim_Flag ? fullDimCardGrid[tcand.vect_fullDim_CellIdx].getCardTotal(first_Slide_Id) - 1 : gcell.getCardTotal(first_Slide_Id) - 1);
                    slideID = itr - slides.size();
                    for (map<vector<int>, Cell> slide : slides)
                    {
                        slideID++;
                        if (slideID < first_Slide_Id)
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
                                                ndQueryTupleItr++;
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
                            // cout << "tcand" << tcand.id << endl;
                            outliers.insert(tcand);
                            // fout_outlier.push_back(tcand);
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
                                ndQueryItr++;
                            }
                        }
                    }
                }
            }
        }
        // set<Tuple>::iterator tupleIterator = outlier_.begin();
        // while (tupleIterator != outlier_.end())
        // {
        //     Query q2 = *tupleIterator;
        //     outliers.insert(tupleIterator);
        //     tupleIterator++;
        // }
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
                            int j = 0;
                            while (j < subdim)
                            {
                                cellCenter[j] = minValues[j] = subDimCellIdx[j] * subDimLength[j] + subDimLength[j] / 2;
                                j++;
                            }
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
