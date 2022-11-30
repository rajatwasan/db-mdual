#pragma once
#include <bits/stdc++.h>
using namespace std;
#include "../Loader/QueryLoader.cpp"
class NAIVE
{
public:
    map<int, Query> query_set;
    list<Tuple> total_tuple;
    set<Tuple> set_outliers;
    // list<Tuple> final_outliers;
    int num_s, gcd_s;

    NAIVE(int num_s, int gcd_s)
    {
        this->num_s = num_s;
        this->gcd_s = gcd_s;
    }

    set<Tuple> findOutlier(vector<Tuple> new_slide_tuples, map<int, Query> new_query_set, int itr)
    {
        if (set_outliers.size() > 0)
            set_outliers.clear();
        int f_SlideID = itr - num_s + 1;
        if (itr > num_s)
        {
            for (auto i : total_tuple)
            {
                list<Tuple>::iterator iteratorTuple = total_tuple.begin();
                while (iteratorTuple != total_tuple.end())
                {
                    Tuple t = *iteratorTuple;
                    if (t.silde_ID < f_SlideID)
                        total_tuple.erase(iteratorTuple);
                    iteratorTuple++;
                }
            }
        }
        // cout << new_slide_tuples.size() << "asa" << endl;
        for (auto i : new_slide_tuples)
        {
            total_tuple.push_back(i);
        }
        // cout << total_tuple.size() << "abs" << endl;
        this->query_set = new_query_set;
        for (auto cd_tuple : total_tuple)
        {
            cd_tuple.set_outlier_queryIds.clear();
            bool outlier_chk = false;
            for (auto q : query_set)
            {
                if ((itr + 1) % (q.second.S / gcd_s) > 0)
                    continue;
                f_SlideID = itr - q.second.W / gcd_s + 1;
                if (cd_tuple.silde_ID < f_SlideID)
                    continue;

                int mn = 0;
                for (auto other_tuple : total_tuple)
                {
                    if (other_tuple.silde_ID < f_SlideID)
                        continue;
                    double dist = distTuple(cd_tuple, other_tuple);
                    if (cd_tuple.id != other_tuple.id && dist <= q.second.R)
                    {
                        mn++;
                    }
                }
                if (mn < q.second.K)
                {
                    outlier_chk = true;
                    cd_tuple.set_outlier_queryIds.insert(q.second.id);
                }
            }
            if (outlier_chk)
            {
                // cout << "Adding" << cd_tuple.id << endl;
                set_outliers.insert(cd_tuple);
                // for (auto o : set_outliers)
                // {
                //     cout << o.id << " :Outlier" << endl;
                // }
                // final_outliers.push_back(cd_tuple);
            }
            // cout << this->final_outliers.size() << "before" << endl;
        }
        // cout << this->set_outliers.size() << "end" << endl;
        return set_outliers;
    }

    static double distTuple(Tuple t1, Tuple t2)
    {
        double dT = 0;

        int i = 0;
        while (i < t1.vect_value.size())
        {
            dT += pow((t1.vect_value[i] - t2.vect_value[i]), 2);
            i++;
        }
        return sqrt(dT);
    }
};