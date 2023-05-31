// sb7.h 헤더 파일을 포함시킨다.
#include <sb7.h>
#include <vmath.h>
#include <shader.h>
#include <vector>

#include "primitive.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "model.h"



// sb6::application을 상속받는다.
class my_application : public sb7::application
{
public:
	// 쉐이더 프로그램 컴파일한다.
	GLuint compile_shader(const char * vertex_shader_name, const char * fragment_shader_name)
	{
		// 버텍스 쉐이더를 생성하고 컴파일한다.
		GLuint vertex_shader = sb7::shader::load(vertex_shader_name, GL_VERTEX_SHADER);

		// 프래그먼트 쉐이더를 생성하고 컴파일한다.
		GLuint fragment_shader = sb7::shader::load(fragment_shader_name, GL_FRAGMENT_SHADER);

		// 프로그램을 생성하고 쉐이더를 Attach시키고 링크한다.
		GLuint program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);
		glLinkProgram(program);

		// 이제 프로그램이 쉐이더를 소유하므로 쉐이더를 삭제한다.
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

	// 애플리케이션 초기화 수행한다.
	virtual void startup()
	{
		// 쉐이더 프로그램 컴파일 및 연결
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

		// 두 번째 객체 정의 : 피라미드 --------------------------------------------------
		// 피라미드 점들의 위치와 컬러, 텍스처 좌표를 정의한다.
		GLfloat pyramid_vertices[] = {
			1.0f, 0.0f, -1.0f,    // 우측 상단
			-1.0f, 0.0f, -1.0f,   // 좌측 상단
			-1.0f, 0.0f, 1.0f,    // 좌측 하단
			1.0f, 0.0f, 1.0f,     // 우측 하단
			0.0f, 1.0f, 0.0f,      // 상단 꼭지점
			0.0f, -1.0f, 0.0f,      // 하단 꼭지점
		};

		// 삼각형으로 그릴 인덱스를 정의한다.
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

		
		// 프레임버퍼 [0]
		glBindFramebuffer(GL_FRAMEBUFFER, FBO[0]);
		// color buffer 텍스처 생성 및 연결
		load_buffer_texture(FBO_texture[0], info.windowWidth, info.windowHeight);

		// depth&stencil buffer를 위한 Render Buffer Object 생성 및 연결
		
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, info.windowWidth, info.windowHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
		

		// 잘 연결되었는지 체크
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			glfwTerminate();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		
		// 프레임버퍼 [1]
		glBindFramebuffer(GL_FRAMEBUFFER, FBO[1]);
		load_buffer_texture(FBO_texture[1], info.windowWidth, info.windowHeight);

		// 잘 연결되었는지 체크
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			glfwTerminate();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		//	post-processing용 스크린 정의
		GLfloat screenVertices[] = {
		//NDC xy좌표 //텍스처 uv
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		};


		// VBO를 생성하여 vertices 값들을 복사
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);

		// VBO를 나누어서 각 버텍스 속성으로 연결
		// 위치 속성 (location = 0)
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// 텍스쳐 속성 (location = 1)
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// VBO 및 버텍스 속성을 다 했으니 VBO와 VAO를 unbind한다.
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// 애플리케이션 끝날 때 호출된다.
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

	// 렌더링 virtual 함수를 작성해서 오버라이딩한다.
	virtual void render(double currentTime)
	{


		// 카메라 매트릭스 계산
		float distance = 2.f;
		vmath::vec3 eye(0.0f, 0.5f, distance);
		vmath::vec3 center(0.0f, 0.0f, 0.0f);
		vmath::vec3 up(0.0, 1.0, 0.0);
		vmath::mat4 lookAt = vmath::lookat(eye, center, up);
		float fov = 50.f;// (float)cos(currentTime)*20.f + 50.0f;
		vmath::mat4 projM = vmath::ortho(-1, 1, -1, 1 , -1, -100); //vmath::perspective(fov, info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);

		
		// Render-To-Texture Framebuffer 바인딩 ----------------------------------------------------------------------
		// FBO 바인딩
		glBindFramebuffer(GL_FRAMEBUFFER, FBO[0]);
		load_buffer_texture(FBO_texture[0], info.windowWidth, info.windowHeight);

		// FBO에 연결된 버퍼들의 값을 지우고, 뎁스 테스팅 활성화
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// 라이팅 설정 ---------------------------------------
		vmath::vec3 lightPos = vmath::vec3((float)sin(currentTime * 0.5f), 0.25f, (float)cos(currentTime * 0.5f) * 0.7f);// (0.0f, 0.5f, 0.0f);

		vmath::vec3 pointLightPos[] = { vmath::vec3((float)sin(currentTime * 0.5f), 0.25f, (float)cos(currentTime * 0.5f) * 0.7f),
										vmath::vec3( 0.25f, (float)sin(currentTime * 1.f), (float)cos(currentTime * 1.f) * 0.7f),
										vmath::vec3((float)cos(currentTime * 1.5f) * 0.7f, (float)sin(currentTime * 1.5f) * 0.7f , 0.25f) };
		vmath::vec3 pointLightColors[] = { vmath::vec3(1.f, 0.f, 0.f),
											vmath::vec3(0.f, 1.f, 0.f),
											vmath::vec3(0.f, 0.f, 1.f) };

		vmath::vec3 lightColor(1.0f, 1.0f, 1.0f);
		vmath::vec3 viewPos = eye;

		// 박스1 그리기 ---------------------------------------
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

		// 피라미드 그리기 (광원) ---------------------------------------
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

		
		// 기본 Framebuffer로 되돌리기
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);

		
		// 화면을 담기 위한 FBO [1]
		glBindFramebuffer(GL_FRAMEBUFFER, FBO[1]);
		load_buffer_texture(FBO_texture[1], info.windowWidth, info.windowHeight);

		// 버퍼들의 값 지우기
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader_program_texture);

		// FBO Texture[0] 액티브
		glUniformMatrix4fv(glGetUniformLocation(shader_program_texture, "model"), 1, GL_FALSE, projM * lookAt * rotateM * vmath::scale(1.3f) * vmath::translate(0.0f, -0.25f, 0.0f));
		glUniform1i(glGetUniformLocation(shader_program_texture, "texture1"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, FBO_texture[0]);
		
		boxModel[1].draw(shader_program_texture);
		
		
		// 기본 Framebuffer로 되돌리기
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		//버퍼들의 값 지우기
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// FBO Texture를 스크린 쉐이더 프로그램에 연결
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

// DECLARE_MAIN의 하나뿐인 인스턴스
DECLARE_MAIN(my_application)