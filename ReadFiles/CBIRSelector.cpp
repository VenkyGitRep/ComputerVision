/*
 Venkateshwaran Sundar
 Fall 2023.
 
 Main class which class Content Based Image Retrieval Selector.
 */

#include <vector>
#include <opencv2/opencv.hpp>
#include "BaselineMatching/BaselineMatching.hpp"
#include "HistogramMatching/HistogramMatching.hpp"
#include "TextureAnalysis/TextureAnalysis.hpp"
#include "CBIR.hpp"


using namespace std;
/**Selector function which performs Content Based Image retrieval, based on operation type.**/
int cbirSelector(char* target, char* operationType, char* imageDir, int n=5){
    vector<string> results;
    if(strcmp("BaseLineMatching",operationType)==0){
        printf("Performing Baseline Matching...\n Target Image: %s\n",target);
        results=baseLineMatching(target,imageDir,n);
    }else if(strcmp("HistogramMatching",operationType)==0){
        printf("Performing Histogram Matching...\n Target Image: %s\n",target);
        results=histogramMatching(target, imageDir, n);
    }else if(strcmp("HistogramMatchingInParts",operationType)==0){
        printf("Performing MultipartHistogram Matching...\n Target Image: %s\n",target);
        results=HistogramMatchingInParts(target,imageDir,n);
    }else if(strcmp("TextureAnalysis",operationType)==0){
        printf("Perfiorming Texture Matching...\n Target Image: %s\n",target);
        results=textureAnalysis(target,imageDir,n);
    }else if((strcmp("CBIR",operationType)==0)){
        printf("Performing Content Based Image Retrieval...\n Target Image: %s\n",target);
        results=getBBallDistances(target,imageDir,10);
    }
    else{
        printf("No match for operation type.Operation type:%s\n",operationType);
    }
    
    if(results.size()>0){
        for(int i=0;i<n;i++){
            
            string imgwindowName = "Image_";
            imgwindowName += std::to_string(i);
            cv::namedWindow(imgwindowName);
            cv::imshow(imgwindowName, cv::imread(results[i]));
            
            
        }
        cv::waitKey(0);
    }
    
    return 0;
}

/*
 Given a directory on the command line, scans through the directory for image files.
 
 Prints out the full path name for each file.  This can be used as an argument to fopen or to cv::imread.
 */
int main(int argc, char *argv[]){
    
    if(argc>1){
        char* target=argv[1];
        char * operationType=argv[2];
        char* imageDir=argv[3];
        cbirSelector(target,operationType,imageDir);
    }
    
}
