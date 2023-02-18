//
//  TextureAnalysis.cpp
//  ReadFiles
//
//  Created by Venky Sundar on 2/16/23.
//

#include "TextureAnalysis.hpp"
#include <opencv2/opencv.hpp>
#include "HistogramMatching.hpp"
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



int sobelX3x3( cv::Mat &src, cv::Mat &dst ){
    int filter1[] = {-1,0,1};
    int filter2[] = {1,2,1};
    dst = cv::Mat::zeros(src.size(),CV_16SC3);
    for(int i=0;i<src.rows;i++){
        cv::Vec3b *rptr = src.ptr<cv::Vec3b>(i);
        cv::Vec3s *dstptr = dst.ptr<cv::Vec3s>(i);
        for(int j=0;j<src.cols;j++){
            
            for(int c=0;c<src.channels();c++){
                if(j==0){
                    dstptr[j][c] = (filter1[2]*rptr[j+1][c]);
                }else if(j==src.cols-1){
                    dstptr[j][c] = (filter1[0]*rptr[j-1][c]);
                }
                else{
                    dstptr[j][c] = (filter1[0]*rptr[j-1][c])+(filter1[1]*rptr[j][c])+(filter1[2]*rptr[j+1][c]);
                    
                }
                
            }
        }
    }
    
    cv::Mat tmpImg =dst.clone();
    for(int i=0;i<tmpImg.rows;i++){
        
        for(int j=0;j<tmpImg.cols;j++){
            
            for(int c=0;c<tmpImg.channels();c++){
                
                if(i==0){
                    //dstptr[j][c] = (filter1[2]*rptr[j+1][c]);
                }else if(i==tmpImg.rows-1){
                    //dstptr[j][c] = (filter1[0]*rptr[j-1][c]);
                }
                else{
                    cv::Vec3s *rptrs[] = {tmpImg.ptr<cv::Vec3s>(i-1),tmpImg.ptr<cv::Vec3s>(i),tmpImg.ptr<cv::Vec3s>(i+1)};
                    cv::Vec3s *dstptr = dst.ptr<cv::Vec3s>(i);
                    dstptr[j][c] = ((filter2[0]*rptrs[0][j][c]) + (filter2[1]*rptrs[1][j][c]) + (filter2[2]*rptrs[2][j][c]))/4;
                    //printf("SobelX dstptr[j][c]=%d\n",dstptr[j][c]);
                }
                
            }
        }
    }
    
    return 0;
    
}
int sobelY3x3( cv::Mat &src, cv::Mat &dst ){
    
    int filter1[] = {1,2,1};
    int filter2[] = {1,0,-1};
    dst = cv::Mat::zeros(src.size(),CV_16SC3);
    for(int i=0;i<src.rows;i++){
        cv::Vec3b *rptr = src.ptr<cv::Vec3b>(i);
        cv::Vec3s *dstptr = dst.ptr<cv::Vec3s>(i);
        for(int j=0;j<src.cols;j++){
            
            for(int c=0;c<src.channels();c++){
                if(j==0){
                    dstptr[j][c] = (filter1[2]*rptr[j+1][c]);
                }else if(j==src.cols-1){
                    dstptr[j][c] = (filter1[0]*rptr[j-1][c]);
                }
                else{
                    dstptr[j][c] = (filter1[0]*rptr[j-1][c])+(filter1[1]*rptr[j][c])+(filter1[2]*rptr[j+1][c])/4;
                }
                
            }
        }
    }
    
    cv::Mat tmpImg =dst.clone();
    for(int i=0;i<tmpImg.rows;i++){
        
        for(int j=0;j<tmpImg.cols;j++){
            
            for(int c=0;c<tmpImg.channels();c++){
                
                if(i==0){
                    //dstptr[j][c] = (filter1[2]*rptr[j+1][c]);
                }else if(i==tmpImg.rows-1){
                    //dstptr[j][c] = (filter1[0]*rptr[j-1][c]);
                }
                else{
                    cv::Vec3s *rptrs[] = {tmpImg.ptr<cv::Vec3s>(i-1),tmpImg.ptr<cv::Vec3s>(i),tmpImg.ptr<cv::Vec3s>(i+1)};
                    cv::Vec3s *dstptr = dst.ptr<cv::Vec3s>(i);
                    dstptr[j][c] = ((filter2[0]*rptrs[0][j][c]) + (filter2[1]*rptrs[1][j][c]) + (filter2[2]*rptrs[2][j][c]));
                }
                
            }
        }
    }
    
    return 0;
}
int magnitude( cv::Mat &sx, cv::Mat &sy, cv::Mat &dst ){
    dst = cv::Mat::zeros(sx.size(),CV_8SC3);
    for(int i=0;i<sx.rows;i++){
        cv::Vec3s *xptr = sx.ptr<cv::Vec3s>(i);
        cv::Vec3s *yptr = sx.ptr<cv::Vec3s>(i);
        cv::Vec3b *dstptr = dst.ptr<cv::Vec3b>(i);
        for(int j=0;j<sx.cols;j++){
            
            for(int c=0;c<sx.channels();c++){
                dstptr[j][c] = sqrt((xptr[j][c]*xptr[j][c])+(yptr[j][c]*yptr[j][c]));
                //printf("SobelX : %d SobelY : %d Output : %d\n",xptr[j][c],yptr[j][c],dstptr[j][c]);
            }
        }
    }
    return 0;
}



int getTextureVector(char* image, std::vector<float> &vec){
    cv::Mat imgSrc;
    
    imgSrc=cv::imread(image);
    if(imgSrc.data==NULL){
        printf("Unable to read image %s\n",image);
    }else{
        cv::Mat sobelXGrad,sobelYGrad,gradDst,gradientImg;
        sobelX3x3(imgSrc, sobelXGrad);
        sobelY3x3(imgSrc, sobelYGrad);
        magnitude(sobelXGrad, sobelYGrad, gradDst);
        convertScaleAbs(gradDst, gradientImg);
        
        std::vector<float> histFeatVec;
        cv::Mat hist = cv::Mat::zeros(gradientImg.size(),CV_32FC3);
        float sumOfBins = 0;
        for(int i=0;i<gradientImg.rows;i++){
            
            cv::Vec3b *rptr = gradientImg.ptr<cv::Vec3b>(i);
            for(int j=0;j<gradientImg.cols;j++){
                int rBin,bBin,gBin;
                rBin = rptr[j][0]*8/256;
                bBin = rptr[j][1]*8/256;
                gBin = rptr[j][2]*8/256;
                cv::Vec3f *histRptr = hist.ptr<cv::Vec3f>(rBin);
                histRptr[bBin][gBin]++;
                sumOfBins = sumOfBins+1;
            }
        }
        
        //Normalise histogram
        //printf("Total NumOf pixels while binning: %d * %d = %d. sumOfBins:%f\n",gradientImg.rows,gradientImg.cols,(gradientImg.rows*gradientImg.cols),sumOfBins);
        float totalNumOfBins = (gradientImg.rows*gradientImg.cols);
        //printf("Totale num of Bins: %f\n",totalNumOfBins);
        float sumOfLieklihoods = 0.0;
        
        for(int i=0;i<hist.rows;i++){
            
            cv::Vec3f *rptr = hist.ptr<cv::Vec3f>(i);
            for(int j=0;j<hist.cols;j++){
                for(int c=0;c<hist.channels();c++){
                    /*if(rptr[j][c]/totalNumOfBins<0.0){
                     printf("Tell me why! %f %s \n",rptr[j][c],target);
                     }*/
                    rptr[j][c] = fabs(rptr[j][c]/totalNumOfBins);
                    sumOfLieklihoods = sumOfLieklihoods+rptr[j][c];
                    histFeatVec.push_back(rptr[j][c]);
                    if(rptr[j][c]<0.0){
                        printf("Tell me why2! %f - Texture \n",rptr[j][c]);
                    }
                    
                }
            }
        }
        //printf("Total sum of values in hist: %f \n",sumOfLieklihoods);
        //printf("Size of vectorized histogram: %lu\n",histFeatVec.size());
        
        
        vec = histFeatVec;
        
        
        
        
        
    }
    
    return 0;
}

struct HistogramTextureMatchingStruct
{
    double histIntersection;
    std::string targetImageName;
    std::string imageName;
    
    HistogramTextureMatchingStruct(std::string target,std::string image, double d)  {
        this->histIntersection = d;
        this->targetImageName =target;
        this->imageName = image;
    }
    
    
    bool operator < (const HistogramTextureMatchingStruct& img) const
    {
        return (histIntersection < img.histIntersection);
    }
};

vector<HistogramTextureMatchingStruct> readImagesForTextureAnalysis(char* dirname,char* targetImage) {
    
    
    char imageFileNameBuffer[256];
    FILE *fp;
    DIR *dirp;
    struct dirent *dp;
    int i;
    vector<HistogramTextureMatchingStruct> distanceVector;
    // open the directory
    dirp = opendir( dirname );
    if( dirp == NULL) {
        printf("Cannot open directory %s\n", dirname);
        exit(-1);
    }
    std::vector<float> targetTextureVec;
    getTextureVector(targetImage, targetTextureVec);
    vector<float> targetRGBHist;
    targetRGBHist=generateNormalisedHistogramVec(targetImage);
    //printf("I've got a Texture Vector.\n");
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
            
            
            std::vector<float> gradTextureVec;
            getTextureVector(imageFileNameBuffer, gradTextureVec);
            vector<float> rgbHistFeatures;
            rgbHistFeatures=generateNormalisedHistogramVec(imageFileNameBuffer);
            //printf("I've got a Texture Vector.\n");
            
            
            
            double rgbDistance = gethistogramIntersectionDistance(targetRGBHist, rgbHistFeatures);
            double gradDistance = gethistogramIntersectionDistance(targetTextureVec, gradTextureVec);
            double totalDistance =rgbDistance+gradDistance;
            //printf("Total distance : %f\n",totalDistance);
            
            distanceVector.push_back(HistogramTextureMatchingStruct(targetImage,imageFileNameBuffer,totalDistance));
        }
    }
    
    
    return distanceVector;
    
    
    //printf("Terminating\n");
    
}

void textureAnalysis(){
    char* img1 ="/Users/venkysundar/Desktop/CV5330/Project2/olympus/pic.0164.jpg";
    char* img2 ="/Users/venkysundar/Desktop/CV5330/Project2/olympus/pic.0110.jpg";
    char* img3 ="/Users/venkysundar/Desktop/CV5330/Project2/olympus/pic.1032.jpg";
    char* img4 ="/Users/venkysundar/Desktop/CV5330/Project2/olympus/pic.0164.jpg";
    
    /*  std::vector<float> gradTextureVec;
     getTextureVector(img1, gradTextureVec);
     vector<float> rgbHistFeatures;
     rgbHistFeatures=generateNormalisedHistogramVec(img1);
     printf("I've got a Texture Vector.\n");
     
     std::vector<float> gradTextureVec2;
     getTextureVector(img2, gradTextureVec2);
     vector<float> rgbHistFeatures2;
     rgbHistFeatures2=generateNormalisedHistogramVec(img2);
     printf("I've got a Texture Vector.\n");
     
     
     
     double rgbDistance = gethistogramIntersectionDistance(rgbHistFeatures, rgbHistFeatures2);
     double gradDistance = gethistogramIntersectionDistance(gradTextureVec, gradTextureVec2);
     
     vector<HistogramTextureMatchingStruct> distanceVector;
     distanceVector.push_back(HistogramTextureMatchingStruct(img1,img2,(rgbDistance+gradDistance)));
     
     std::vector<float> gradTextureVec3;
     getTextureVector(img3, gradTextureVec3);
     vector<float> rgbHistFeatures3;
     rgbHistFeatures3=generateNormalisedHistogramVec(img3);
     printf("I've got a Texture Vector.\n");
     
     
     
     rgbDistance = gethistogramIntersectionDistance(rgbHistFeatures, rgbHistFeatures3);
     gradDistance = gethistogramIntersectionDistance(gradTextureVec, gradTextureVec3);
     distanceVector.push_back(HistogramTextureMatchingStruct(img1,img3,(rgbDistance+gradDistance)));
     
     std::vector<float> gradTextureVec4;
     getTextureVector(img4, gradTextureVec4);
     vector<float> rgbHistFeatures4;
     rgbHistFeatures4=generateNormalisedHistogramVec(img4);
     printf("I've got a Texture Vector.\n");
     
     
     
     rgbDistance = gethistogramIntersectionDistance(rgbHistFeatures, rgbHistFeatures4);
     gradDistance = gethistogramIntersectionDistance(gradTextureVec, gradTextureVec4);
     distanceVector.push_back(HistogramTextureMatchingStruct(img1,img4,(rgbDistance+gradDistance)));
     */
    vector<HistogramTextureMatchingStruct> distanceVector;
    distanceVector=readImagesForTextureAnalysis("/Users/venkysundar/Desktop/CV5330/Project2/olympus/",img1);
    int n=5;
    std::sort(distanceVector.begin(),distanceVector.end());
    cout<<"Iterator output."<<endl;
    printf("Printing first %d closest images.\n",n);
    int i=1;
    for(std::vector<HistogramTextureMatchingStruct>::iterator  it = distanceVector.begin(); it != distanceVector.end(); it++) {
        cout << it->imageName<<" "<< it->histIntersection<<endl;
        i++;
        if(i>n)
            break;
    }
    
    
}


