// sb6.h 헤더 파일을 포함시킨다.
#include <sb7.h>
#include <vmath.h>
#include <shader.h>
#include <vector>
#include "primitive.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Model.h"
#include "Camera.h"

// sb6::application을 상속받는다.
class my_application : public sb7::application
{
public:
	// 쉐이더 컴파일한다.
	// basic multiple lighting shader
	GLuint compile_shader(void)
	{
		// 버텍스 쉐이더를 생성하고 컴파일한다.
		GLuint vertex_shader = sb7::shader::load("multiple_lights_vs.glsl", GL_VERTEX_SHADER);

		// 프래그먼트 쉐이더를 생성하고 컴파일한다.
		GLuint fragment_shader = sb7::shader::load("multiple_lights_fs.glsl", GL_FRAGMENT_SHADER);

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

	// 애플리케이션 초기화 수행한다.
	virtual void startup()
	{
		initValues();


		// 쉐이더 프로그램 컴파일 및 연결
		shader_program = compile_shader();

		stbi_set_flip_vertically_on_load(true);


		// 첫 번째 객체 정의 : OBJ 모델  --------------------------------------------------
		apple.init();
		//objModel.setupMesh(36, box_positions, box_texCoords, box_normals);
		//objModel.loadDiffuseMap("../../src/_myApp_/container2.png");
		//objModel.loadSpecularMap("../../src/_myApp_/container2_specular.png");
		//objModel.loadOBJ("../../src/_myApp_/suzanne.obj");
		//objModel.defaultDiffuse = vmath::vec3(1.0f, 0.0f, 0.0f);
		apple.loadOBJ("model/low_green_apple.obj");
		apple.loadDiffuseMap("GREEN_baseColor.png");

		glass.init();
		//glass.loadOBJ("model/lowwineglass.obj");
		glass.loadOBJ("model/glass2_removevertices.obj");
		glass.loadDiffuseMap("glass_basecolor.png");
		glass.alpha = 0.8f;
		
		
		comb.init();
		comb.loadOBJ("model/comb.obj");
		comb.loadDiffuseMap("comb_basecolor.png");

		bedding.init();
		bedding.loadOBJ("model/Bedding.obj");
		bedding.loadDiffuseMap("Bedding.png");

		bedframe.init();
		bedframe.loadOBJ("model/Bedframe.obj");
		bedframe.loadDiffuseMap("Bedframe.png");

		closet.init();
		closet.loadOBJ("model/closet.obj");
		closet.loadDiffuseMap("closet_texture.jpg");
		closet.loadSpecularMap("closet_specular.jpg");

		brush.init();
		brush.loadOBJ("model/brush.obj");
		brush.loadDiffuseMap("brush.png");
	
		soap.init();
		soap.loadOBJ("model/soap.obj");
		soap.loadDiffuseMap("soap_basecolor.png");

		match.init();
		match.loadOBJ("model/match.obj");
		match.loadDiffuseMap("match.jpg");

		carpet[0].init();
		carpet[0].loadOBJ("model/carpet.obj");
		carpet[0].loadDiffuseMap("carpet_basecolor.png");

		carpet[1].init();
		carpet[1].loadOBJ("model/carpet.obj");
		carpet[1].loadDiffuseMap("carpet2_basecolor.png");

		// Green Apple Room Box ----------------------------------------------------------------
		green_apple_floor.init(PLANE_TOP, 1.0f, 0.0f, 5.0f, 5.0f);
		green_apple_floor.loadDiffuseMap("woodtexture.png");
		green_apple_wall.init(PLANE_FRONT, 1.0f, 0.8f, 3.0f, 3.0f);
		green_apple_wall.loadDiffuseMap("pink_wall.png");
		green_apple_ceiling.init(PLANE_BOTTOM, 1.0f, 1.0f, 5.0f, 5.0f);
		green_apple_ceiling.loadDiffuseMap("ivory_ceiling.png");


		// Object Room Box
		object_floor.init(PLANE_TOP, 1.0f, 0.0f, 10.0f, 10.0f);
		object_floor.loadDiffuseMap("woodtexture2.png");
		object_wall.init(PLANE_FRONT, 1.0f, 0.8f, 1.0f, 1.0f);
		object_wall.loadDiffuseMap("clouds.jpg");
		object_ceiling.init(PLANE_BOTTOM, 1.0f, 1.0f, 5.0f, 5.0f);
		object_ceiling.loadDiffuseMap("ivory_ceiling.png");


		// 세 번째 객체 정의 : 피라미드 --------------------------------------------------
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

		glEnable(GL_MULTISAMPLE);

		// Camera setup ------------------------------------------------------
		camera.eye = vmath::vec3(0.0f, 1.0f, 5.f);
		camera.center = vmath::vec3(0.0f, 0.0, 0.0);
		camera.up = vmath::vec3(0.0, 1.0f, 0.0);
		camera.fov = 50.f;
	}

	// 애플리케이션 끝날 때 호출된다.
	virtual void shutdown()
	{
		glDeleteProgram(shader_program);
	}

	// 렌더링 virtual 함수를 작성해서 오버라이딩한다.
	virtual void render(double currentTime) // currentTime : 프로그램이 실행된 이후 시간이 얼마나 걸렸는지 저장되는 변수
	{	
		if (pause) {
			previousTime = currentTime;
			return; // render 함수가 멈추도록
		}

		animationTime += currentTime - previousTime;
		previousTime = currentTime;

		//const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f, (float)cos(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f };
		const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, black);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// 카메라 매트릭스 계산 -------------------------------------------------------------
		//float distance = 5.f;
		//vmath::vec3 eye((float)cos(animationTime*0.3f)*distance, 1.0, (float)sin(animationTime*0.3f)*distance);
		//vmath::vec3 eye(0.0f, 1.0f, distance);
		//vmath::vec3 center(0.0, 0.0, 0.0);
		//vmath::vec3 up(0.0, 1.0, 0.0);
		//vmath::mat4 lookAt = vmath::lookat(eye, center, up);
		//vmath::mat4 projM = vmath::perspective(fov, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
		vmath::mat4 lookAt = camera.lookat();
		vmath::mat4 projM = camera.perspective(info.windowWidth, info.windowHeight, 0.1f, 1000.0f);

		// 라이팅 설정 ---------------------------------------
		float r = 2.f;
		//vmath::vec3 pointLightPos[] = { vmath::vec3((float)sin(animationTime * 0.5f) * r, 0.0f, (float)cos(animationTime * 0.5f) * r),
		//	vmath::vec3(0.0f, (float)sin(animationTime * 1.f) * r, (float)cos(animationTime * 1.f) * r) };
		vmath::vec3 pointLightPos[] = { vmath::vec3(0.0f,4.0f,4.0f), vmath::vec3(0.0f,8.0f,0.0f) };
		vmath::vec3 lightColor(1.0f, 1.0f, 1.0f);
		vmath::vec3 viewPos = camera.eye;


		// 모델 그리기 ---------------------------------------
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
		apple.draw(shader_program);


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

		// Object Room -----------------------------------------------------------
		// object

		vmath::mat4 object_room_translate = vmath::translate(center[2], 0.f, 0.f);
		model = vmath::translate(-1.2f, 1.6f, -4.5f) *
			vmath::rotate(90.f,0.0f,115.0f) *
			vmath::scale(0.06f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, object_room_translate* model);
		comb.draw(shader_program);
		
		model = vmath::translate(-2.8f, 0.5f, -4.f) *
			vmath::rotate(0.f, 90.0f, 0.f) *
			vmath::scale(1.f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, object_room_translate* model);
		bedframe.draw(shader_program);

		model = vmath::translate(-2.8f, 0.5f, -4.f) *
			vmath::rotate(0.f, 90.f, 0.f) *
			vmath::scale(1.f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, object_room_translate* model);
		bedding.draw(shader_program);

		model = vmath::translate(3.f, 0.0f, -4.3f) *
			vmath::rotate(0.f, -90.f, 0.f) *
			vmath::scale(15.f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, object_room_translate* model);
		closet.draw(shader_program);

		model = vmath::translate(2.6f, 5.4f, -4.f) *
			vmath::rotate(0.f, 25.f, 5.f) *
			vmath::scale(15.f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, object_room_translate* model);
		brush.draw(shader_program);

		model = vmath::translate(3.f, 0.45f, -0.2f) *
			vmath::rotate(0.f, 45.f, 0.f) *
			vmath::scale(0.4f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, object_room_translate* model);
		soap.draw(shader_program);

		model = vmath::translate(-2.f, 0.1f, 1.f) *
			vmath::rotate(0.f, 0.f, -89.f) *
			vmath::scale(100.f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, object_room_translate* model);
		match.draw(shader_program);

		model = vmath::translate(1.5f, 0.0f, -0.7f) *
			vmath::rotate(0.f, 100.f, 0.f) *
			vmath::scale(2.6f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, object_room_translate* model);
		carpet[0].draw(shader_program);

		model = vmath::translate(-2.4f, 0.01f, 0.7f) *
			vmath::rotate(0.f, 80.f, 0.f) *
			vmath::scale(2.f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, object_room_translate* model);
		carpet[1].draw(shader_program);

		// BOTTOM
		model = vmath::scale(10.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, object_room_translate* model);
		object_floor.draw(shader_program);

		// BACK
		model = vmath::translate(0.f, 0.f, -10.f) *
			vmath::scale(10.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, object_room_translate* model);
		object_wall.draw(shader_program);
		// LEFT
		model = vmath::translate(-10.f, 0.f, 0.f) *
			vmath::rotate(0.0f, 90.f, 0.0f) *
			vmath::scale(10.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, object_room_translate* model);
		object_wall.draw(shader_program);
		// Right
		model = vmath::translate(10.f, 0.f, 0.f) *
			vmath::rotate(0.0f, -90.f, 0.0f) *
			vmath::scale(10.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, object_room_translate* model);
		object_wall.draw(shader_program);
		// TOP
		model = vmath::translate(0.f, 8.0f, 0.f) *
			vmath::scale(10.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, object_room_translate* model);
		object_ceiling.draw(shader_program);

		model = vmath::translate(0.4f, 1.75f, 1.2f) *
			vmath::scale(0.4f);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, object_room_translate* model);
		glass.draw(shader_program);

		// 피라미드 그리기 ---------------------------------------
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
		
		center[0] = 0.0f; // 메인방
		center[1] = 10.f; // 사과방
		center[2] = 0.0f; // 사물방
		center[3] = 0.0f; // 착시방
		//wheelPos = 0;

		fov = 50.f;
	}

private:
	GLuint shader_program;

	Model pyramidModel;
	Model apple;
	Model glass, comb, bedding, bedframe, closet, brush, soap, match, carpet[2];
	vmath::vec3 objPosition;
	Primitive green_apple_floor, green_apple_wall, green_apple_ceiling;
	Primitive object_floor, object_wall, object_ceiling;
		
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

// DECLARE_MAIN의 하나뿐인 인스턴스
DECLARE_MAIN(my_application)