#pragma once
// #ifndef TUPLE_HPP
// #define TUPLE_HPP
#include <bits/stdc++.h>
using namespace std;
/**
 * Tuple represent the data strucutre for dataset loaded
 */
class Tuple
{
public:
  int id, silde_ID, mc, nn;
  vector<double> vect_value;
  vector<int> vect_subDim_CellIdx, vect_fullDim_CellIdx;
  // int nn;
  set<int> set_outlier_queryIds;
  map<Tuple, double> mp_LSky;
  vector<int> vect_layer_count;
  map<int, bool> mp_safeDue;

  // int mc;

  Tuple(int id, int silde_ID, vector<double> vect_value)
  {
    this->id = id;
    this->silde_ID = silde_ID;
    this->vect_value = vect_value;
    this->mc = -1;
  }

  // bool equals(Tuple obj)
  // {
  //   return obj.id == this->id;
  // }
  bool operator<(const Tuple &rhs) const
  {
    return rhs.id == this->id;
  }
};
//endif