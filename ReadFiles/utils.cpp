//
//  utils.cpp
//  ReadFiles
//
//  Created by Venky Sundar on 2/10/23.
//

#include "utils.hpp"
#include <opencv.hpp>

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
#include "HistogramMatching.hpp"
using namespace std;
using namespace std;
int calculateFeatureOfImage(char* imageFileName,vector<float> &fVec){
    
    cv::Mat imageSrc;
    imageSrc = cv::imread(imageFileName);
    
    if(imageSrc.data==NULL){
        printf("Unable to read image %s\n",imageFileName);
    }else{
        //printf("Read %s successfully\n",imageFileName);
        
        /**Calculate Feature vector**/
        //printf("Size of image:%d X %d X %d\n",imageSrc.rows,imageSrc.cols,imageSrc.channels());
        
        int cx =(imageSrc.rows/2)-4;int cy=(imageSrc.cols/2)-4;
        for(int row=cx;row<=cx+4;row++){
            cv::Vec3b* rptr = imageSrc.ptr<cv::Vec3b>(row);
            for(int col=cy;col<=cy+4;col++){
                for(int c=0;c<imageSrc.channels();c++){
                    fVec.push_back(rptr[col][c]);
                }
            }
        }
        
    }
    return 0;
    
    
}

int readImages(char* dirname,char* featureListCsv,int featureType) {
   
    
    char imageFileNameBuffer[256];
    FILE *fp;
    DIR *dirp;
    struct dirent *dp;
    int i;
  
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
            
//printf("processing image file: %s\n", dp->d_name);
            
            // build the overall filename
            strcpy(imageFileNameBuffer, dirname);
            //strcat(imageFileNameBuffer, "/");
            strcat(imageFileNameBuffer, dp->d_name);
            
            //printf("full path name: %s\n", imageFileNameBuffer);
            vector<float> vec;
            vector<vector<float>> vecOfVec;
            if(featureType==1){
                calculateFeatureOfImage(imageFileNameBuffer,vec);
                
            }else if(featureType==2){
                vec = generateNormalisedHistogramVec(imageFileNameBuffer);
                
                
            }
            else if(featureType==3){
                vecOfVec = generateNormalisedHistogramVecParts(imageFileNameBuffer);
                
                
            }
           
            append_image_data_csv(featureListCsv, imageFileNameBuffer, vec);
            
        }
    }
   
    //printf("Terminating\n");
    
    return(0);
}
    

struct BaseLineMatchingStruct
{
    double ssd;
    std::string targetImageName;
    std::string imageName;
    
    BaseLineMatchingStruct(std::string target,std::string image, double d)  {
        this->ssd = d;
        this->targetImageName =target;
        this->imageName = image;
    }
    
    
    bool operator < (const BaseLineMatchingStruct& img) const
    {
        return (ssd < img.ssd);
    }
};
//Get image Vector
//Compute dsitance from existing features
//sort

int baseLineMatching(char* targetImage,char* imagesDir,int n){
    
    vector<float> targetFeatures,imgFeatures1,imgFeatures2;
    calculateFeatureOfImage(targetImage, targetFeatures);
    
    
    std::vector<char*> imagefilenames;
    std::vector<std::vector<float>> data;
    char* featureListCsv = "/Users/venkysundar/Desktop/CV5330/Project2/testOlympus/featureList.csv";
    //readImages(imagesDir,featureListCsv,1);
    read_image_data_csv(featureListCsv, imagefilenames, data);
    
    vector<BaseLineMatchingStruct> distanceVector;
    
    for(int i=0;i<imagefilenames.size();i++){
        
        distanceVector.push_back(BaseLineMatchingStruct(targetImage,imagefilenames[i],getSumOfSquareDistance(targetFeatures, data[i])));
    }
    std::sort(distanceVector.begin(),distanceVector.end());
    cout<<"Iterator output."<<endl;
    printf("Printing first %d closest images.\n",n);
    int i=1;
    for(std::vector<BaseLineMatchingStruct>::iterator  it = distanceVector.begin(); it != distanceVector.end(); it++) {
        cout << it->imageName<<" "<<it->ssd<<endl;
        i++;
        if(i>n)
            break;
    }
   
    return 0;
}

double getSumOfSquareDistance(vector<float> &v1,vector<float> &v2){
    //Compute distance
    double sumOfSquareDifferences=0;
    for(int i=0;i<v1.size();i++){
        sumOfSquareDifferences+=(v1[i]-v2[i])*(v1[i]-v2[i]);
    }
    
    return sumOfSquareDifferences;
}
