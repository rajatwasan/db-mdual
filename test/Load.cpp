#include <bits/stdc++.h>
#include "../Simulator/QueryGenerator.cpp"
#include "../Simulator/Simulator.cpp"

using namespace std;
/**
 * This will load the dataset and calculate memoery used and CPU time of the algorithm
 * Simulator is written for windows platform. Use Windows system for running this file
*/
int main()
{
	string data_set = "STK";
	int Queries_count = 10;
	int default_w = 1000;
	int gcdS = 50;
	int default_k = 5;
	int num_w = 10;
	int same_num = 5;
	int diff_times = 10;
	double QRatio_diff = 0.2;
	vector<string> vary_prm{"R", "K", "S", "W"};

	QueryGenerator query_gen(data_set, default_w, gcdS, default_k, diff_times);

	printf("%-10s %10s %10s %10s %10s %10s %10s %10s", "Dataset", "Queryset", "ChangedQueryRatio", "AverageTime", "AverageMemory", "MaxMemory", "NoOfOutliers", "NoOfOutQuery");

	int i = 0;
	while (i < same_num)
	{
		string query_set = query_gen.generate(Queries_count, num_w, vary_prm);
		Simulator sim(data_set, query_set);
		sim.run(num_w, Queries_count, QRatio_diff);
		i++;
	}
	cout << endl;
	return 0;
}
