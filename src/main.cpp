#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>



#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3        (x,y,z)
#include <glm/vec4.hpp> // glm::vec4        (x,y,z,w)
#include <glm/mat4x4.hpp> // glm::mat4
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Shader/cShaderManager.h"
#include "VAOManager/cVAOManager.h"
#include "MeshObj/cMeshObj.h"
#include "Light/cLightManager.h"
#include "GUI/cGUI.h"
#include "Texture/cTextureManager.h"

#define MODEL_LIST_XML          "asset/model.xml"
#define VERTEX_SHADER_FILE      "src/shader/vertexShader.glsl"
#define FRAGMENT_SHADER_FILE    "src/shader/fragmentShader.glsl"
#define TEXTURE_PATH            "asset/texture"

glm::vec3 g_cameraEye = glm::vec3(0.0, 5.0, 0.0f);
glm::vec3 g_cameraTarget = glm::vec3(-16.0f, 47.0f, 0.0f);
glm::vec3 g_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 g_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
bool bIsWalkAround = false;
bool firstMouse = true;
float yaw = -90.0f;	
float pitch = 0.0f;
float lastX = 1280.0f / 2.0;
float lastY = 800.0 / 2.0;
float fov = 45.0f;

cLightManager* g_pTheLightManager = NULL;
static GLFWwindow* window = nullptr;

cTextureManager* g_pTextureManager = NULL;


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void updateInstanceObj(cShaderManager* pShaderManager, cVAOManager* pVAOManager, glm::mat4x4 matView, glm::mat4x4 matProjection);
void drawObj(cMeshObj* pCurrentMeshObject, glm::mat4x4 mat_PARENT_Model, cShaderManager* pShaderManager, cVAOManager* pVAOManager, glm::mat4x4 matView, glm::mat4x4 matProjection);
void light0Setup();
void light1Setup();
void light2Setup();
void light3Setup();
void light4Setup();

int main(void)
{

    bool result;
    std::cout << "starting..\n";
    //GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    //GLint mvp_location, vpos_location, vcol_location;
    GLuint shaderID = 0;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWmonitor* pMainScreen = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(pMainScreen);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    //window = glfwCreateWindow(mode->width, mode->height, "6028 Final", pMainScreen, NULL); //full screen
    window = glfwCreateWindow(1280, 800, "6028 Final", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    std::cout << "created window" << std::endl;

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    //initialize imgui
    cGUI* gui_ = new cGUI(&g_cameraEye,&g_cameraTarget);
    result = gui_->ImGUI_init(window);
    if (!result)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    //create shader program
    cShaderManager* pShaderManager = new cShaderManager();
    cShaderManager::cShader vertexShader;
    cShaderManager::cShader fragmentShader;

    vertexShader.fileName = VERTEX_SHADER_FILE;
    fragmentShader.fileName = FRAGMENT_SHADER_FILE;
    result = pShaderManager->createShaderProgram("Shader01", vertexShader, fragmentShader);
    if (!result)
    {
        std::cout << "error: Shader compilation fail" << std::endl;

        glfwDestroyWindow(window);

        delete pShaderManager;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "shader compilation OK" << std::endl;
    }

    pShaderManager->useShaderPRogram("Shader01");
    shaderID = pShaderManager->getIDfromName("Shader01");
    glUseProgram(shaderID);

    //todo lighting
    ::g_pTheLightManager = new cLightManager();


    
    ::g_pTheLightManager->loadLightUniformLocation(shaderID);
    for (size_t i = 0; i < MAX_LIGHT_SOURCE; i++)
    {
        gui_->pLight[i] = ::g_pTheLightManager->plight[i];
    }
    //load model
    cVAOManager* pVAOManager = new cVAOManager();

    result = pVAOManager->loadModelList(MODEL_LIST_XML, shaderID);
    if (!result)
    {
        std::cout << "cannot load model list" << std::endl;

        glfwDestroyWindow(window);

        delete pVAOManager;
        delete pShaderManager;

        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    ::g_cameraEye = pVAOManager->cameraEyeFromXML;

    //gui_->pMapInstanceNametoMeshObj = &pVAOManager->mapInstanceNametoMeshObj;
    gui_->pVecInstanceMeshObj = &pVAOManager->pVecInstanceMeshObj;

    //load texture
    ::g_pTextureManager = new cTextureManager();
    ::g_pTextureManager->setBasePath(TEXTURE_PATH);
    ::g_pTextureManager->create2DTextureFromBMP("Dungeons_2_Texture_01_A.bmp");
    ::g_pTextureManager->create2DTextureFromBMP("lroc_color_poles_4k.bmp");
    
    std::string load_texture_error = "";
    if (g_pTextureManager->createCubeTextureFromBMP("SpaceBox",
        "SpaceBox_right1_posX.bmp", /* positive X */
        "SpaceBox_left2_negX.bmp",  /* negative X */
        "SpaceBox_top3_posY.bmp",    /* positive Y */
        "SpaceBox_bottom4_negY.bmp",  /* negative Y */
        "SpaceBox_front5_posZ.bmp",  /* positive Z */
        "SpaceBox_back6_negZ.bmp", /* negative Z */
        true, load_texture_error))
    {
        std::cout << "Loaded the tropical sunny day cube map OK" << std::endl;
    }
    else
    {
        std::cout << "ERROR: Didn't load the tropical sunny day cube map.->" << load_texture_error << std::endl;
    }
    //setup object
    //result = pVAOManager->setInstanceObjVisible("terrain01", true);
   // result = pVAOManager->setInstanceObjRGB("floor", glm::vec4(1.f,1.f,1.f,1.f));
   // result = pVAOManager->setInstanceObjSpecularPower("floor", glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f));
   // //result = pVAOManager->setInstanceObjScale("terrain01", 20);
   // result = pVAOManager->setUseRGBColorFlag("floor", false);
   //result = pVAOManager->setTexture("floor", "Dungeons_2_Texture_01_A.bmp", 0);
   result = pVAOManager->setDungeonTexture("floorA", "Dungeons_2_Texture_01_A.bmp");
   result = pVAOManager->setTexture("moon", "lroc_color_poles_4k.bmp", 0);
   result = pVAOManager->setInstanceObjPosition("moon", glm::vec4(200.f,200.f,-100.f,0.f));
   result = pVAOManager->setInstanceObjScale("moon", 10);
   result = pVAOManager->setInstanceObjLighting("moon", false);

   //result = pVAOManager->setTexture("floor", "Dungeons_2_Texture_01_A.bmp", 1);

   //result = pVAOManager->setTextureRatio("floor",0, 1);
   //result = pVAOManager->setTextureRatio("floor", 1, 1);


   // result = pVAOManager->setInstanceObjVisible("sphere01", true);
    //result = pVAOManager->setInstanceObjRGB("traffic", glm::vec4(1.f, 1.f, 1.f, 1.f));
    //result = pVAOManager->setInstanceObjSpecularPower("traffic", glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f));
    //result = pVAOManager->setInstanceObjScale("traffic", 2);

    //result = pVAOManager->setInstanceObjRGB("skybox", glm::vec4(0.8f, 0.8f, 0.8f, 1.f));
    //result = pVAOManager->setInstanceObjSpecularPower("skybox", glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f));
    //result = pVAOManager->setInstanceObjScale("skybox", 0.4);
    result = pVAOManager->setSkyBoxFlag("skybox",true);

    /*result = pVAOManager->setInstanceObjRGB("building02", glm::vec4(1.f, 0.8f, 0.5f, 1.f));
    result = pVAOManager->setInstanceObjSpecularPower("building02", glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f));
    result = pVAOManager->setInstanceObjScale("building02", 0.3);

    result = pVAOManager->setInstanceObjRGB("building03", glm::vec4(0.1f, 0.1f, 1.f, 1.f));
    result = pVAOManager->setInstanceObjSpecularPower("building03", glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f));
    result = pVAOManager->setInstanceObjScale("building03", 0.1);

    result = pVAOManager->setInstanceObjRGB("Electricbox01", glm::vec4(1.f, 0.f, 1.f, 1.f));
    result = pVAOManager->setInstanceObjSpecularPower("Electricbox01", glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f));
    result = pVAOManager->setInstanceObjScale("Electricbox01", 0.4);

    result = pVAOManager->setInstanceObjRGB("mac01", glm::vec4(1.f, 0.f, 0.f, 1.f));
    result = pVAOManager->setInstanceObjSpecularPower("mac01", glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f));
    result = pVAOManager->setInstanceObjScale("mac01", 1);

    result = pVAOManager->setInstanceObjRGB("truck01", glm::vec4(0.65f, 0.45f, 0.f, 1.f));
    result = pVAOManager->setInstanceObjSpecularPower("truck01", glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f));
    result = pVAOManager->setInstanceObjScale("truck01", 0.7);

    result = pVAOManager->setInstanceObjRGB("road01", glm::vec4(1.f, 1.f, 1.f, 1.f));
    result = pVAOManager->setInstanceObjSpecularPower("road01", glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f));
    result = pVAOManager->setInstanceObjScale("road01", 2);

    result = pVAOManager->setInstanceObjRGB("road02", glm::vec4(1.f, 1.f, 1.f, 1.f));
    result = pVAOManager->setInstanceObjSpecularPower("road02", glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f));
    result = pVAOManager->setInstanceObjScale("road02", 2);

    result = pVAOManager->setInstanceObjRGB("lamp01", glm::vec4(1.f, 1.f, 1.f, 1.f));
    result = pVAOManager->setInstanceObjSpecularPower("lamp01", glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f));
    result = pVAOManager->setInstanceObjScale("lamp01", 1.5);
    result = pVAOManager->setInstanceObjRGB("lamp02", glm::vec4(1.f, 1.f, 1.f, 1.f));
    result = pVAOManager->setInstanceObjSpecularPower("lamp02", glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f));
    result = pVAOManager->setInstanceObjScale("lamp02", 1.5);
    result = pVAOManager->setInstanceObjRGB("lamp03", glm::vec4(1.f, 1.f, 1.f, 1.f));
    result = pVAOManager->setInstanceObjSpecularPower("lamp03", glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f));
    result = pVAOManager->setInstanceObjScale("lamp03", 1.5);
    result = pVAOManager->setInstanceObjRGB("lamp04", glm::vec4(1.f, 1.f, 1.f, 1.f));
    result = pVAOManager->setInstanceObjSpecularPower("lamp04", glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f));
    result = pVAOManager->setInstanceObjScale("lamp04",1.5);*/

    //result = pVAOManager->setInstanceObjVisible("light1", false);
    //result = pVAOManager->setInstanceObjVisible("light2", false);
    //result = pVAOManager->setInstanceObjVisible("light3", false);
    //result = pVAOManager->setInstanceObjVisible("light4", false);

    light0Setup(); // Dir light
    //light1Setup();
    //light2Setup();
    //light3Setup();
    //light4Setup();



    while (!glfwWindowShouldClose(window))
    {
        ::g_pTheLightManager->setLightToShader(shaderID);

        float ratio;
        int width, height;
        //glm::mat4x4 matModel;
        glm::mat4x4 matProjection;
        glm::mat4x4 matView;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       

        //glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraDirection = glm::normalize(g_cameraEye - g_cameraTarget);
        glm::vec3 cameraRight = glm::normalize(glm::cross(g_upVector, cameraDirection));
        if (!bIsWalkAround)
        {

            matView = glm::lookAt(::g_cameraEye, ::g_cameraTarget, ::g_upVector);
        }
        else
        {
            matView = glm::lookAt(::g_cameraEye, ::g_cameraEye+::g_cameraFront, ::g_upVector);
        }
        GLint eyeLocation_UniLoc = glGetUniformLocation(shaderID, "eyeLocation");

        glUniform4f(eyeLocation_UniLoc, ::g_cameraEye.x, ::g_cameraEye.y, ::g_cameraEye.z, 1.0f);

        matProjection = glm::perspective(glm::radians(fov), ratio, 0.1f, 10000.0f);
        //matProjection = glm::perspective(0.6f, ratio, 0.1f, 10000.0f);
        //glUniformMatrix4fv(mView_location, 1, GL_FALSE, glm::value_ptr(matView));
        //glUniformMatrix4fv(mProjection_location, 1, GL_FALSE, glm::value_ptr(matProjection));

        updateInstanceObj(pShaderManager, pVAOManager, matView, matProjection);

        gui_->ImGUICreateFrame();

        glfwSwapBuffers(window);
        glfwPollEvents();

        //set window title
        //std::stringstream 

    }

    gui_->ImGUI_shutdown();

    glfwDestroyWindow(window);

    delete pVAOManager;
    delete pShaderManager;

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void updateInstanceObj(cShaderManager* pShaderManager, cVAOManager* pVAOManager, glm::mat4x4 matView, glm::mat4x4 matProjection)
{
    glm::mat4x4 matModel;

    for (std::map<std::string, cMeshObj* >::iterator itCurrentMesh = pVAOManager->mapInstanceNametoMeshObj.begin();
        itCurrentMesh != pVAOManager->mapInstanceNametoMeshObj.end();
        itCurrentMesh++)
    {
        cMeshObj* pCurrentMeshObject = (itCurrentMesh->second);        // * is the iterator access thing

        //pCurrentMeshObject->textureRatios[0] -= 0.001f;
        //pCurrentMeshObject->textureRatios[1] += 0.001f;

        if (!pCurrentMeshObject->isVisible)
        {
            // Skip the rest of the loop
            continue;
        }
        if (pCurrentMeshObject->isIslandModel)
        {
            pShaderManager->setShaderUniform1f("bIsIlandModel", (GLfloat)GL_TRUE);
        }
        if (pCurrentMeshObject->isSkybox)
        {
            pShaderManager->setShaderUniform1f("bIsSkyboxObject", (GLfloat)GL_TRUE);
            pCurrentMeshObject->position = ::g_cameraEye;
            pCurrentMeshObject->scale = 7500.f;
        }
        matModel = glm::mat4x4(1.0f);

        drawObj(pCurrentMeshObject, matModel, pShaderManager, pVAOManager, matView, matProjection);
        if (pCurrentMeshObject->isSkybox)
        {
            pShaderManager->setShaderUniform1f("bIsSkyboxObject", (GLfloat)GL_FALSE);
        }
        if (pCurrentMeshObject->isIslandModel)
        {
            pShaderManager->setShaderUniform1f("bIsIlandModel", (GLfloat)GL_FALSE);
        }
    }
}

void drawObj(cMeshObj* pCurrentMeshObject, glm::mat4x4 mat_PARENT_Model, cShaderManager* pShaderManager, cVAOManager* pVAOManager, glm::mat4x4 matView, glm::mat4x4 matProjection)
{
    // Don't draw any "back facing" triangles
    glCullFace(GL_BACK);
    //glEnable(GL_CULL_FACE);
    // Turn on depth buffer test at draw time
    glEnable(GL_DEPTH_TEST);

    //matModel = glm::mat4x4(1.0f);  // identity matrix

#if 0   //flame effect
    pShaderManager->setShaderUniform1f("bIsFlameObject", (GLfloat)GL_TRUE);
    glDepthMask(GL_FALSE);
#endif
#if 0   //discard texture
    pShaderManager->setShaderUniform1f("bUseDiscardTexture", (GLfloat)GL_TRUE);
#endif

    //GLuint texture07_Number = g_pTextureManager->getTexttureID(pCurrentMeshObject->textures[7]);
    //GLuint texture07_Unit = 7;
    //glActiveTexture(texture07_Unit + GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, texture07_Number);
    //pShaderManager->setShaderUniform1i("texture7", texture07_Unit);

    glm::mat4x4 matModel = mat_PARENT_Model;
    // Move the object 
    glm::mat4 matTranslation = glm::translate(glm::mat4(1.0f), pCurrentMeshObject->position);

    //std::cout << pCurrentMeshObject->instanceName << " position x = " << pCurrentMeshObject->position.x << " y = " << pCurrentMeshObject->position.y << " z = " << pCurrentMeshObject->position.z << std::endl;

    //rotate
    glm::mat4 matRoationZ = glm::rotate(glm::mat4(1.0f), pCurrentMeshObject->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 matRoationY = glm::rotate(glm::mat4(1.0f), pCurrentMeshObject->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 matRoationX = glm::rotate(glm::mat4(1.0f), pCurrentMeshObject->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));

    // Scale the object
    float uniformScale = pCurrentMeshObject->scale;
    glm::mat4 matScale = glm::scale(glm::mat4(1.0f), glm::vec3(uniformScale, uniformScale, uniformScale));

    matModel = matModel * matTranslation;

    matModel = matModel * matRoationX;
    matModel = matModel * matRoationY;
    matModel = matModel * matRoationZ;

    matModel = matModel * matScale;

    pShaderManager->setShaderUniformM4fv("mModel", matModel);
    pShaderManager->setShaderUniformM4fv("mView", matView);
    pShaderManager->setShaderUniformM4fv("mProjection", matProjection);

    glm::mat4 mModelInverseTransform = glm::inverse(glm::transpose(matModel));
    pShaderManager->setShaderUniformM4fv("mModelInverseTranspose", mModelInverseTransform);

    // Wireframe
    if (pCurrentMeshObject->isWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);      // GL_POINT, GL_LINE, GL_FILL
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    pShaderManager->setShaderUniform4f("RGBA_Color",
        pCurrentMeshObject->color_RGBA.r,
        pCurrentMeshObject->color_RGBA.g,
        pCurrentMeshObject->color_RGBA.b,
        pCurrentMeshObject->color_RGBA.w);

    if (pCurrentMeshObject->bUse_RGBA_colour)
    {
        pShaderManager->setShaderUniform1f("bUseRGBA_Color", (GLfloat)GL_TRUE);
    }
    else
    {
        pShaderManager->setShaderUniform1f("bUseRGBA_Color", (GLfloat)GL_FALSE);
    }
    pShaderManager->setShaderUniform4f("specularColour",
        pCurrentMeshObject->specular_colour_and_power.r,
        pCurrentMeshObject->specular_colour_and_power.g,
        pCurrentMeshObject->specular_colour_and_power.b,
        pCurrentMeshObject->specular_colour_and_power.w);

    //uniform bool bDoNotLight;	
    if (pCurrentMeshObject->bDoNotLight)
    {
        pShaderManager->setShaderUniform1f("bDoNotLight", (GLfloat)GL_TRUE);
    }
    else
    {
        pShaderManager->setShaderUniform1f("bDoNotLight", (GLfloat)GL_FALSE);
    }

    //set texture0
    GLuint texture00_Number = ::g_pTextureManager->getTexttureID(pCurrentMeshObject->textures[0]);
    GLuint texture00_Unit = 0;
    glActiveTexture(texture00_Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture00_Number);
    pShaderManager->setShaderUniform1i("texture0", texture00_Unit);
    //set texture1
    GLuint texture01_Number = ::g_pTextureManager->getTexttureID(pCurrentMeshObject->textures[1]);
    GLuint texture01_Unit = 1;
    glActiveTexture(texture01_Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture01_Number);
    pShaderManager->setShaderUniform1i("texture1", texture01_Unit);
    //set texture2
    GLuint texture02_Number = ::g_pTextureManager->getTexttureID(pCurrentMeshObject->textures[2]);
    GLuint texture02_Unit = 2;
    glActiveTexture(texture02_Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture02_Number);
    pShaderManager->setShaderUniform1i("texture2", texture02_Unit);
    //set texture3
    GLuint texture03_Number = ::g_pTextureManager->getTexttureID(pCurrentMeshObject->textures[3]);
    GLuint texture03_Unit = 3;
    glActiveTexture(texture03_Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture03_Number);
    pShaderManager->setShaderUniform1i("texture3", texture03_Unit);
    //set texture4
    GLuint texture04_Number = ::g_pTextureManager->getTexttureID(pCurrentMeshObject->textures[4]);
    GLuint texture04_Unit = 4;
    glActiveTexture(texture04_Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture04_Number);
    pShaderManager->setShaderUniform1i("texture4", texture04_Unit);
    //set texture5
    GLuint texture05_Number = ::g_pTextureManager->getTexttureID(pCurrentMeshObject->textures[5]);
    GLuint texture05_Unit = 5;
    glActiveTexture(texture05_Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture05_Number);
    pShaderManager->setShaderUniform1i("texture5", texture05_Unit);
    //set texture6
    GLuint texture06_Number = ::g_pTextureManager->getTexttureID(pCurrentMeshObject->textures[6]);
    GLuint texture06_Unit = 6;
    glActiveTexture(texture06_Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture06_Number);
    pShaderManager->setShaderUniform1i("texture6", texture06_Unit);

    pShaderManager->setShaderUniform4f("texRatio_0_3",
        pCurrentMeshObject->textureRatios[0],
        pCurrentMeshObject->textureRatios[1],
        pCurrentMeshObject->textureRatios[2],
        pCurrentMeshObject->textureRatios[3]);

    //cube map texture
    GLuint cubeMapTextureNumber = g_pTextureManager->getTexttureID("SpaceBox");
    GLuint texture30Unit = 30;
    glActiveTexture(texture30Unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureNumber);
    pShaderManager->setShaderUniform1i("skyboxTexture", texture30Unit);

    cModelDrawInfo drawingInformation;
    if (pVAOManager->FindDrawInfo(pCurrentMeshObject->meshName, drawingInformation))
    {
        glBindVertexArray(drawingInformation.VAO_ID);

        glDrawElements(GL_TRIANGLES, drawingInformation.numberOfIndices, GL_UNSIGNED_INT, (void*)0);

        glBindVertexArray(0);

    }
    else
    {
        // Didn't find that model
        std::cout << "Error: didn't find model to draw." << std::endl;

    }

    for (std::vector<cMeshObj* >::iterator itCurrentMesh = pCurrentMeshObject->vecChildMesh.begin();
        itCurrentMesh != pCurrentMeshObject->vecChildMesh.end();
        itCurrentMesh++)
    {
        cMeshObj* pCurrentChildMeshObject = *itCurrentMesh;
        drawObj(pCurrentChildMeshObject, matModel, pShaderManager, pVAOManager, matView, matProjection);
    }
}

void light0Setup() //lamp
{
    //Directional light
    ::g_pTheLightManager->plight[0]->direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    ::g_pTheLightManager->plight[0]->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::g_pTheLightManager->plight[0]->specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    ::g_pTheLightManager->plight[0]->type = cLight::LightType::LIGHT_DIRECTION;
    ::g_pTheLightManager->plight[0]->turnON = 1;


    //::g_pTheLightManager->plight[1]->type = cLight::LightType::LIGHT_POINT;
    //::g_pTheLightManager->plight[1]->diffuse = glm::vec4(100.0f, 100.f, 100.f, 1.0f);
    //::g_pTheLightManager->plight[1]->position = glm::vec4(260.f, 200.f, -300.f, 1.0f);
    //::g_pTheLightManager->plight[1]->attenuation = glm::vec4(1.0f, 0.001f, 0.00001f, 1.0f);
    //::g_pTheLightManager->plight[1]->turnON = 1; 

    //::g_pTheLightManager->plight[0]->position = glm::vec4(6.8f, 2.7f, -1.8f, 1.0f);
    //::g_pTheLightManager->plight[0]->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //::g_pTheLightManager->plight[0]->attenuation = glm::vec4(0.19f, 0.003f, 0.072f, 1.0f);
    //::g_pTheLightManager->plight[0]->type = cLight::LightType::LIGHT_SPOT;
    //::g_pTheLightManager->plight[0]->direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    //// inner and outer angles
    //::g_pTheLightManager->plight[0]->angle.x = 10.0f;     // Degrees
    //::g_pTheLightManager->plight[0]->angle.y = 20.0f;     // Degrees
    //::g_pTheLightManager->plight[0]->turnON = 1;

    //::g_pTheLightManager->plight[1]->position = glm::vec4(13.35f, 2.7f, 5.02f, 1.0f);
    //::g_pTheLightManager->plight[1]->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //::g_pTheLightManager->plight[1]->attenuation = glm::vec4(0.19f, 0.003f, 0.072f, 1.0f);
    //::g_pTheLightManager->plight[1]->type = cLight::LightType::LIGHT_SPOT;
    //::g_pTheLightManager->plight[1]->direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    //// inner and outer angles
    //::g_pTheLightManager->plight[1]->angle.x = 10.0f;     // Degrees
    //::g_pTheLightManager->plight[1]->angle.y = 20.0f;     // Degrees
    //::g_pTheLightManager->plight[1]->turnON = 1;

    //::g_pTheLightManager->plight[2]->position = glm::vec4(14.75f, 2.7f, 13.15f, 1.0f);
    //::g_pTheLightManager->plight[2]->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //::g_pTheLightManager->plight[2]->attenuation = glm::vec4(0.19f, 0.003f, 0.072f, 1.0f);
    //::g_pTheLightManager->plight[2]->type = cLight::LightType::LIGHT_SPOT;
    //::g_pTheLightManager->plight[2]->direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    //// inner and outer angles
    //::g_pTheLightManager->plight[2]->angle.x = 10.0f;     // Degrees
    //::g_pTheLightManager->plight[2]->angle.y = 20.0f;     // Degrees
    //::g_pTheLightManager->plight[2]->turnON = 1;

    //::g_pTheLightManager->plight[3]->position = glm::vec4(1.95f, 2.7f, -0.75f, 1.f);
    //::g_pTheLightManager->plight[3]->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //::g_pTheLightManager->plight[3]->attenuation = glm::vec4(0.19f, 0.003f, 0.072f, 1.0f);
    //::g_pTheLightManager->plight[3]->type = cLight::LightType::LIGHT_SPOT;
    //::g_pTheLightManager->plight[3]->direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    //// inner and outer angles
    //::g_pTheLightManager->plight[3]->angle.x = 10.0f;     // Degrees
    //::g_pTheLightManager->plight[3]->angle.y = 20.0f;     // Degrees
    //::g_pTheLightManager->plight[3]->turnON = 1;
}

void light1Setup()
{
    //cDirLight* pDirLight = new cDirLight(*::g_pTheLightManager->plight[1]);
    //*pDirLight->pDirection = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    //*pDirLight->pDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //*pDirLight->pSpecular = glm::vec4(0.f, 0.5f, 0.5f, 1.f);
    //*pDirLight->pTurnON = 1;
    ::g_pTheLightManager->plight[4]->type = cLight::LightType::LIGHT_POINT;
    ::g_pTheLightManager->plight[4]->diffuse = glm::vec4(12.0f, 0.3f, 0.3f, 1.0f);
    ::g_pTheLightManager->plight[4]->position = glm::vec4(18.75f, 14.f, 3.65f, 1.0f);
    ::g_pTheLightManager->plight[4]->attenuation = glm::vec4(5.0f, 2.f, 0.5f, 1.0f);
    ::g_pTheLightManager->plight[4]->turnON = 1;



}
void light2Setup()
{
    cDirLight* pDirLight = new cDirLight(*::g_pTheLightManager->plight[5]);
    *pDirLight->pDirection = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    *pDirLight->pDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    *pDirLight->pSpecular = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
    *pDirLight->pTurnON = 1;
}

void light3Setup()
{

    ::g_pTheLightManager->plight[6]->position = glm::vec4(15.6f, 0.6f, 8.7f, 1.0f);
    ::g_pTheLightManager->plight[6]->diffuse = glm::vec4(0.8f, .50f, 0.4f, 1.0f);
    ::g_pTheLightManager->plight[6]->attenuation = glm::vec4(0.19f, 0.003f, 0.072f, 1.0f);
    ::g_pTheLightManager->plight[6]->type = cLight::LightType::LIGHT_SPOT;
    ::g_pTheLightManager->plight[6]->direction = glm::vec4(0.07f, -0.5f, 1.0f, 1.0f);
    // inner and outer angles
    ::g_pTheLightManager->plight[6]->angle.x = 10.0f;     // Degrees
    ::g_pTheLightManager->plight[6]->angle.y = 20.0f;     // Degrees
    ::g_pTheLightManager->plight[6]->turnON = 1;

    ::g_pTheLightManager->plight[7]->position = glm::vec4(15.0f, 0.6f, 9.5f, 1.0f);
    ::g_pTheLightManager->plight[7]->diffuse = glm::vec4(0.8f, .50f, 0.4f, 1.0f);
    ::g_pTheLightManager->plight[7]->attenuation = glm::vec4(0.19f, 0.003f, 0.072f, 1.0f);
    ::g_pTheLightManager->plight[7]->type = cLight::LightType::LIGHT_SPOT;
    ::g_pTheLightManager->plight[7]->direction = glm::vec4(0.07f, -0.5f, 1.0f, 1.0f);
    // inner and outer angles
    ::g_pTheLightManager->plight[7]->angle.x = 10.0f;     // Degrees
    ::g_pTheLightManager->plight[7]->angle.y = 20.0f;     // Degrees
    ::g_pTheLightManager->plight[7]->turnON = 1;

}

void light4Setup()
{
    ::g_pTheLightManager->plight[8]->position = glm::vec4(11.9f, 0.4f, 5.f, 1.0f);
    ::g_pTheLightManager->plight[8]->diffuse = glm::vec4(1.f, 1.f, 1.f, 1.0f);
    ::g_pTheLightManager->plight[8]->attenuation = glm::vec4(0.19f, 0.003f, 0.072f, 1.0f);
    ::g_pTheLightManager->plight[8]->type = cLight::LightType::LIGHT_SPOT;
    ::g_pTheLightManager->plight[8]->direction = glm::vec4(0.07f, -0.5f, 1.0f, 1.0f);
    // inner and outer angles    
    ::g_pTheLightManager->plight[8]->angle.x = 10.0f;     // Degrees
    ::g_pTheLightManager->plight[8]->angle.y = 20.0f;     // Degrees
    ::g_pTheLightManager->plight[8]->turnON = 1;

    ::g_pTheLightManager->plight[9]->position = glm::vec4(11.2f, 0.6f, 5.7f, 1.0f);
    ::g_pTheLightManager->plight[9]->diffuse = glm::vec4(1.f, 1.f, 1.f, 1.0f);
    ::g_pTheLightManager->plight[9]->attenuation = glm::vec4(0.19f, 0.003f, 0.072f, 1.0f);
    ::g_pTheLightManager->plight[9]->type = cLight::LightType::LIGHT_SPOT;
    ::g_pTheLightManager->plight[9]->direction = glm::vec4(0.07f, -0.5f, 1.0f, 1.0f);
    // inner and outer angle
    ::g_pTheLightManager->plight[9]->angle.x = 10.0f;     // Degrees
    ::g_pTheLightManager->plight[9]->angle.y = 20.0f;     // Degrees
    ::g_pTheLightManager->plight[9]->turnON = 1;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    //move camera
    // AWSD AD-Left, Right
    //      WS-Forward, Back
    const float CAMERA_MOVE_SPEED = 1.f;
    if (key == GLFW_KEY_A)
    {
        ::g_cameraEye.x -= CAMERA_MOVE_SPEED;
    }
    if (key == GLFW_KEY_D)
    {
        ::g_cameraEye.x += CAMERA_MOVE_SPEED;
    }
    if (key == GLFW_KEY_W)
    {
        ::g_cameraEye.z -= CAMERA_MOVE_SPEED;
    }
    if (key == GLFW_KEY_S)
    {
        ::g_cameraEye.z += CAMERA_MOVE_SPEED;
    }
    if (key == GLFW_KEY_Q)
    {
        ::g_cameraEye.y -= CAMERA_MOVE_SPEED;
    }
    if (key == GLFW_KEY_E)
    {
        ::g_cameraEye.y += CAMERA_MOVE_SPEED;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    {
        //::g_cameraEye = glm::vec3(-5.5f, -3.4f, 15.0f);
        //::g_cameraEye = glm::vec3(0.0, 100.0, 300.0f);
        //::g_cameraTarget = glm::vec3(5.0f, 0.0f, 0.0f);
        bIsWalkAround = !bIsWalkAround;

    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    if (bIsWalkAround)
    {


        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f; // change this value to your liking
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        ::g_cameraFront = glm::normalize(front);
    }
    else
    {
        ::g_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

