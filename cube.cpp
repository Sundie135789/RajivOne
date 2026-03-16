#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
int main(){
  glfwInit();
  GLFWwindow* window = glfwCreateWindow( 800, 800, "hello", nullptr, nullptr);
  float vertices[] = {
    // front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

    // back face
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,

    // left face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

    // right face
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,

    // top face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,

    // bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f
};
 unsigned int indices[] = {
    0,1,2, 2,3,0,
    4,5,6, 6,7,4,
    8,9,10, 10,11,8,
    12,13,14, 14,15,12,
    16,17,18, 18,19,16,
    20,21,22, 22,23,20
};
// stb
	int width, height ,channels;
	unsigned char* data = stbi_load("rajiv.png", &width,  &height, &channels, 0);
	if(!data){
					std::cerr << "Failed to load rajiv.png" << std::endl;
					return -1;
	}
  glewExperimental = GL_TRUE;
  glfwMakeContextCurrent(window);
  if(glewInit() != GLEW_OK){
		std::cerr << "GLEW init failed" << std::endl;
		return -1;
	}

    glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, 800, 800);
	//Texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, (channels == 3) ? GL_RGB : GL_RGBA, width, height,0, 
	channels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	//VBO, VAO
	GLuint vbo, vao, ebo;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
	glBindVertexArray(0);

	// SHADER
	const char* vertexSource = R"(
	#version 330 core
	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec2 atexturecoords;
	uniform mat4 projection;
	uniform mat4 view;
	uniform mat4 model;
	out vec2 textureCoords;
	void main(){
		gl_Position = projection * view * model * vec4(aPos, 1.0);
		textureCoords = atexturecoords;
	}
	)";
	const char* fragmentSource = R"(
	#version 330 core
	out vec4 FragColor;
	in vec2 textureCoords;
	uniform sampler2D uImage;
	void main(){
		FragColor = texture(uImage, textureCoords);
	}
	)";

	

	GLuint vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint shaderProgram;
	glShaderSource(vertex, 1, &vertexSource, nullptr);
	glShaderSource(fragment, 1, &fragmentSource, nullptr);
	glCompileShader(vertex);
	glCompileShader(fragment);
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertex);
	glAttachShader(shaderProgram, fragment);
	glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);
	glEnable(GL_DEPTH_TEST);
	float rotation = 45.0f;
	while(!glfwWindowShouldClose(window)){
		glBindVertexArray(vao);

		glUseProgram(shaderProgram);
		rotation+=0.3f;
		glm::mat4 model(1.0f); 
		glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
		//Transform the model matrix;
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.5f, 1.0f, 0.0f));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shaderProgram, "uImage"), 0);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		std::cout << "Drawing frame..." << std::endl;
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(shaderProgram);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glfwDestroyWindow(window);
	
	glfwTerminate();
	return 0;
}
