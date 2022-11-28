#pragma once
#include <bits/stdc++.h>
using namespace std;
#include "..//Loader/Tuple.cpp"
/**
 * Query data structure: 
 * where
 * W is Window
 * R is distance threshold of a query
 * K is neighbor count threshold
 * S slide size of a query
 */
class Query
{
public:
  set<Tuple> outliers;
  int id, nnToFind;
  int K, W, S;
  double R;

  Query() {}

  Query(int id, double R, int K, int W, int S)
  {
    this->id = id;
    this->R = R;
    this->K = K;
    this->W = W;
    this->S = S;
  }

  Query(int id, double R)
  {
    this->id = id;
    this->R = R;
  }
};
