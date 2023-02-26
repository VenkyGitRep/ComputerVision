//
//  Containts helper methods to perform Baseline Matching.
//
//  Created by Venky Sundar on 2/10/23.
//



#include <opencv2/opencv.hpp>
#include "csv_util.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <dirent.h>
#include <vector>
#include "utils.hpp"
#include "HistogramMatching/HistogramMatching.hpp"
#include "BaselineMatching/BaselineMatching.hpp"
using namespace std;
using namespace std;
/**
 Reads images from Image repository, and created feature vector depending on opearation type.
 */
int readImages(char* dirname,char* featureListCsv,int featureType) {
    char imageFileNameBuffer[256];
    DIR *dirp;
    struct dirent *dp;
    // open the directory
    dirp = opendir( dirname );
    if( dirp == NULL) {
        printf("Cannot open directory %s\n", dirname);
        exit(-1);
    }
    clearContentsOfFile(featureListCsv);
    // loop over all the files in the image file listing
    while( (dp = readdir(dirp)) != NULL ) {
        
        // check if the file is an image
        if( strstr(dp->d_name, ".jpg") ||
           strstr(dp->d_name, ".png") ||
           strstr(dp->d_name, ".ppm") ||
           strstr(dp->d_name, ".tif") ) {
            
            // build the overall filename
            strcpy(imageFileNameBuffer, dirname);
            //strcat(imageFileNameBuffer, "/");
            strcat(imageFileNameBuffer, dp->d_name);
            
            //printf("full path name: %s\n", imageFileNameBuffer);
            vector<float> vec;
            vector<vector<float>> vecOfVec;
            //BaselineMatching
            if(featureType==1){
                calculateFeatureOfImage(imageFileNameBuffer,vec);
                
            }
            //Histogram matching
            else if(featureType==2){
                vec = generateNormalisedHistogramVec(imageFileNameBuffer);
                
            }
            //Histogram matching in parts
            else if(featureType==3){
                vecOfVec = generateNormalisedHistogramVecParts(imageFileNameBuffer);
                
            }
            append_image_data_csv(featureListCsv, imageFileNameBuffer, vec);
            
        }
    }
    return(0);
}
