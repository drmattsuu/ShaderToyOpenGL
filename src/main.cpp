#include "Drawing/GLRenderWindow.h"

#include <filesystem>
#include <print>

int main()
{
#ifdef TOY_WORKING_DIR
    std::filesystem::current_path(TOY_WORKING_DIR);
#endif
    std::println("Hello Shaders!");
    std::println( "Working Directory: {0}", std::filesystem::current_path().string());

    GLRenderWindow glRenderWindow;

    if (glRenderWindow.GLInit())
    {
        glRenderWindow.AddAllRenderables();
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
