#pragma once
#include <bits/stdc++.h>
using namespace std;
#include "../Loader/DataLoader.cpp"
/**
 * QueryLoader loads the query sets and create query data structre
 */
class QueryLoader
{
private:
    ifstream fin;
    string filePath;

public:
    int maxW, gcdS;

    double minR;

    QueryLoader() {}

    QueryLoader(string queryset)
    {
        filePath = "../querysets/" + queryset + ".csv";
        // cout << filePath;
        // ifstream fin ;
        fin.open(filePath);
        this->minR = INT32_MAX;
        this->maxW = INT_MIN;
        this->gcdS = INT_MAX;

        string str;
        int count = 0;
        while (getline(fin, str))
        {
            count++;
            // getline(fin, str);

            vector<string> rawValues;

            stringstream ss(str);
            string t;
            while (getline(ss, t, ','))
            {
                rawValues.push_back(t);
            }
            // for (auto i : rawValues)
            //     cout << i << " ";
            // cout << endl;
            double R = stod(rawValues[3]);
            int W = stoi(rawValues[5]);
            int S = stoi(rawValues[6]);
            if (this->gcdS > S)
                this->gcdS = S;
            if (this->maxW < W)
                this->maxW = W;
            if (this->minR > R)
                this->minR = R;
        }
        fin.close();
        // cout << count;
    }

    map<int, Query> getQuerySet(int curr_itr)
    {
        map<int, Query> querySet;

        fin.open(filePath);
        while (fin)
        {
            string str;
            getline(fin, str);
            stringstream x(str);
            string t;
            vector<string> arr;
            vector<int> vm(5, 0);
            while (getline(x, t, ','))
            {
                arr.push_back(t);
            }
            int id = stoi(arr[0]);
            int s_time = stoi(arr[1]);
            int e_time = stoi(arr[2]);

            if (s_time <= curr_itr and curr_itr < e_time)
            {
                double R = atof(arr[3].c_str());
                int K = stoi(arr[4]);
                int W = stoi(arr[5]);
                int S = stoi(arr[6]);

                Query query(id, R, K, W, S);
                querySet[id] = query;
            }
            else if (vm.size() == 0)
            {
                continue;
            }
        }
        return querySet;
    }

    map<int, Query> getQuerySetByQID(int fromQID, int numQueries)
    {
        map<int, Query> querySet;
        fin.open(filePath);
        string str;
        while (getline(fin, str))
        {
            // getline(fin, str);
            stringstream x(str);
            string t;
            vector<string> arr;
            while (getline(x, t, ','))
            {
                arr.push_back(t);
            }
            // for (auto i : arr)
            //     cout << i << " ";
            // cout << endl;
            // cout << arr;
            int id = stoi(arr[0]);

            if (id >= fromQID && id < (fromQID + numQueries))
            {
                int R = stod(arr[3]);
                int K = stoi(arr[4]);
                int W = stoi(arr[5]);
                int S = stoi(arr[6]);

                Query query(id, R, K, W, S);
                querySet[id] = query;
            }
        }
        fin.close();

        return querySet;
    }
};
