#include "Shader.h"
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;
typedef unsigned char byte;
struct ModelTransform
{
	glm::vec3 pos;
	glm::vec3 rotation;
	glm::vec3 scale;

	void setScale(float s)
	{
		scale.x = s;
		scale.y = s;
		scale.z = s;
	}

};
struct Color {
	float r, g, b, a;
};
bool wireframeMode = false;
void UpdatePolygonMode()
{
	if(wireframeMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}
Color background = { 0.f, 0.f, 0.f, 1.f };
Camera camera(glm::vec3(0.f, 0.f, -2.f));
void Resize(GLFWwindow* win, int width, int height)
{
	glViewport(0, 0, width, height);
}
void onScroll(GLFWwindow* win, double x, double y)
{
	camera.ChangeFOV(y);
}
void onKeyAction(GLFWwindow* win, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_SPACE: 
			wireframeMode = !wireframeMode;
			UpdatePolygonMode();
			break;

		}
	}
}
bool flag = false;
glm::vec3 rayT(GLFWwindow* win)
{
	double xpos, ypos;
	glfwGetCursorPos(win, &xpos, &ypos);
	glm::vec3 view = camera.Front - camera.Position;
	glm::normalize(view);
	glm::vec3 horiz = glm::normalize(glm::cross(view, camera.Up));
	glm::vec3 vert = glm::normalize(glm::cross(horiz, view));
	float rad = glm::radians(camera.Fov);
	float height = tan(rad / 2) * camera.zNear;
	float width = height * (camera.AspectRatio);
	vert *= height;
	horiz *= width;

	xpos -= 1280.0f / 2.0f;
	ypos -= 720.0f / 2.0f;

	// Скалируем координаты
	xpos /= (1280.0f / 2.0f);
	ypos /= (720.0f / 2.0f);

	glm::vec3 cursorPos = camera.Position + view * camera.zNear + horiz * (float)xpos + vert * (float)ypos;
	glm::vec3 rayDir = glm::normalize(cursorPos - camera.Position);
	return rayDir;
}
void processInput(GLFWwindow* win, double dt, glm::vec3 &d)
{
	if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(win, true);
	if (glfwGetKey(win, GLFW_KEY_1) == GLFW_PRESS)
		background = { 1.f, 0.f, 0.f, 1.f };
	if (glfwGetKey(win, GLFW_KEY_2) == GLFW_PRESS)
		background = { 0.f, 1.f, 0.f, 1.f };
	if (glfwGetKey(win, GLFW_KEY_3) == GLFW_PRESS)
		background = { 0.f, 0.f, 1.f, 1.f };
	
	if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS)
	{
		d = rayT(win);
	}
	uint32_t dir = 0;
	if (glfwGetKey(win, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		dir |= CAM_UP;
	if (glfwGetKey(win, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		dir |= CAM_DOWN;
	if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
		dir |= CAM_FORWARD;
	if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
		dir |= CAM_BACKWARD;
	if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
		dir |= CAM_LEFT;
	if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
		dir |= CAM_RIGHT;
	static double  newx = 0.f, newy = 0.f;
	glfwGetCursorPos(win, &newx, &newy);
	static double  x = newx, y = newy;
	double xoffset = newx - x, yoffset = newy - y;
	x = newx;
	y = newy;
	camera.Move(dir, dt);
	//camera.Rotate(xoffset, -yoffset);
}
bool rayBoxIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& boxMin, const glm::vec3& boxMax)
{
	glm::vec3 invDir = 1.0f / rayDirection;
	glm::vec3 tMin = (boxMin - rayOrigin) * invDir;
	glm::vec3 tMax = (boxMax - rayOrigin) * invDir;

	glm::vec3 t1 = glm::min(tMin, tMax);
	glm::vec3 t2 = glm::max(tMin, tMax);

	float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
	float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);

	return tNear <= tFar;
}
struct Object {
	ModelTransform transform;
	unsigned int VAO;
	unsigned int texture;
	int numIndices;
};


int main()
{
	Object objects[3];
#pragma region WIN INIT
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* win = glfwCreateWindow(1280, 720, "OpenGL Window", NULL, NULL);
	if (win == NULL)
	{
		cout << "Error. No window!" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(win);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Error. No GLAD!" << endl;
		glfwTerminate();
		return -1;
	}
	glfwSetFramebufferSizeCallback(win, Resize);
	glfwSetScrollCallback(win, onScroll);
	glfwSetKeyCallback(win, onKeyAction);
	
	glViewport(0, 0, 1280, 720);
	glEnable(GL_DEPTH_TEST);
	/*glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);*/
	UpdatePolygonMode();
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW); 
#pragma endregion
	int box_width, box_height, channels;
	byte* data = stbi_load("images\\box.png", &box_width, &box_height, &channels, 0);
	const int verts = 8;
	float cube[verts*8] = {
		-1.0f, 1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		0.f, 1.f,
		1.0f, 1.0f, -1.0f,		0.5f, 0.5f, 0.0f,		1.f, 1.f,
		1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,		1.f, 0.f,
		-1.0f, 1.0f, 1.0f,		0.0f, 0.5f, 0.5f,		0.f, 0.f,
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 1.0f,		1.f, 0.f,
		1.0f, -1.0f, -1.0f,		0.5f, 0.0f, 0.5f,		0.f, 0.f,	
		1.0f, -1.0f, 1.0f,		0.5f, 0.5f, 0.5f,		0.f, 1.f,
		-1.0f, -1.0f, 1.0f,		1.0f, 1.0f, 1.0f,		1.f, 1.f
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3,
		0, 4, 1,
		1, 4, 5,
		0, 3, 7,
		0, 7, 4,
		1, 6, 2,
		1, 5, 6,
		2, 7, 3,
		2, 6, 7,
		4, 7, 5,
		5, 7, 6
	};

	ModelTransform polygonTrans = { glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.0f, 1.0f, 1.0f)};
	ModelTransform polygonTrans2 = { glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.0f, 1.0f, 1.0f)};
	ModelTransform polygonTrans3 = { glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.0f, 1.0f, 1.0f)};

#pragma region BUFFERS INITIALIZATION
	unsigned int box_texture;
	glGenTextures(1, &box_texture);

	glBindTexture(GL_TEXTURE_2D, box_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	if(channels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, box_width, box_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, box_width, box_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	//glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	//полигоны
	unsigned int VBO_polygon, VAO_polygon,  EBO_polygon;
	glGenBuffers(1, &VBO_polygon);
	glGenBuffers(1, &EBO_polygon);
	glGenVertexArrays(1, &VAO_polygon);

	glBindVertexArray(VAO_polygon);
	//загрузка вбо
	glBindBuffer(GL_ARRAY_BUFFER, VBO_polygon);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verts * 8, cube, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_polygon);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 36, indices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
#pragma endregion
	Shader* polygon_shader = new Shader("Shaders\\basic.vert", "Shaders\\basic.frag");
	
	double oldTime = glfwGetTime(), newTime, deltaTime;
	glm::vec3 dir;
	while (!glfwWindowShouldClose(win))
	{
		//fps
		newTime = glfwGetTime();
		deltaTime = newTime - oldTime;
		oldTime = newTime;

		processInput(win, deltaTime, dir);
		
		polygonTrans.rotation.z = glfwGetTime() * 70.0;
		//polygonTrans.rotation.x = glfwGetTime() * 45.0;
		polygonTrans.pos.x = 0.8f * cos(glfwGetTime()*3);
		polygonTrans.pos.y = 0.8f * sin(glfwGetTime()*3);
		polygonTrans.setScale(0.2f);
		polygonTrans2.rotation.z = glfwGetTime() * 30.0;
		//polygonTrans2.rotation.y = glfwGetTime() * 45.0;
		polygonTrans2.pos.x = 0.8f * cos(glfwGetTime()*3 + 3.14159f);
		polygonTrans2.pos.y = 0.8f * sin(glfwGetTime()*3 + 3.14159f);
		polygonTrans2.setScale(0.2f);


		//polygonTrans3.rotation.x = glfwGetTime() * 90.0;
		//polygonTrans3.rotation.y = glfwGetTime() * 60.0;
		polygonTrans3.setScale(0.2f);

		glClearColor(background.r, background.g, background.b, background.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		polygon_shader->use();

		//camera.Rotate(glfwGetTime(), 0);


		glm::mat4 pv = camera.GetProjectionMatrix() * camera.GetViewMatrix();


		//1
		objects[0].transform = polygonTrans;
		objects[0].texture = box_texture;
		objects[0].numIndices = 36;

		objects[1].transform = polygonTrans2;
		objects[1].texture = box_texture;
		objects[1].numIndices = 36;

		objects[2].transform = polygonTrans3;
		objects[2].texture = box_texture;
		objects[2].numIndices = 36;

		// Цикл рендеринга
		for (int i = 0; i < 3; i++) {
			Object& obj = objects[i];

			obj.transform.pos;
			bool intersects = rayBoxIntersection(camera.Position, dir, obj.transform.pos - obj.transform.scale, obj.transform.pos + obj.transform.scale);
			if (intersects)
			{
				cout << i << endl;
				intersects = false;
			}
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, obj.transform.pos);
			model = glm::rotate(model, glm::radians(obj.transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(obj.transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(obj.transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, obj.transform.scale);

			glm::mat4 pvm = pv * model;
			glm::vec3 distance = obj.transform.pos - camera.Position;

			polygon_shader->SetMatrix4F("pvm", pvm);
			polygon_shader->setBool("wireframeMode", wireframeMode);

			glBindTexture(GL_TEXTURE_2D, obj.texture);
			glBindVertexArray(VAO_polygon);
			glDrawElements(GL_TRIANGLES, obj.numIndices, GL_UNSIGNED_INT, 0);
		}

		glfwSwapBuffers(win);
		glfwPollEvents();
	}
	delete polygon_shader;
	glfwTerminate();
	return 0;
}