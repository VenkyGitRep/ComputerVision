//
//  BaselineMatching.cpp
//  ReadFiles
//
//  Created by Venky Sundar on 2/18/23.
//

#include "BaselineMatching.hpp"
//
//  Containts helper methods to perform Baseline Matching.
//
//  Created by Venky Sundar on 2/10/23.
//
#include <opencv2/opencv.hpp>
#include <vector>
#include "csv_util.h"
using namespace std;
using namespace std;
/**Creates Baseline Matching feature of image**/
int calculateFeatureOfImage(char* imageFileName,vector<float> &fVec){
    cv::Mat imageSrc;
    imageSrc = cv::imread(imageFileName);
    
    if(imageSrc.data==NULL){
        printf("Unable to read image %s\n",imageFileName);
    }else{
        /**Calculate Feature vector**/
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


/**A vector of BaseLineMatchingStruct is used to compare results of Baseline matching between images. **/
struct BaseLineMatchingStruct
{
    double ssd;//Sum of squared distance.
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

/**
 Task1 : Baseline Matching. Takes the targetImage(qualified path), image repository, and number of matches to be shown.
 Prints the top n matches and returns the list of image name.
 **/
vector<string> baseLineMatching(char* targetImage,char* imagesDir,int n,bool readFromCsv){
    
    vector<float> targetFeatures;
    calculateFeatureOfImage(targetImage, targetFeatures);
    vector<string> result;
    
    std::vector<char*> imagefilenames;
    std::vector<std::vector<float>> data;
    char* featureListCsv = "/Users/venkysundar/Desktop/CV5330/Project2/testOlympus/featureList.csv";
    if(!readFromCsv){
        readImages(imagesDir,featureListCsv,1);
    }
    read_image_data_csv(featureListCsv, imagefilenames, data);
    
    vector<BaseLineMatchingStruct> distanceVector;
    
    for(int i=0;i<imagefilenames.size();i++){
        
        distanceVector.push_back(BaseLineMatchingStruct(targetImage,imagefilenames[i],getSumOfSquareDistance(targetFeatures, data[i])));
    }
    std::sort(distanceVector.begin(),distanceVector.end());
    
    printf("Baseline Matching: Printing first %d closest images.\n",n);
    int i=1;
    for(std::vector<BaseLineMatchingStruct>::iterator  it = distanceVector.begin(); it != distanceVector.end(); it++) {
        cout << it->imageName<<endl;
        i++;
        result.push_back(string(it->imageName));
        if(i>n)
            break;
    }
    
    return result;
}

/**Given two feature vectors, this method returns the sum of squared difference errror.**/

double getSumOfSquareDistance(vector<float> &v1,vector<float> &v2){
    //Compute distance
    double sumOfSquareDifferences=0;
    for(int i=0;i<v1.size();i++){
        sumOfSquareDifferences+=(v1[i]-v2[i])*(v1[i]-v2[i]);
    }
    
    return sumOfSquareDifferences;
}
