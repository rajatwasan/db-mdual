#pragma once
#include <bits/stdc++.h>
#include "../Loader/Query.cpp"
class QueryGenerator
{
public:
    int numQueries;
    int defaultW;
    int gcdS;
    int defaultK;
    int defaultR;
    int nW;
    int nItr;
    int variationTimes;
    string dataset;
    int main(int argc, char const *argv[])
    {
        defaultR = 0.5;
        numQueries = 100;
        defaultW = 1000;
        gcdS = 500;
        defaultK = 50;
        nW = 10;
        nItr = defaultW / gcdS + nW;
        variationTimes = 5;
        vector<string> s{"R", "K", "S", "W"};
        string queryset = generate(numQueries, nW, s);
        ifstream fin;
        string filePath = "querysets/" + queryset + ".csv";
        fin.open(filePath);
        string k;
        while (fin)
        {
            getline(fin, k);
            cout << k << endl;
        }
        return 0;
    }
    QueryGenerator(string dataset, int defaultW, int gcdS, int defaultK, int variationTimes)
    {
        this->dataset = dataset;
        this->defaultW = defaultW;
        this->gcdS = gcdS;
        this->defaultK = defaultK;
        this->variationTimes = variationTimes;

        if (dataset == "STK")
        {
            this->defaultR = 0.5;
        }
        else if (dataset == "TAO")
        {
            this->defaultR = 1.5;
        }
        else if (dataset == "HPC")
        {
            this->defaultR = 10;
        }
        else if (dataset == "GAS")
        {
            this->defaultR = 1.5;
        }
        else if (dataset == "EM")
        {
            this->defaultR = 115;
        }
        else if (dataset == "FC")
        {
            this->defaultR = 525;
        }
    }
    string generate(int numQueries, int nw, vector<string> &varyingparams)
    {
        string queryset = dataset + "_Q" + to_string(numQueries);
        string filePath = "querysets/" + queryset + ".csv";
        ofstream myfile;
        myfile.open(filePath);
        nW = nw;
        nItr = defaultW * variationTimes / gcdS + nW;
        double R = defaultR;
        int K = defaultR;
        int S = gcdS;
        int W = defaultW;

        string defaultQuery = "0,0," + to_string(nItr) + "," + to_string(defaultR) + "," + to_string(defaultK) + "," + to_string(defaultW) + "," + to_string(gcdS);
        myfile << defaultQuery << endl;
        for (int i = 1; i < numQueries * nItr; i++)
        {
            int startTime = 0;
            int endTime = nItr;
            for (auto param : varyingparams)
            {
                char t = param[0];
                switch (t)
                {
                case 'R':
                    R = ceil((1 + rand() * (variationTimes - 1)) * defaultR * 100) / ((double)100);
                    break;
                case 'K':
                    K = defaultK * (int)(1 + rand() * variationTimes);
                    break;
                case 'S':
                    S = gcdS * (int)(1 + rand() * variationTimes);
                    break;
                case 'W':
                    W = S + S * (int)(rand() * ((defaultW * variationTimes) / S));
                    break;
                default:
                    break;
                }
            }
            string query = to_string(i) + "," + "+startTime+" + "," + to_string(R) + "," + to_string(K) + "," + to_string(W) + "," + to_string(S);
            myfile << query << endl;
        }
        myfile.close();
        return queryset;
    }

    Query generateOne(int qID, vector<string> &varyingParams)
    {
        double R = defaultR;
        int K = defaultK;
        int S = gcdS;
        int W = defaultW;
        for (string param : varyingParams)
        {
            switch (param[0])
            {
            case 'R':
                R = ceil((1 + rand()) * defaultR * 100) / ((double)100);
                break;
            case 'K':
                K = (int)(1 + rand() * defaultK * 2);
                break;
            case 'S':
                S = gcdS * (int)(1 + rand() * 4);
                break;
            case 'W':
                W = S + S * (int)(rand() * (defaultW / S));
                break;
            default:
                break;
            }
        }
        Query *q = new Query(qID, R, K, W, S);
        return *q;
    }
};
