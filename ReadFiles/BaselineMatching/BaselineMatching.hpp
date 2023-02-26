//
//  BaselineMatching.hpp
//  Header file for Baseline Matching
//
//  Created by Venky Sundar on 2/18/23.
//

#ifndef BaselineMatching_hpp
#define BaselineMatching_hpp

#include <stdio.h>

#include <vector>
using namespace std;
int calculateFeatureOfImage(char* imageFileName,vector<float> &fVec);
vector<string> baseLineMatching(char* targetImage,char* imagesDir,int n,bool readFromCsv=false);
double getSumOfSquareDistance(vector<float> &v1,vector<float> &v2);


#endif /* BaselineMatching_hpp */
