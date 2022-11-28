#pragma once
#include <bits/stdc++.h>
#include <unistd.h>
#include "../Simulator/Thread.cpp"

#include "../MDUAL/MDUAL.cpp"
#include "../Loader/Dataloader.cpp"
#include "../Loader/QueryLoader.cpp"
#include "../Simulator/MemoryThread.cpp"
#include "../Simulator/Measure.cpp"

using namespace std;

class Simulator
{
public:
	Dataloader dLoader;
	QueryLoader qLoader;
	double allTimeSum;
	double allMemSum;
	MemoryThread memThread;
	string dataset;
	string queryset;
	int maxW;
	int gcdS;
	int nS;

	Simulator(string dataset, string queryset)
	{
		Dataloader dLoader(dataset);
		QueryLoader qLoader(queryset);
		this->dataset = dataset;
		this->queryset = queryset;
		this->gcdS = qLoader.gcdS;
		this->nS = qLoader.maxW / qLoader.gcdS;
		// memThread = new MemoryThread();
		MemoryThread memThread();
	}

	void run(int nW, int numQueries, double changedQueryRatio)
	{
		MDUAL *mdual = new MDUAL(dLoader.dim, dLoader.subdim, nS, gcdS, dLoader.getMinValues());

		memThread.start();
		memThread.join();

		int numWindows = 0;
		int numChangedQueries = (int)(numQueries * changedQueryRatio);
		int totalNoOfOutliers = 0;
		int totalOutQueries = 0;

		int itr = 0;
		while (itr++ < (nW + nS - 1))
		{
			map<int, Query> updatedQuerySet = qLoader.getQuerySetByQID(itr * numChangedQueries, numQueries);

			vector<Tuple> updatedSlideTuples = dLoader.getNewSlideTuples(itr, gcdS);

			set<Tuple> outliers;

			Measure obj;
			if (updatedQuerySet.empty())
				break;
			if (updatedSlideTuples.empty())
				break;
			long startTime = obj.getCPUTime();

			outliers = mdual->findOutlier(updatedSlideTuples, updatedQuerySet, itr);

			long closeTime = obj.getCPUTime();
			long memoryUsed = obj.getMemory();
			// Calculate total memory and CPU used
			if (itr >= nS - 1)
			{
				totalNoOfOutliers += outliers.size();
				cout << "Window " << (itr - nS + 1) << ", iteration " << itr << "\t"
					 << "No of queries: " << updatedQuerySet.size() << "\t"
					 << "No of outliers: " << outliers.size();
				for (auto t : outliers)
					totalOutQueries += t.set_outlier_queryIds.size();
				// Total CPU time in milliseconds
				allTimeSum += (closeTime - startTime) / (double)1000000;
				allMemSum += memoryUsed;
				numWindows++;
			}
			else
			{
				cout << "Initial window(slide " << itr << ")";
			}
		}

		printf("%-10s %10s %10.1f %10.2f %10.1f %10.1f %10d %10d", dataset, queryset, changedQueryRatio, allTimeSum / numWindows, allMemSum / numWindows, memThread.maxMemory, totalNoOfOutliers / numWindows, totalOutQueries / numWindows);

		memThread.detach();
	}
};
