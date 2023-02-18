//
//  TextureAnalysis.hpp
//  ReadFiles
//
//  Created by Venky Sundar on 2/16/23.
//

#ifndef TextureAnalysis_hpp
#define TextureAnalysis_hpp

#include <stdio.h>
#include <vector>
void textureAnalysis();
//int vectorizeMatrix(cv::Mat &img,std::vector<float> &vec);
int getTextureVector(char* image, std::vector<float> &vec);
#endif /* TextureAnalysis_hpp */
