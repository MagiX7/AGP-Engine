#include "Input.h"
#include "engine.h"

#include <GLFW/glfw3.h>

float Input::lastMousePosX = 0;
float Input::lastMousePosY = 0;

bool Input::IsKeyPressed(int key)
{
	GLFWwindow* window = Application::GetInstance().GetWindow();
	int state = glfwGetKey(window, key);

	return state == GLFW_PRESS;
}

bool Input::IsMouseButtonPressed(int button)
{
	GLFWwindow* window = Application::GetInstance().GetWindow();

	int state = glfwGetMouseButton(window, button);
	return state == GLFW_PRESS;
}

float Input::GetMouseX()
{
	double x, y;
	GLFWwindow* window = Application::GetInstance().GetWindow();
	glfwGetCursorPos(window, &x, &y);
	return (float)x;
}

float Input::GetMouseY()
{
	double x, y;
	GLFWwindow* window = Application::GetInstance().GetWindow();
	glfwGetCursorPos(window, &x, &y);
	return (float)y;
}

float Input::GetMouseMotionX()
{
	return (GetMouseX() - lastMousePosX) / Application::GetInstance().GetDisplaySize().x;
}

float Input::GetMouseMotionY()
{
	return (GetMouseX() - lastMousePosY) / Application::GetInstance().GetDisplaySize().y;
}

glm::vec2 Input::GetMousePosition()
{
	GLFWwindow* window = Application::GetInstance().GetWindow();

	static double x, y;
	lastMousePosX = x;
	lastMousePosY = y;
	glfwGetCursorPos(window, &x, &y);

	return glm::vec2(x, y);
}
