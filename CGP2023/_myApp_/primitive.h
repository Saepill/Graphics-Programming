#ifndef PRIMITIVE_MODEL_H
#define PRIMITIVE_MODEL_H

#include <stdio.h>
#include <string>
#include <cstring>
#include <vector>

#include <sb7.h>
#include <vmath.h>
#include "stb_image.h"

enum PRIMITIVE_TYPE {
	CUBE, PYRAMID, PLANE_FRONT, PLANE_BACK, PLANE_RIGHT, PLANE_LEFT, PLANE_TOP, PLANE_BOTTOM
};

class Primitive {
public:
	float shininess, alpha , diffuse_control;
	GLuint diffuseMap, specularMap;
	vmath::vec3 defaultAmbient, defaultDiffuse, defaultSpecular;

private:
	GLuint VAO;
	GLuint VBO;
	int num_vertices = 0;
	bool useDiffuseMap, useSpecularMap;

public:
	Primitive() {
		shininess = 32.f;
		alpha = 1.0f;
		diffuse_control = 1.0f;

		useDiffuseMap = false;
		useSpecularMap = false;

		defaultAmbient = vmath::vec3(1.0f, 1.0f, 1.0f);
		defaultDiffuse = vmath::vec3(1.0f, 1.0f, 1.0f);
		defaultSpecular = vmath::vec3(0.0f, 0.0f, 0.0f);
	}

	~Primitive() {
		glDeleteTextures(1, &diffuseMap);
		glDeleteTextures(1, &specularMap);

		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}

	void init(PRIMITIVE_TYPE type, float width, float height, float tex_s, float tex_t) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glGenTextures(1, &diffuseMap);
		glGenTextures(1, &specularMap);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		float width_unit = width/2;

		switch(type){
			case CUBE:
			{
				GLfloat data[] = {
					// back
					-width_unit, height, -width_unit,	tex_s, tex_t,	0.0f, 0.0f, -1.0f,
					width_unit, 0.0f, -width_unit,	0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
					-width_unit, 0.0f, -width_unit, 	tex_s, 0.0f,	0.0f, 0.0f, -1.0f,

					width_unit, 0.0f, -width_unit,	0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
					-width_unit, height, -width_unit,	tex_s, tex_t,	0.0f, 0.0f, -1.0f,
					width_unit, height, -width_unit,	0.0f, tex_t,	0.0f, 0.0f, -1.0f,

					// right
					width_unit, 0.0f, -width_unit, 	tex_s, 0.0f,	1.0f, 0.0f, 0.0f,
					width_unit, height, -width_unit,	tex_s, tex_t,	1.0f, 0.0f, 0.0f,
					width_unit, 0.0f, width_unit, 	0.0f, 0.0f,		1.0f, 0.0f, 0.0f,

					width_unit, 0.0f, width_unit,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
					width_unit, height, -width_unit,	tex_s, tex_t,	1.0f, 0.0f, 0.0f,
					width_unit, height, width_unit,		0.0f, tex_t,	1.0f, 0.0f, 0.0f,

					// front
					width_unit, 0.0f, width_unit,		tex_s, 0.0f,	0.0f, 0.0f, 1.0f,
					width_unit, height, width_unit,		tex_s, tex_t,	0.0f, 0.0f, 1.0f,
					-width_unit, 0.0f, width_unit,	0.0f, 0.0f,		0.0f, 0.0f, 1.0f,

					-width_unit, 0.0f, width_unit,	0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
					width_unit, height, width_unit,		tex_s, tex_t,	0.0f, 0.0f, 1.0f,
					-width_unit, height, width_unit,	0.0f, tex_t,	0.0f, 0.0f, 1.0f,

					// left
					-width_unit, 0.0f, width_unit, 	tex_s, 0.0f,	-1.0f, 0.0f, 0.0f,
					-width_unit, height, width_unit,	tex_s, tex_t,	-1.0f, 0.0f, 0.0f,
					-width_unit, 0.0f, -width_unit,	0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,

					-width_unit, 0.0f, -width_unit,	0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
					-width_unit, height, width_unit,	tex_s, tex_t,	-1.0f, 0.0f, 0.0f,
					-width_unit, height, -width_unit,	0.0f, tex_t,	-1.0f, 0.0f, 0.0f,

					// bottom
					-width_unit, 0.0f, width_unit, 	tex_s, 0.0f,	0.0f, -1.0f, 0.0f,
					width_unit, 0.0f, -width_unit,	0.0f, tex_t,	0.0f, -1.0f, 0.0f,
					width_unit, 0.0f, width_unit,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,

					width_unit, 0.0f, -width_unit, 	0.0f, tex_t,	0.0f, -1.0f, 0.0f,
					-width_unit, 0.0f, width_unit,	tex_s, 0.0,		0.0f, -1.0f, 0.0f,
					-width_unit, 0.0f, -width_unit,	tex_s, tex_t,	0.0f, -1.0f, 0.0f,

					// top
					-width_unit, height, -width_unit,	0.0f, tex_t,	0.0f, 1.0f, 0.0f,
					width_unit, height, width_unit, 	tex_s, 0.0f,	0.0f, 1.0f, 0.0f,
					width_unit, height, -width_unit, 	tex_s, tex_t,	0.0f, 1.0f, 0.0f,

					width_unit, height, width_unit, 	tex_s, 0.0f,	0.0f, 1.0f, 0.0f,
					-width_unit, height, -width_unit,	0.0f, tex_t,	0.0f, 1.0f, 0.0f,
					-width_unit, height, width_unit,	0.0f, 0.0f,		0.0f, 1.0f, 0.0f
				};

				num_vertices = 36;
				glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
				break;
			}
			case PLANE_BACK: {
				GLfloat data[] = {
					// back
					-width_unit, height, -width_unit,	tex_s, tex_t,	0.0f, 0.0f, -1.0f,
					width_unit, 0.0f, -width_unit,	0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
					-width_unit, 0.0f, -width_unit, 	tex_s, 0.0f,	0.0f, 0.0f, -1.0f,

					width_unit, 0.0f, -width_unit,	0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
					-width_unit, height, -width_unit,	tex_s, tex_t,	0.0f, 0.0f, -1.0f,
					width_unit, height, -width_unit,	0.0f, tex_t,	0.0f, 0.0f, -1.0f
				};
				num_vertices = 6;
				glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
				break;
			}
			case PLANE_RIGHT: {
				GLfloat data[] = {
					// right
					width_unit, 0.0f, -width_unit, 	tex_s, 0.0f,	1.0f, 0.0f, 0.0f,
					width_unit, height, -width_unit,	tex_s, tex_t,	1.0f, 0.0f, 0.0f,
					width_unit, 0.0f, width_unit, 	0.0f, 0.0f,		1.0f, 0.0f, 0.0f,

					width_unit, 0.0f, width_unit,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
					width_unit, height, -width_unit,	tex_s, tex_t,	1.0f, 0.0f, 0.0f,
					width_unit, height, width_unit,		0.0f, tex_t,	1.0f, 0.0f, 0.0f
				};
				num_vertices = 6;
				glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
				break;
			}
			case PLANE_FRONT: {
				GLfloat data[] = {
					// front
					width_unit, 0.0f, width_unit,		tex_s, 0.0f,	0.0f, 0.0f, 1.0f,
					width_unit, height, width_unit,		tex_s, tex_t,	0.0f, 0.0f, 1.0f,
					-width_unit, 0.0f, width_unit,	0.0f, 0.0f,		0.0f, 0.0f, 1.0f,

					-width_unit, 0.0f, width_unit,	0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
					width_unit, height, width_unit,		tex_s, tex_t,	0.0f, 0.0f, 1.0f,
					-width_unit, height, width_unit,	0.0f, tex_t,	0.0f, 0.0f, 1.0f
				};
				num_vertices = 6;
				glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
				break;
			}
			case PLANE_LEFT:{
				GLfloat data[] = {
					// left
					-width_unit, 0.0f, width_unit, 	tex_s, 0.0f,	-1.0f, 0.0f, 0.0f,
					-width_unit, height, width_unit,	tex_s, tex_t,	-1.0f, 0.0f, 0.0f,
					-width_unit, 0.0f, -width_unit,	0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,

					-width_unit, 0.0f, -width_unit,	0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
					-width_unit, height, width_unit,	tex_s, tex_t,	-1.0f, 0.0f, 0.0f,
					-width_unit, height, -width_unit,	0.0f, tex_t,	-1.0f, 0.0f, 0.0f,
				};
				num_vertices = 6;
				glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
				break;
			}
			case PLANE_BOTTOM: {
				GLfloat data[] = {
					// bottom
					-width_unit, 0.0f, width_unit, 	tex_s, 0.0f,	0.0f, -1.0f, 0.0f,
					width_unit, 0.0f, -width_unit,	0.0f, tex_t,	0.0f, -1.0f, 0.0f, 
					width_unit, 0.0f, width_unit, 	tex_s, tex_t,	0.0f, -1.0f, 0.0f,

					width_unit, 0.0f, -width_unit, 	0.0f, tex_t,	0.0f, -1.0f, 0.0f,
					-width_unit, 0.0f, width_unit,	tex_s, 0.0,		0.0f, -1.0f, 0.0f,
					-width_unit, 0.0f, -width_unit,	tex_s, tex_t,	0.0f, -1.0f, 0.0f,
				};
				num_vertices = 6;
				glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
				break;
			}
			case PLANE_TOP:{
				GLfloat data[] = {
					// top
					-width_unit, height, -width_unit,	0.0f, tex_t,	0.0f, 1.0f, 0.0f,
					width_unit, height, width_unit, 	tex_s, 0.0f,	0.0f, 1.0f, 0.0f,
					width_unit, height, -width_unit, 	tex_s, tex_t,	0.0f, 1.0f, 0.0f,

					width_unit, height, width_unit, 	tex_s, 0.0f,	0.0f, 1.0f, 0.0f,
					-width_unit, height, -width_unit,	0.0f, tex_t,	0.0f, 1.0f, 0.0f,
					-width_unit, height, width_unit,	0.0f, 0.0f,		0.0f, 1.0f, 0.0f
				};
				num_vertices = 6;
				glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
				break;
			}
		}

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 5));
		glBindVertexArray(0);
	}

	bool loadDiffuseMap(const char* _filepath) {
		if (loadTextureFile(diffuseMap, _filepath)) {
			useDiffuseMap = true;
			return true;
		}

		useDiffuseMap = false;
		return false;
	}

	bool loadDiffuseMap(GLuint texture)
	{
		if (texture) {
			diffuseMap = texture;
			useDiffuseMap = true;
			return true;
		}

		useDiffuseMap = false;
		return false;
	}

	bool loadSpecularMap(const char* _filepath) {
		if (loadTextureFile(specularMap, _filepath)) {
			useSpecularMap = true;
			return true;
		}

		useSpecularMap = false;
		return false;
	}

	bool loadSpecularMap(GLuint texture)
	{
		if (texture) {
			specularMap = texture;
			useSpecularMap = true;
			return true;
		}

		useSpecularMap = false;
		return false;
	}

	void draw(GLuint _shaderID) {
		glUniform3fv(glGetUniformLocation(_shaderID, "material.defaultAmbient"), 1, defaultAmbient);
		glUniform3fv(glGetUniformLocation(_shaderID, "material.defaultDiffuse"), 1, defaultDiffuse);
		glUniform3fv(glGetUniformLocation(_shaderID, "material.defaultSpecular"), 1, defaultSpecular);
		glUniform1i(glGetUniformLocation(_shaderID, "material.useDiffuseMap"), (int)useDiffuseMap);
		glUniform1i(glGetUniformLocation(_shaderID, "material.useSpecularMap"), (int)useSpecularMap);

		glUniform1i(glGetUniformLocation(_shaderID, "useNormal"), 1);
		glUniform1f(glGetUniformLocation(_shaderID, "material.shininess"), shininess);
		glUniform1f(glGetUniformLocation(_shaderID, "material.alpha"), alpha);
		glUniform1f(glGetUniformLocation(_shaderID, "material.diffuse_control"), diffuse_control);

		if (useDiffuseMap) {
			glUniform1i(glGetUniformLocation(_shaderID, "material.diffuse"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
		}

		if (useSpecularMap) {
			glUniform1i(glGetUniformLocation(_shaderID, "material.specular"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specularMap);
		}

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}



private:
	bool loadTextureFile(GLuint textureID, const char* filepath) {
		int width, height, nrChannels;
		unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);

		if (data) {
			GLenum format;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3) // R, G, B (ex. jpg)
				format = GL_RGB;
			else if (nrChannels == 4) // PNG
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
			return true;
		}

		stbi_image_free(data);
		return false;
	}
};

#endif 