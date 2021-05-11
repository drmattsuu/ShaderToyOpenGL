#include "Drawing/GLRenderWindow.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    std::cout << "Hello Shaders!" << std::endl;

    GLRenderWindow glRenderWindow;
    if (glRenderWindow.GLInit())
    {
        while (!glRenderWindow.GetShouldClose())
        {
            glRenderWindow.NewFrame();

            glRenderWindow.Render();
        }
    }

    glRenderWindow.GLCleanUp();

    return 0;
}
