#ifndef __LITTLEUTILS_HPP__
#define __LITTLEUTILS_HPP__

#include <iostream>
#include <random>

float randomlr(float l, float r)
{
    std::random_device e;
    std::uniform_real_distribution <float> u(l, r);
    return u(e);
}

void showError(std::string message){
    std::cout << "[Error] : " << message << std::endl;
}

void showProcessStart(std::string message){
    std::cout << ">>> Process <" << message << "> Start" << std::endl;
}

void showProcessEnd(std::string message){
    std::cout << "--- Process <" << message << ">  End" << std::endl;
}

bool checkFileExist(std::string fileName){
    std::fstream file;
    file.open(fileName, std::ios::in);
    if (!file.is_open()){
        return false;
    } else {
        file.close();
        return true;
    }
}

const double epsilon = 1e-6;

bool pointInQuad(cv::Point2f point, cv::Point2f p1, cv::Point2f p2, cv::Point2f p3, cv::Point2f p4){

    if (
        ((point-p1).cross(p2-p1) - epsilon <= 0)
        &&((point-p2).cross(p4-p2) - epsilon <= 0)
        &&((point-p4).cross(p3-p4) - epsilon <= 0)
        &&((point-p3).cross(p1-p3) - epsilon <= 0)
        )
        return true;
    else return false;
}

#endif