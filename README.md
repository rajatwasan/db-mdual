# Implementation of Multiple Dynamic Outlier-Detection from a Data Stream by Exploiting Duality of Data and Queries

## 1. Configuration

This code was written on CPP with GCC Compiler verion 8.3.0 on windows platform. It is recommended to use windows for some library functions used in the code. Most of the code works on Linux and Mac platform as well.

## 2. Implementation

To detect outliers in Code Run Outliers.cpp 

    cpp test/Outlier.cpp

Above class runs the MDUAL on window slides and return outliers in each iteration.
Example Output:
![Output 1](output/Fig1.png?raw=true "Outlier")

To check CPU and Memory used run Load.cpp with these commands:

    cpp test/Load.cpp

Load will run and return the CPU time and average memory used by algorithm along with changed Q ratio and dataset info.

Example Output:
![Output 2](output/Fig2.png?raw=true "Outlier")

## Datasets
Code includes the three datasets and respective querysets in the codebase. To test these datasets or querysets, change the name in Outliers.cpp or Load.cpp file and compile and run. 

|Dataset|Dimensions|Data Points|
| STK   |   1      |    1.05M  |
| TAO   |   3      |    0.58M  |
| FC    |   55     |    1M     |

# Evaluations

We used the pMCSKY and SOP algorithms for comparisons on Stock dataset(STK.csv) as used in the paper and the code for pMCSKY and SOP is available publicly at: https://github.com/tatoliop/parallel-streaming-outlier-detection 
