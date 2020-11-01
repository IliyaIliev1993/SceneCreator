#include <main_app/renderer/Renderer.h>
#include <main_app/debug/Log.h>
#include <main_app/debug/Debug.h>
#include <main_app/event_handler/EventHandler.h>
#include <main_app/MainApp.h>

/** GLFW callbacks */
void framebuffer_size_callback(GLFWwindow* ptrWindow, int nWidth, int nHeight)
{
    /*Resize Window*/
    glViewport(0, 0, nWidth, nHeight);
}

void cursor_position_callback(GLFWwindow* ptrWindow, double dXPos, double dYPos)
{
    MainApp::Get().ptrEventHandler->CursorInput(ptrWindow,
                        static_cast<float>(dXPos), static_cast<float>(dYPos));
}

void drop_callback(GLFWwindow* ptrWindow, int nPathCount, const char* strPaths[])
{
    MainApp::Get().ptrDebug->DragFromOutside(nPathCount, strPaths);
}

void Renderer::SetGLFWCallbacks()
{
    /*Resize Window*/
//    glfwSetFramebufferSizeCallback(GetWindow(), framebuffer_size_callback);
    /*Mouse Control*/
    glfwSetCursorPosCallback(GetWindow(), cursor_position_callback);
    /*Called when dragged and dropped from outside window*/
    glfwSetDropCallback(GetWindow(), drop_callback);
}

Renderer::Renderer()
{

}

bool Renderer::InitSystem(const std::string& strTitle,
                          unsigned int nScreenWidth, unsigned int nScreenHeight,
                          unsigned int nScreenDepth)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    SCREEN_WIDTH = nScreenWidth;
    SCREEN_HEIGHT = nScreenHeight;
    SCREEN_DEPTH = nScreenDepth;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, strTitle.c_str(), NULL, NULL);

    if(window == nullptr)
    {
        LOG_ERROR("Failed to create WINDOW!");
        return false;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR("Failed to initialize GLAD!");
        return false;
    }

    SetGLFWCallbacks();


    LOG_INFO("OpenGL Info:\n Vendor: {0}\n Renderer: {1}\n Version: {2}",
             glGetString(GL_VENDOR), glGetString(GL_RENDERER),
             glGetString(GL_VERSION));
    return true;
}

void Renderer::Deinit()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Renderer::CreateAndFill2DBuffers()
{
    float fVertices2D[] =
    {
    /*Top Left*/ 0.0f, 0.0f,                                   /*Texture Top Left*/ 0.0f, 1.0f,
   /*Top Right*/(float)SCREEN_WIDTH, 0.0f,                    /*Texture Top Right*/ 1.0f, 1.0f,
/*Bottom Right*/(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, /*Texture Bottom Right*/ 1.0f, 0.0f,
  /*Bottom Left*/0.0f, (float)SCREEN_HEIGHT,                /*Texture Bottom Left*/ 0.0f, 0.0f,
    };

    unsigned int unIndices[] =
    {
        0, 1, 3, /*First Triangle*/
        1, 2, 3  /*Second Triangle*/
    };

    /*Creates the VertexArrayObject, VertexBufferObject, ElementBufferObject*/
    glGenVertexArrays(1, &VAO2D);
    glGenBuffers(1, &VBO2D);
    glGenBuffers(1, &EBO2D);

    /*Bind current VAO to push all the datas*/
    glBindVertexArray(VAO2D);

    /*Fill VBO*/
    glBindBuffer(GL_ARRAY_BUFFER, VBO2D);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fVertices2D), fVertices2D, GL_STATIC_DRAW);

    /*Fill EBO*/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2D);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unIndices), unIndices, GL_STATIC_DRAW);

    /*Set Position Attributes*/
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /*Set Texture Attributes*/
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    /*** DEBUG BUFFERS ***/
    float fVertices2DDebug[] =
    {
        m_vec2TopLeft.x, m_vec2TopLeft.y,        /*Texture Top Left*/ 0.0f, 1.0f,
        m_vec2TopRight.x, m_vec2TopRight.y,      /*Texture Top Right*/ 1.0f, 1.0f,
        m_vec2BottomRight.x, m_vec2BottomRight.y,/*Texture Bottom Right*/ 1.0f, 0.0f,
        m_vec2BottomLeft.x, m_vec2BottomLeft.y,  /*Texture Bottom Left*/ 0.0f, 0.0f,
    };

    unsigned int unIndicesDebug[] =
    {
        0, 1, 3, /*First Triangle*/
        1, 2, 3  /*Second Triangle*/
    };

    /*Creates the VertexArrayObject, VertexBufferObject, ElementBufferObject*/
    glGenVertexArrays(1, &VAO2DDebug);
    glGenBuffers(1, &VBO2DDebug);
    glGenBuffers(1, &EBO2DDebug);

    /*Bind current VAO to push all the datas*/
    glBindVertexArray(VAO2DDebug);

    /*Fill VBO*/
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DDebug);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fVertices2DDebug), fVertices2DDebug, GL_DYNAMIC_DRAW);

    /*Fill EBO*/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2DDebug);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unIndicesDebug), unIndicesDebug, GL_DYNAMIC_DRAW);

    /*Set Position Attributes*/
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /*Set Texture Attributes*/
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    /*Enable Blending*/
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void Renderer::CreateAndFill3DBuffers()
{
    float fVertices3D[] =
    {
    -1.0f, 1.0f, 0.0f,  /*Texture Top Left*/     0.0f, 1.0f,
     1.0f, 1.0f, 0.0f,  /*Texture Top Right*/    1.0f, 1.0f,
     1.0f,-1.0f, 0.0f,  /*Texture Bottom Right*/ 1.0f, 0.0f,
    -1.0f,-1.0f, 0.0f,  /*Texture Bottom Left*/  0.0f, 0.0f,
    };

    unsigned int unIndices3D[] =
    {
        0, 1, 3, /*First Triangle*/
        1, 2, 3  /*Second Triangle*/
    };

    /*Creates the VertexArrayObject, VertexBufferObject, ElementBufferObject*/
    glGenVertexArrays(1, &VAO3D);
    glGenBuffers(1, &VBO3D);
    glGenBuffers(1, &EBO3D);

    /*Bind current VAO to push all the datas*/
    glBindVertexArray(VAO3D);

    /*Fill VBO*/
    glBindBuffer(GL_ARRAY_BUFFER, VBO3D);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fVertices3D), fVertices3D, GL_STATIC_DRAW);

    /*Fill EBO*/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3D);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unIndices3D), unIndices3D, GL_STATIC_DRAW);

    /*Set Position Attributes*/
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /*Set Texture Attributes*/
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    /*Enable Blending*/
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void Renderer::SetColor(float fRed, float fGreen, float fBlue, float fAlpha)
{
    m_vColor = glm::vec4(fRed, fGreen, fBlue, fAlpha);
}

void Renderer::DrawLine(float fXInitial, float fYInitial, float fXFinal, float fYFinal, float fThickness, Shader &shaderLine)
{
    /*Bind VertexArrayObject and shader to use*/
    glBindVertexArray(VAO2D);
    shaderLine.use();

    /*Create transformations*/
    glm::mat4 model         = glm::mat4(1.0f);
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::mat4(1.0f);
    glm::mat4 projectionToPush = glm::mat4(1.0f);

    float fDeltaX = fXInitial - fXFinal;
    float fDeltaY = fYInitial - fYFinal;

    /*Transformations of Model*/
    model = glm::translate(model, glm::vec3(fXInitial, fYInitial - (fThickness / 2), 0.0f));
    model = glm::rotate(model,
                        std::atan2(fDeltaY, fDeltaX),
                        glm::vec3(0.0f, 0.0f, 1.0f));

    float fLenghtOfVector = std::sqrt((fDeltaX * fDeltaX) + (fDeltaY * fDeltaY));

    model = glm::scale(model, glm::vec3(-(std::abs(fLenghtOfVector)) / (float)SCREEN_WIDTH,
                                        fThickness / (float)SCREEN_HEIGHT,
                                        0.0f));

    /*Transformations of Projection, in this case is Orthographic projection*/
    projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);

    /*The current matrix projeciton will be pushed in the vertex shader*/
    projectionToPush = projection * view * model;

    shaderLine.setMat4("uProjection", projectionToPush);
    shaderLine.setVec4("vColor", m_vColor);

    /*Current draw call*/
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawLine(float fXInitial, float fYInitial, float fXFinal, float fYFinal, float fThickness, Shader &shaderLine, std::shared_ptr<Texture> ptrTexture)
{
    /*Bind VertexArrayObject, texture and shader to use*/
    glBindVertexArray(VAO2D);
    ptrTexture->Bind();
    shaderLine.use();

    /*Create transformations*/
    glm::mat4 model         = glm::mat4(1.0f);
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::mat4(1.0f);
    glm::mat4 projectionToPush = glm::mat4(1.0f);

    float fDeltaX = fXInitial - fXFinal;
    float fDeltaY = fYInitial - fYFinal;

    /*Transformations of Model*/
    model = glm::translate(model, glm::vec3(fXInitial, fYInitial - (fThickness / 2), 0.0f));
    model = glm::rotate(model,
                        std::atan2(fDeltaY, fDeltaX),
                        glm::vec3(0.0f, 0.0f, 1.0f));

    float fLenghtOfVector = std::sqrt((fDeltaX * fDeltaX) + (fDeltaY * fDeltaY));

    model = glm::scale(model, glm::vec3(-(std::abs(fLenghtOfVector)) / (float)SCREEN_WIDTH,
                                        fThickness / (float)SCREEN_HEIGHT,
                                        0.0f));

    /*Transformations of Projection, in this case is Orthographic projection*/
    projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);

    /*The current matrix projeciton will be pushed in the vertex shader*/
    projectionToPush = projection * view * model;

    shaderLine.setMat4("uProjection", projectionToPush);
    shaderLine.setVec4("vColor", m_vColor);

    /*Current draw call*/
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawRect(float fX, float fY, float fWidth, float fHeight, Shader &shaderRect)
{
    /*Bind VertexArrayObject, texture and shader to use*/
    glBindVertexArray(VAO2D);
    shaderRect.use();

    /*Create transformations*/
    glm::mat4 model         = glm::mat4(1.0f);
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::mat4(1.0f);
    glm::mat4 projectionToPush = glm::mat4(1.0f);

    /*Transformations of Model*/
    model = glm::translate(model, glm::vec3(fX, fY, 0.0f));
    model = glm::scale(model, glm::vec3(fWidth / (float)SCREEN_WIDTH,
                                        fHeight / (float)SCREEN_HEIGHT,
                                        0.0f));

    /*Transformations of Projection, in this case is Orthographic projection*/
    projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);

    /*The current matrix projeciton will be pushed in the vertex shader*/
    projectionToPush = projection * view * model;

    shaderRect.setMat4("uProjection", projectionToPush);
    shaderRect.setVec4("vColor", m_vColor);

    /*Current draw call*/
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawPicture(std::shared_ptr<Texture> ptrTexture, float fX, float fY, Shader &shaderTexture)
{
    /*Bind VertexArrayObject, texture and shader to use*/
    glBindVertexArray(VAO2D);
    ptrTexture->Bind();
    shaderTexture.use();

    /*Create transformations*/
    glm::mat4 model         = glm::mat4(1.0f);
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::mat4(1.0f);
    glm::mat4 projectionToPush = glm::mat4(1.0f);

    /*Transformations of Model*/
    model = glm::translate(model, glm::vec3(fX, fY, 0.0f));
    model = glm::scale(model, glm::vec3((float)ptrTexture->GetWidth() / (float)SCREEN_WIDTH,
                                        (float)ptrTexture->GetHeight() / (float)SCREEN_HEIGHT
                                        , 0.0f));

    /*Transformations of Projection, in this case is Orthographic projection*/
    projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);

    /*The current matrix projeciton will be pushed in the vertex shader*/
    projectionToPush = projection * view * model;

    shaderTexture.setMat4("uProjection", projectionToPush);
    shaderTexture.setVec4("vColor", m_vColor);

    /*Current draw call*/
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawPicture3D(std::shared_ptr<Texture> ptrTexture, Shader &shaderTexture, glm::mat4& matrixModel, glm::mat4& matrixView)
{
    /*Bind VertexArrayObject, texture and shader to use*/
    glBindVertexArray(VAO3D);
    ptrTexture->Bind();
    shaderTexture.use();

    /*Create transformations*/
    glm::mat4 model         = matrixModel;
    glm::mat4 view          = matrixView;
    glm::mat4 projection    = glm::mat4(1.0f);
    glm::mat4 projectionToPush = glm::mat4(1.0f);

    /*Transformations of Projection, in this case is Perspective projection*/
    projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    /*The current matrix projeciton will be pushed in the vertex shader*/
    projectionToPush = projection * view * model;

    shaderTexture.setMat4("uProjection", projectionToPush);
    shaderTexture.setVec4("vColor", m_vColor);

    /*Current draw call*/
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawPictureScaled(std::shared_ptr<Texture> ptrTexture, float fX, float fY, float fScaleFactor, Shader &shaderTexture)
{
    /*Bind VertexArrayObject, texture and shader to use*/
    glBindVertexArray(VAO2D);
    ptrTexture->Bind();
    shaderTexture.use();

    /*Create transformations*/
    glm::mat4 model         = glm::mat4(1.0f);
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::mat4(1.0f);
    glm::mat4 projectionToPush = glm::mat4(1.0f);

    /*Transformations of Model*/
    model = glm::translate(model, glm::vec3(fX, fY, 0.0f));
    model = glm::scale(model, glm::vec3(fScaleFactor * ((float)ptrTexture->GetWidth() / (float)SCREEN_WIDTH),
                                        fScaleFactor * ((float)ptrTexture->GetHeight() / (float)SCREEN_HEIGHT)
                                        , 0.0f));

    /*Transformations of Projection, in this case is Orthographic projection*/
    projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);

    /*The current matrix projeciton will be pushed in the vertex shader*/
    projectionToPush = projection * view * model;

    shaderTexture.setMat4("uProjection", projectionToPush);
    shaderTexture.setVec4("vColor", m_vColor);

    /*Current draw call*/
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawPictureRotated(std::shared_ptr<Texture> ptrTexture, float fX, float fY, float fDegrees, Shader &shaderTexture)
{
    /*Bind VertexArrayObject, texture and shader to use*/
    glBindVertexArray(VAO2D);
    ptrTexture->Bind();
    shaderTexture.use();

    /*Create transformations*/
    glm::mat4 model         = glm::mat4(1.0f);
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::mat4(1.0f);
    glm::mat4 projectionToPush = glm::mat4(1.0f);

    /*Transformations of Model*/
    model = glm::translate(model, glm::vec3(fX + (float)ptrTexture->GetWidth() / 2,
                                            fY + (float)ptrTexture->GetHeight() / 2,
                                            0.0f));
    model = glm::rotate(model, glm::radians(fDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-(float)ptrTexture->GetWidth() / 2,
                                            -(float)ptrTexture->GetHeight() / 2,
                                            0.0f));
    model = glm::scale(model, glm::vec3((float)ptrTexture->GetWidth() / (float)SCREEN_WIDTH,
                                        (float)ptrTexture->GetHeight() / (float)SCREEN_HEIGHT
                                        , 0.0f));

    /*Transformations of Projection, in this case is Orthographic projection*/
    projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);

    /*The current matrix projeciton will be pushed in the vertex shader*/
    projectionToPush = projection * view * model;

    shaderTexture.setMat4("uProjection", projectionToPush);
    shaderTexture.setVec4("vColor", m_vColor);

    /*Current draw call*/
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawPictureAroundPoint(std::shared_ptr<Texture> ptrTexture, float fX, float fY, float fDegrees, float fVertPivotFromCenter, Shader &shaderTexture)
{
    /*Bind VertexArrayObject, texture and shader to use*/
    glBindVertexArray(VAO2D);
    ptrTexture->Bind();
    shaderTexture.use();

    /*Create transformations*/
    glm::mat4 model         = glm::mat4(1.0f);
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::mat4(1.0f);
    glm::mat4 projectionToPush = glm::mat4(1.0f);

    /*Transformations of Model*/
    model = glm::translate(model, glm::vec3(fX + (float)ptrTexture->GetWidth() / 2,
                                            fY + (float)ptrTexture->GetHeight() / 2,
                                            0.0f));
    model = glm::rotate(model, glm::radians(fDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-((float)ptrTexture->GetWidth() / 2 - fVertPivotFromCenter),
                                            -(float)ptrTexture->GetHeight() / 2,
                                            0.0f));
    model = glm::scale(model, glm::vec3((float)ptrTexture->GetWidth() / (float)SCREEN_WIDTH,
                                        (float)ptrTexture->GetHeight() / (float)SCREEN_HEIGHT
                                        , 0.0f));

    /*Transformations of Projection, in this case is Orthographic projection*/
    projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);

    /*The current matrix projeciton will be pushed in the vertex shader*/
    projectionToPush = projection * view * model;

    shaderTexture.setMat4("uProjection", projectionToPush);
    shaderTexture.setVec4("vColor", m_vColor);

    /*Current draw call*/
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawPictureDebug(std::shared_ptr<Texture> ptrTexture,
                                float fX,
                                float fY,
                                float fScaleFactor,
                                float fDegrees,
                                float fHorizontalPivotFromCenter,
                                float fVerticalPivotFromCenter,
                                bool bRotateAroundX,
                                bool bRotateAroundY,
                                bool bRotateAroundZ,
                                Shader &shaderTexture)
{
    SendDebugGeometry(ptrTexture);
    ptrTexture->Bind();

    shaderTexture.use();

    /*Create transformations*/
    glm::mat4 model         = glm::mat4(1.0f);
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::mat4(1.0f);
    glm::mat4 projectionToPush = glm::mat4(1.0f);

    /*Transformations of Model*/
    model = glm::translate(model, glm::vec3(fX + (float)ptrTexture->GetWidth() / 2,
                                            fY + (float)ptrTexture->GetHeight() / 2,
                                            0.0f));
    model = glm::rotate(model, glm::radians(fDegrees), glm::vec3((float)bRotateAroundX, (float)bRotateAroundY, (float)bRotateAroundZ));
    model = glm::translate(model, glm::vec3(-((float)ptrTexture->GetWidth() / 2 + fHorizontalPivotFromCenter),
                                            -(float)ptrTexture->GetHeight() / 2 - fVerticalPivotFromCenter,
                                            0.0f));
    model = glm::scale(model, glm::vec3(fScaleFactor * (float)ptrTexture->GetWidth() / (float)SCREEN_WIDTH,
                                        fScaleFactor * (float)ptrTexture->GetHeight() / (float)SCREEN_HEIGHT
                                        , 0.0f));

    /*Transformations of Projection, in this case is Orthographic projection*/
    projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);

    /*The current matrix projeciton will be pushed in the vertex shader*/
    projectionToPush = projection * view * model;

    shaderTexture.setMat4("uProjection", projectionToPush);
    shaderTexture.setVec4("vColor", m_vColor);

    /*Current draw call*/
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void Renderer::SendDebugGeometry(std::shared_ptr<Texture> ptrTexture)
{
    SendTextureCoordsToVBODebug(ptrTexture->GetTexCoord(Texture::ETexCoords::eTopLeft),
                                ptrTexture->GetTexCoord(Texture::ETexCoords::eTopRight),
                                ptrTexture->GetTexCoord(Texture::ETexCoords::eBottomRight),
                                ptrTexture->GetTexCoord(Texture::ETexCoords::eBottomLeft));

    /*Bind VertexArrayObject, texture and shader to use*/
    float fVertices2DDebug[] =
    {
        m_vec2TopLeft.x, m_vec2TopLeft.y,        /*Texture Top Left*/       m_vec2TextCoordTopLeft.x, m_vec2TextCoordTopLeft.y,
        m_vec2TopRight.x, m_vec2TopRight.y,      /*Texture Top Right*/      m_vec2TextCoordTopRight.x, m_vec2TextCoordTopRight.y,
        m_vec2BottomRight.x, m_vec2BottomRight.y,/*Texture Bottom Right*/   m_vec2TextCoordBottomRight.x, m_vec2TextCoordBottomRight.y,
        m_vec2BottomLeft.x, m_vec2BottomLeft.y,  /*Texture Bottom Left*/    m_vec2TextCoordBottomLeft.x, m_vec2TextCoordBottomLeft.y,
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO2DDebug);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fVertices2DDebug), fVertices2DDebug, GL_DYNAMIC_DRAW);
    glBindVertexArray(VAO2DDebug);
}

void Renderer::Send2DGeometry(std::shared_ptr<Texture> ptrTexture)
{

}


void Renderer::SendVertsToVBODebug(const glm::vec2& vec2TopLeft,
                                   const glm::vec2& vec2TopRight,
                                   const glm::vec2& vec2BottomRight,
                                   const glm::vec2& vec2BottomLeft)
{
    m_vec2TopLeft = vec2TopLeft;
    m_vec2TopRight = vec2TopRight;
    m_vec2BottomRight = vec2BottomRight;
    m_vec2BottomLeft = vec2BottomLeft;
}

void Renderer::SendVertsToVBODebug()
{
    SendVertsToVBODebug(glm::vec2(0.0f, 0.0f),
    glm::vec2((float)Renderer::SCREEN_WIDTH, 0.0f),
    glm::vec2((float)Renderer::SCREEN_WIDTH, (float)Renderer::SCREEN_HEIGHT),
    glm::vec2(0.0f, (float)Renderer::SCREEN_HEIGHT));
}


void Renderer::SendTextureCoordsToVBODebug(const glm::vec2& vec2TextCoordTopLeft,
                                          const glm::vec2& vec2TextCoordTopRight,
                                          const glm::vec2& vec2TextCoordBottomRight,
                                          const glm::vec2& vec2TextCoordBottomLeft)
{
    m_vec2TextCoordTopLeft = vec2TextCoordTopLeft;
    m_vec2TextCoordTopRight = vec2TextCoordTopRight;
    m_vec2TextCoordBottomRight = vec2TextCoordBottomRight;
    m_vec2TextCoordBottomLeft = vec2TextCoordBottomLeft;
}

void Renderer::CallEventsAndSwapBuffers()
{
    glfwSwapBuffers(window);
    glfwPollEvents();
}
