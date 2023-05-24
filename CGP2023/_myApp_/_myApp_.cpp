// sb7.h 헤더 파일을 포함시킨다.
#include <sb7.h>
#include <vmath.h>
#include <shader.h>
#include <vector>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// sb6::application을 상속받는다.
class my_application : public sb7::application
{
public:
	// 쉐이더 프로그램 컴파일한다.
	GLuint compile_shader(const char* vs_file, const char* fs_file)
	{
		// 버텍스 쉐이더를 생성하고 컴파일한다.
		GLuint vertex_shader = sb7::shader::load(vs_file, GL_VERTEX_SHADER);

		// 프래그먼트 쉐이더를 생성하고 컴파일한다.
		GLuint fragment_shader = sb7::shader::load(fs_file, GL_FRAGMENT_SHADER);

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

	void load_texture(GLuint textureID, char const* filename, GLint internalformat = GL_RGB, GLenum inputColorformat = GL_RGB)
	{
		// 텍스처 객체 만들고 바인딩		
		glBindTexture(GL_TEXTURE_2D, textureID);

		// 텍스처 이미지 로드하기
		int width, height, nrChannels;
		unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, inputColorformat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);

		// 텍스처 샘플링/필터링 설정
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	// 애플리케이션 초기화 수행한다.
	virtual void startup()
	{
		// 쉐이더 프로그램 컴파일 및 연결
		shader_programs[0] = compile_shader("basic_texturing_vs.glsl", "basic_texturing_fs.glsl");
		shader_programs[1] = compile_shader("basic_lighting_vs.glsl", "basic_lighting_fs.glsl");
		shader_programs[2] = compile_shader("simple_color_vs.glsl", "simple_color_fs.glsl");

		// VAO, VBO, EBO, texture 생성
		glGenVertexArrays(3, VAOs);
		glGenBuffers(3, VBOs);
		glGenBuffers(2, EBOs);
		glGenTextures(3, textures);
		glGenFramebuffers(1, &FBO);

		stbi_set_flip_vertically_on_load(true);

		load_texture(textures[0], "wall.jpg", GL_RGB, GL_RGB);
		load_texture(textures[1], "container2.png", GL_RGB, GL_RGBA);
		load_texture(textures[2], "container2_specular.png", GL_RGB, GL_RGBA);

		// 프레임버퍼
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		// color buffer 텍스처 생성 및 연결
		glGenTextures(1, &FBO_texture);
		glBindTexture(GL_TEXTURE_2D, FBO_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info.windowWidth, info.windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBO_texture, 0);

		// depth&stencil buffer를 위한 Render Buffer Object 생성 및 연결
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, info.windowWidth, info.windowHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

		// 잘 연결되었는지 체크
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			glfwTerminate();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);



		// 첫 번째 객체 정의 : 바닥 --------------------------------------------------
		glBindVertexArray(VAOs[0]);
		// 바닥 점들의 위치와 컬러, 텍스처 좌표를 정의한다.
		float floor_s = 3.0f, floor_t = 3.0f;
		GLfloat floor_vertices[] = {
			1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, floor_s, floor_t,  // 우측 상단
			-1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, floor_t,  // 좌측 상단
			-1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // 좌측 하단
			1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, floor_s, 0.0f   // 우측 하단
		};

		// 삼각형으로 그릴 인덱스를 정의한다.
		GLuint floor_indices[] = {
			0, 1, 2,	// 첫번째 삼각형
			0, 2, 3		// 두번째 삼각형
		};

		// VBO를 생성하여 vertices 값들을 복사
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);

		// VBO를 나누어서 각 버텍스 속성으로 연결
		// 위치 속성 (location = 0)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// 컬러 속성 (location = 1)
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// 텍스처 좌표 속성 (location = 2)
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		// EBO를 생성하고 indices 값들을 복사
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floor_indices), floor_indices, GL_STATIC_DRAW);

		// VBO 및 버텍스 속성을 다 했으니 VBO와 VAO를 unbind한다.
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



		// 두 번째 객체 정의 : 박스 1 --------------------------------------------------
		glBindVertexArray(VAOs[1]);
		// 박스 점들의 위치와 컬러, 텍스처 좌표를 정의한다.
		float box_s = 1.0f, box_t = 1.0f;
		GLfloat box_vertices[] = {
			// 뒷면
			-0.25f, 0.5f, -0.25f, 1.0f, 0.0f, 0.0f,		box_s, box_t,	0.0f, 0.0f, -1.0f,
			0.25f, 0.0f, -0.25f, 1.0f, 0.0f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
			-0.25f, 0.0f, -0.25f, 1.0f, 0.0f, 0.0f,		box_s, 0.0f,	0.0f, 0.0f, -1.0f,

			0.25f, 0.0f, -0.25f, 1.0f, 0.0f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
			-0.25f, 0.5f, -0.25f, 1.0f, 0.0f, 0.0f,		box_s, box_t,	0.0f, 0.0f, -1.0f,
			0.25f, 0.5f, -0.25f, 1.0f, 0.0f, 0.0f,		0.0f, box_t,	0.0f, 0.0f, -1.0f,
			// 우측면
			0.25f, 0.0f, -0.25f, 0.0f, 1.0f, 0.0f,		box_s, 0.0f,	1.0f, 0.0f, 0.0f,
			0.25f, 0.5f, -0.25f, 0.0f, 1.0f, 0.0f,		box_s, box_t,	1.0f, 0.0f, 0.0f,
			0.25f, 0.0f, 0.25f, 0.0f, 1.0f, 0.0f,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f,

			0.25f, 0.0f, 0.25f, 0.0f, 1.0f, 0.0f,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
			0.25f, 0.5f, -0.25f, 0.0f, 1.0f, 0.0f,		box_s, box_t,	1.0f, 0.0f, 0.0f,
			0.25f, 0.5f, 0.25f, 0.0f, 1.0f, 0.0f,		0.0f, box_t,	1.0f, 0.0f, 0.0f,
			// 정면
			0.25f, 0.0f, 0.25f, 0.0f, 0.0f, 1.0f,		box_s, 0.0f,	0.0f, 0.0f, 1.0f,
			0.25f, 0.5f, 0.25f, 0.0f, 0.0f, 1.0f,		box_s, box_t,	0.0f, 0.0f, 1.0f,
			-0.25f, 0.0f, 0.25f, 0.0f, 0.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 1.0f,

			-0.25f, 0.0f, 0.25f, 0.0f, 0.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			0.25f, 0.5f, 0.25f, 0.0f, 0.0f, 1.0f,		box_s, box_t,	0.0f, 0.0f, 1.0f,
			-0.25f, 0.5f, 0.25f, 0.0f, 0.0f, 1.0f,		0.0f, box_t,	0.0f, 0.0f, 1.0f,
			// 좌측면
			-0.25f, 0.0f, 0.25f, 1.0f, 0.0f, 1.0f,		box_s, 0.0f,	-1.0f, 0.0f, 0.0f,
			-0.25f, 0.5f, 0.25f, 1.0f, 0.0f, 1.0f,		box_s, box_t,	-1.0f, 0.0f, 0.0f,
			-0.25f, 0.0f, -0.25f, 1.0f, 0.0f, 1.0f,		0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,

			-0.25f, 0.0f, -0.25f, 1.0f, 0.0f, 1.0f,		0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
			-0.25f, 0.5f, 0.25f, 1.0f, 0.0f, 1.0f,		box_s, box_t,	-1.0f, 0.0f, 0.0f,
			-0.25f, 0.5f, -0.25f, 1.0f, 0.0f, 1.0f,		0.0f, box_t,	-1.0f, 0.0f, 0.0f,
			// 바닥면
			-0.25f, 0.0f, 0.25f, 1.0f, 1.0f, 0.0f,		box_s, 0.0f,	0.0f, -1.0f, 0.0f,
			0.25f, 0.0f, -0.25f, 1.0f, 1.0f, 0.0f,		0.0f, box_t,	0.0f, -1.0f, 0.0f,
			0.25f, 0.0f, 0.25f, 1.0f, 1.0f, 0.0f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,

			0.25f, 0.0f, -0.25f, 1.0f, 1.0f, 0.0f,		0.0f, box_t,	0.0f, -1.0f, 0.0f,
			-0.25f, 0.0f, 0.25f, 1.0f, 1.0f, 0.0f,		box_s, 0.0,		0.0f, -1.0f, 0.0f,
			-0.25f, 0.0f, -0.25f, 1.0f, 1.0f, 0.0f,		box_s, box_t,	0.0f, -1.0f, 0.0f,
			// 윗면
			-0.25f, 0.5f, -0.25f, 0.0f, 1.0f, 1.0f,		0.0f, box_t,	0.0f, 1.0f, 0.0f,
			0.25f, 0.5f, 0.25f, 0.0f, 1.0f, 1.0f,		box_s, 0.0f,	0.0f, 1.0f, 0.0f,
			0.25f, 0.5f, -0.25f, 0.0f, 1.0f, 1.0f,		box_s, box_t,	0.0f, 1.0f, 0.0f,

			0.25f, 0.5f, 0.25f, 0.0f, 1.0f, 1.0f,		box_s, 0.0f,	0.0f, 1.0f, 0.0f,
			-0.25f, 0.5f, -0.25f, 0.0f, 1.0f, 1.0f,		0.0f, box_t,	0.0f, 1.0f, 0.0f,
			-0.25f, 0.5f, 0.25f, 0.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 1.0f, 0.0f
		};

		// 박스 10개 포지션 설정
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

		// VBO를 생성하여 vertices 값들을 복사
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(box_vertices), box_vertices, GL_STATIC_DRAW);

		// VBO를 나누어서 각 버텍스 속성으로 연결
		// 위치 속성 (location = 0)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// 컬러 속성 (location = 1)
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// 텍스처 좌표 속성 (location = 2)
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		// 노멀 속성 (location = 3)
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);

		// VBO 및 버텍스 속성을 다 했으니 VBO와 VAO를 unbind한다.
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//  네 번째 객체 정의 : 피라미드 --------------------------------------------------
		glBindVertexArray(VAOs[2]);
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

		// VBO를 생성하여 vertices 값들을 복사
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_vertices), pyramid_vertices, GL_STATIC_DRAW);

		// VBO를 나누어서 각 버텍스 속성으로 연결
		// 위치 속성 (location = 0)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// EBO를 생성하고 indices 값들을 복사
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramid_indices), pyramid_indices, GL_STATIC_DRAW);

		// VBO 및 버텍스 속성을 다 했으니 VBO와 VAO를 unbind한다.
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	}

	// 애플리케이션 끝날 때 호출된다.
	virtual void shutdown()
	{
		glDeleteTextures(3, textures);
		glDeleteTextures(1, &FBO_texture);
		glDeleteBuffers(2, EBOs);
		glDeleteBuffers(3, VBOs);
		glDeleteFramebuffers(1, &FBO);

		glDeleteVertexArrays(3, VAOs);
		glDeleteProgram(shader_programs[0]);
		glDeleteProgram(shader_programs[1]);
		glDeleteProgram(shader_programs[2]);
	}

	// 렌더링 virtual 함수를 작성해서 오버라이딩한다.
	virtual void render(double currentTime)
	{

		GLint uniform_transform1 = glGetUniformLocation(shader_programs[0], "transform");
		GLint uniform_transform2 = glGetUniformLocation(shader_programs[1], "transform");

		// 카메라 매트릭스 계산
		float distance = 2.f;
		vmath::vec3 eye(0.0f, 0.5f, (float)cos(currentTime) + distance);
		vmath::vec3 center(0.0f, 0.0f, 0.0f);
		vmath::vec3 up(0.0, 1.0, 0.0);
		vmath::mat4 lookAt = vmath::lookat(eye, center, up);
		float fov = 50.f;// (float)cos(currentTime)*20.f + 50.0f;
		vmath::mat4 projM = vmath::perspective(fov, info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);


		// Render-To-Texture Framebuffer 바인딩 ----------------------------------------------------------------------
		// FBO 바인딩
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		// FBO에 연결된 버퍼들의 값을 지우고, 뎁스 테스팅 활성화
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// Do 1st Rendering

		/*
		// 바닥 그리기 ---------------------------------------
		glUseProgram(shader_programs[0]);
		glUniformMatrix4fv(glGetUniformLocation(shader_programs[0], "transform"), 1, GL_FALSE, projM * lookAt * vmath::scale(1.5f));
		glUniform1i(glGetUniformLocation(shader_programs[0], "texture1"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glBindVertexArray(VAOs[0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		*/

		// 라이팅 설정 ---------------------------------------
		vmath::vec3 lightPos = vmath::vec3((float)sin(currentTime * 0.5f), 0.25f, (float)cos(currentTime * 0.5f) * 0.7f);// (0.0f, 0.5f, 0.0f);

		vmath::vec3 pointLightPos[] = { vmath::vec3((float)sin(currentTime * 0.5f), 0.25f, (float)cos(currentTime * 0.5f) * 0.7f),
										vmath::vec3(0.25f, (float)sin(currentTime * 1.f), (float)cos(currentTime * 1.f) * 0.7f),
										vmath::vec3((float)cos(currentTime * 1.5f) * 0.7f, (float)sin(currentTime * 1.5f) * 0.7f , 0.25f) };
		vmath::vec3 pointLightColors[] = { vmath::vec3(1.f, 0.f, 0.f),
											vmath::vec3(0.f, 1.f, 0.f),
											vmath::vec3(0.f, 0.f, 1.f) };

		vmath::vec3 lightColor(1.0f, 1.0f, 1.0f);
		vmath::vec3 viewPos = eye;

		// 박스1 그리기 ---------------------------------------
		//vmath::mat4 transM = vmath::translate(vmath::vec3((float)sin(currentTime * 0.5f), 0.0f, (float)cos(currentTime * 0.5f) * 0.7f));
		vmath::mat4 transM = vmath::translate(vmath::vec3(0.0f, 0.0f, 0.0f));
		float angle = currentTime * 100;
		vmath::mat4 rotateM = vmath::rotate(angle, 0.0f, 1.0f, 0.0f);
		float yellow_shininess = 32.f;

		vmath::vec3 yellow_ambient(1.0f, 0.5f, 0.31f);
		vmath::vec3 yellow_diffuse(1.0f, 0.5f, 0.31f);
		vmath::vec3 yellow_specular(1.0f, 0.5f, 0.31f);


		glUseProgram(shader_programs[1]);
		glUniform1f(glGetUniformLocation(shader_programs[1], "material.shininess"), yellow_shininess);
		glUniform1i(glGetUniformLocation(shader_programs[1], "material.diffuse"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);

		glUniform1i(glGetUniformLocation(shader_programs[1], "material.specular"), 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures[2]);

		glUniform3f(glGetUniformLocation(shader_programs[1], "dirLight.direction"), 0.0f, -1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader_programs[1], "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader_programs[1], "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(shader_programs[1], "dirLight.specular"), 0.5f, 0.5f, 0.5f);

		glUniform1f(glGetUniformLocation(shader_programs[1], "pointLights[0].c1"), 0.09f);
		glUniform1f(glGetUniformLocation(shader_programs[1], "pointLights[0].c2"), 0.032f);
		glUniform3fv(glGetUniformLocation(shader_programs[1], "pointLights[0].position"), 1, pointLightPos[0]);
		glUniform3f(glGetUniformLocation(shader_programs[1], "pointLights[0].ambient"), 0.2f, 0.f, 0.f);
		glUniform3f(glGetUniformLocation(shader_programs[1], "pointLights[0].diffuse"), 0.5f, 0.f, 0.f);
		glUniform3fv(glGetUniformLocation(shader_programs[1], "pointLights[0].specular"), 1, pointLightColors[0]);

		glUniform1f(glGetUniformLocation(shader_programs[1], "pointLights[1].c1"), 0.09f);
		glUniform1f(glGetUniformLocation(shader_programs[1], "pointLights[1].c2"), 0.032f);
		glUniform3fv(glGetUniformLocation(shader_programs[1], "pointLights[1].position"), 1, pointLightPos[1]);
		glUniform3f(glGetUniformLocation(shader_programs[1], "pointLights[1].ambient"), 0.f, 0.2f, 0.f);
		glUniform3f(glGetUniformLocation(shader_programs[1], "pointLights[1].diffuse"), 0.f, 0.5f, 0.f);
		glUniform3fv(glGetUniformLocation(shader_programs[1], "pointLights[1].specular"), 1, pointLightColors[1]);


		glUniform1f(glGetUniformLocation(shader_programs[1], "pointLights[2].c1"), 0.09f);
		glUniform1f(glGetUniformLocation(shader_programs[1], "pointLights[2].c2"), 0.032f);
		glUniform3fv(glGetUniformLocation(shader_programs[1], "pointLights[2].position"), 1, pointLightPos[2]);
		glUniform3f(glGetUniformLocation(shader_programs[1], "pointLights[2].ambient"), 0.f, 0.f, 0.2f);
		glUniform3f(glGetUniformLocation(shader_programs[1], "pointLights[2].diffuse"), 0.f, 0.f, 0.5f);
		glUniform3fv(glGetUniformLocation(shader_programs[1], "pointLights[2].specular"), 1, pointLightColors[2]);

		glUniform3fv(glGetUniformLocation(shader_programs[1], "spotLight.position"), 1, eye);
		glUniform3fv(glGetUniformLocation(shader_programs[1], "spotLight.direction"), 1, center - eye);
		glUniform1f(glGetUniformLocation(shader_programs[1], "spotLight.cutOff"), (float)cos(vmath::radians(12.5)));
		glUniform1f(glGetUniformLocation(shader_programs[1], "spotLight.outerCutOff"), (float)cos(vmath::radians(15.5)));
		glUniform1f(glGetUniformLocation(shader_programs[1], "spotLight.c1"), 0.09f);
		glUniform1f(glGetUniformLocation(shader_programs[1], "spotLight.c2"), 0.032f);
		glUniform3f(glGetUniformLocation(shader_programs[1], "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader_programs[1], "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shader_programs[1], "spotLight.specular"), 1.0f, 1.0f, 1.0f);


		glUniformMatrix4fv(glGetUniformLocation(shader_programs[1], "projection"), 1, GL_FALSE, projM * transM);
		glUniformMatrix4fv(glGetUniformLocation(shader_programs[1], "view"), 1, GL_FALSE, lookAt);
		glUniformMatrix4fv(glGetUniformLocation(shader_programs[1], "model"), 1, GL_FALSE, rotateM);
		glUniform3fv(glGetUniformLocation(shader_programs[1], "viewPos"), 1, viewPos);

		glBindVertexArray(VAOs[1]);

		for (int i = 0; i < boxPositions.size(); i++)
		{
			float angle = 10.f * i * currentTime;
			vmath::mat4 model = vmath::translate(boxPositions[i]) *
				vmath::rotate(angle, 0.0f, 0.3f, 1.0f) *
				vmath::rotate(angle, 1.0f, 0.3f, 0.5f) *
				vmath::scale(1.0f);
			glUniformMatrix4fv(glGetUniformLocation(shader_programs[1], "model"), 1, GL_FALSE, model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glDrawArrays(GL_TRIANGLES, 0, 36);



		// 피라미드 그리기 (광원) ---------------------------------------
		float move_y = (float)cos(currentTime) * 0.2f + 0.5f;
		float scaleFactor = 0.05f;
		vmath::mat4 transform = vmath::translate(pointLightPos[0]) *
			vmath::rotate(angle, 0.0f, 1.0f, 0.0f) *
			vmath::scale(scaleFactor, scaleFactor, scaleFactor);



		glUseProgram(shader_programs[2]);

		glUniform3fv(glGetUniformLocation(shader_programs[2], "color"), 1, pointLightColors[0]);
		glUniformMatrix4fv(glGetUniformLocation(shader_programs[2], "projection"), 1, GL_FALSE, projM);
		glUniformMatrix4fv(glGetUniformLocation(shader_programs[2], "view"), 1, GL_FALSE, lookAt);
		glUniformMatrix4fv(glGetUniformLocation(shader_programs[2], "model"), 1, GL_FALSE, transform);

		glBindVertexArray(VAOs[2]);
		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);


		glUniform3fv(glGetUniformLocation(shader_programs[2], "color"), 1, pointLightColors[1]);
		transform = vmath::translate(pointLightPos[1]) *
			vmath::rotate(angle, 0.0f, 1.0f, 0.0f) *
			vmath::scale(scaleFactor, scaleFactor, scaleFactor);


		glUniformMatrix4fv(glGetUniformLocation(shader_programs[2], "model"), 1, GL_FALSE, transform);
		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);

		glUniform3fv(glGetUniformLocation(shader_programs[2], "color"), 1, pointLightColors[2]);
		transform = vmath::translate(pointLightPos[2]) *
			vmath::rotate(angle, 0.0f, 1.0f, 0.0f) *
			vmath::scale(scaleFactor, scaleFactor, scaleFactor);


		glUniformMatrix4fv(glGetUniformLocation(shader_programs[2], "model"), 1, GL_FALSE, transform);
		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);


		// 기본 Framebuffer로 되돌리기
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		// 버퍼들의 값 지우기
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// FBO Texture를 쉐이더 프로그램에 연결
		glUseProgram(shader_programs[0]);
		//glUniformMatrix4fv(uniform_transform1, 1, GL_FALSE, projM * lookAt * rotateM * vmath::scale(1.3f) * vmath::translate(0.0f, -0.25f, 0.0f));
		glUniform1i(glGetUniformLocation(shader_programs[0], "texture1"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, FBO_texture);
		
		glBindVertexArray(VAOs[1]);

		for (int i = 0; i < boxPositions.size(); i++)
		{
			float angle = 10.f * i * currentTime;
			vmath::mat4 model = vmath::translate(boxPositions[i]) *
				vmath::rotate(angle, 0.0f, 0.3f, 1.0f) *
				vmath::rotate(angle, 1.0f, 0.3f, 0.5f) *
				vmath::scale(1.0f);
			glUniformMatrix4fv(uniform_transform1, 1, GL_FALSE, projM * lookAt * model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		

	}

private:
	GLuint shader_programs[3];
	GLuint VAOs[3], VBOs[3], EBOs[2], FBO, RBO;
	GLuint textures[3], FBO_texture;

	std::vector<vmath::vec3> boxPositions;
};

// DECLARE_MAIN의 하나뿐인 인스턴스
DECLARE_MAIN(my_application)