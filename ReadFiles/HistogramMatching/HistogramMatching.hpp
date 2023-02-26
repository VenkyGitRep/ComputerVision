//
//  HistogramMatching.hpp
//  Header file for Histogram Matching
//
//  Created by Venky Sundar on 2/12/23.
//

#ifndef HistogramMatching_hpp
#define HistogramMatching_hpp

#include <stdio.h>
#include <vector>
using namespace std;
vector<float> generateNormalisedHistogramVec(char* target);
vector<string>  histogramMatching(char* targetImage,char* imagesDir,int n,bool readfromCsv=true);
std::vector<std::vector<float>> generateNormalisedHistogramVecParts(char* target,int rowDiv=2, int colDiv=1);
double gethistogramIntersectionDistance(vector<float> targetFeatures,vector<float> data);
vector<string> HistogramMatchingInParts(char* targetImage, char* imageDir, int n);
#endif /* HistogramMatching_hpp */
