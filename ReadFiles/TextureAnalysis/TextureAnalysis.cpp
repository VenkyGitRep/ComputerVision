//
//  TextureAnalysis.cpp
//  ReadFiles
//
//  Created by Venky Sundar on 2/16/23.
//

#include "TextureAnalysis.hpp"
#include <opencv2/opencv.hpp>
#include "../HistogramMatching/HistogramMatching.hpp"
#include <dirent.h>
#include <vector>


/** Applies the Sobel -X filter on the image.
 */
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
/**Applies a Sobel-Y filter on the image**/
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

/**Computes the gradient magnitude of the image**/
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


/**Returns a normalised histogram vector of gradient magnitude of an image**/
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

/**A vector of HistogramTextureMatchingStruct is used to compare results of Texture Analysis between images. **/
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

/**Reads images from directory, compares it with target image for texture analysis and returns a vector of comparison result**/
vector<HistogramTextureMatchingStruct> readImagesForTextureAnalysis(char* dirname,char* targetImage) {
    
    
    char imageFileNameBuffer[256];
   
    DIR *dirp;
    struct dirent *dp;
   
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

vector<string> textureAnalysis(char* target,char* dir, int n){
    
    vector<HistogramTextureMatchingStruct> distanceVector;
    distanceVector=readImagesForTextureAnalysis(dir,target);
    vector<string> result;
    std::sort(distanceVector.begin(),distanceVector.end());
   
    printf("Printing first %d closest images after Texture analysis.\n",n);
    int i=1;
    for(std::vector<HistogramTextureMatchingStruct>::iterator  it = distanceVector.begin(); it != distanceVector.end(); it++) {
        cout << it->imageName<<endl;
        i++;
        result.push_back(it->imageName);
        if(i>n)
            break;
    }
    return result;
    
}


