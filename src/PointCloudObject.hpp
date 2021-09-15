#ifndef __POINTCLOUDOBJECT_HPP__
#define __POINTCLOUDOBJECT_HPP__

#include "BasicObject.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>
#include "LittleUtils.hpp"

class PointCloudObject : public BasicObject{
private:
    unsigned int VBOHandle;
    std::vector <cv::Point3f> data;
    float * dataArray;

    void createVAO(){

        dataArray = new float[data.size()*3];

        for (int i=0; i<data.size(); ++i){
            dataArray[3*i+0]=data[i].x;
            dataArray[3*i+1]=data[i].y;
            dataArray[3*i+2]=data[i].z;
        }

        glGenVertexArrays(1, &VAOHandle);
        glGenBuffers(1, &VBOHandle);

        glBindVertexArray(VAOHandle);

        glBindBuffer(GL_ARRAY_BUFFER, VBOHandle);
        glBufferData(GL_ARRAY_BUFFER, data.size() * 3 * sizeof(float), dataArray, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        delete [] dataArray;

    }

public:

    PointCloudObject() = delete;

    PointCloudObject(std::vector<cv::Point3f> mesh) {
        data = mesh;
        randomColor();
        createVAO();
    }

    GLuint getDrawType() {
        return GL_POINTS;
    }

    unsigned int getPointNum() {
        return static_cast<unsigned>(data.size());
    }

    ~PointCloudObject(){
        glDeleteVertexArrays(1, &VAOHandle);
        glDeleteBuffers(1, &VBOHandle);
    }

    void preProcess() {
        glPointSize(5);
    }

};

#endif
