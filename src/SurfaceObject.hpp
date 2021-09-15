#ifndef __SURFACEOBJECT_HPP__
#define __SURFACEOBJECT_HPP__

#include "BasicObject.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>
#include "LittleUtils.hpp"

class SurfaceObject : public BasicObject{
private:
    unsigned int VBOHandle;
    std::vector <cv::Point3f> data;
    cv::Size size;
    float * dataArray;

    void createVAO(){

        dataArray = new float[ ((size.width-1) * size.height + size.width * (size.height - 1)) * 6];

        int top = 0;
        for (int i=0; i<size.height; ++i){
            for (int j=0;j<size.width-1; ++j){
                dataArray[top+0] = data[i*size.width + j].x;
                dataArray[top+1] = data[i*size.width + j].y;
                dataArray[top+2] = data[i*size.width + j].z;
                top+=3;
                dataArray[top+0] = data[i*size.width + j + 1].x;
                dataArray[top+1] = data[i*size.width + j + 1].y;
                dataArray[top+2] = data[i*size.width + j + 1].z;
                top+=3;
            }
        }

        for (int i=0;i<size.height-1; ++i){
            for (int j=0;j<size.width; ++j){
                dataArray[top+0] = data[i *size.width + j].x;
                dataArray[top+1] = data[i *size.width + j].y;
                dataArray[top+2] = data[i *size.width + j].z;
                top+=3;
                dataArray[top+0] = data[(i+1) *size.width + j].x;
                dataArray[top+1] = data[(i+1) *size.width + j].y;
                dataArray[top+2] = data[(i+1) *size.width + j].z;
                top+=3;
            }
        }

        // for (int i=0;i<top; i+=3){
        //     std::cout << dataArray[i] << " " << dataArray[i+1] << ";";
        // }
        // std::cout << std::endl;

        glGenVertexArrays(1, &VAOHandle);
        glGenBuffers(1, &VBOHandle);

        glBindVertexArray(VAOHandle);

        glBindBuffer(GL_ARRAY_BUFFER, VBOHandle);
        glBufferData(GL_ARRAY_BUFFER, top * sizeof(float), dataArray, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        delete [] dataArray;

    }

public:

    SurfaceObject() = delete;

    SurfaceObject(std::vector<cv::Point3f> surf, cv::Size surfSize) {
        data = surf;
        size = surfSize;
        if (size.width * size.height != data.size()){
            std::cout << "Surface Initial Failed" << std::endl;
        }
        randomColor();   
        createVAO();
    }

    GLuint getDrawType() {
        return GL_LINES;
    }

    unsigned int getPointNum() {
        return ((size.width-1) * size.height + size.width * (size.height - 1)) * 2;
    }

    ~SurfaceObject(){
        glDeleteVertexArrays(1, &VAOHandle);
        glDeleteBuffers(1, &VBOHandle);
    }

    void preProcess() {
        glLineWidth(3);
    }

};

#endif
