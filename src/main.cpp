#include <iostream>
#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"
#include "include/Shader/Shader.h"
#include "include/Camera/camera.h"
#include "include/Texture/stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


//time
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = 800 / 2.0f;
float lastY = 600 / 2.0f;
bool firstMouse = true;
bool jump = false;

int main() {

	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
	


	GLFWwindow* window = glfwCreateWindow(800, 600, "RayMarching", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char *data = stbi_load("/home/blake/projects/GLFW_test/textures/blue.png", &width, &height,
			&nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
				GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	shader.use(); // don’t forget to activate the shader first!
	glUniform1i(glGetUniformLocation(shader.ID, "matcap"), 0); // manually

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	const float vertices[] {
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
			GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
			2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	float resolution[2] = {800, 600};
	int resLoc = glGetUniformLocation(shader.ID, "iResolution");
	glUniform2fv(resLoc, 1, &resolution[0]);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	unsigned int counter = 0;
	unsigned int frame = 0;
	float posY = 0.0;
	float posX = 0.0;
	float posZ = 0.0;
	camera.Position.y -= 4;
	while(!glfwWindowShouldClose(window)) {

		//time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		counter++;
		if(counter >= 15) {
			std::cout << 1 / deltaTime << "\n";
			counter = 0;
		}

		float velocity = 5.0 * deltaTime;
////////	if(jump) {
////////		frame++;
////////	}
////////	if(frame >= 1 && frame <= 30) {
////////		camera.Position.y += 5.0 * velocity;
////////	} 
////////	if(frame >= 100) {
////////		frame = 0;
////////		jump = false;
////////	}
		

		//input
		processInput(window);

////////	camera.Position.y -= 9.8 * deltaTime;
////////	if(camera.Position.y >= 4.5 || camera.Position.y <= -4.5) {
////////		camera.Position.y = posY;
////////	}

////////	if(camera.Position.x >= 9.5 || camera.Position.x <= -9.5) {
////////		camera.Position.x = posX;
////////	}

////////	if(camera.Position.z >= 6.5 || camera.Position.z <= -6.5) {
////////		camera.Position.z = posZ;
////////	}

		
		glClear(GL_COLOR_BUFFER_BIT); 
		glfwSetCursorPosCallback(window, mouse_callback); 
		glfwSetScrollCallback(window, scroll_callback); 
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		glm::vec2 mouse = (glm::vec2(xPos, yPos) * glm::vec2(2.0,2.0) - glm::vec2(800,600)) / glm::vec2(600,600);
		//rendering commands
		mouse.y = -mouse.y;
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(VAO);
		shader.use();
		shader.setFloat("iTime", glfwGetTime());
		shader.setVec3("Front", camera.Front);
		shader.setVec3("Right", camera.Right);
		shader.setVec3("Up", camera.Up);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec2("mouse", mouse);
		resLoc = glGetUniformLocation(shader.ID, "iResolution");
		glUniform2fv(resLoc, 1, &resolution[0]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		posY = camera.Position.y;
		posX = camera.Position.x;
		posZ = camera.Position.z;
		
		//check and call events and swap the buffers.
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow *window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

//	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
//		camera.Position.z += 5;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {


	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset), deltaTime);
}
