// sb6.h ��� ������ ���Խ�Ų��.
#include <sb7.h>
#include <vmath.h>
#include <shader.h>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Model.h"

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
	}

	// ���ø����̼� ���� �� ȣ��ȴ�.
	virtual void shutdown()
	{
		glDeleteProgram(shader_program);
	}

	// ������ virtual �Լ��� �ۼ��ؼ� �������̵��Ѵ�.
	virtual void render(double currentTime)
	{
		//const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f, (float)cos(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f };
		const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, black);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);


		// ī�޶� ��Ʈ���� ���
		float distance = 5.f;
		//vmath::vec3 eye((float)cos(animationTime*0.3f)*distance, 1.0, (float)sin(animationTime*0.3f)*distance);
		vmath::vec3 eye(0.0f, 1.0f, distance);
		vmath::vec3 center(0.0, 0.0, 0.0);
		vmath::vec3 up(0.0, 1.0, 0.0);
		vmath::mat4 lookAt = vmath::lookat(eye, center, up);
		float fov = 50.f;// (float)cos(currentTime)*20.f + 50.0f;
		vmath::mat4 projM = vmath::perspective(fov, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);

		// ������ ���� ---------------------------------------
		float r = 2.f;
		vmath::vec3 pointLightPos[] = { vmath::vec3((float)sin(currentTime * 0.5f) * r, 0.0f, (float)cos(currentTime * 0.5f) * r),
			vmath::vec3(0.0f, (float)sin(currentTime * 1.f) * r, (float)cos(currentTime * 1.f) * r) };
		vmath::vec3 lightColor(1.0f, 1.0f, 1.0f);
		vmath::vec3 viewPos = eye;

		// �� �׸��� ---------------------------------------
		glUseProgram(shader_program);

		glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, projM);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, lookAt);

		glUniform3fv(glGetUniformLocation(shader_program, "viewPos"), 1, viewPos);

		glUniform3f(glGetUniformLocation(shader_program, "dirLight.direction"), -1.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader_program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader_program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(shader_program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

		glUniform3fv(glGetUniformLocation(shader_program, "pointLights[0].position"), 1, pointLightPos[0]);
		glUniform3f(glGetUniformLocation(shader_program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader_program, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shader_program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader_program, "pointLights[0].c1"), 0.09f);
		glUniform1f(glGetUniformLocation(shader_program, "pointLights[0].c2"), 0.032f);
		glUniform3fv(glGetUniformLocation(shader_program, "pointLights[1].position"), 1, pointLightPos[1]);
		glUniform3f(glGetUniformLocation(shader_program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader_program, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shader_program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader_program, "pointLights[1].c1"), 0.09f);
		glUniform1f(glGetUniformLocation(shader_program, "pointLights[1].c2"), 0.032f);

		glUniform3fv(glGetUniformLocation(shader_program, "spotLight.position"), 1, eye);
		glUniform3fv(glGetUniformLocation(shader_program, "spotLight.direction"), 1, center - eye);
		glUniform1f(glGetUniformLocation(shader_program, "spotLight.cutOff"), (float)cos(vmath::radians(12.5)));
		glUniform1f(glGetUniformLocation(shader_program, "spotLight.outerCutOff"), (float)cos(vmath::radians(15.5)));
		glUniform1f(glGetUniformLocation(shader_program, "spotLight.c1"), 0.09f);
		glUniform1f(glGetUniformLocation(shader_program, "spotLight.c2"), 0.032f);
		glUniform3f(glGetUniformLocation(shader_program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader_program, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shader_program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);

		vmath::mat4 model = vmath::translate(0.f, -0.5f, 0.f) *
			vmath::rotate(180.f, 0.0f, 1.0f, 0.0f) *
			vmath::scale(0.3f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, model);
		objModel.draw(shader_program);

		// �Ƕ�̵� �׸��� ---------------------------------------
		for (int i = 0; i < 2; i++)
		{
			float scaleFactor = 0.05f;
			vmath::mat4 transform = vmath::translate(pointLightPos[i]) *
				vmath::scale(scaleFactor, scaleFactor, scaleFactor);
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, transform);

			pyramidModel.draw(shader_program);
		}



	}

	virtual void onResize(int w, int h)
	{
		sb7::application::onResize(w, h);

		if (glViewport != NULL)
			glViewport(0, 0, info.windowWidth, info.windowHeight);
	}



private:
	GLuint shader_program;

	Model objModel, pyramidModel;
};

// DECLARE_MAIN�� �ϳ����� �ν��Ͻ�
DECLARE_MAIN(my_application)