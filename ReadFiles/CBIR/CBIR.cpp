//
//  CBIR.cpp
//  CBIR Class
//
//  Created by Venky Sundar on 2/17/23.
//

#include "CBIR.hpp"
#include <opencv2/opencv.hpp>
#include <dirent.h>
#include <vector>
#include "../HistogramMatching/HistogramMatching.hpp"
#include "../TextureAnalysis/TextureAnalysis.hpp"
#include "../BaselineMatching/BaselineMatching.hpp"
/**A vector of BBallMatchingStruct is used to compare distance metrices for Content based image retrieval. **/
struct BBallMatchingStruct
{
    double distance;
    std::string targetImageName;
    std::string imageName;
    
    BBallMatchingStruct(std::string target,std::string image, double d)  {
        this->distance = d;
        this->targetImageName =target;
        this->imageName = image;
    }
    
    
    bool operator < (const BBallMatchingStruct& img) const
    {
        return (distance < img.distance);
    }
};
/**Retursn distance for CBIR between target and image2. Distance = 0.3*histogramIntersection+0.6*textureDistance+0.4*baselineMatchingDistance**/
double getBBallDistanceForImage(vector<float> targetHist ,vector<float> targetTextureVec,vector<float> targetBaselineMatchingFeatures, char* image2){
  
   
    vector<float> img2Histogram = generateNormalisedHistogramVec(image2);
    double histogramIntersection = gethistogramIntersectionDistance(targetHist, img2Histogram);
    //printf("Histogram distance : %f\n",histogramIntersection);
    
    std::vector<float> img2TextureVec;
    getTextureVector(image2, img2TextureVec);
    double textureDistance = gethistogramIntersectionDistance(targetTextureVec,img2TextureVec);
    vector<float> imageBaselineMatchingVec;
    calculateFeatureOfImage(image2, imageBaselineMatchingVec);
    
    double baselineMatchingDistance = getSumOfSquareDistance(targetBaselineMatchingFeatures, imageBaselineMatchingVec);
    return (0.3*histogramIntersection+0.6*textureDistance+0.4*baselineMatchingDistance);
}

/**Main function which uses Baseline Matching, histogram intersection and Texture analysis to perform Content based Image retrieval.**/
vector<string> getBBallDistances(char* target,char* dirname,int n){
    
    char imageFileNameBuffer[256];
    
    DIR *dirp;
    struct dirent *dp;
    vector<string> result;
    
    // open the directory
    
    dirp = opendir( dirname );
    if( dirp == NULL) {
        printf("Cannot open directory %s\n", dirname);
        exit(-1);
    }
    
    // loop over all the files in the image file listing
    vector<BBallMatchingStruct> bballVector;
    vector<float> targetHist = generateNormalisedHistogramVec(target) ;
    vector<float> targetTextureVec;
    getTextureVector(target, targetTextureVec);
    vector<float> targetBaselineMatchingFeatures;
    calculateFeatureOfImage(target, targetBaselineMatchingFeatures);
    while( (dp = readdir(dirp)) != NULL ) {
        
        // check if the file is an image
        if( strstr(dp->d_name, ".jpg") ||
           strstr(dp->d_name, ".png") ||
           strstr(dp->d_name, ".ppm") ||
           strstr(dp->d_name, ".tif") ) {
            
            //printf("processing image file: %s\n", dp->d_name);
            
            // build the overall filename
            strcpy(imageFileNameBuffer, dirname);
            //strcat(imageFileNameBuffer, "/");
            strcat(imageFileNameBuffer, dp->d_name);
            
            //printf("full path name: %s\n", imageFileNameBuffer);
            vector<float> vec;
            vector<vector<float>> vecOfVec;
            
            double distance = getBBallDistanceForImage( targetHist , targetTextureVec, targetBaselineMatchingFeatures,imageFileNameBuffer);
            bballVector.push_back(BBallMatchingStruct(target,imageFileNameBuffer,distance));
            
            
        }
    }
    std::sort(bballVector.begin(),bballVector.end());
   
    printf("Printing first %d closest images for CBIR.\n",n);
    int i=1;
    for(std::vector<BBallMatchingStruct>::iterator  it = bballVector.begin(); it != bballVector.end(); it++) {
        cout << it->imageName<<endl;
        i++;
        result.push_back(it->imageName);
        if(i>n)
            break;
    }
    
    //printf("Terminating\n");
    return result;
    
    
}
