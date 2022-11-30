#include <bits/stdc++.h>
using namespace std;
#include "../MDUAL/MDUAL.cpp"
#include "../Loader/DataLoader.cpp"
#include "../Loader/Query.cpp"
#include "../Loader/QueryLoader.cpp"
#include "../Loader/Tuple.cpp"
/**
 * Outlier Detection: This file return the outliers detected by MDUAL algorithm
 */
int main(int argc, char const *argv[])
{
    string dataset = "STK";
    string queryset = dataset + "_Q10";
    int nW = 50;
    Dataloader *dLoader = new Dataloader(dataset);
    QueryLoader *qLoader = new QueryLoader(queryset);
    int nS = qLoader->maxW / qLoader->gcdS;
    int gcdS = qLoader->gcdS;
    MDUAL *mdual = new MDUAL(dLoader->dim, dLoader->subdim, nS, gcdS, dLoader->getMinValues());
    for (int itr = 0; itr < (nW + nS - 1); itr++){
        map<int, Query> newQuerySet = qLoader->getQuerySetByQID(itr * 5, 10);
        if (newQuerySet.empty())
            break;
        vector<Tuple> newSlideTuples_NETSPlus = dLoader->getNewSlideTuples(itr, qLoader->gcdS);
        if (newSlideTuples_NETSPlus.empty())
            break;
        set<Tuple> outliers_MDUAL = mdual->findOutlier(newSlideTuples_NETSPlus, newQuerySet, itr);
        cout << "Iteration: " << itr << "-Total Outliers: " << outliers_MDUAL.size() << endl;
    }
    return 0;
}
