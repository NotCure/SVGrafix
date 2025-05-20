#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<utility/File.h>
#include <render/Shader.h>


#include<svg/FileSVG.h>
#include<render/SceneBuilder.h>

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 800, "SVG Renderer", nullptr, nullptr);
    if (!window) { std::cerr << "GLFW Failed\n"; return -1; }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, 800, 800);

    Shader shader("default.vert", "default.frag");

    std::string svgText = utility::file::read_file("C:\\Users\\Not Sure\\Downloads\\test.svg");
    FileSVG fileSvg(std::move(svgText));
    auto result = fileSvg.parse();
    if (!result.ok()) {
        for (auto& e : result.errors) std::cerr << e << "\n";
        return -1;
    }

    SceneBuilder scene(fileSvg.get_dom());
    scene.build();

	while (!glfwWindowShouldClose(window)) {
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Activate();
        scene.draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();

	}

    glfwDestroyWindow(window);
    glfwTerminate();
	return 0;
}

