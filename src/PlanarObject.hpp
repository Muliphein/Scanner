#ifndef __PLANNAROBJECT_HPP__
#define __PLANNAROBJECT_HPP__

#include "BasicObject.hpp"
#include "SurfaceObject.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>
#include "LittleUtils.hpp"

class PlannarObject : public BasicObject{
private:
    unsigned int VBOHandle;
    std::vector <cv::Point3f> data;
    cv::Size size;
    SurfaceObject * mySurface;

    void createVAO(){

        std::vector <cv::Point3f> surData;
        
        int top = 0;
        for (int i=0; i<size.height; ++i){
            for (int j=0;j<size.width; ++j){

                cv::Point3f p1 = data[0] * i/(size.height-1) + data[1] * (size.height-1-i) / (size.height-1);
                cv::Point3f p2 = data[2] * i/(size.height-1) + data[3] * (size.height-1-i) / (size.height-1);
                cv::Point3f p = p1 * j/(size.width-1) + p2 * (size.width-1-j) / (size.width-1);

                surData.push_back(p);

            }
        }

        mySurface = new SurfaceObject(surData, size);

    }

public:

    PlannarObject() = delete;

    PlannarObject(std::vector<cv::Point3f> surf, cv::Size surfSize) {
        data = surf;
        size = surfSize;
        if (data.size() != 4){
            std::cout << "Plannr Initial Failed" << std::endl;
        }
        randomColor();   
        createVAO();
    }

    PlannarObject(float a, float b, float c, float d, cv::Size surfSize = cv::Size(12, 12)) {
        // ax + by + cz + d = 0
        if (fabs(a*b*c) > 1e-6) {
            data.clear();
            data.emplace_back(0, 0, -d/c);
            data.emplace_back(0, -d/b, 0);
            data.emplace_back(-d/a, 0, 0);
            data.emplace_back(-d/a, -d/b, d/c);
            size = surfSize;
        } else {
            showError("Unsupport Planar Parallel to Axis");
        }
        randomColor();
        createVAO();

    }

    GLuint getDrawType() {
        return mySurface->getDrawType();
    }

    unsigned int getPointNum() {
        return mySurface->getPointNum();
    }

    ~PlannarObject(){
        delete mySurface;
    }

    void preProcess() {
        mySurface->preProcess();
    }

    unsigned int getVAOHandle() override {
        return mySurface->getVAOHandle();
    }

};

#endif
