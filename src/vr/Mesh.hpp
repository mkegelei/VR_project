#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Shader.hpp>
#include <Light.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    unsigned int VAO;

    /*  Functions  */
    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    void Draw(Shader shader) 
    {
        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int reflectionNr = 1;
        unsigned int emissionNr = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to stream
            else if(name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to stream
            else if(name == "texture_reflection")
                number = std::to_string(reflectionNr++); // transfer unsigned int to stream
            else if(name == "texture_emission")
                number = std::to_string(emissionNr++); // transfer unsigned int to stream

                                                     // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, ("material." + name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

    void DrawWithShadow(Shader shader, DirLight* dirLight, vector<PointLight*> pointLights, vector<FlashLight*> flashLights, unsigned int skybox) 
    {
        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int reflectionNr   = 1;
        unsigned int emissionNr = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to stream
            else if(name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to stream
            else if(name == "texture_reflection")
                number = std::to_string(reflectionNr++); // transfer unsigned int to stream
            else if(name == "texture_emission")
                number = std::to_string(emissionNr++); // transfer unsigned int to stream

                                                     // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, ("material." + name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        unsigned int j = textures.size();
        //add shadow maps
        glActiveTexture(GL_TEXTURE0 + j);
        glUniform1i(glGetUniformLocation(shader.ID, "dirShadowMap"), j);
        glBindTexture(GL_TEXTURE_2D, dirLight->depthMap.map);
        j++;

        for (unsigned int i = 0; i < pointLights.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + j);
            glUniform1i(glGetUniformLocation(shader.ID, ("pointLights["+std::to_string(i)+"].pointShadowMap").c_str()), j);
            glBindTexture(GL_TEXTURE_CUBE_MAP, pointLights[i]->depthMap.map);
            j++;
        }
        
        for (unsigned int i = 0; i < flashLights.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + j);
            glUniform1i(glGetUniformLocation(shader.ID, ("flashLights["+std::to_string(i)+"].flashShadowMap").c_str()), j);
            glBindTexture(GL_TEXTURE_CUBE_MAP, flashLights[i]->depthMap.map);
            j++;
        }
        glActiveTexture(GL_TEXTURE0 + j);
        glUniform1i(glGetUniformLocation(shader.ID, "skybox"), j);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
        
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

    void DrawAsteroid(Shader shader, DirLight* dirLight, vector<PointLight*> pointLights, vector<FlashLight*> flashLights, unsigned int diffuse, unsigned int specular, unsigned int normal, unsigned int disparity, unsigned int emission, unsigned int skybox) 
    {
        unsigned int j = 0;

        glActiveTexture(GL_TEXTURE0 + j);
        glUniform1i(glGetUniformLocation(shader.ID, "material.texture_diffuse1"), j);
        glBindTexture(GL_TEXTURE_2D, diffuse);
        j++;
        glActiveTexture(GL_TEXTURE0 + j);
        glUniform1i(glGetUniformLocation(shader.ID, "material.texture_specular1"), j);
        glBindTexture(GL_TEXTURE_2D, specular);
        j++;
        glActiveTexture(GL_TEXTURE0 + j);
        glUniform1i(glGetUniformLocation(shader.ID, "material.texture_normal1"), j);
        glBindTexture(GL_TEXTURE_2D, normal);
        j++;
        glActiveTexture(GL_TEXTURE0 + j);
        glUniform1i(glGetUniformLocation(shader.ID, "material.texture_height1"), j);
        glBindTexture(GL_TEXTURE_2D, disparity);
        j++;
        glActiveTexture(GL_TEXTURE0 + j);
        glUniform1i(glGetUniformLocation(shader.ID, "material.texture_emission1"), j);
        glBindTexture(GL_TEXTURE_2D, emission);
        j++;

        //add shadow maps
        glActiveTexture(GL_TEXTURE0 + j);
        glUniform1i(glGetUniformLocation(shader.ID, "dirShadowMap"), j);
        glBindTexture(GL_TEXTURE_2D, dirLight->depthMap.map);
        j++;

        for (unsigned int i = 0; i < pointLights.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + j);
            glUniform1i(glGetUniformLocation(shader.ID, ("pointLights["+std::to_string(i)+"].pointShadowMap").c_str()), j);
            glBindTexture(GL_TEXTURE_CUBE_MAP, pointLights[i]->depthMap.map);
            j++;
        }
        
        for (unsigned int i = 0; i < flashLights.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + j);
            glUniform1i(glGetUniformLocation(shader.ID, ("flashLights["+std::to_string(i)+"].flashShadowMap").c_str()), j);
            glBindTexture(GL_TEXTURE_CUBE_MAP, flashLights[i]->depthMap.map);
            j++;
        }
        glActiveTexture(GL_TEXTURE0 + j);
        glUniform1i(glGetUniformLocation(shader.ID, "skybox"), j);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
        
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

    void DrawForDepth() 
    {   
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

    }

private:
    /*  Render data  */
    unsigned int VBO, EBO;

    /*  Functions    */
    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);   
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);   
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);   
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }
};
#endif

