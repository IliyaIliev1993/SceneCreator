#include <main_app/event_handler/EventHandler.h>
#include <main_app/debug/Log.h>

EventHandler::EventHandler()
{

}

void EventHandler::ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if( glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS )
    {

    }

    if( glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS )
    {

    }
}

void EventHandler::CursorInput(GLFWwindow *window, float fXPos, float fYPos)
{
    /*Global mouse coords*/
    m_fXCurrPosMouse = fXPos;
    m_fYCurrPosMouse = fYPos;

    /*Drag Event*/
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if(!m_bMousePressedNow)
        {
            m_fXMemoryMouse = fXPos;
            m_fYMemoryMouse = fXPos;
            m_bMousePressedNow = true;
        }

        //LOG_TRACE("CursorInput(X: {0}, Y: {1})", f_gXPos, f_gYPos);
        float xPolar = fXPos - m_fXMemoryMouse;
        float yPolar = fYPos - m_fYMemoryMouse;

        f_gXPos = xPolar;
        f_gYPos = yPolar;
    }
}
