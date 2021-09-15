#ifndef __CAMERAOBJECT_HPP__
#define __CAMERAOBJECT_HPP__

#include "BasicObject.hpp"
#include <opencv2/opencv.hpp>

class CameraObject : public BasicObject{
private:
    unsigned int VBOHandle;
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    float * dataArray;
    float next[4] = {1, 3, 0, 2};

    float frontLength = 2.0f;
    float widthLength = 0.8f;
    float heightLength = 0.6f;

    void createVAO(){
        int top = 0;
        dataArray = new float[16 * 3];

        for (int i=0; i<4; ++i){

            dataArray[top+0] = position.x;
            dataArray[top+1] = position.y;
            dataArray[top+2] = position.z;
            top += 3;

            dataArray[top+0] = position.x + frontLength * front.x + pow(-1, (i>>0) & 1) * widthLength * right.x + pow(-1, (i>>1) & 1) * heightLength * up.x;
            dataArray[top+1] = position.y + frontLength * front.y + pow(-1, (i>>0) & 1) * widthLength * right.y + pow(-1, (i>>1) & 1) * heightLength * up.y;
            dataArray[top+2] = position.z + frontLength * front.z + pow(-1, (i>>0) & 1) * widthLength * right.z + pow(-1, (i>>1) & 1) * heightLength * up.z;

            top += 3;

            dataArray[top+0] = position.x + frontLength * front.x + pow(-1, (i>>0) & 1) * widthLength * right.x + pow(-1, (i>>1) & 1) * heightLength * up.x;
            dataArray[top+1] = position.y + frontLength * front.y + pow(-1, (i>>0) & 1) * widthLength * right.y + pow(-1, (i>>1) & 1) * heightLength * up.y;
            dataArray[top+2] = position.z + frontLength * front.z + pow(-1, (i>>0) & 1) * widthLength * right.z + pow(-1, (i>>1) & 1) * heightLength * up.z;

            top += 3;
            
            int j =next[i];
            dataArray[top+0] = position.x + frontLength * front.x + pow(-1, (j>>0) & 1) * widthLength * right.x + pow(-1, (j>>1) & 1) * heightLength * up.x;
            dataArray[top+1] = position.y + frontLength * front.y + pow(-1, (j>>0) & 1) * widthLength * right.y + pow(-1, (j>>1) & 1) * heightLength * up.y;
            dataArray[top+2] = position.z + frontLength * front.z + pow(-1, (j>>0) & 1) * widthLength * right.z + pow(-1, (j>>1) & 1) * heightLength * up.z;
            top += 3;

        }

        // for (int i=0; i<top; i+=3) {
        //     std::cout << dataArray[i+0] << ", " << dataArray[i+1] << ", " << dataArray[i+2] << std::endl;
        // }

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
    CameraObject(){
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        front = glm::vec3(0.0f, 0.0f, 1.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        right = glm::cross(front, up);
        randomColor();
        createVAO();
    }

    GLuint getDrawType(){
        return GL_LINES;
    }

    void preProcess(){
        glLineWidth(3);
    }

    unsigned int getPointNum() {
        return 16;
    }

    ~CameraObject(){
        glDeleteVertexArrays(1, &VAOHandle);
        glDeleteBuffers(1, &VBOHandle);
    }


};

#endif