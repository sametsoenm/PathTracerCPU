#include <iostream>
#include <stdexcept>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "output/image_output.h"
#include "renderer/renderer.h"
#include "rendersettings.h"
#include "app/path_tracer_app.h"

//int main() {
//
//	Renderer renderer;
//	renderer.render();
//	img::output::output_image(settings::FILE_NAME, renderer.buffer());
//	return 0;
//
//}

int main() {
    try {
        PathTracerApp app;
        app.run();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }
}