#include "Drawing.hpp"
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>

namespace ng {
namespace Drawing {
	
	// shaders
	static unsigned int readShader(std::string shaderStr, unsigned int shaderType)
	{
		const std::string &shaderString = shaderStr;
		const char *shaderSource = shaderString.c_str();
		GLint shaderLength = shaderString.size();

		// creating shader
		GLuint shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, (const GLchar**)&shaderSource, (GLint*)&shaderLength);

		// compiling shader
		GLint compileStatus;

		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

		if(compileStatus != GL_TRUE) {
			char buffer[512];
			glGetShaderInfoLog(shader, 512, NULL, buffer);

			throw std::string(buffer);
		}

		return shader;
	}

	static GLuint loadShader(const char* vertexShaderFile, const char* fragmentShaderFile)
    {
		GLuint vertexShader;
		GLuint fragmentShader;
		try {
			vertexShader = readShader(vertexShaderFile, GL_VERTEX_SHADER);
			fragmentShader = readShader(fragmentShaderFile, GL_FRAGMENT_SHADER);
		} catch( std::exception &e ) {
			std::cout << e.what() << std::endl;
		}

        // linking
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);

        glLinkProgram(shaderProgram);

        if(!shaderProgram) {
            throw std::string("Failed to create shader program");
        }

        return shaderProgram;
    }

	static const char* vertexShader_code =
	"#version 330\n"
	"layout (location = 0) in vec2 position;"
	"layout (location = 1) in vec4 color;"
	"out vec4 inColor;"
	"void main() {"
	"	gl_Position = vec4( position, 0.0, 1.0 );"
	"	inColor = color;"
	"}";

	static const char* fragmentShader_code =
	"#version 330\n"
	"in vec4 inColor;"
	"out vec4 color;"
	"in vec4 gl_FragCoord;"

	"void main() {"
	"	color = vec4(inColor);"
	"}";
	
	
	
	static const char* vertexShader2_code =
	"\n\
	#version 330\n\
	\n\
	layout (location = 0) in vec2 position;\n\
	layout (location = 1) in vec2 texCoord;\n\
	\n\
	out vec2 inTexCoord;\n\
	\n\
	void main() {\n\
		gl_Position = vec4( position, 0.0, 1.0 );\n\
		\n\
		//inTexCoord = texCoord;\n\
		\n\
		inTexCoord = vec2(1.0 - texCoord.x, 1.0 - texCoord.y);\n\
	}\n\
	";

	static const char* fragmentShader2_code =
	"\n\
	#version 330\n\
	\n\
	in vec2 inTexCoord;\n\
	\n\
	out vec4 color;\n\
	\n\
	uniform sampler2D textureUniform;\n\
	uniform float max_alpha;\n\
	\n\
	void main() {\n\
		vec4 c = texture(textureUniform, inTexCoord);\n\
		c.a = min(max_alpha, c.a);\n\
		color = c;\n\
	}\n\
	";


	static GLuint vao = 0,
		vbo_position = 0,
		vbo_color = 0,
		ebo = 0;

	static GLuint shader = 0;
	static GLuint shader2 = 0;
	
	static float max_alpha = 1.0f;
	void SetMaxAlpha(float _max_alpha) {
		max_alpha = _max_alpha;
	}
	
	static float rotation = 0.0f;
	void SetRotation(float _rotation) {
		rotation = _rotation;
	}

	void Init() {
		static bool inited = false;
		
		if(!inited) {			
			try {
				shader = loadShader( vertexShader_code, fragmentShader_code );
				shader2 = loadShader( vertexShader2_code, fragmentShader2_code );
			} catch( std::string& e ) {
				std::cout << e << std::endl;
			}

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			glGenBuffers(1, &vbo_position);
			glGenBuffers(1, &vbo_color);
			glGenBuffers(1, &ebo);
			glBindVertexArray(0);
			
		}
	}

	int sizeX=800, sizeY=800;
	void SetResolution( int w, int h ) {
		sizeX = w;
		sizeY = h;
	}
	void GetResolution( int &w, int &h ) {
		w = sizeX;
		h = sizeY;
	}

	void Rect(int x, int y, int w, int h, GLuint color) {
		glUseProgram(shader);
		glBindVertexArray(vao);

		float x1 = (float)(x+0.3) / sizeX * 2.0 - 1.0;
		float y1 = (float)(y) / sizeY * 2.0 - 1.0;
		float x2 = (float)(x+w) / sizeX * 2.0 - 1.0;
		float y2 = (float)(y+h) / sizeY * 2.0 - 1.0;

		GLfloat positions[] = {
			x1, -y1,
			x1, -y2,
			x2, -y2,
			x2, -y1,
		};

		float cr = (float)((color >> 16) & 0xff) / 255.0f;
		float cg = (float)((color >> 8) & 0xff) / 255.0f;
		float cb = (float)((color) & 0xff) / 255.0f;
		float ca = (float)(color >> 24) / 255.0f;
		ca = std::min<float>(max_alpha, ca);
		
		GLfloat colors[] = {
			 cr, cg, cb, ca,
			 cr, cg, cb, ca,
			 cr, cg, cb, ca,
			 cr, cg, cb, ca,
		};

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		glBindVertexArray(0);
		glUseProgram(0);
	}

	void FillRect(int x, int y, int w, int h, GLuint color) {
		glUseProgram(shader);
		glBindVertexArray(vao);

		float x1 = (float)(x+0.3) / sizeX * 2.0 - 1.0;
		float y1 = (float)(y) / sizeY * 2.0 - 1.0;
		float x2 = (float)(x+w) / sizeX * 2.0 - 1.0;
		float y2 = (float)(y+h) / sizeY * 2.0 - 1.0;

		GLfloat positions[] = {
			x1, -y1,
			x1, -y2,
			x2, -y2,
			x2, -y1,
		};

		float cr = (float)((color >> 16) & 0xff) / 255.0f;
		float cg = (float)((color >> 8) & 0xff) / 255.0f;
		float cb = (float)((color) & 0xff) / 255.0f;
		float ca = (float)(color >> 24) / 255.0f;
		ca = std::min<float>(max_alpha, ca);
		
		GLfloat colors[] = {
			 cr, cg, cb, ca,
			 cr, cg, cb, ca,
			 cr, cg, cb, ca,
			 cr, cg, cb, ca,
		};

		GLuint indices[] = {
			0, 1, 3,
			1, 2, 3,
		};
	
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glUseProgram(0);
	}


	void FillCircle(int x, int y, float radius, GLuint color) {
		glUseProgram(shader);
		glBindVertexArray(vao);

		int triangleAmount = 20;
		GLfloat twicePi = 2.0f * 3.141592;

		float x1 = (float)(x) / sizeX * 2.0 - 1.0;
		float y1 = (float)(y) / sizeY * 2.0 - 1.0;
		float x2 = (float)(x+radius) / sizeX * 2.0 - 1.0;
		float y2 = (float)(y+radius) / sizeY * 2.0 - 1.0;

		float cr = (float)((color >> 16) & 0xff) / 255.0f;
		float cg = (float)((color >> 8) & 0xff) / 255.0f;
		float cb = (float)((color) & 0xff) / 255.0f;
		float ca = (float)(color >> 24) / 255.0f;
		ca = std::min<float>(max_alpha, ca);
		
		std::vector<glm::vec4> colors;
		std::vector<glm::vec2> positions;

		glm::vec2 centerPos = glm::vec2(x1, -y1);
		positions.push_back(centerPos);

		for(int i = 0; i <= triangleAmount + 1;i++) {
			glm::vec2 pos = glm::vec2(
				x1 + (radius * cos(i *  twicePi / triangleAmount) / sizeX),
				-y1 + (radius * sin(i * twicePi / triangleAmount) / sizeY)
			);

			positions.push_back(pos);
			colors.emplace_back(cr, cg, cb, ca);
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * colors.size(), &colors[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glDrawArrays(GL_TRIANGLE_FAN, 0, triangleAmount+2);

		glBindVertexArray(0);
		glUseProgram(0);
	}

	void Circle(int x, int y, float radius, GLuint color) {
		glUseProgram(shader);
		glBindVertexArray(vao);

		int triangleAmount = 15;
		GLfloat twicePi = 2.0f * 3.141592;

		float x1 = (float)(x) / sizeX * 2.0 - 1.0;
		float y1 = (float)(y) / sizeY * 2.0 - 1.0;
		float x2 = (float)(x+radius) / sizeX * 2.0 - 1.0;
		float y2 = (float)(y+radius) / sizeY * 2.0 - 1.0;

		float cr = (float)((color >> 16) & 0xff) / 255.0f;
		float cg = (float)((color >> 8) & 0xff) / 255.0f;
		float cb = (float)((color) & 0xff) / 255.0f;
		float ca = (float)(color >> 24) / 255.0f;
		ca = std::min<float>(max_alpha, ca);
		
		std::vector<glm::vec4> colors;
		std::vector<glm::vec2> positions;

		for(int i = 0; i <= triangleAmount;i++) {
			glm::vec2 pos = glm::vec2(
				x1 + (radius * cos(i *  twicePi / triangleAmount) / sizeX),
				-y1 + (radius * sin(i * twicePi / triangleAmount) / sizeY)
			);

			positions.push_back(pos);
			colors.push_back(glm::vec4(cr, cg, cb, ca));
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * colors.size(), &colors[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glDrawArrays(GL_LINE_LOOP, 0, triangleAmount+1);

		glBindVertexArray(0);
		glUseProgram(0);
	}

	//void TexRect(int x, int y, int w, int h, GLuint texture) {
	void TexRect(int x, int y, int w, int h, GLuint texture, bool repeat, int texWidth, int texHeight) {
		glUseProgram(shader2);
		glBindVertexArray(vao);

		float x1 = (float)(x) / sizeX * 2.0 - 1.0;
		float y1 = (float)(y) / sizeY * 2.0 - 1.0;
		float x2 = (float)(x+w) / sizeX * 2.0 - 1.0;
		float y2 = (float)(y+h) / sizeY * 2.0 - 1.0;

		GLfloat positions[] = {
			x1, -y1,
			x1, -y2,
			x2, -y2,
			x2, -y1,
		};

		GLfloat texCoords[] = {
			repeat ? (float)w/texWidth : 1.0f, repeat ? (float)h/texHeight : 1.0f,
			repeat ? (float)w/texWidth : 1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, repeat ? (float)h/texHeight : 1.0f,
		};

		GLuint indices[] = {
			0, 1, 3,
			1, 2, 3,
		};

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color); //rename vbo_position to vbo1 and vbo2 or create vbo_texcoord, cuz vbo_color is confusing here
		glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shader2, "textureUniform"), 0);
		glUniform1f(glGetUniformLocation(shader2, "max_alpha"), max_alpha);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glUseProgram(0);
	}

	GLuint GetTextureFromSurface(SDL_Surface* surface, GLuint textureID) {
		if(!surface) {
			throw std::string("Error nullptr surface!");
		}

		if(!glIsTexture(textureID)) {
			GLuint newTextureID;
			glGenTextures(1, &newTextureID);

			textureID = newTextureID;

			// std::cout << "generating new texture " << textureID << std::endl;
		} else {
			// std::cout << "reusing " << textureID << std::endl;
		}
		
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);

		glBindTexture(GL_TEXTURE_2D, 0);
		return textureID;
	}
	
	GLuint GetTextureFromSurface2(SDL_Surface* surface, GLuint textureID) {
		if(!surface) {
			throw std::string("Error nullptr surface!");
		}

		if(!glIsTexture(textureID)) {
			GLuint newTextureID;
			glGenTextures(1, &newTextureID);

			textureID = newTextureID;

			// std::cout << "generating new texture" << std::endl;
		} else {
			glDeleteTextures(1, &textureID);
		}

		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

		glBindTexture(GL_TEXTURE_2D, 0);
		return textureID;
	}

	void DeleteTexture(GLuint textureID) {
		if(glIsTexture(textureID)) {
			glDeleteTextures(1, &textureID);
		}
	}

	void Line(int xA, int yA, int xB, int yB, GLuint color) {
		glUseProgram(shader);
		glBindVertexArray(vao);

		float x1 = (float)(xA) / sizeX * 2.0 - 1.0;
		float y1 = (float)(yA) / sizeY * 2.0 - 1.0;
		float x2 = (float)(xB) / sizeX * 2.0 - 1.0;
		float y2 = (float)(yB) / sizeY * 2.0 - 1.0;

		float cr = (float)((color >> 16) & 0xff) / 255.0f;
		float cg = (float)((color >> 8) & 0xff) / 255.0f;
		float cb = (float)((color) & 0xff) / 255.0f;
		float ca = (float)(color >> 24) / 255.0f;
		ca = std::min<float>(max_alpha, ca);
		
		GLfloat positions[] = {
			x1, -y1,
			x2, -y2,
		};

		GLfloat colors[] = {
			 cr, cg, cb, ca,
			 cr, cg, cb, ca
		};

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glDrawArrays(GL_LINES, 0, 2);

		glBindVertexArray(0);
		glUseProgram(0);
	}

	void VLine(int x1, int y1, int y2, GLuint color) {
		Line(x1, y1, x1, y2, color);
	}

	
}
}
