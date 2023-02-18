//
//  utils.hpp
//  ReadFiles
//
//  Created by Venky Sundar on 2/10/23.
//

#ifndef utils_hpp
#define utils_hpp

#include <stdio.h>
#include <vector>
using namespace std;
int calculateFeatureOfImage(char* imageFileName,vector<float> &fVec);
int baseLineMatching(char* targetImage,char* imagesDir,int n);
double getSumOfSquareDistance(vector<float> &v1,vector<float> &v2);
int readImages(char* dirname,char* featureListCsv,int featureType);
#endif /* utils_hpp */
