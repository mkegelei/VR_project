#ifndef DEPTHMAP_H
#define DEPTHMAP_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class DepthMap
{
public:
    unsigned int FBO;
    unsigned int map;

    DepthMap(){}

protected:
    void configDepthMap(unsigned int /*SHADOW_WIDTH*/, unsigned int /*SHADOW_HEIGHT*/){}
    
};

class SimpleDepthMap : public DepthMap
{
public:
    SimpleDepthMap(unsigned int SHADOW_WIDTH, unsigned int SHADOW_HEIGHT)
    {
        configDepthMap(SHADOW_WIDTH, SHADOW_HEIGHT);
    }

protected:
    void configDepthMap(unsigned int SHADOW_WIDTH, unsigned int SHADOW_HEIGHT)
    {
        glGenFramebuffers(1, &FBO);
        // create depth texture
        glGenTextures(1, &map);
        glBindTexture(GL_TEXTURE_2D, map);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, map, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

class CubeDepthMap : public DepthMap
{
public:
    CubeDepthMap(unsigned int SHADOW_WIDTH, unsigned int SHADOW_HEIGHT)
    {
        configDepthMap(SHADOW_WIDTH, SHADOW_HEIGHT);
    }

private:
    void configDepthMap(unsigned int SHADOW_WIDTH, unsigned int SHADOW_HEIGHT)
    {
        glGenFramebuffers(1, &FBO);
        // create depth cubemap texture
        glGenTextures(1, &map);
        glBindTexture(GL_TEXTURE_CUBE_MAP, map);
        for (unsigned int i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, map, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};
#endif

