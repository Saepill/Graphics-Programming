// sb7.h ��� ������ ���Խ�Ų��.
#include <sb7.h>
#include <vmath.h>
#include <shader.h>
#include <vector>

#include "primitive.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "model.h"



// sb6::application�� ��ӹ޴´�.
class my_application : public sb7::application
{
public:
	// ���̴� ���α׷� �������Ѵ�.
	GLuint compile_shader(const char * vertex_shader_name, const char * fragment_shader_name)
	{
		// ���ؽ� ���̴��� �����ϰ� �������Ѵ�.
		GLuint vertex_shader = sb7::shader::load(vertex_shader_name, GL_VERTEX_SHADER);

		// �����׸�Ʈ ���̴��� �����ϰ� �������Ѵ�.
		GLuint fragment_shader = sb7::shader::load(fragment_shader_name, GL_FRAGMENT_SHADER);

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


	void load_buffer_texture(GLuint textureID, int width, int height)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
	}

	// ���ø����̼� �ʱ�ȭ �����Ѵ�.
	virtual void startup()
	{
		// ���̴� ���α׷� ������ �� ����
		shader_program_model = compile_shader("multiple_lights_vs.glsl","multiple_lights_fs.glsl" );
		shader_program_texture = compile_shader("basic_texturing_vs.glsl", "basic_texturing_fs.glsl");
		shader_program_pp = compile_shader("shader_program_screen_vs.glsl", "shader_program_screen_fs.glsl");

		stbi_set_flip_vertically_on_load(true);

		boxModel[0].init(CUBE, 0.5f, 0.5f, 1.0f, 1.0f);
		boxModel[0].loadDiffuseMap("container2.png");
		boxModel[0].loadSpecularMap("container2_specular.png");

		boxModel[1].init(CUBE, 0.5f, 0.5f, 1.0f, 1.0f);

		/*
		boxPositions.push_back(vmath::vec3(0.0f, 0.0f, 0.0f));
		boxPositions.push_back(vmath::vec3(2.0f, 5.0f, -15.0f));
		boxPositions.push_back(vmath::vec3(-1.5f, -2.2f, -2.5f));
		boxPositions.push_back(vmath::vec3(-3.8f, -2.0f, -12.3f));
		boxPositions.push_back(vmath::vec3(2.4f, -0.4f, -3.5f));
		boxPositions.push_back(vmath::vec3(-1.7f, 3.0f, -7.5f));
		boxPositions.push_back(vmath::vec3(1.3f, -2.0f, -2.5f));
		boxPositions.push_back(vmath::vec3(1.5f, 2.0f, -2.5f));
		boxPositions.push_back(vmath::vec3(1.5f, 0.2f, -1.5f));
		boxPositions.push_back(vmath::vec3(-1.3f, 1.0f, -1.5f));
		*/

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
		pyramidModel.setDefaultColor(vmath::vec3(1, 1, 1));

		
		glGenTextures(2, FBO_texture);
		glGenFramebuffers(2, FBO);

		
		// �����ӹ��� [0]
		glBindFramebuffer(GL_FRAMEBUFFER, FBO[0]);
		// color buffer �ؽ�ó ���� �� ����
		load_buffer_texture(FBO_texture[0], info.windowWidth, info.windowHeight);

		// depth&stencil buffer�� ���� Render Buffer Object ���� �� ����
		
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, info.windowWidth, info.windowHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
		

		// �� ����Ǿ����� üũ
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			glfwTerminate();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		
		// �����ӹ��� [1]
		glBindFramebuffer(GL_FRAMEBUFFER, FBO[1]);
		load_buffer_texture(FBO_texture[1], info.windowWidth, info.windowHeight);

		// �� ����Ǿ����� üũ
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			glfwTerminate();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		//	post-processing�� ��ũ�� ����
		GLfloat screenVertices[] = {
		//NDC xy��ǥ //�ؽ�ó uv
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		};


		// VBO�� �����Ͽ� vertices ������ ����
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);

		// VBO�� ����� �� ���ؽ� �Ӽ����� ����
		// ��ġ �Ӽ� (location = 0)
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// �ؽ��� �Ӽ� (location = 1)
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// VBO �� ���ؽ� �Ӽ��� �� ������ VBO�� VAO�� unbind�Ѵ�.
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// ���ø����̼� ���� �� ȣ��ȴ�.
	virtual void shutdown()
	{
		glDeleteTextures(2, FBO_texture);
		glDeleteBuffers(1, &VBO);
		glDeleteFramebuffers(2, FBO);

		glDeleteRenderbuffers(1, &RBO);
		glDeleteVertexArrays(1, &VAO);

		glDeleteProgram(shader_program_model);
		glDeleteProgram(shader_program_pp);
	}

	// ������ virtual �Լ��� �ۼ��ؼ� �������̵��Ѵ�.
	virtual void render(double currentTime)
	{


		// ī�޶� ��Ʈ���� ���
		float distance = 2.f;
		vmath::vec3 eye(0.0f, 0.5f, distance);
		vmath::vec3 center(0.0f, 0.0f, 0.0f);
		vmath::vec3 up(0.0, 1.0, 0.0);
		vmath::mat4 lookAt = vmath::lookat(eye, center, up);
		float fov = 50.f;// (float)cos(currentTime)*20.f + 50.0f;
		vmath::mat4 projM = vmath::ortho(-1, 1, -1, 1 , -1, -100); //vmath::perspective(fov, info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);

		
		// Render-To-Texture Framebuffer ���ε� ----------------------------------------------------------------------
		// FBO ���ε�
		glBindFramebuffer(GL_FRAMEBUFFER, FBO[0]);
		load_buffer_texture(FBO_texture[0], info.windowWidth, info.windowHeight);

		// FBO�� ����� ���۵��� ���� �����, ���� �׽��� Ȱ��ȭ
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// ������ ���� ---------------------------------------
		vmath::vec3 lightPos = vmath::vec3((float)sin(currentTime * 0.5f), 0.25f, (float)cos(currentTime * 0.5f) * 0.7f);// (0.0f, 0.5f, 0.0f);

		vmath::vec3 pointLightPos[] = { vmath::vec3((float)sin(currentTime * 0.5f), 0.25f, (float)cos(currentTime * 0.5f) * 0.7f),
										vmath::vec3( 0.25f, (float)sin(currentTime * 1.f), (float)cos(currentTime * 1.f) * 0.7f),
										vmath::vec3((float)cos(currentTime * 1.5f) * 0.7f, (float)sin(currentTime * 1.5f) * 0.7f , 0.25f) };
		vmath::vec3 pointLightColors[] = { vmath::vec3(1.f, 0.f, 0.f),
											vmath::vec3(0.f, 1.f, 0.f),
											vmath::vec3(0.f, 0.f, 1.f) };

		vmath::vec3 lightColor(1.0f, 1.0f, 1.0f);
		vmath::vec3 viewPos = eye;

		// �ڽ�1 �׸��� ---------------------------------------
		vmath::mat4 transM = vmath::translate(vmath::vec3(0.0f, 0.0f, 0.0f));
		float angle = currentTime * 100;
		vmath::mat4 rotateM = vmath::rotate(angle, 0.0f, 1.0f, 0.0f);

		glUseProgram(shader_program_model);

#pragma region Shader_Uniform
		glUniformMatrix4fv(glGetUniformLocation(shader_program_model, "projection"), 1, GL_FALSE, projM * transM);
		glUniformMatrix4fv(glGetUniformLocation(shader_program_model, "view"), 1, GL_FALSE, lookAt);
		
		glUniform3fv(glGetUniformLocation(shader_program_model, "viewPos"), 1, viewPos);
		
		glUniform3f(glGetUniformLocation(shader_program_model, "dirLight.direction"), 0.0f, -1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader_program_model, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader_program_model, "dirLight.diffuse"), 1.0f, 1.f, 1.f);
		glUniform3f(glGetUniformLocation(shader_program_model, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

		glUniform1f(glGetUniformLocation(shader_program_model, "pointLights[0].c1"), 0.09f);
		glUniform1f(glGetUniformLocation(shader_program_model, "pointLights[0].c2"), 0.032f);
		glUniform3fv(glGetUniformLocation(shader_program_model, "pointLights[0].position"), 1, pointLightPos[0]);
		glUniform3f(glGetUniformLocation(shader_program_model, "pointLights[0].ambient"), 0.2f, 0.f, 0.f);
		glUniform3f(glGetUniformLocation(shader_program_model, "pointLights[0].diffuse"), 0.5f, 0.f, 0.f);
		glUniform3fv(glGetUniformLocation(shader_program_model, "pointLights[0].specular"), 1, pointLightColors[0]);

		glUniform1f(glGetUniformLocation(shader_program_model, "pointLights[1].c1"), 0.09f);
		glUniform1f(glGetUniformLocation(shader_program_model, "pointLights[1].c2"), 0.032f);
		glUniform3fv(glGetUniformLocation(shader_program_model, "pointLights[1].position"), 1, pointLightPos[1]);
		glUniform3f(glGetUniformLocation(shader_program_model, "pointLights[1].ambient"), 0.f, 0.2f, 0.f);
		glUniform3f(glGetUniformLocation(shader_program_model, "pointLights[1].diffuse"), 0.f, 0.5f, 0.f);
		glUniform3fv(glGetUniformLocation(shader_program_model, "pointLights[1].specular"), 1, pointLightColors[1]);

		glUniform1f(glGetUniformLocation(shader_program_model, "pointLights[2].c1"), 0.09f);
		glUniform1f(glGetUniformLocation(shader_program_model, "pointLights[2].c2"), 0.032f);
		glUniform3fv(glGetUniformLocation(shader_program_model, "pointLights[2].position"), 1, pointLightPos[2]);
		glUniform3f(glGetUniformLocation(shader_program_model, "pointLights[2].ambient"), 0.f, 0.f, 0.2f);
		glUniform3f(glGetUniformLocation(shader_program_model, "pointLights[2].diffuse"), 0.f, 0.f, 0.5f);
		glUniform3fv(glGetUniformLocation(shader_program_model, "pointLights[2].specular"), 1, pointLightColors[2]);

		
		glUniform3fv(glGetUniformLocation(shader_program_model, "spotLight.position"), 1, eye);
		glUniform3fv(glGetUniformLocation(shader_program_model, "spotLight.direction"), 1, center - eye);
		glUniform1f(glGetUniformLocation(shader_program_model, "spotLight.cutOff"), (float)cos(vmath::radians(12.5)));
		glUniform1f(glGetUniformLocation(shader_program_model, "spotLight.outerCutOff"), (float)cos(vmath::radians(15.5)));
		glUniform1f(glGetUniformLocation(shader_program_model, "spotLight.c1"), 0.09f);
		glUniform1f(glGetUniformLocation(shader_program_model, "spotLight.c2"), 0.032f);
		glUniform3f(glGetUniformLocation(shader_program_model, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader_program_model, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shader_program_model, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
#pragma endregion	

		/*
		for (int i = 0; i < boxPositions.size(); i++)
		{
			float angle = 10.f * i * currentTime;
			vmath::mat4 model = vmath::translate(boxPositions[i]) *
				vmath::rotate(angle, 0.0f, 0.3f, 1.0f) *
				vmath::rotate(angle, 1.0f, 0.3f, 0.5f) *
				vmath::scale(1.0f);
			glUniformMatrix4fv(glGetUniformLocation(shader_program_model, "model"), 1, GL_FALSE, model);
			boxModel.draw(shader_program_model);
		}
		*/
		vmath::mat4 model = 
			vmath::rotate(0.0f, 0.0f, 0.3f, 1.0f) *
			vmath::rotate(0.0f, 1.0f, 0.3f, 0.5f) *
			vmath::scale(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program_model, "model"), 1, GL_FALSE, model);
		boxModel[0].draw(shader_program_model);

		// �Ƕ�̵� �׸��� (����) ---------------------------------------
		float move_y = (float)cos(currentTime) * 0.2f + 0.5f;
		float scaleFactor = 0.05f;
		vmath::mat4 transform = vmath::translate(pointLightPos[0]) *
			vmath::rotate(angle, 0.0f, 1.0f, 0.0f) *
			vmath::scale(scaleFactor, scaleFactor, scaleFactor);
		
		pyramidModel.defaultColor= pointLightColors[0];
		glUniformMatrix4fv(glGetUniformLocation(shader_program_model, "projection"), 1, GL_FALSE, projM);
		glUniformMatrix4fv(glGetUniformLocation(shader_program_model, "view"), 1, GL_FALSE, lookAt);
		glUniformMatrix4fv(glGetUniformLocation(shader_program_model, "model"), 1, GL_FALSE, transform);

		pyramidModel.draw(shader_program_model);

		pyramidModel.defaultColor = pointLightColors[1];
		transform = vmath::translate(pointLightPos[1]) *
			vmath::rotate(angle , 0.0f, 1.0f, 0.0f) *
			vmath::scale(scaleFactor, scaleFactor, scaleFactor);


		glUniformMatrix4fv(glGetUniformLocation(shader_program_model, "model"), 1, GL_FALSE, transform);
		pyramidModel.draw(shader_program_model);


		pyramidModel.defaultColor = pointLightColors[2];
		transform = vmath::translate(pointLightPos[2]) *
			vmath::rotate(angle, 0.0f, 1.0f, 0.0f) *
			vmath::scale(scaleFactor, scaleFactor, scaleFactor);


		glUniformMatrix4fv(glGetUniformLocation(shader_program_model, "model"), 1, GL_FALSE, transform);
		pyramidModel.draw(shader_program_model);

		
		// �⺻ Framebuffer�� �ǵ�����
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);

		
		// ȭ���� ��� ���� FBO [1]
		glBindFramebuffer(GL_FRAMEBUFFER, FBO[1]);
		load_buffer_texture(FBO_texture[1], info.windowWidth, info.windowHeight);

		// ���۵��� �� �����
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader_program_texture);

		// FBO Texture[0] ��Ƽ��
		glUniformMatrix4fv(glGetUniformLocation(shader_program_texture, "model"), 1, GL_FALSE, projM * lookAt * rotateM * vmath::scale(1.3f) * vmath::translate(0.0f, -0.25f, 0.0f));
		glUniform1i(glGetUniformLocation(shader_program_texture, "texture1"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, FBO_texture[0]);
		
		boxModel[1].draw(shader_program_texture);
		
		
		// �⺻ Framebuffer�� �ǵ�����
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		//���۵��� �� �����
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// FBO Texture�� ��ũ�� ���̴� ���α׷��� ����
		glUseProgram(shader_program_pp);
		glUniform1i(glGetUniformLocation(shader_program_pp, "screenTexture"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, FBO_texture[1]);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		
			
	}

	void onResize(int w, int h)
	{
		sb7::application::onResize(w, h);
		if (glViewport != NULL) glViewport(0, 0, info.windowWidth, info.windowHeight);
	}


private:
	GLuint shader_program_model;
	GLuint shader_program_pp;
	GLuint shader_program_texture;
	//GLuint shader_basic_texture;
	GLuint VAO, VBO, FBO[2], RBO;
	GLuint FBO_texture[2];
	Model pyramidModel;
	
	Primitive boxModel[2];

	//std::vector<vmath::vec3> boxPositions;
};

// DECLARE_MAIN�� �ϳ����� �ν��Ͻ�
DECLARE_MAIN(my_application)