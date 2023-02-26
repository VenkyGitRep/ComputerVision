//
//  HistogramMatching.cpp
//  ReadFiles
//
//  Created by Venky Sundar on 2/12/23.
//


#include <opencv2/opencv.hpp>
#include "HistogramMatching.hpp"
#include <dirent.h>
#include <vector>
#include "csv_util.h"


/**Returns a vector of the Normalised histogram of an image.**/
std::vector<float> generateNormalisedHistogramVec(char* target){
    cv::Mat imgSrc;
    std::vector<float> histFeatVec;
    imgSrc=cv::imread(target);
    if(imgSrc.data==NULL){
        printf("Unable to read image %s\n",target);
    }else{
        //printf("Generating histogram vector for %s\n",target);
        cv::Mat hist = cv::Mat::zeros(imgSrc.size(),CV_32FC3);
        float sumOfBins = 0;
        for(int i=0;i<imgSrc.rows;i++){
            
            cv::Vec3b *rptr = imgSrc.ptr<cv::Vec3b>(i);
            for(int j=0;j<imgSrc.cols;j++){
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
        //printf("Total NumOf pixels while binning: %d * %d = %d. sumOfBins:%f\n",imgSrc.rows,imgSrc.cols,(imgSrc.rows*imgSrc.cols),sumOfBins);
        float totalNumOfBins = (imgSrc.rows*imgSrc.cols);
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
                        printf("Tell me why2! %f %s \n",rptr[j][c],target);
                    }
                    
                }
            }
        }
        //printf("Total sum of values in hist: %f \n",sumOfLieklihoods);
        //printf("Size of vectorized histogram: %lu\n",histFeatVec.size());
        
        
    }
    
    return histFeatVec;
}


/**A vector of HistogramMatchingStruct is used to compare results of Histogram matching between images. **/
struct HistogramMatchingStruct
{
    double histIntersection;
    std::string targetImageName;
    std::string imageName;
    
    HistogramMatchingStruct(std::string target,std::string image, double d)  {
        this->histIntersection = d;
        this->targetImageName =target;
        this->imageName = image;
    }
    
    
    bool operator < (const HistogramMatchingStruct& img) const
    {
        return (histIntersection < img.histIntersection);
    }
};

/**Compares two vectors(which are normalised histograms of images) and returns and histogram intersection. **/
double gethistogramIntersectionDistance(vector<float> targetFeatures,vector<float> data){
    //Normalise histogram
    float sumOfMins = 0.0;
    for(int i=0;i<targetFeatures.size();i++){
        if(min(targetFeatures[i],data[i])<0.0){
            printf("Got a negative likelihood.%f\n",min(targetFeatures[i],data[i]));
        }
        //printf("Values compared : %f and %f\n",targetFeatures[i],data[i]);
        sumOfMins = sumOfMins+min(targetFeatures[i],data[i]);
    }
    //printf("Sum of Mins:%f\n",sumOfMins);
    
    //printf("Distance between target and img:%f\n",fabs(1.0-sumOfMins));
    
    return fabs(1.0-sumOfMins);
}

/**Main function for HIstogram matching. Input arguments are targetImage, image repository and n-number of best matches.**/
vector<string> histogramMatching(char* targetImage,char* imagesDir,int n, bool readfromCsv){
    vector<float> targetFeatures;
    targetFeatures=generateNormalisedHistogramVec(targetImage);
    
    vector<string> result;
    std::vector<char*> imagefilenames;
    std::vector<std::vector<float>> data;
    char* histogramfeatureListCsv = "/Users/venkysundar/Desktop/CV5330/Project2/TestOlympusHist/HistogramfeatureList.csv";
    
    if(readfromCsv){
        readImages(imagesDir,histogramfeatureListCsv,2);
    }
    
    read_image_data_csv(histogramfeatureListCsv, imagefilenames, data);
    
    vector<HistogramMatchingStruct> distanceVector;
    
    for(int i=0;i<imagefilenames.size();i++){
        
        distanceVector.push_back(HistogramMatchingStruct(targetImage,imagefilenames[i],gethistogramIntersectionDistance(targetFeatures, data[i])));
    }
    std::sort(distanceVector.begin(),distanceVector.end());
    
    printf("Printing first %d closest images for histogram matching.\n",n);
    int i=1;
    for(std::vector<HistogramMatchingStruct>::iterator  it = distanceVector.begin(); it != distanceVector.end(); it++) {
        cout << it->imageName<<endl;
        result.push_back(it->imageName);
        i++;
        if(i>n)
            break;
    }
    
    return result;
}

/**This  returns a  normalised histogram vector of part of an image**/
vector<float> getNormalizedVector(cv::Mat &mat, int rowStart,int rowEnd,int colStart,int colEnd){
    
    cv::Mat hist = cv::Mat::zeros(mat.size(),CV_32FC3);
    float sumOfBins = 0;
    for(int i=rowStart;i<=rowEnd;i++){
        
        cv::Vec3b *rptr = mat.ptr<cv::Vec3b>(i);
        for(int j=colStart;j<=colEnd;j++){
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
    float totalNumOfBins = sumOfBins;
    
    float sumOfLieklihoods = 0.0;
    vector<float> histFeatVec;
    for(int i=0;i<hist.rows;i++){
        
        cv::Vec3f *rptr = hist.ptr<cv::Vec3f>(i);
        for(int j=0;j<hist.cols;j++){
            for(int c=0;c<hist.channels();c++){
                
                rptr[j][c] = fabs(rptr[j][c]/totalNumOfBins);
                sumOfLieklihoods = sumOfLieklihoods+rptr[j][c];
                histFeatVec.push_back(rptr[j][c]);
                if(rptr[j][c]<0.0){
                    printf("Tell me why2! %f \n",rptr[j][c]);
                }
                
            }
        }
    }
    //printf("Sum of likelihoods:%f\n",sumOfLieklihoods);
    if(sumOfLieklihoods>1.5){
        printf("Sum of Likelihoods gone wrong:%f \n",sumOfLieklihoods);
    }
    return histFeatVec;
}

/*Returns a Normalised histogram vectors of parts of image. The image dimensions are split up as Image.row/rowDiv and Image.cols/colDiv  **/
std::vector<std::vector<float>> generateNormalisedHistogramVecParts(char* target,int rowDiv, int colDiv){
    cv::Mat imgSrc;
    vector<vector<float>> normHistVectors;
    std::vector<float> histFeatVec;
    imgSrc=cv::imread(target);
    if(imgSrc.data==NULL){
        printf("Unable to read image %s\n",target);
    }else{
        //printf("Generating histogram vector for %s\n",target);
        
        
        for(int i=0;i<rowDiv;i++){
            int rowStart = (imgSrc.rows/rowDiv)*i;
            int rowEnd = rowStart+(imgSrc.rows/i);
            if(i==rowDiv-1){
                rowEnd=imgSrc.rows;
            }
            normHistVectors.push_back(getNormalizedVector(imgSrc, rowStart, rowEnd, 0, imgSrc.cols-1));
        }
        
    }
    
    return normHistVectors;
}


/**Creates a vector of Normalised histogram comparisons(with 2 histograms per image). The image is split horizontally.**/
vector<HistogramMatchingStruct> readImagesForHistParts(char* dirname,char* targetImage,vector<vector<float>> targetFeature) {
    
    
    char imageFileNameBuffer[256];
    
    DIR *dirp;
    struct dirent *dp;
    
    vector<HistogramMatchingStruct> distanceVector;
    // open the directory
    dirp = opendir( dirname );
    if( dirp == NULL) {
        printf("Cannot open directory %s\n", dirname);
        exit(-1);
    }
    
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
            
            
            vector<vector<float>> imageFeature;
            imageFeature = generateNormalisedHistogramVecParts(imageFileNameBuffer);
            //Get Distance from target and store.
            double topDistance = gethistogramIntersectionDistance(targetFeature[0],imageFeature[0]);
            double bottomDistance = gethistogramIntersectionDistance(targetFeature[1],imageFeature[1]);
            
            double totalDistance = (0.7*topDistance)+ (0.3*bottomDistance);
            //printf("Total distance : %f\n",totalDistance);
            
            distanceVector.push_back(HistogramMatchingStruct(targetImage,imageFileNameBuffer,totalDistance));
        }
    }
    
    
    return distanceVector;
    

    
}

/**This function compares target image with images in repositiry. Distance metric is the sum of histogram intersection of top halves and bottom halves of images being compared.**/
vector<string> HistogramMatchingInParts(char* targetImage, char* imageDir, int n){
    vector<vector<float>> targetFeatures;
    targetFeatures=generateNormalisedHistogramVecParts(targetImage);
    vector<string> result;
    
    std::vector<char*> imagefilenames;
    std::vector<std::vector<float>> data;
    
    
    vector<HistogramMatchingStruct> distanceVector;
    distanceVector=readImagesForHistParts(imageDir, targetImage, targetFeatures);
    
    std::sort(distanceVector.begin(),distanceVector.end());
    
    printf("Printing first %d closest images for Multiple Histogram matching.\n",n);
    int i=1;
    for(std::vector<HistogramMatchingStruct>::iterator  it = distanceVector.begin(); it != distanceVector.end(); it++) {
        cout << it->imageName<<endl;
        result.push_back(it->imageName);
        i++;
        if(i>n)
            break;
    }
    
    return result;
}
