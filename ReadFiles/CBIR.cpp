//
//  CBIR.cpp
//  ReadFiles
//
//  Created by Venky Sundar on 2/17/23.
//

#include "CBIR.hpp"
#include <opencv2/opencv.hpp>
#include "HistogramMatching.hpp"
#include "utils.hpp"

#include "csv_util.h"
#include "FeatureCompareUtils.hpp"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <dirent.h>
#include <vector>
#include "csv_util.h"
#include <opencv2/opencv.hpp>
#include "utils.hpp"
#include "TextureAnalysis.hpp"
#include <opencv2/opencv.hpp>

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

int rgChromaticity(char* image,vector<float> &rgNormalisedVec)
{
    // Load the image
    cv::Mat img = cv::imread(image);
    
    // Convert the image to the RG chromaticity space
    // Convert image to RG chromaticity space
    cv::Mat rgImage(img.size(), CV_32FC2);
    cv::Mat bgrChannels[3];
    cv::split(img, bgrChannels);
    cv::Mat bChannel = bgrChannels[0];
    cv::Mat gChannel = bgrChannels[1];
    cv::Mat rChannel = bgrChannels[2];
    cv::Mat sumChannel = rChannel + gChannel + bChannel + 1e-10; // Add small value to avoid division by zero
    cv::Mat rChannelNorm = rChannel / sumChannel;
    cv::Mat gChannelNorm = gChannel / sumChannel;
    cv::Mat rgChannels[] = { rChannelNorm, gChannelNorm };
    cv::merge(rgChannels, 2, rgImage);
    
    // Define the histogram parameters
    int histSize[] = { 16, 16 };
    float rgRanges[] = { 0, 1, 0, 1 };
    const float* ranges[] = { rgRanges, rgRanges };
    int channels[] = { 0, 1 };
    
    // Calculate the histogram
    cv::Mat hist;
    cv::calcHist(&rgImage, 1, channels, cv::noArray(), hist, 2, histSize, ranges, true, false);
    
    // Normalize the histogram
    cv::normalize(hist, hist, 0, 255, cv::NORM_MINMAX);
    
    // Display the histogram
    cv::namedWindow("Histogram", cv::WINDOW_NORMAL);
    cv::imshow("Histogram", hist);
    cv::waitKey(0);
    // Display the histogram
    cv::imshow("RG Chromaticity Histogram", hist);
    for(int i=0;i<hist.rows;i++){
        
        for(int j=0;j<hist.cols;j++){
            for(int c=0;c<hist.channels();c++){
                
                rgNormalisedVec.push_back(hist.at<float>(i,j,c));
            }
        }
    }
    
    return 0;
}
void getrgChromaticity(char* image1){
    vector<float> img1RGVector ;
    rgChromaticity(image1,img1RGVector);
   
}
double baseLineMatching(char* targetImage,char* image){
    vector<float> targetFeatures,imgFeatures;
    calculateFeatureOfImage(targetImage, targetFeatures);
    calculateFeatureOfImage(image, imgFeatures);
    
    return getSumOfSquareDistance(targetFeatures, imgFeatures);
    
}
double getBBallDistance(char* image1, char* image2){
    //Hist Features
    cv::Mat imgSrc2;
    imgSrc2=cv::imread(image2);
    vector<float> img1Histogram = generateNormalisedHistogramVec(image1);
    vector<float> img2Histogram = generateNormalisedHistogramVec(image2);
    double histogramIntersection = gethistogramIntersectionDistance(img1Histogram, img2Histogram);
    printf("Histogram distance : %f\n",histogramIntersection);
    
    
    
    
    std::vector<float> img1TextureVec,img2TextureVec;
    getTextureVector(image1, img1TextureVec);
    getTextureVector(image2, img2TextureVec);
    double textureDistance = gethistogramIntersectionDistance(img1TextureVec,img2TextureVec);
    double baselineMatchingDistance = baseLineMatching(image1,image2);
    return (0.3*histogramIntersection+0.6*textureDistance+0.4*baseLineMatching(image1,image2));
}

double getBBallDistanceForImage(vector<float> targetHist ,vector<float> targetTextureVec,vector<float> targetBaselineMatchingFeatures, char* image2){
    //Hist Features
   
   
    vector<float> img2Histogram = generateNormalisedHistogramVec(image2);
    double histogramIntersection = gethistogramIntersectionDistance(targetHist, img2Histogram);
    printf("Histogram distance : %f\n",histogramIntersection);
    
    
    
    
    std::vector<float> img2TextureVec;
    getTextureVector(image2, img2TextureVec);
    double textureDistance = gethistogramIntersectionDistance(targetTextureVec,img2TextureVec);
    vector<float> imageBaselineMatchingVec;
    calculateFeatureOfImage(image2, imageBaselineMatchingVec);
    
    double baselineMatchingDistance = getSumOfSquareDistance(targetBaselineMatchingFeatures, imageBaselineMatchingVec);
    return (0.3*histogramIntersection+0.6*textureDistance+0.4*baselineMatchingDistance);
}


void getBBallDistances(char* target,char* dirname,int n){
    
    char imageFileNameBuffer[256];
    FILE *fp;
    DIR *dirp;
    struct dirent *dp;
    
    
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
    cout<<"Iterator output."<<endl;
    printf("Printing first %d closest images.\n",n);
    int i=1;
    for(std::vector<BBallMatchingStruct>::iterator  it = bballVector.begin(); it != bballVector.end(); it++) {
        cout << it->imageName<<endl;
        i++;
        if(i>n)
            break;
    }
    
    //printf("Terminating\n");
    
    
    
}
