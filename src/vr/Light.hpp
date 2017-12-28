#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <DepthMap.hpp>
#include <Shader.hpp>

#include <vector>

class Light
{
public:
    Shader shader;
    DepthMap depthMap;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float near_plane;
    float far_plane;
    const unsigned int SHADOW_WIDTH = 1024;
    const unsigned int SHADOW_HEIGHT = 1024;

    Light(){}
    virtual void updateLightVariables (){} 
    virtual void addToShader(Shader /*shader*/){}
    
};

class DirLight : public Light
{
public:
    glm::vec3 direction;
    glm::mat4 lightProjection;
    glm::mat4 lightView;
    glm::mat4 lightSpaceMatrix;

    DirLight(Shader shader, glm::vec3 direction, 
        glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, 
        float near_plane, float far_plane)
    {
        this->shader = shader;
        this->depthMap = *(new SimpleDepthMap(SHADOW_WIDTH, SHADOW_HEIGHT));
        this->direction = direction;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->near_plane = near_plane;
        this->far_plane = far_plane;
    }

    void updateLightVariables()
    {
        //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
    }

    void addToShader(Shader shader)
    {
        shader.use();
        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        shader.setVec3("dirLight.direction", -direction);
        shader.setVec3("dirLight.ambient", ambient);
        shader.setVec3("dirLight.diffuse", diffuse);
        shader.setVec3("dirLight.specular", specular);
    }

};

class PointLight : public Light
{
public:
    glm::vec3 position;
    Camera* lightCam;
    glm::mat4 shadowProj;
    std::vector<glm::mat4> shadowTransforms;
    float constant;
    float linear;
    float quadratic;

    PointLight(Shader shader, glm::vec3 position, 
        glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, 
        float near_plane, float far_plane,
        float constant, float linear, float quadratic)
    {
        this->shader = shader;
        this->depthMap = *(new CubeDepthMap(SHADOW_WIDTH, SHADOW_HEIGHT));
        this->position = position;
        this->lightCam = NULL;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->near_plane = near_plane;
        this->far_plane = far_plane;
        this->constant = constant;
        this->linear = linear;
        this->quadratic = quadratic;
    }

    PointLight(Shader shader, Camera* lightCam, 
        glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, 
        float near_plane, float far_plane,
        float constant, float linear, float quadratic)
    {
        this->shader = shader;
        this->depthMap = *(new CubeDepthMap(SHADOW_WIDTH, SHADOW_HEIGHT));
        this->lightCam = lightCam;
        this->position = lightCam->Position;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->near_plane = near_plane;
        this->far_plane = far_plane;
        this->constant = constant;
        this->linear = linear;
        this->quadratic = quadratic;
    }

    void updateLightVariables()
    {
        if(lightCam != NULL)
            position = lightCam->Position;
        shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        shadowTransforms.clear();
        shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    }

    void addToShader(Shader shader)
    {
        shader.use();
        shader.setVec3("pointLights[0].position", position);
        shader.setVec3("pointLights[0].ambient", ambient);
        shader.setVec3("pointLights[0].diffuse", diffuse);
        shader.setVec3("pointLights[0].specular", specular);
        shader.setFloat("pointLights[0].constant", constant);
        shader.setFloat("pointLights[0].linear", linear);
        shader.setFloat("pointLights[0].quadratic", quadratic);
    } 

};

class FlashLight : public Light
{
public:
    glm::vec3 position;
    glm::vec3 direction;
    Camera* lightCam;
    glm::mat4 shadowProj;
    std::vector<glm::mat4> shadowTransforms;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;

    FlashLight(Shader shader, glm::vec3 position, glm::vec3 direction, 
        glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, 
        float near_plane, float far_plane,
        float constant, float linear, float quadratic,
        float cutOff, float outerCutOff)
    {
        this->shader = shader;
        this->depthMap = *(new CubeDepthMap(SHADOW_WIDTH, SHADOW_HEIGHT));
        this->position = position;
        this->direction = direction;
        this->lightCam = NULL;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->near_plane = near_plane;
        this->far_plane = far_plane;
        this->constant = constant;
        this->linear = linear;
        this->quadratic = quadratic;
        this->cutOff = cutOff;
        this->outerCutOff = outerCutOff;
    }

    FlashLight(Shader shader, Camera* lightCam, 
        glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, 
        float near_plane, float far_plane,
        float constant, float linear, float quadratic,
        float cutOff, float outerCutOff)
    {
        this->shader = shader;
        this->depthMap = *(new CubeDepthMap(SHADOW_WIDTH, SHADOW_HEIGHT));
        this->lightCam = lightCam;
        this->position = lightCam->Position;
        this->direction = lightCam->Front;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->near_plane = near_plane;
        this->far_plane = far_plane;
        this->constant = constant;
        this->linear = linear;
        this->quadratic = quadratic;
        this->cutOff = cutOff;
        this->outerCutOff = outerCutOff;
    }

    void updateLightVariables()
    {
        if(lightCam != NULL)
        {
            position = lightCam->Position;
            direction = lightCam->Front;   
        }
        shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        shadowTransforms.clear();
        shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    }

    void addToShader(Shader shader)
    {
        shader.use();
        shader.setVec3("flashLight.position", position);
        shader.setVec3("flashLight.direction", direction);
        shader.setVec3("flashLight.ambient", ambient);
        shader.setVec3("flashLight.diffuse", diffuse);
        shader.setVec3("flashLight.specular", specular);
        shader.setFloat("flashLight.constant", constant);
        shader.setFloat("flashLight.linear", linear);
        shader.setFloat("flashLight.quadratic", quadratic);
        shader.setFloat("flashLight.cutOff", cutOff);
        shader.setFloat("flashLight.outerCutOff", outerCutOff);
    } 

};

#endif

