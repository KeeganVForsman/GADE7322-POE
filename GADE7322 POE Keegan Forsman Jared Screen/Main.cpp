#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
//#include <GL/glew.h>
                                       //all the libraries we use
#include "linmath.h/linmath.h"//maths

#include <stdlib.h>//standard library
#include <stdio.h>

#include <assimp/Importer.hpp>//asset importer 
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>//wrapper library
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <physx/PxPhysicsAPI.h>//Physics

#include <AL/alc.h>//sound
#include <AL/al.h>

#include <nanogui/opengl.h>//GUI
#include <nanogui/nanogui.h>

#include <Windows.h>


// Vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";



int main()
{

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//tells opengl what version and profile we use
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /*drawCube();*/
    GLfloat vertices[] =
    {
        // Front face
        -0.5f, -0.5f, 0.5f,  // Bottom-left
         0.5f, -0.5f, 0.5f,  // Bottom-right
         0.5f,  0.5f, 0.5f,  // Top-right
        -0.5f,  0.5f, 0.5f,  // Top-left

        // Back face
        -0.5f, -0.5f, -0.5f,  // Bottom-left
         0.5f, -0.5f, -0.5f,  // Bottom-right
         0.5f,  0.5f, -0.5f,  // Top-right
        -0.5f,  0.5f, -0.5f,  // Top-left

        // Left face
        -0.5f, -0.5f, -0.5f,  // Bottom-back
        -0.5f, -0.5f, 0.5f,   // Bottom-front
        -0.5f,  0.5f, 0.5f,   // Top-front
        -0.5f,  0.5f, -0.5f,  // Top-back

        // Right face
         0.5f, -0.5f, -0.5f,  // Bottom-back
         0.5f, -0.5f, 0.5f,   // Bottom-front
         0.5f,  0.5f, 0.5f,   // Top-front
         0.5f,  0.5f, -0.5f,  // Top-back

         // Top face
         -0.5f,  0.5f, 0.5f,   // Front-left
          0.5f,  0.5f, 0.5f,   // Front-right
          0.5f,  0.5f, -0.5f,  // Back-right
         -0.5f,  0.5f, -0.5f,  // Back-left

         // Bottom face
         -0.5f, -0.5f, 0.5f,   // Front-left
          0.5f, -0.5f, 0.5f,   // Front-right
          0.5f, -0.5f, -0.5f,  // Back-right
         -0.5f, -0.5f, -0.5f   // Back-left
    };

	GLFWwindow* window = glfwCreateWindow(800, 800, "POE CHESS", NULL, NULL);//Creates a window that is 800 pixels and names it
	if (window == NULL)
	{
		std::cout << "Fail" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();//loads GLAD


	glViewport(0, 0, 800, 800);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint Vertex_Array, Vertex_Buffer;

    glGenVertexArrays(1, &Vertex_Array);

    glGenBuffers(1, &Vertex_Buffer);
    glBindBuffer(GL_ARRAY_BUFFER, Vertex_Buffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader heightMapShader("8.3.cpuheight.vs", "8.3.cpuheight.fs");



    //Step 1
    stbi_set_flip_vertically_on_load(true);
    int width, height, nChannels;
    unsigned char* data = stbi_load("resources/heightmap.png",
        &width, &height, &nChannels,
        0);


    //Step 2
    std::vector<float> vertices;
    float yScale = 64.0f / 256.0f, yShift = 16.0f;
    int rez = 1;
    unsigned bytePerPixel = nChannels;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            unsigned char* pixelOffset = data + (j + width * i) * bytePerPixel;
            unsigned char y = pixelOffset[0];

            // vertex
            vertices.push_back(-height / 2.0f + height * i / (float)height);   // vx
            vertices.push_back((int)y * yScale - yShift);   // vy
            vertices.push_back(-width / 2.0f + width * j / (float)width);   // vz
        }
    }
    //std::cout << "Loaded " << vertices.size() / 3 << " vertices" << std::endl;
    stbi_image_free(data);


    //Step 4
    std::vector<unsigned> indices;
    for (unsigned i = 0; i < height - 1; i += rez)
    {
        for (unsigned j = 0; j < width; j += rez)
        {
            for (unsigned k = 0; k < 2; k++)
            {
                indices.push_back(j + width * (i + k * rez));
            }
        }
    }
    std::cout << "Loaded " << indices.size() << " indices" << std::endl;

    //Step 5
    const int numStrips = (height - 1) / rez;
    const int numTrisPerStrip = (width / rez) * 2 - 2;
    std::cout << "Created lattice of " << numStrips << " strips with " << numTrisPerStrip << " triangles each" << std::endl;
    std::cout << "Created " << numStrips * numTrisPerStrip << " triangles total" << std::endl;


    unsigned int terrainVAO, terrainVBO, terrainIBO; // first, configure the cube's VAO (and terrainVBO + terrainIBO)
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);   // position attribute
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &terrainIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);




	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);// background colour
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);

	while (!glfwWindowShouldClose(window))
	{
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);// background colour
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glBindVertexArray(Vertex_Array);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();

            
		glfwPollEvents();
	}

    glDeleteVertexArrays(1, &Vertex_Array);
    glDeleteBuffers(1, &Vertex_Buffer);
    glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;

}

