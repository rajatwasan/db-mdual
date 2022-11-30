#pragma once
#include <bits/stdc++.h>
using namespace std;
#include "../MDUAL/Cell.cpp"
/* Utils:
 * Find distance between tuple and check neighbour cells/points
 */
class utils
{
public:
  double distance(Tuple t1, Tuple t2)
  {
    double ssum = 0;
    int x = 0;
    while (x < t1.vect_value.size())
    {
      ssum += pow((t1.vect_value[x] - t2.vect_value[x]), 2);
      x++;
    }
    return sqrt(ssum);
  }
  static double distance(Tuple t1, Tuple t2, double threshold)
  {
    double ssum = 0;
    double ss_thred = pow(threshold, 2);
    for (int j = 0; j < t1.vect_value.size(); j++)
    {
      ssum += pow((t1.vect_value[j] - t2.vect_value[j]), 2);
      if (ssum > ss_thred)
        return std::numeric_limits<double>::max();
    }
    return sqrt(ssum);
  }

  static bool isNeighborTuple(Tuple t1, Tuple t2, double threshold)
  {
    double ssum = 0;
    threshold *= threshold;
    int j = 0;
    while (j < t1.vect_value.size())
    {
      ssum += pow(t1.vect_value[j] - t2.vect_value[j], 2);
      if (ssum > threshold)
        return false;
      j++;
    }

    return true;
  }

  static bool isNeighborTupleCell(vector<double> v1, vector<double> v2, double threshold)
  {
    double ssum = 0;
    threshold *= threshold;
    int i = 0;
    while (i < v2.size())
    {
      ssum += pow((v1[i] - v2[i]), 2);
      if (ssum > threshold)
        return false;
      i++;
    }
    return true;
  }

  static double getNeighborCellDist(vector<int> c1, vector<int> c2, double minR, double threshold)
  {
    double ssum = 0;
    int i = 0;
    while (i < c1.size())
    {
      ssum += pow(c1[i] - c2[i], 2);
      if (ssum / (c1.size() * minR * minR) >= threshold * threshold)
        return std::numeric_limits<double>::max();
      i++;
    }
    return sqrt(ssum / c1.size()) * minR;
  }

  static bool isNeighborCell(vector<int> c1, vector<int> c2, double minR, double threshold)
  {
    double ssum = 0;
    int i = 0;
    while (i < c1.size())
    {
      ssum += pow(c1[i] - c2[i], 2);
      if (ssum / c1.size() * minR * minR >= threshold * threshold)
        return false;
      i++;
    }
    return true;
  }
};