// #include "Drawing.hpp"
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <iostream>
#include <cmath>

#include "SDLOpenGLScreen.hpp"
#include "../../common.hpp"

namespace ng {
	
	struct vec2 {
		float x,y;
	};
	struct vec4 {
		float x,y,z,w;
	};
	
	#define SHADER_VERSION "#version 130\n"
	
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
	SHADER_VERSION
	// "#version 330\n"
	// "#extension GL_ARB_separate_shader_objects : enable\n"
	// "layout (location = 0) in vec2 position;"
	// "layout (location = 1) in vec4 color;"
	"in vec2 position;"
	"in vec4 color;"
	"out vec4 inColor;"
	"void main() {"
	"	gl_Position = vec4( position, 0.0, 1.0 );"
	"	inColor = color;"
	"}";

	static const char* fragmentShader_code =
	SHADER_VERSION
	// "#version 330\n"
	"in vec4 inColor;"
	"out vec4 color;"
	// "in vec4 gl_FragCoord;"

	"void main() {"
	"	color = vec4(inColor);"
	"}";
	
	
	
	static const char* texture_vertexshader_code =
	SHADER_VERSION
	// "#version 330\n"
	// "#extension GL_ARB_separate_shader_objects : enable\n"
	// "layout (location = 0) in vec2 position;"
	// "layout (location = 1) in vec2 texCoord;"
	"in vec2 position;"
	"in vec2 texCoord;"
	"out vec2 inTexCoord;"
	"void main() {"
		"gl_Position = vec4( position, 0.0, 1.0 );"
		// "inTexCoord = vec2(1.0 - texCoord.x, 1.0 - texCoord.y);"
		"inTexCoord = vec2(texCoord.x, texCoord.y);"
	"}";

	static const char* texture_fragmentshader_code =
	SHADER_VERSION
	// "#version 330\n"
	"in vec2 inTexCoord;"
	"out vec4 color;"
	"uniform sampler2D textureUniform;"
	"uniform float max_alpha;"
	// "uniform float custom_color_interp;"
	// "uniform vec3 custom_color;"
	"void main() {"
		"vec4 c = texture(textureUniform, inTexCoord);"
		"c.a = min(max_alpha, c.a);"
		// "c.rgb = c.rgb * (1-custom_color_interp) + (custom_color_interp) * custom_color;"
		"color = c;"
	"}";
	

	static GLuint vao = 0,
		vbo_position = 0,
		vbo_color = 0,
		ebo = 0;

	static GLuint shader = 0;
	static GLuint shader2 = 0;
	

	SDLOpenGLScreen::SDLOpenGLScreen() {
		using_scissor = false;
	}
	
	void SDLOpenGLScreen::Init() {
		static bool inited = false;
		
		if(!inited) {			
			try {
				shader = loadShader( vertexShader_code, fragmentShader_code );
				shader2 = loadShader( texture_vertexshader_code, texture_fragmentshader_code );
			} catch( std::string& e ) {
				std::cout << e << std::endl;
			}

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			glGenBuffers(1, &vbo_position);
			glGenBuffers(1, &vbo_color);
			glGenBuffers(1, &ebo);
			glBindVertexArray(0);
			
			inited = true;
			
		}
	}

	int sizeX=800, sizeY=800;
	void SDLOpenGLScreen::SetResolution( int w, int h ) {
		sizeX = w;
		sizeY = h;
	}
	void SDLOpenGLScreen::GetResolution( int &w, int &h ) {
		w = sizeX;
		h = sizeY;
	}
	
	
	static float custom_color[4] = {1.0,1.0,1.0,0};
	static float custom_color_interp = 0.0;
	
	static float max_alpha = 1.0f;
	void SDLOpenGLScreen::SetMaxAlpha(float _max_alpha) {
		max_alpha = _max_alpha;
	}
	
	static float rotation = 0.0f;
	static float rot_sin=0.0f, rot_cos=1.0f;
	void SDLOpenGLScreen::SetRotation(float _rotation, int cx, int cy) {
		if(rotation != _rotation) {
			rotation = _rotation;
			if(rotation == 0.0f) return;
			rot_cos = cos(rotation * M_PI/180.0f);
			rot_sin = sin(rotation * M_PI/180.0f);
			SetRotationPoint(cx,cy);
		}
	}
	
	static float center_x=0.0f, center_y=0.0f;
	void SDLOpenGLScreen::SetRotationPoint(int x, int y) {
		center_x = (x * 2.0f) / sizeX - 1.0f;
		center_y = -((y * 2.0f) / sizeY - 1.0f);
	}
	
	static void rotate(GLfloat* p) {
		// [ cos -sin ]   [ x ]
		// [ sin cos  ]   [ y ]
		float x = p[0] - center_x;
		float y = p[1] - center_y;
		p[0] = (x * rot_cos - y * rot_sin) + center_x;
		p[1] = (x * rot_sin + y * rot_cos) + center_y;
	}

	void SDLOpenGLScreen::Rect(int x, int y, int w, int h, GLuint color) {
		glUseProgram(shader);
		glBindVertexArray(vao);

		float x1 = (float)(x+0.3) / sizeX * 2.0 - 1.0;
		float y1 = -((float)(y+0.3) / sizeY * 2.0 - 1.0);
		float x2 = (float)(x+w) / sizeX * 2.0 - 1.0;
		float y2 = -((float)(y+h-0.1) / sizeY * 2.0 - 1.0);

		GLfloat positions[] = {
			x1, y1,
			x1, y2,
			x2, y2,
			x2, y1,
		};
		
		if(rotation != 0.0f) {
			rotate(&positions[0]);
			rotate(&positions[2]);
			rotate(&positions[4]);
			rotate(&positions[6]);
		}

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

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		
		glDisable(GL_BLEND);

		glBindVertexArray(0);
		glUseProgram(0);
	}

	void SDLOpenGLScreen::FillRect(int x, int y, int w, int h, unsigned int color) {
		glUseProgram(shader);
		glBindVertexArray(vao);
		
		float x1 = (float)(x+0.3) / sizeX * 2.0 - 1.0;
		float y1 = -((float)(y) / sizeY * 2.0 - 1.0);
		float x2 = (float)(x+w) / sizeX * 2.0 - 1.0;
		float y2 = -((float)(y+h) / sizeY * 2.0 - 1.0);

		GLfloat positions[] = {
			x1, y1,
			x1, y2,
			x2, y2,
			x2, y1,
		};
		
		if(rotation != 0.0f) {
			rotate(&positions[0]);
			rotate(&positions[2]);
			rotate(&positions[4]);
			rotate(&positions[6]);
		}
		

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

		glDisable(GL_BLEND);
		glBindVertexArray(0);
		glUseProgram(0);
	}


	void SDLOpenGLScreen::FillCircle(int x, int y, float radius, GLuint color) {
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
		
		std::vector<vec4> colors;
		std::vector<vec2> positions;

		vec2 centerPos{x1, -y1};
		positions.push_back(centerPos);

		for(int i = 0; i <= triangleAmount + 1; i++) {
			vec2 pos = {
				x1 + (radius * cosf(i *  twicePi / triangleAmount) / sizeX),
				-y1 + (radius * sinf(i * twicePi / triangleAmount) / sizeY)
			};

			positions.push_back(pos);
			colors.push_back({cr, cg, cb, ca});
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * colors.size(), &colors[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glDrawArrays(GL_TRIANGLE_FAN, 0, triangleAmount+2);

		glBindVertexArray(0);
		glUseProgram(0);
	}

	void SDLOpenGLScreen::Circle(int x, int y, float radius, GLuint color) {
		glUseProgram(shader);
		glBindVertexArray(vao);

		int triangleAmount = 15;
		GLfloat twicePi = 2.0f * 3.141592f;

		float x1 = (float)(x) / sizeX * 2.0 - 1.0;
		float y1 = -((float)(y) / sizeY * 2.0 - 1.0);
		float x2 = (float)(x+radius) / sizeX * 2.0 - 1.0;
		float y2 = -((float)(y+radius) / sizeY * 2.0 - 1.0);

		float cr = (float)((color >> 16) & 0xff) / 255.0f;
		float cg = (float)((color >> 8) & 0xff) / 255.0f;
		float cb = (float)((color) & 0xff) / 255.0f;
		float ca = (float)(color >> 24) / 255.0f;
		ca = std::min<float>(max_alpha, ca);
		
		std::vector<vec4> colors;
		std::vector<vec2> positions;

		for(int i = 0; i <= triangleAmount;i++) {
			vec2 pos = {
				x1 + (radius * cosf(i *  twicePi / triangleAmount) / sizeX),
				y1 + (radius * sinf(i * twicePi / triangleAmount) / sizeY)
			};

			positions.push_back(pos);
			colors.push_back({cr, cg, cb, ca});
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * colors.size(), &colors[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glDrawArrays(GL_LINE_LOOP, 0, triangleAmount+1);

		glBindVertexArray(0);
		glUseProgram(0);
	}
	
	void SDLOpenGLScreen::CacheImage(Image* img) {
		
		img->Update();
		Point a, b;
		Size s = img->GetImageSize();
		unsigned int texid = img->GetTextureId();
		if(img->GetAffectedRegion(a,b)) {
			// img->FreeCache();
			// std::cout << "cache img\n";
			if(texid == NO_TEXTURE) {
				glGenTextures(1, &texid);
				// tex->SetTextureId(texid);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture( GL_TEXTURE_2D, texid );
				// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, s.w, s.h, 0, GL_BGRA, GL_UNSIGNED_BYTE, img->GetImage() );
				// std::cout << "caching img size " << s.w << ", " << s.h << "\n";
			} else {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texid);
				
				// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, s.w, s.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.GetTexture() );
				// std::cout << "caching img2 size " << s.w << ", " << s.h << "\n";
				glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, s.w, s.h, GL_BGRA, GL_UNSIGNED_BYTE, img->GetImage() );
			}
			SetCache(img, texid);
			img->ResetAffectedRegion();
		}
	}
	
	void SDLOpenGLScreen::SetClipRegion(int x, int y, int w, int h, bool enable) {
		if(enable) {
			if(x != -1) {
				glScissor(x, sizeY-y-h, w, h);
				clip_region = ng::Rect(x,y,w,h);
			}
			using_scissor = true;
			glEnable(GL_SCISSOR_TEST);
		} else {
			using_scissor = false;
			glDisable(GL_SCISSOR_TEST);
		}
	}
	
	bool SDLOpenGLScreen::GetClipRegion(int &x, int &y, int &w, int &h) {
		x = clip_region.x;
		y = clip_region.y;
		w = clip_region.w;
		h = clip_region.h;
		return using_scissor;
	}
	
	void SDLOpenGLScreen::TexRect(int x, int y, Image* tex, bool repeat) {
		ng::Rect s = tex->GetImageCropRegion();
		if(s.w == 0 || s.h == 0 || !tex->GetImage() ) return;
		
		CacheImage(tex);
		// std::cout << "rendering tex2\n";
		TexRect(x, y, s.w, s.h, tex, repeat, s.w, s.h);
	}

	//void TexRect(int x, int y, int w, int h, GLuint texture) {
	void SDLOpenGLScreen::TexRect(int x, int y, int w, int h, Image* texture, bool repeat, int texWidth, int texHeight) {
		
		// Size s = texture->GetImageSize();
		// if(s.w == 0 || s.h == 0 || !texture->GetImage() ) return;
		CacheImage(texture);
		// std::cout << "rendering tex\n";
		
		glUseProgram(shader2);
		glBindVertexArray(vao);
		
		Size s = texture->GetImageSize();
		ng::Rect crop = texture->GetImageCropRegion();
		float crop_x1 = ((float)crop.x / (float)s.w);
		float crop_y1 = ((float)crop.y / (float)s.h);
		float crop_x2 = ((float)crop.w / (float)s.w) + crop_x1;
		float crop_y2 = ((float)crop.h / (float)s.h) + crop_y1;
		
		float x1 = ((float)(x) / sizeX) * 2.0 - 1.0;
		float y1 = -(((float)(y) / sizeY) * 2.0 - 1.0);
		float x2 = ((float)(x+w) / sizeX) * 2.0 - 1.0;
		float y2 = -(((float)(y+h) / sizeY) * 2.0 - 1.0);
		

		GLfloat positions[] = {
			x1, y1,
			x1, y2,
			x2, y2,
			x2, y1,
		};
		
		
		crop_x2 = repeat ? (float)w/texWidth : crop_x2;
		crop_y2 = repeat ? (float)h/texHeight : crop_y2;
		
		GLfloat texCoords[] = {
			crop_x1, crop_y1,
			crop_x1, crop_y2,
			crop_x2, crop_y2,
			crop_x2, crop_y1,
		};

		GLuint indices[] = {
			0, 1, 2,
			2, 3, 0,
		};
		
		if(rotation != 0.0f) {
			rotate(&positions[0]);
			rotate(&positions[2]);
			rotate(&positions[4]);
			rotate(&positions[6]);
		}
		
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
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());
		glUniform1i(glGetUniformLocation(shader2, "textureUniform"), 0);
		glUniform1f(glGetUniformLocation(shader2, "max_alpha"), max_alpha);
		glUniform1f(glGetUniformLocation(shader2, "custom_color_interp"), custom_color_interp);
		glUniform4fv(glGetUniformLocation(shader2, "custom_color"), 1, custom_color);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glDisable(GL_BLEND);
		glBindVertexArray(0);
		glUseProgram(0);
	}


	void SDLOpenGLScreen::DeleteTexture(GLuint textureID) {
		if(glIsTexture(textureID)) {
			glDeleteTextures(1, &textureID);
		}
	}

	void SDLOpenGLScreen::Line(int xA, int yA, int xB, int yB, GLuint color) {
		glUseProgram(shader);
		glBindVertexArray(vao);

		float x1 = (float)(xA) / sizeX * 2.0 - 1.0;
		float y1 = -((float)(yA) / sizeY * 2.0 - 1.0);
		float x2 = (float)(xB) / sizeX * 2.0 - 1.0;
		float y2 = -((float)(yB) / sizeY * 2.0 - 1.0);

		float cr = (float)((color >> 16) & 0xff) / 255.0f;
		float cg = (float)((color >> 8) & 0xff) / 255.0f;
		float cb = (float)((color) & 0xff) / 255.0f;
		float ca = (float)(color >> 24) / 255.0f;
		ca = std::min<float>(max_alpha, ca);
		
		GLfloat positions[] = {
			x1, y1,
			x2, y2,
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

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArrays(GL_LINES, 0, 2);
		glDisable(GL_BLEND);

		glBindVertexArray(0);
		glUseProgram(0);
	}

	void SDLOpenGLScreen::VLine(int x1, int y1, int y2, GLuint color) {
		Line(x1, y1, x1, y2, color);
	}

	uint32_t SDLOpenGLScreen::CompileShader(std::string vertexShader, std::string fragmentShader ) {
		return loadShader(vertexShader.c_str(), fragmentShader.c_str());
	}
	
	void SDLOpenGLScreen::SetShader(uint32_t shader_id) {
		glUseProgram(shader_id);
	}
	
	bool SDLOpenGLScreen::IsShadersSupported() {
		return true;
	}
	
	int	 SDLOpenGLScreen::SetNewOffScreenRender() {
		GLuint fboId, rboDepthId, textureId;
		
		#define TEXTURE_WIDTH 100
		#define TEXTURE_HEIGHT 100
		
		glGenFramebuffers(1, &fboId);
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		
		glGenRenderbuffers(1, &rboDepthId);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepthId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, TEXTURE_WIDTH, TEXTURE_HEIGHT);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepthId);
		
		/*
		printFramebufferInfo(fboId);
		bool status = checkFramebufferStatus(fboId);
		if(!status)
		fboUsed = false;
		*/

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return 0;
	}
	
	bool SDLOpenGLScreen::FreeOffScreenRender(int id) {
		// glDestro
		return false;
	}
	
	bool SDLOpenGLScreen::SetOffScreenRender(int id) {
		return false;
	}
	
	Image* SDLOpenGLScreen::GetOffScreenTexture(int id) {
		return 0;
	}
	
}
