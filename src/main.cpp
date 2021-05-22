#include "Drawing/GLRenderWindow.h"
#include "Drawing/GLRenderable.h"

#include <boost/filesystem.hpp>
#include <iostream>
#include <string>

int main()
{
#ifdef TOY_WORKING_DIR
    boost::filesystem::current_path(TOY_WORKING_DIR);
#endif
    std::cout << "Hello Shaders!" << std::endl;
    std::cout << "Working Directory: " << boost::filesystem::current_path() << std::endl;

    GLRenderWindow glRenderWindow;

    if (glRenderWindow.GLInit())
    {
        GLRenderablePtr helloTriangle(new GLHelloTriangle());
        glRenderWindow.AddRenderable(helloTriangle);

        while (!glRenderWindow.GetShouldClose())
        {
            // Will also update the internal state of our window so needs to go at the start of the frame.
            glRenderWindow.NewFrame();

            // Update anything we want to update between frames.

            // Do the actual render.
            glRenderWindow.Render();
        }
    }

    glRenderWindow.GLCleanUp();

    return 0;
}
