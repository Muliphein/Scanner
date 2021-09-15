#ifndef __AXISOBJECT_HPP__
#define __AXISOBJECT_HPP__

#include "BasicObject.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>
#include "LittleUtils.hpp"

class AxisObject : public BasicObject{
private:
    unsigned int VBOHandle;
    const int axisLength = 500;
    float * dataArray;
    void createVAO(){
        dataArray = new float[axisLength * 3 * 2 * 3];

        int top = 0;
        for (int i=0; i<axisLength; ++i){
            for (int j=0; j<3; ++j){
                for (int k=0; k<3; ++k){
                    if (j==k){
                        dataArray[top + j*6 + k] = i * 1.0f;
                        dataArray[top + j*6 + k + 3] = i * 1.0f + 1;
                    }
                    else {
                        dataArray[top + j*6 + k] = 0;
                        dataArray[top + j*6 + k + 3] = 0;
                    }
                }
            }
            top += 18;
        }

        glGenVertexArrays(1, &VAOHandle);
        glGenBuffers(1, &VBOHandle);

        glBindVertexArray(VAOHandle);

        glBindBuffer(GL_ARRAY_BUFFER, VBOHandle);
        glBufferData(GL_ARRAY_BUFFER, top * sizeof(float), dataArray, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);


        delete[] dataArray;
    }

public:

    AxisObject() {
        randomColor();   
        createVAO();
    }

    GLuint getDrawType() {
        return GL_LINES;
    }

    unsigned int getPointNum() {
        return axisLength * 3 * 2;
    }

    ~AxisObject(){
        glDeleteVertexArrays(1, &VAOHandle);
        glDeleteBuffers(1, &VBOHandle);
    }

    void preProcess() {
        glLineWidth(2);
    }

};

#endif