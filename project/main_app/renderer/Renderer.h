#ifndef RENDERER_H
#define RENDERER_H
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <main_app/texture/Texture.h>
#include <shader/shader_m.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Renderer
{

private:

    GLFWwindow* window = nullptr;

    GLuint VBO2D;
    GLuint VAO2D;
    GLuint EBO2D;

    GLuint VBO3D;
    GLuint VAO3D;
    GLuint EBO3D;

    GLuint VBO2DDebug;
    GLuint VAO2DDebug;
    GLuint EBO2DDebug;

    /*Used ONLY for DebugDraw*/
    /* TOP LEFT->  *---------------------------------* <-TOP RIGHT
                   |                                 |
                   |                                 |
                   |                                 |
                   |                                 |
    BOTTOM LEFT -> *---------------------------------* <-BOTTOM RIGHT */

    /*Every cicle send the coords to the video card via VBO*/
    glm::vec2 m_vec2TopRight = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_vec2TopLeft = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_vec2BottomLeft = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_vec2BottomRight = glm::vec2(0.0f, 0.0f);

    glm::vec2 m_vec2TextCoordTopLeft = glm::vec2(0.0f, 1.0f);
    glm::vec2 m_vec2TextCoordTopRight = glm::vec2(1.0f, 1.0f);
    glm::vec2 m_vec2TextCoordBottomRight = glm::vec2(1.0f, 0.0f);
    glm::vec2 m_vec2TextCoordBottomLeft = glm::vec2(0.0f, 0.0f);

    glm::vec4 m_vColor = glm::vec4(1.0, 1.0f, 1.0f, 1.0f);

    void SetGLFWCallbacks();
public:
    Renderer();

    unsigned int SCREEN_WIDTH = 1920;
    unsigned int SCREEN_HEIGHT = 1080;
    unsigned int SCREEN_DEPTH = 100;

    GLFWwindow* GetWindow(){ return window; }
    bool InitSystem(const std::string& strTitle,
                    unsigned int nScreenWidth, unsigned int nScreenHeight,
                    unsigned int nScreenDepth);
    void Deinit();
    void CreateAndFill2DBuffers();
    void CreateAndFill3DBuffers();
    void CallEventsAndSwapBuffers();

    void SendVertsToVBODebug(const glm::vec2& vec2TopLeft,
                             const glm::vec2& vec2TopRight,
                             const glm::vec2& vec2BottomRight,
                             const glm::vec2& vec2BottomLeft);

    void SendVertsToVBODebug();

    void SendTextureCoordsToVBODebug(const glm::vec2& vec2TextCoordTopLeft = glm::vec2(0.0f, 1.0f),
                                    const glm::vec2& vec2TextCoordTopRight = glm::vec2(1.0f, 1.0f),
                                    const glm::vec2& vec2TextCoordBottomRight = glm::vec2(1.0f, 0.0f),
                                    const glm::vec2& vec2TextCoordBottomLeft = glm::vec2(0.0f, 0.0f));

    void SetColor(float fRed = 1.0f, float fGreen = 1.0f, float fBlue = 1.0f, float fAlpha = 1.0f);

    /*2D*/
    void DrawLine(float fXInitial, float fYInitial, float fXFinal, float fYFinal, float fThickness, Shader& shaderLine);
    void DrawLine(float fXInitial, float fYInitial, float fXFinal, float fYFinal, float fThickness, Shader& shaderLine, std::shared_ptr<Texture> ptrTexture);
    void DrawRect(float fX, float fY, float fWidth, float fHeight, Shader& shaderRect);
    void DrawPicture(std::shared_ptr<Texture> ptrTexture, float fX, float fY, Shader& shaderTexture);
    void DrawPictureScaled(std::shared_ptr<Texture> ptrTexture, float fX, float fY, float fScaleFactor, Shader& shaderTexture);
    void DrawPictureRotated(std::shared_ptr<Texture> ptrTexture, float fX, float fY, float fDegrees, Shader& shaderTexture);
    void DrawPictureAroundPoint(std::shared_ptr<Texture> ptrTexture, float fX, float fY, float fDegrees, float fVertPivotFromCenter, Shader& shaderTexture);


    /*3D*/
    void DrawPicture3D(std::shared_ptr<Texture> ptrTexture, Shader& shaderTexture, glm::mat4& matrixModel, glm::mat4& matrixView);

    /*Debug Draw*/
    void DrawPictureDebug(std::shared_ptr<Texture> ptrTexture,
                          float fX,
                          float fY,
                          float fScaleFactor,
                          float fDegrees,
                          float fHorizontalPivotFromCenter,
                          float fVerticalPivotFromCenter,
                          bool bRotateAroundX,
                          bool bRotateAroundY,
                          bool bRotateAroundZ,
                          Shader &shaderTexture);
private:
    void SendDebugGeometry(std::shared_ptr<Texture> ptrTexture);
    void Send2DGeometry(std::shared_ptr<Texture> ptrTexture);

};
#endif
