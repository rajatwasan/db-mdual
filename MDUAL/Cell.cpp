#pragma once
#include <bits/stdc++.h>

#include "../MDUAL/GlobalCell.cpp"
using namespace std;
/**
 * Cell data structure for block groups
*/
class Cell {
    public:
      vector<int>  vect_cellIdx;
      map<vector<int>  , Cell> mp_childCells;
      set<Tuple> set_tuples;
      vector<double> center;
      Cell(){}

      Cell(vector<int> cell_idx , vector<double> dim_length , vector<double> min_values){
         this->vect_cellIdx = cell_idx;
         for(int i=0;i<dim_length.size();i++){
             this->center.push_back(((min_values[i] + (vect_cellIdx[i]*dim_length[i]) + dim_length[i])/2));
         }
      }
      Cell(vector<int> cellIdx , vector<double> cellCenter){
         this->vect_cellIdx = cellIdx;
         this->center = cellCenter;
      }
      Cell(vector<int> cell_idx){
        this->vect_cellIdx = cell_idx;
      }
     
      Cell(vector<int> cellIdx ,vector<double> cellCenter , bool subDimFlag){
         this->vect_cellIdx = cellIdx;
         this->center = cellCenter;
      }
    int getNumTuples(){
       return this->set_tuples.size();
    }
      
    void addTupleSubDim(Tuple t, vector<double> dim_length , vector<double> minValues){
        this->set_tuples.insert(t);
        if(this->mp_childCells.find(t.vect_fullDim_CellIdx) == this->mp_childCells.end() ){
            Cell* obj = new Cell(t.vect_fullDim_CellIdx , dim_length , minValues);
            this->mp_childCells[t.vect_fullDim_CellIdx] = *obj;
        }
      else{
        for(auto i:dim_length){
          if(i>1500){
            i=(i/10)*10;
          }
          else continue;
        }
      }
        this->mp_childCells[t.vect_fullDim_CellIdx].addTuple(t);
    }

    void removetuple(Tuple t){
      this->set_tuples.clear();
    }

    void addTuple(Tuple t){
        this->set_tuples.insert(t);
    }
};

