#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <vec2.hpp>
#include <vec3.hpp>
#include <vec4.hpp>
#include <mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <SOIL2.h>

struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texcoord;
};

Vertex vertices[] = {
	//pos							//color							//textureCoord
	glm::vec3(-0.5f, 0.5f,	0.0f),	glm::vec3(1.0f, 0.0f,	0.f),	glm::vec2(0.0f, 1.0f),
	glm::vec3(-0.5f, -0.5f,	0.0f),	glm::vec3(.0f, 1.0f,	0.f),	glm::vec2(0.0f, 0.0f),
	glm::vec3(0.5f, -0.5f,	0.0f),	glm::vec3(.0f, 0.0f,	1.f),	glm::vec2(1.0f, 0.0f),
	glm::vec3(0.5f, 0.5f,	0.0f),	glm::vec3(1.0f, 1.0f,	0.f),	glm::vec2(1.0f, 1.0f)
};

unsigned verticesCount = sizeof(vertices) / sizeof(Vertex);

GLuint indices[] = {
	0, 1, 2,	// Triangle 1
	0, 2, 3		// Triangle 2
};

unsigned indicesCount = sizeof(indices) / sizeof(GLuint);

void updateInput(GLFWwindow *wnd) {
	if (glfwGetKey(wnd, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(wnd, GLFW_TRUE);
	}
}

void fbResize(GLFWwindow *wnd, int fbW, int fbH) {
	glViewport(0, 0, fbW, fbH);
}

bool loadShaders(GLuint &program) {
	bool loadSuccess = true;
	char infoLog[512];
	GLint success;

	std::string temp = "";
	std::string src = "";

	std::ifstream in_file;

	//Vertex
	in_file.open("shaders/vertex_core.glsl");
	if (in_file.is_open()) {
		while (std::getline(in_file, temp)) {
			src += temp + "\n";
		}
	}
	else {
		std::cout << "ERROR::LOADSHADERS::COULDNOT OPEN VERTEXFILE" << std::endl;
		loadSuccess = false;
	}
	in_file.close();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const GLchar *vertSrc = src.c_str();

	glShaderSource(vertexShader, 1, &vertSrc, nullptr);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cout << "ERROR::LOADSHADERS::COULDNOT COMPILE VERTEXFILE" << std::endl;
		std::cout << infoLog << std::endl;
		loadSuccess = false;
	}

	temp = "";
	src = "";

	in_file.open("shaders/fragment_core.glsl");
	if (in_file.is_open()) {
		while (std::getline(in_file, temp)) {
			src += temp + "\n";
		}
	}
	else {
		std::cout << "ERROR::LOADSHADERS::COULDNOT OPEN Framgent File" << std::endl;
		loadSuccess = false;
	}

	in_file.close();

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar *fragSrc = src.c_str();

	glShaderSource(fragmentShader, 1, &fragSrc, nullptr);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cout << "ERROR::LOADSHADERS::COULDNOT COMPILE FRAGMENTFILE" << std::endl;
		std::cout << infoLog << std::endl;
		loadSuccess = false;
	}

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, nullptr, infoLog);
		std::cout << "ERROR::LOADSHADERS::COULDNOT LINK PROGRAM" << std::endl;
		std::cout << infoLog << std::endl;
		loadSuccess = false;
	}

	glUseProgram(0);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return loadSuccess;
}

int main() {
	glfwInit();

	const int W = 640;
	const int H = 480;

	int frameBuffW = 0;
	int frameBuffH = 0;

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow *window = glfwCreateWindow(W, H, "hello :D", nullptr, nullptr);

	glfwSetFramebufferSizeCallback(window, fbResize);
	//glfwGetFramebufferSize(window, &frameBuffW, &frameBuffH);
	//glViewport(0, 0, frameBuffW, frameBuffH);

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cout << "ERROR::MAIN.CPP::GLEW_INIT_FAIL" << std::endl;
		glfwTerminate();
	}

	//oGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint core_program;
	if (!loadShaders(core_program))
		glfwTerminate();

	GLuint VAO = 0;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);
	//texcoord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoord));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// TEXTURE 0
	int img_w = 0;
	int img_h = 0;
	unsigned char *img = SOIL_load_image("imgs/pusheen.png", &img_w, &img_h, NULL, SOIL_LOAD_RGBA);

	GLuint texture0;
	glGenTextures(1, &texture0);
	glBindTexture(GL_TEXTURE_2D, texture0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if (img) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_w, img_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "ERROR::TEXTURE LOAD FAIL" << std::endl;
	}

	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(img);

	// TEXTURE 1
	int img_w1 = 0;
	int img_h1 = 0;
	unsigned char *img1 = SOIL_load_image("imgs/container.png", &img_w1, &img_h1, NULL, SOIL_LOAD_RGBA);

	GLuint texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if (img1) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_w1, img_h1, 0, GL_RGBA, GL_UNSIGNED_BYTE, img1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "ERROR::TEXTURE LOAD FAIL" << std::endl;
	}

	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(img1);

	while (!glfwWindowShouldClose(window)) {
		// UPDATE INPUT ---
		glfwPollEvents();
		//glfwSetWindowShouldClose(window, true);
		// UPDATE ---
		updateInput(window);
		// DRAW ---
		// Clear
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glUseProgram(core_program);

		glUniform1i(glGetUniformLocation(core_program, "texture0"), 0);
		glUniform1i(glGetUniformLocation(core_program, "texture1"), 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glFlush();

		glBindVertexArray(0);
		glUseProgram(0);
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glfwDestroyWindow(window);
	glfwTerminate();

	glDeleteProgram(core_program);

	return 0;
}