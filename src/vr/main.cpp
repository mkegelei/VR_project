#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Shader.hpp>
#include <Camera.hpp>
#include <Model.hpp>
#include <DepthMap.hpp>
#include <Light.hpp>
#include <Circuit.hpp>

#include <iostream>

// Prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path, bool gamma = false);
Shader createShader(const char* vert, const char* frag, const char* geom = nullptr);
void renderDepthMap(DirLight* light);
void renderDepthMap(PointLight* light);
void renderDepthMap(FlashLight* light);
void renderSceneForDepth(Shader shader);
void renderFloor();
void renderQuad();

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;
const float near_plane = 1.0f;
const float far_plane = 25.0f;
const char* dir;
const char* objName;
float heightScale = 0.1;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
Camera lightPos(glm::vec3(1.2f, 1.0f, 2.0f));
//std::vector<DirLight> dirLights;
DirLight* dirLight;
unsigned int plId = 0;
std::vector<PointLight*> pointLights;
unsigned int flId = 0;
std::vector<FlashLight*> flashLights;

//models
Model ourModel;

int main(int argc, char *argv[])
{
    if(argc > 1){
        dir = argv[1];
        if(argc > 2){
            objName = argv[2];
        }
        else{
            objName = "cyborg/cyborg.obj";
        }
    }
    else{
        dir = "";
    }

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader modelShader = createShader("test1.vert", "test1.frag");
    Shader floorShader = createShader("floor.vert", "floor.frag");

    Shader debugDepth = createShader("debugDepth.vert", "debugDepth.frag");
    debugDepth.use();
    debugDepth.setInt("depthMap", 0);

    Shader depthShader = createShader("depthShader.vert", "depthShader.frag");

    Shader depthCubeShader = createShader("depthCubeShader.vert", "depthCubeShader.frag", "depthCubeShader.geom");

    Shader circuitShader = createShader("circuitBTN.vert", "circuitBTN.frag", "circuitBTN.geom");
    //Shader circuitShader = createShader("circuit.vert", "circuit.frag");
    // load models
    // -----------
    stringstream ss;
    ss.str("");
    ss << dir << "resources/objects/" << objName;
    ourModel = *(new Model(ss.str()));

    Circuit circuit = Circuit();

    // load additionnal textures
    // -------------------------
    ss.str("");
    ss << dir << "resources/textures/wood.png";
    unsigned int woodTexture = loadTexture(ss.str().c_str());
    ss.str("");
    ss << dir << "resources/textures/brickwall.jpg";
    unsigned int brickwallTexture = loadTexture(ss.str().c_str());
    ss.str("");
    ss << dir << "resources/textures/brickwall_normal.jpg";
    unsigned int brickwallNormalTexture = loadTexture(ss.str().c_str());
    ss.str("");
    ss << dir << "resources/textures/toy_box_diffuse.png";
    unsigned int toyboxTexture = loadTexture(ss.str().c_str());
    ss.str("");
    ss << dir << "resources/textures/toy_box_normal.png";
    unsigned int toyboxNormalTexture = loadTexture(ss.str().c_str());
    ss.str("");
    ss << dir << "resources/textures/toy_box_disp.png";
    unsigned int toyboxDispTexture = loadTexture(ss.str().c_str());
    //unsigned int emissionMap = loadTexture(ss4.str().c_str());

    // Shadow
    // ------
    // configure depth map FBO
    dirLight = new DirLight(0, depthShader, glm::vec3(-2.0f, 4.0f, -1.0f),
        glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.5f, 0.5f, 0.5f),
        near_plane, far_plane);

    // configure cube depth map FBO
    PointLight* pointLight = new PointLight(0, depthCubeShader, &lightPos,
        glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f),
        near_plane, far_plane,
        1.0f, 0.09f, 0.032f);
    pointLights.push_back(pointLight);

    // configure cube depth map FBO for flashlight
    FlashLight* flashLight = new FlashLight(0, depthCubeShader, &camera,
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
        near_plane, far_plane,
        1.0f, 0.09f, 0.032f,
        glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));
    flashLights.push_back(flashLight);

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. render depth of scene to texture (from light's perspective)
        // --------------------------------------------------------------

        // render scene from light's point of view
        renderDepthMap(dirLight);

        // point shadows rendering
        for (unsigned int i = 0; i < pointLights.size(); ++i)
        {
            renderDepthMap(pointLights[i]);
        }

        // point shadows rendering for flashlight
        for (unsigned int i = 0; i < flashLights.size(); ++i)
        {
            renderDepthMap(flashLights[i]);
        }

        // 2. render scene as normal using the generated depth/shadow map
        // --------------------------------------------------------------
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        modelShader.use();

        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setFloat("material.shininess", 16.0f);
        modelShader.setFloat("far_plane", far_plane);
        modelShader.setFloat("heightScale", heightScale);
        modelShader.setInt("parallax", 0);
        modelShader.setInt("material.texture_height1", 0);
        
        // directional light
        dirLight->addToShader(modelShader);

        // point light 1
        for (unsigned int i = 0; i < pointLights.size(); ++i)
        {
            pointLights[i]->addToShader(modelShader);
        }

        // flashLight
        for (unsigned int i = 0; i < flashLights.size(); ++i)
        {
            flashLights[i]->addToShader(modelShader);
        }

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        // render the loaded model
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f)); // it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);

        glm::mat3 normalMatrix = glm::mat3(transpose(inverse(model)));
        modelShader.setMat3("normalMatrix", normalMatrix);

        ourModel.DrawWithShadow(modelShader, dirLight, pointLights, flashLights);

        // Draw brickwall
        modelShader.setInt("parallax", 1);
        modelShader.setFloat("material.shininess", 2.0f);

        modelShader.setInt("material.texture_diffuse1", 0);
        modelShader.setInt("material.texture_specular1", 0);
        modelShader.setInt("material.texture_normal1", 4);
        modelShader.setInt("material.texture_height1", 5);
        modelShader.setInt("dirShadowMap", 1);
        modelShader.setInt("pointLights[0].pointShadowMap", 2);
        modelShader.setInt("flashLights[0].flashShadowMap", 3);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, toyboxTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, dirLight->depthMap.map);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, pointLights[0]->depthMap.map);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, flashLights[0]->depthMap.map);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, toyboxNormalTexture);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, toyboxDispTexture);
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(3.0f, 0.5f, -2.0f));
        modelShader.setMat4("model", model);
        
        normalMatrix = glm::mat3(transpose(inverse(model)));
        modelShader.setMat3("normalMatrix", normalMatrix);
        renderQuad();

        // Draw floor
        floorShader.use();

        floorShader.setVec3("viewPos", camera.Position);
        floorShader.setFloat("material.shininess", 2.0f);
        floorShader.setFloat("far_plane", far_plane);
        
        // directional light
        dirLight->addToShader(floorShader);

        // point light 1
        for (unsigned int i = 0; i < pointLights.size(); ++i)
        {
            pointLights[i]->addToShader(floorShader);
        }
        
        // flashLight
        for (unsigned int i = 0; i < flashLights.size(); ++i)
        {
            flashLights[i]->addToShader(floorShader);
        }
        
        // view/projection transformations
        floorShader.setMat4("projection", projection);
        floorShader.setMat4("view", view);

        
        floorShader.setInt("material.texture_diffuse1", 0);
        floorShader.setInt("material.texture_specular1", 0);
        floorShader.setInt("dirShadowMap", 1);
        floorShader.setInt("pointLights[0].pointShadowMap", 2);
        floorShader.setInt("flashLights[0].flashShadowMap", 3);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, dirLight->depthMap.map);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, pointLights[0]->depthMap.map);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, flashLights[0]->depthMap.map);
        model = glm::mat4();
        floorShader.setMat4("model", model);
        normalMatrix = glm::mat3(transpose(inverse(model)));
        floorShader.setMat3("normalMatrix", normalMatrix);
        renderFloor();

        // Debug depth map
        // ---------------------------------------------
        debugDepth.use();
        debugDepth.setFloat("near_plane", 1.0f);
        debugDepth.setFloat("far_plane", 25.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, dirLight->depthMap.map);
        //renderQuad();

        // circuit
        // -------

        circuitShader.use();
        // view/projection transformations
        circuitShader.setMat4("projection", projection);
        circuitShader.setMat4("view", view);

        // render the loaded model
        model = glm::mat4();
        //model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f)); // it's a bit too big for our scene, so scale it down
        circuitShader.setMat4("model", model);
        circuit.Draw();


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // Moving light
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        lightPos.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        lightPos.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        lightPos.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        lightPos.ProcessKeyboard(RIGHT, deltaTime);

    // Heigth scale
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        if (heightScale > 0.0f)
            heightScale -= 0.0005f;
        else
            heightScale = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        if (heightScale < 1.0f)
            heightScale += 0.0005f;
        else
            heightScale = 1.0f;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* /*window*/, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const * path, bool gamma)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        if(gamma)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

Shader createShader(const char* vert, const char* frag, const char* geom)
{
    stringstream ss1;
    stringstream ss2;
    Shader* shader;
    ss1 << dir << "src/shaders/" << vert;
    ss2 << dir << "src/shaders/" << frag;
    if(geom != nullptr)
    {
        stringstream ss3;
        ss3 << dir << "src/shaders/" << geom;
        shader = new Shader(ss1.str().c_str(), ss2.str().c_str(), ss3.str().c_str());
    }
    else
    {
        shader = new Shader(ss1.str().c_str(), ss2.str().c_str());
    }

    return *shader;
}

void renderDepthMap(DirLight* dirLight)
{
    dirLight->updateLightVariables();
    dirLight->shader.use();
    dirLight->shader.setMat4("lightSpaceMatrix", dirLight->lightSpaceMatrix);

    glBindFramebuffer(GL_FRAMEBUFFER, dirLight->depthMap.FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, dirLight->SHADOW_WIDTH, dirLight->SHADOW_HEIGHT);
    renderSceneForDepth(dirLight->shader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderDepthMap(PointLight* light)
{
    light->updateLightVariables();
    light->shader.use();
    for (unsigned int i = 0; i < 6; ++i)
        light->shader.setMat4("shadowMatrices[" + std::to_string(i) + "]", light->shadowTransforms[i]);
    light->shader.setFloat("far_plane", light->far_plane);
    light->shader.setVec3("lightPos", light->position);

    glViewport(0, 0, light->SHADOW_WIDTH, light->SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, light->depthMap.FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderSceneForDepth(light->shader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void renderDepthMap(FlashLight* light)
{
    light->updateLightVariables();
    light->shader.use();
    for (unsigned int i = 0; i < 6; ++i)
        light->shader.setMat4("shadowMatrices[" + std::to_string(i) + "]", light->shadowTransforms[i]);
    light->shader.setFloat("far_plane", light->far_plane);
    light->shader.setVec3("lightPos", light->position);

    glViewport(0, 0, light->SHADOW_WIDTH, light->SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, light->depthMap.FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderSceneForDepth(light->shader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderSceneForDepth(Shader shader)
{
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f)); // it's a bit too big for our scene, so scale it down
    //shader.use();
    shader.setMat4("model", model);
    glCullFace(GL_FRONT);
    ourModel.DrawForDepth();
    glCullFace(GL_BACK);
    model = glm::mat4();
    shader.setMat4("model", model);
    renderFloor();
    model = glm::mat4();
    model = glm::translate(model, glm::vec3(3.0f, 0.0f, -2.0f));
    shader.setMat4("model", model);
    renderQuad();
}

// Floor
// -----
unsigned int planeVAO = 0;
unsigned int planeVBO;
void renderFloor()
{
    if (planeVAO == 0)
    {
        float planeVertices[] = {
            // positions            // normals         // texcoords
             25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,  
            -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,  
            -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,  

             25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,  
            -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,  
             25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
        };
        // plane VAO
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindVertexArray(0);
    }
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        // positions
        glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
        glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);  
        glm::vec2 uv4(1.0f, 1.0f);
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // triangle 1
        // ----------
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        // triangle 2
        // ----------
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent2 = glm::normalize(tangent2);


        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent2 = glm::normalize(bitangent2);


        float quadVertices[] = {
            // positions            // normal         // texcoords  // tangent                          // bitangent
            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };
        // configure plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
