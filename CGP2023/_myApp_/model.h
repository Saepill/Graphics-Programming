#include <sb7.h>
#include <vmath.h>
#include <string>
#include <vector>


class Model {
public:
	std::vector<vmath::vec3> vPositions;
	std::vector<vmath::vec2> vTexCoords;
	std::vector<vmath::vec3> vNormals;
	std::vector<GLuint> vIndices;

	GLuint diffuseMap, specularMap;
	float shininess;
	vmath::vec3 defaultColor, defaultSpecular;

	GLuint VAO;

	Model() {
		shininess = 32.f;
		useDiffuseMap = false;
		useSpecularMap = false;

		defaultColor = vmath::vec3(1.0f, 1.0f, 1.0f);
		defaultSpecular = vmath::vec3(0.0f, 0.0f, 0.0f);
	}

	~Model() {
		glDeleteTextures(1, &diffuseMap);
		glDeleteTextures(1, &specularMap);

		glDeleteBuffers(1, &EBO);
		glDeleteBuffers(1, &VBO_positions);
		glDeleteBuffers(1, &VBO_texCoords);
		glDeleteBuffers(1, &VBO_normals);
		glDeleteVertexArrays(1, &VAO);
	}

	void init() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO_positions);
		glGenBuffers(1, &VBO_texCoords);
		glGenBuffers(1, &VBO_normals);
		glGenBuffers(1, &EBO);

		glGenTextures(1, &diffuseMap);
		glGenTextures(1, &specularMap);
	}

	void setDefaultColor(vmath::vec3 _color) {
		defaultColor = _color;
	}

	void setDefaultSpecular(vmath::vec3 _color) {
		defaultSpecular = _color;
	}

	void setupMesh(int _numVertices, GLfloat* _vPositions, GLfloat* _vTexCoords = NULL, GLfloat* _vNormals = NULL) {
		// 1. copy data from arrays
		for (int i = 0; i < _numVertices; i++) {
			vmath::vec3 position;
			position[0] = _vPositions[i * 3 + 0];
			position[1] = _vPositions[i * 3 + 1];
			position[2] = _vPositions[i * 3 + 2];
			this->vPositions.push_back(position);
		}

		if (_vTexCoords) {
			for (int i = 0; i < _numVertices; i++) {
				vmath::vec2 texCoords;
				texCoords[0] = _vTexCoords[i * 2 + 0];
				texCoords[1] = _vTexCoords[i * 2 + 1];
				this->vTexCoords.push_back(texCoords);
			}
		}

		if (_vNormals) {
			for (int i = 0; i < _numVertices; i++) {
				vmath::vec3 normal;
				normal[0] = _vNormals[i * 3 + 0];
				normal[1] = _vNormals[i * 3 + 1];
				normal[2] = _vNormals[i * 3 + 2];
				this->vNormals.push_back(normal);
			}
		}

		// 2. create buffers
		prepareBuffers();

	}

	void setupIndices(int _numIndices, GLuint* _indices) {
		for (int i = 0; i < _numIndices; i++) {
			vIndices.push_back(_indices[i]);
		}

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _numIndices * sizeof(int), &vIndices[0], GL_STATIC_DRAW);
		glBindVertexArray(0);
	}

	bool loadDiffuseMap(std::string _filepath) {
		if (loadTextureFile(diffuseMap, _filepath)) {
			useDiffuseMap = true;
			return true;
		}

		useDiffuseMap = false;
		return false;
	}

	bool loadSpecularMap(std::string _filepath) {
		if (loadTextureFile(specularMap, _filepath)) {
			useSpecularMap = true;
			return true;
		}

		useSpecularMap = false;
		return false;
	}

	void draw(GLuint _shaderID) {
		glUniform3fv(glGetUniformLocation(_shaderID, "material.defaultColor"), 1, defaultColor);
		glUniform3fv(glGetUniformLocation(_shaderID, "material.defaultSpecular"), 1, defaultSpecular);
		glUniform1i(glGetUniformLocation(_shaderID, "material.useDiffuseMap"), (int)useDiffuseMap);
		glUniform1i(glGetUniformLocation(_shaderID, "material.useSpecularMap"), (int)useSpecularMap);

		glUniform1i(glGetUniformLocation(_shaderID, "useNormal"), (int)vNormals.size());

		if (useDiffuseMap) {
			glUniform1i(glGetUniformLocation(_shaderID, "material.diffuse"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
		}

		if (useSpecularMap) {
			glUniform1i(glGetUniformLocation(_shaderID, "material.specular"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specularMap);

			glUniform1f(glGetUniformLocation(_shaderID, "material.shininess"), shininess);
		}

		glBindVertexArray(VAO);
		if (vIndices.empty())
			glDrawArrays(GL_TRIANGLES, 0, vPositions.size());
		else
			glDrawElements(GL_TRIANGLES, vIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

private:
	GLuint VBO_positions, VBO_texCoords, VBO_normals;
	GLuint EBO;
	bool useDiffuseMap, useSpecularMap;

	void prepareBuffers() {
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
		glBufferData(GL_ARRAY_BUFFER, vPositions.size() * sizeof(vmath::vec3), &vPositions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		if (!vTexCoords.empty()) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO_texCoords);
			glBufferData(GL_ARRAY_BUFFER, vTexCoords.size() * sizeof(vmath::vec2), &vTexCoords[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		}

		if (!vNormals.empty()) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
			glBufferData(GL_ARRAY_BUFFER, vNormals.size() * sizeof(vmath::vec3), &vNormals[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	bool loadTextureFile(GLuint textureID, std::string filepath) {
		int width, height, nrChannels;
		unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);

		if (data) {
			GLenum format;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
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