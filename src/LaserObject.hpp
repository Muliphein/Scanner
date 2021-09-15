#ifndef __LASEROBJECT_HPP__
#define __LASEROBJECT_HPP__

#include "BasicObject.hpp"
#include "PointCloudObject.hpp"

class LaserObject: public PointCloudObject {
protected:
    void randomColorNew() {
        color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }

public:
    LaserObject(std::vector<cv::Point3f> mesh):PointCloudObject(mesh) {
        randomColorNew();
    }

    void preProcess() override{
        glPointSize(0.2f);
    }

};

#endif