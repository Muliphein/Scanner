#ifndef __BASICOBJECT_HPP__
#define __BASICOBJECT_HPP__

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "LittleUtils.hpp"

class BasicObject{
protected:
    unsigned int VAOHandle;
    glm::vec4 color;
    void randomColor() {
        color = glm::vec4(randomlr(0.0f, 0.8f), randomlr(0.0f, 0.8f), randomlr(0.0f, 0.8f), 1.0f);
    }

public:
    unsigned int getVAOHandle() {
        return VAOHandle;
    }
    const glm::vec4 getColor() {
        return color;
    }
    virtual GLuint getDrawType() = 0;
    virtual unsigned int getPointNum() = 0;
    virtual void preProcess() = 0;
};

#endif