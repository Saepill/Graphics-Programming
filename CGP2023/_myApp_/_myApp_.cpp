// sb6.h ��� ������ ���Խ�Ų��.
#include <sb7.h>
#include <vmath.h>
#include <shader.h>
#include <vector>
#include "primitive.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Model.h"
#include "Camera.h"

// sb6::application�� ��ӹ޴´�.
class my_application : public sb7::application
{
public:
	// ���̴� �������Ѵ�.
	// basic multiple lighting shader
	GLuint compile_shader(void)
	{
		// ���ؽ� ���̴��� �����ϰ� �������Ѵ�.
		GLuint vertex_shader = sb7::shader::load("multiple_lights_vs.glsl", GL_VERTEX_SHADER);

		// �����׸�Ʈ ���̴��� �����ϰ� �������Ѵ�.
		GLuint fragment_shader = sb7::shader::load("multiple_lights_fs.glsl", GL_FRAGMENT_SHADER);

		// ���α׷��� �����ϰ� ���̴��� Attach��Ű�� ��ũ�Ѵ�.
		GLuint program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);
		glLinkProgram(program);

		// ���� ���α׷��� ���̴��� �����ϹǷ� ���̴��� �����Ѵ�.
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		return program;
	}

	// ���ø����̼� �ʱ�ȭ �����Ѵ�.
	virtual void startup()
	{
		initValues();


		// ���̴� ���α׷� ������ �� ����
		shader_program = compile_shader();

		stbi_set_flip_vertically_on_load(true);


		// ù ��° ��ü ���� : OBJ ��  --------------------------------------------------
		objModel.init();
		//objModel.setupMesh(36, box_positions, box_texCoords, box_normals);
		//objModel.loadDiffuseMap("../../src/_myApp_/container2.png");
		//objModel.loadSpecularMap("../../src/_myApp_/container2_specular.png");
		//objModel.loadOBJ("../../src/_myApp_/suzanne.obj");
		//objModel.defaultDiffuse = vmath::vec3(1.0f, 0.0f, 0.0f);
		objModel.loadOBJ("low_green_apple.obj");
		objModel.loadDiffuseMap("GREEN_baseColor.png");

		// Green Apple Room ----------------------------------------------------------------
		green_apple_floor.init(PLANE_TOP, 1.0f, 0.0f, 5.0f, 5.0f);
		green_apple_floor.loadDiffuseMap("woodtexture.png");
		green_apple_wall.init(PLANE_FRONT, 1.0f, 0.8f, 3.0f, 3.0f);
		green_apple_wall.loadDiffuseMap("pink_wall.png");
		green_apple_ceiling.init(PLANE_BOTTOM, 1.0f, 1.0f, 5.0f, 5.0f);
		green_apple_ceiling.loadDiffuseMap("ivory_ceiling.png");


		// �� ��° ��ü ���� : �Ƕ�̵� --------------------------------------------------
		// �Ƕ�̵� ������ ��ġ�� �÷�, �ؽ�ó ��ǥ�� �����Ѵ�.
		GLfloat pyramid_vertices[] = {
			1.0f, 0.0f, -1.0f,    // ���� ���
			-1.0f, 0.0f, -1.0f,   // ���� ���
			-1.0f, 0.0f, 1.0f,    // ���� �ϴ�
			1.0f, 0.0f, 1.0f,     // ���� �ϴ�
			0.0f, 1.0f, 0.0f,      // ��� ������
			0.0f, -1.0f, 0.0f,      // �ϴ� ������
		};

		// �ﰢ������ �׸� �ε����� �����Ѵ�.
		GLuint pyramid_indices[] = {
			4, 0, 1,
			4, 1, 2,
			4, 2, 3,
			4, 3, 0,

			5, 1, 0,
			5, 2, 1,
			5, 3, 2,
			5, 0, 3,
		};

		pyramidModel.init();
		pyramidModel.setupMesh(6, pyramid_vertices);
		pyramidModel.setupIndices(24, pyramid_indices);

		glEnable(GL_MULTISAMPLE);

		// Camera setup ------------------------------------------------------
		camera.eye = vmath::vec3(0.0f, 1.0f, 5.f);
		camera.center = vmath::vec3(0.0f, 0.0, 0.0);
		camera.up = vmath::vec3(0.0, 1.0f, 0.0);
		camera.fov = 50.f;
	}

	// ���ø����̼� ���� �� ȣ��ȴ�.
	virtual void shutdown()
	{
		glDeleteProgram(shader_program);
	}

	// ������ virtual �Լ��� �ۼ��ؼ� �������̵��Ѵ�.
	virtual void render(double currentTime) // currentTime : ���α׷��� ����� ���� �ð��� �󸶳� �ɷȴ��� ����Ǵ� ����
	{	
		if (pause) {
			previousTime = currentTime;
			return; // render �Լ��� ���ߵ���
		}

		animationTime += currentTime - previousTime;
		previousTime = currentTime;

		//const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f, (float)cos(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f };
		const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, black);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);


		// ī�޶� ��Ʈ���� ��� -------------------------------------------------------------
		//float distance = 5.f;
		//vmath::vec3 eye((float)cos(animationTime*0.3f)*distance, 1.0, (float)sin(animationTime*0.3f)*distance);
		//vmath::vec3 eye(0.0f, 1.0f, distance);
		//vmath::vec3 center(0.0, 0.0, 0.0);
		//vmath::vec3 up(0.0, 1.0, 0.0);
		//vmath::mat4 lookAt = vmath::lookat(eye, center, up);
		//vmath::mat4 projM = vmath::perspective(fov, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
		vmath::mat4 lookAt = camera.lookat();
		vmath::mat4 projM = camera.perspective(info.windowWidth, info.windowHeight, 0.1f, 1000.0f);

		// ������ ���� ---------------------------------------
		float r = 2.f;
		//vmath::vec3 pointLightPos[] = { vmath::vec3((float)sin(animationTime * 0.5f) * r, 0.0f, (float)cos(animationTime * 0.5f) * r),
		//	vmath::vec3(0.0f, (float)sin(animationTime * 1.f) * r, (float)cos(animationTime * 1.f) * r) };
		vmath::vec3 pointLightPos[] = { vmath::vec3(0.0f,4.0f,4.0f), vmath::vec3(0.0f,8.0f,0.0f) };
		vmath::vec3 lightColor(1.0f, 1.0f, 1.0f);
		vmath::vec3 viewPos = camera.eye;

		// �� �׸��� ---------------------------------------
		glUseProgram(shader_program);

		glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, projM);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, lookAt);

		glUniform3fv(glGetUniformLocation(shader_program, "viewPos"), 1, viewPos);

		glUniform3f(glGetUniformLocation(shader_program, "dirLight.direction"), 1.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader_program, "dirLight.ambient"), 0.3f, 0.3f, 0.3f);
		glUniform3f(glGetUniformLocation(shader_program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(shader_program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

		glUniform3fv(glGetUniformLocation(shader_program, "pointLights[0].position"), 1, pointLightPos[0]);
		glUniform3f(glGetUniformLocation(shader_program, "pointLights[0].ambient"), 0.3f, 0.3f, 0.3f);
		glUniform3f(glGetUniformLocation(shader_program, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shader_program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader_program, "pointLights[0].c1"), 0.09f);
		glUniform1f(glGetUniformLocation(shader_program, "pointLights[0].c2"), 0.032f);
		glUniform3fv(glGetUniformLocation(shader_program, "pointLights[1].position"), 1, pointLightPos[1]);
		glUniform3f(glGetUniformLocation(shader_program, "pointLights[1].ambient"), 0.3f, 0.3f, 0.3f);
		glUniform3f(glGetUniformLocation(shader_program, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shader_program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader_program, "pointLights[1].c1"), 0.09f);
		glUniform1f(glGetUniformLocation(shader_program, "pointLights[1].c2"), 0.032f);

		//glUniform3fv(glGetUniformLocation(shader_program, "spotLight.position"), 1, camera.eye);
		//glUniform3fv(glGetUniformLocation(shader_program, "spotLight.direction"), 1, camera.center - camera.eye);
		//glUniform1f(glGetUniformLocation(shader_program, "spotLight.cutOff"), (float)cos(vmath::radians(12.5)));
		//glUniform1f(glGetUniformLocation(shader_program, "spotLight.outerCutOff"), (float)cos(vmath::radians(15.5)));
		//glUniform1f(glGetUniformLocation(shader_program, "spotLight.c1"), 0.09f);
		//glUniform1f(glGetUniformLocation(shader_program, "spotLight.c2"), 0.032f);
		//glUniform3f(glGetUniformLocation(shader_program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		//glUniform3f(glGetUniformLocation(shader_program, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
		//glUniform3f(glGetUniformLocation(shader_program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);

		// Green Apple Room -----------------------------------------------------------
		// green apple
		vmath::mat4 apple_room_translate = vmath::translate(center[1], 0.f, 0.f);
		vmath::mat4 model = vmath::translate(0.f, 3.25f, -1.f) *
			vmath::scale(4.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, apple_room_translate * model);
		objModel.draw(shader_program);

		// BOTTOM
		model = vmath::scale(10.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, apple_room_translate * model);
		green_apple_floor.draw(shader_program);

		// BACK
		model = vmath::translate(0.f, 0.f, -10.f) *
			vmath::scale(10.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, apple_room_translate * model);
		green_apple_wall.draw(shader_program);
		// LEFT
		model = vmath::translate(-10.f, 0.f, 0.f) *
			vmath::rotate(0.0f, 90.f, 0.0f) *
			vmath::scale(10.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, apple_room_translate * model);
		green_apple_wall.draw(shader_program);
		// Right
		model = vmath::translate(10.f, 0.f, 0.f) *
			vmath::rotate(0.0f, -90.f, 0.0f) *
			vmath::scale(10.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, apple_room_translate * model);
		green_apple_wall.draw(shader_program);
		// TOP
		model = vmath::translate(0.f, 8.0f, 0.f) *
			vmath::scale(10.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, apple_room_translate * model);
		green_apple_ceiling.draw(shader_program);



		// �Ƕ�̵� �׸��� ---------------------------------------
		for (int i = 0; i < 2; i++)
		{
			float scaleFactor = 0.05f;
			vmath::mat4 transform = vmath::translate(pointLightPos[i]) *
				vmath::scale(scaleFactor, scaleFactor, scaleFactor);
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, apple_room_translate * transform);

			pyramidModel.draw(shader_program);
		}

		// ObjectRoom -----------------------------------------------------
		// object

	}

	virtual void onKey(int key, int action) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			switch (key) {
			case ' ':
				pause = !pause;
				break;
			case '1':
				drawModel = !drawModel;
				break;
			case 'W':
				camera.moveAlongDirection(FORWARD);
				break;
			case 'S':
				camera.moveAlongDirection(BACKWARD);
				break;
			case 'D':
				camera.moveAlongDirection(RIGHT);
				break;
			case 'A':
				camera.moveAlongDirection(LEFT);
				break;
			default:
				break;
			};
		}
	}

	virtual void onMouseButton(int button, int action) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			mouseDown = true;

			int x, y;
			getMousePosition(x, y);
			//mousePosition = vmath::vec2(float(x), float(y));
			last_mouse_x = x;
			last_mouse_y = y;
		}
		else
		{
			mouseDown = false;
		}

	}

	virtual void onMouseMove(int x, int y)
	{
		if (mouseDown)
		{
			//objYangle += float(x - mousePostion[0]) / 2.f;
			//mousePostion = vmath::vec2(float(x), float(y));
			camera.rotate(x - last_mouse_x, y - last_mouse_y);
			last_mouse_x = x;
			last_mouse_y = y;
		}
	}

	virtual void onMouseWheel(int pos)
	{
		//int deltaPos = pos - wheelPos;
		//if (deltaPos >0)
		// fov = vmath::min(MAX_FOV, fov + 1.0f); 
		//else
		// fov = vmath::max(MIN_FOV, fov - 1.0f);

		//wheelPos = pos;

		camera.onMouseWheel(pos);
	}

	virtual void onResize(int w, int h)
	{
		sb7::application::onResize(w, h);

		if (glViewport != NULL)
			glViewport(0, 0, info.windowWidth, info.windowHeight);
	}

	void initValues()
	{
		drawModel = true;
		drawLight = true;
		pause = false;
		animationTime = 0;
		previousTime = 0;
		lineMode = false;
		mouseDown = false;
		
		center[0] = 0.0f; // ���ι�
		center[1] = 1000.f; // �����
		center[2] = 0.0f; // �繰��
		center[3] = 0.0f; // ���ù�
		//wheelPos = 0;

		fov = 50.f;
	}

private:
	GLuint shader_program;

	Model objModel, pyramidModel;
	vmath::vec3 objPosition;
	Primitive green_apple_floor;
	Primitive green_apple_wall;
	Primitive green_apple_ceiling;
		
	float fov;
	float objYangle;
	float center[4];
	bool drawModel, drawLight;
	bool lineMode;
	bool edgeKernelMode;
	bool pause;
	bool mouseDown;
	
	double previousTime;
	double animationTime;

	int last_mouse_x, last_mouse_y;

	Camera camera;
};

// DECLARE_MAIN�� �ϳ����� �ν��Ͻ�
DECLARE_MAIN(my_application)