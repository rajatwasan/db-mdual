#pragma once
#include <bits/stdc++.h>
using namespace std;
#include "..//Loader/Query.cpp"
/**
 * Dataloader loads the datasets and creates tuples for data stream
 */
class Dataloader
{
private:
    vector<double> vect_min_Values, vect_max_Values;
    string str_filePath;
    vector<int> vect_priorityList;
    ifstream reader;

public:
    int dim, subdim;
    Dataloader() {}

    Dataloader(string dataset)
    {
        str_filePath = "../datasets/" + dataset + ".csv";
        ifstream fin;
        fin.open(str_filePath);
        string line;
        getline(fin, line);
        int count_dim = 0;
        stringstream str_strm(line);
        string tmp;
        while (getline(str_strm, tmp, ','))
        {
            count_dim += 1;
        }
        dim = count_dim;
        // cout << "dim" << endl;
        // cout << dim << endl;
        if (dim > 15)
            subdim = 3;
        else
            subdim = count_dim;
        // cout << subdim << endl;
        vect_min_Values.resize(dim);
        vect_max_Values.resize(dim);
        int i1 = 0;
        while (i1 < dim)
        {
            vect_min_Values[i1] = std::numeric_limits<double>::max();
            vect_max_Values[i1] = std::numeric_limits<double>::lowest();
            vect_priorityList.push_back(i1);
            i1++;
        }
        fin.open(str_filePath);
        while (fin)
        {
            getline(fin, line);
            stringstream ss(line);
            string t;
            int i = 0;
            while (getline(ss, t, ','))
            {
                double value = atof(t.c_str());
                if (vect_min_Values[i] > value)
                    vect_min_Values[i] = value;
                if (vect_max_Values[i] < value)
                    vect_max_Values[i] = value;
                i += 1;
            }
        }
    }
    vector<Tuple> getNewSlideTuples(int itr, int S)
    {
        vector<Tuple> newSlide;
        ifstream fin;
        fin.open(str_filePath);
        int tid = 0;
        int tx = 3;
        string line;
        while (fin)
        {
            getline(fin, line);
            if (tid >= (itr * S))
            {
                vector<double> value;
                stringstream ss(line);
                string t;
                vector<string> rawValues;
                while (getline(ss, t, ','))
                {
                    rawValues.push_back(t);
                }
                for (int i = 0; i < dim; i++)
                {
                    value.push_back(atof(rawValues[vect_priorityList[i]].c_str()));
                }
                Tuple *tuple = new Tuple(tid, itr, value);
                newSlide.push_back(*tuple);
            }

            else if (!(tid > tx))
            {
                tx = tid;
            }
            tid += 1;
            if (tid == (itr + 1) * S)
            {
                break;
            }
        }
        return newSlide;
    }

    vector<double> getMaxValues()
    {
        vector<double> arr;
        for (int i = 0; i < dim; i++)
        {
            arr.push_back(vect_max_Values[i]);
        }
        return arr;
    }

    vector<double> getMinValues()
    {
        vector<double> arr;
        for (int i = 0; i < dim; i++)
        {
            arr.push_back(vect_min_Values[i]);
        }
        return arr;
    }
};
