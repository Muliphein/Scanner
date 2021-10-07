#ifndef __MATHALGORITHM_HPP__
#define __MATHALGORITHM_HPP__

#include <opencv2/opencv.hpp>
#include <vector>
#include <Eigen/Dense>

void fitPlanar(const std::vector <cv::Point3f>& data, float & a, float & b, float & c, float & d)
{
    cv::Point3f sum(0.0f, 0.0f, 0.0f);
    cv::Point3f mean(0.0f, 0.0f, 0.0f);

    for (int i=0; i<data.size(); ++i){
        sum += data[i];
    }
    mean = sum / (data.size() * 1.0f);

    showProcessStart("Build Matrix");
    Eigen::MatrixXf dataMatrix(3, data.size());
    for (int i=0; i<data.size(); ++i){
        dataMatrix(0, i) = data[i].x - mean.x;
        dataMatrix(1, i) = data[i].y - mean.y;
        dataMatrix(2, i) = data[i].z - mean.z;
    }
    showProcessEnd("Build Matrix");

    Eigen::JacobiSVD<Eigen::MatrixXf> SVDSolver(dataMatrix.transpose(), Eigen::ComputeThinU | Eigen::ComputeThinV);

    Eigen::MatrixXf V = SVDSolver.matrixV();
    std::cout << V << std::endl;

    a = V(0, 2);
    b = V(1, 2);
    c = V(2, 2);

    d = -(a*mean.x + b*mean.y + c*mean.z);

}

#endif