#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "MeshObj/cMeshObj.h"
#include <iostream>
#include "FBO/cFBO.h"
#include "Animation/AnimationManager.h"

extern cMeshObj* g_MeshBoss;
extern glm::vec3 g_cameraEye;
extern glm::vec3 g_upVector;
extern glm::vec3 g_cameraFront;
extern bool toggleblur;
extern bool bIsWalkAround;
extern bool firstMouse;
extern float yaw;
extern float pitch;
extern float lastX;
extern float lastY;
extern float fov;
extern cFBO* g_FBO_01;
extern AnimationManager* g_pAnimationManager;
extern bool g_PlayAnimation;
extern unsigned int g_AnimationSeq;

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    //move camera
    // AWSD AD-Left, Right
    //      WS-Forward, Back
    const float CAMERA_MOVE_SPEED = 5.f;
    if (key == GLFW_KEY_LEFT)
    {
        //::g_cameraEye.x -= CAMERA_MOVE_SPEED;
        //::g_cameraEye += (glm::normalize(glm::cross(g_upVector, (::g_cameraFront * glm::vec3(1, 0, 1)) * CAMERA_MOVE_SPEED)));
        //g_MeshBoss->position.x -= 0.1;
    }
    if (key == GLFW_KEY_RIGHT)
    {
        //::g_cameraEye.x += CAMERA_MOVE_SPEED;
        //::g_cameraEye -= (glm::normalize(glm::cross(g_upVector, (::g_cameraFront * glm::vec3(1, 0, 1)) * CAMERA_MOVE_SPEED)));
        //g_MeshBoss->position.x += 0.1;
    }
    if (key == GLFW_KEY_UP)
    {
        //::g_cameraEye.z -= CAMERA_MOVE_SPEED;
        //::g_cameraEye += ((::g_cameraFront * glm::vec3(1, 0, 1)) * CAMERA_MOVE_SPEED);
        //g_MeshBoss->position.z -= 0.1;
    }
    if (key == GLFW_KEY_DOWN)
    {
        //::g_cameraEye.z += CAMERA_MOVE_SPEED;
        //::g_cameraEye -= ((::g_cameraFront * glm::vec3(1, 0, 1)) * CAMERA_MOVE_SPEED);
        //g_MeshBoss->position.z += 0.1;
    }
    if (key == GLFW_KEY_A)
    {
        //::g_cameraEye += (glm::normalize(glm::cross(g_upVector, (::g_cameraFront * glm::vec3(1, 0, 1)) * CAMERA_MOVE_SPEED)));
    }
    if (key == GLFW_KEY_D)
    {
        //::g_cameraEye -= (glm::normalize(glm::cross(g_upVector, (::g_cameraFront * glm::vec3(1, 0, 1)) * CAMERA_MOVE_SPEED)));
    }
    if (key == GLFW_KEY_W)
    {
        //::g_cameraEye += ((::g_cameraFront * glm::vec3(1, 0, 1)) * CAMERA_MOVE_SPEED);
    }
    if (key == GLFW_KEY_S)
    {
        //::g_cameraEye -= ((::g_cameraFront * glm::vec3(1, 0, 1)) * CAMERA_MOVE_SPEED);
    }
    if (key == GLFW_KEY_Q)
    {
        //::g_cameraEye.y -= CAMERA_MOVE_SPEED;
    }
    if (key == GLFW_KEY_E)
    {
        //::g_cameraEye.y += CAMERA_MOVE_SPEED;
    }
    if (key == GLFW_KEY_1 && action == GLFW_RELEASE)
    {
        g_AnimationSeq = 0;
        g_pAnimationManager->setSequence(g_AnimationSeq);

    }
    if (key == GLFW_KEY_2 && action == GLFW_RELEASE)
    {
        g_AnimationSeq = 1;
        g_pAnimationManager->setSequence(g_AnimationSeq);
    }
    if (key == GLFW_KEY_3 && action == GLFW_RELEASE)
    {
        g_AnimationSeq = 2;
        g_pAnimationManager->setSequence(g_AnimationSeq);
    }
    if (key == GLFW_KEY_4 && action == GLFW_RELEASE)
    {
        g_AnimationSeq = 3;
        g_pAnimationManager->setSequence(g_AnimationSeq);
    }
    if (key == GLFW_KEY_5 && action == GLFW_RELEASE)
    {
        g_AnimationSeq = 4;
        g_pAnimationManager->setSequence(g_AnimationSeq);
    }
    if (key == GLFW_KEY_C && action == GLFW_RELEASE)
    {
        g_pAnimationManager->continuePlay = !g_pAnimationManager->continuePlay;
        if (g_pAnimationManager->continuePlay)
        {
            std::cout << "coninue play enable" << std::endl;
        }
        else
        {
            std::cout << "coninue play disable" << std::endl;
        }

    }
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    {
        //::g_cameraEye = glm::vec3(-5.5f, -3.4f, 15.0f);
        //::g_cameraEye = glm::vec3(0.0, 100.0, 300.0f);
        //::g_cameraTarget = glm::vec3(5.0f, 0.0f, 0.0f);
        g_PlayAnimation = !g_PlayAnimation;
        g_pAnimationManager->play(g_PlayAnimation);
        if (g_PlayAnimation)
        {
            std::cout << "Play Animation" << std::endl;
        }
        else
        {
            std::cout << "Pause Animation" << std::endl;
        }

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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        std::cout << "R click" << std::endl;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        std::cout << "L click" << std::endl;
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

void window_size_callback(GLFWwindow* window, int width, int height)
{
    std::string error = "";
    bool result = ::g_FBO_01->reset(width, height, error);

    if (!result)
    {
        std::cout << "Error: FBO cannot reset: " << error << std::endl;
    }
}

