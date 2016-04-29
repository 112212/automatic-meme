#include "Drawing.hpp"
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/matrix_access.hpp>
// #include <glm/gtc/type_ptr.hpp>

namespace ng {
	
// TODO: remove
/*
CSurface::CSurface() {
}
 

bool CSurface::OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y) {
    if(Surf_Dest == NULL || Surf_Src == NULL) {
        return false;
    }
 
    SDL_Rect DestR;
 
    DestR.x = X;
    DestR.y = Y;
 
    SDL_BlitSurface(Surf_Src, NULL, Surf_Dest, &DestR);
 
    return true;
}
bool CSurface::OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y, int X2, int Y2, int W, int H) {
    if(Surf_Dest == NULL || Surf_Src == NULL) {
        return false;
    }
 
    SDL_Rect DestR;
 
    DestR.x = X;
    DestR.y = Y;
 
    SDL_Rect SrcR;
 
    SrcR.x = X2;
    SrcR.y = Y2;
    SrcR.w = W;
    SrcR.h = H;
 
    SDL_BlitSurface(Surf_Src, &SrcR, Surf_Dest, &DestR);
 
    return true;
}

SDL_Surface* CSurface::CreateSurface(GLuint flags,int width,int height,const SDL_Surface* display)
{
  // 'display' is the surface whose format you want to match
  //  if this is really the display format, then use the surface returned from SDL_SetVideoMode

  const SDL_PixelFormat& fmt = *(display->format);
  return SDL_CreateRGBSurface(flags,width,height,
                  fmt.BitsPerPixel,
                  fmt.Rmask,fmt.Gmask,fmt.Bmask,fmt.Amask );
}

bool CSurface::OnDraw(SDL_Renderer* ren, SDL_Surface* Surf_Src, int x, int y) {
	if(ren == NULL || Surf_Src == NULL) {
        return false;
    }
 
	SDL_Texture *tex = SDL_CreateTextureFromSurface( ren, Surf_Src );
	
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    SDL_RenderCopy(ren, tex, NULL, &dst);
	SDL_DestroyTexture(tex);
    return true;
}

bool CSurface::OnDraw(SDL_Renderer* ren, SDL_Texture* tex, SDL_Surface* Surf_Src, int x, int y) {
	SDL_Rect dst;
    dst.x = x;
    dst.y = y;
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
	SDL_UpdateTexture(tex, NULL, Surf_Src->pixels, Surf_Src->pitch);
    SDL_RenderCopy(ren, tex, NULL, &dst);
    return true;
}

bool CSurface::OnDraw(SDL_Renderer* ren, SDL_Surface* Surf_Src, int x, int y, int x2, int y2, int w, int h) {
	if(ren == NULL || Surf_Src == NULL) {
        return false;
    }
 
	SDL_Texture *tex = SDL_CreateTextureFromSurface( ren, Surf_Src );
	SDL_Rect src;
	src.x = x2;
	src.y = y2;
	src.w = w;
	src.h = h;
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    SDL_RenderCopy(ren, tex, &src, &dst);
	SDL_DestroyTexture(tex);
    return true;
}
*/



namespace Drawing {
	
	// shaders
	unsigned int readShader(std::string shaderStr, unsigned int shaderType)
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
	
	GLuint loadShader(const char* vertexShaderFile, const char* fragmentShaderFile)
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
	"\n\
	#version 330\n\
	\n\
	layout (location = 0) in vec2 position;\n\
	layout (location = 1) in vec3 color;\n\
	\n\
	out vec3 inColor;\n\
	\n\
	void main() {\n\
		gl_Position = vec4( position, 0.0, 1.0 );\n\
		\n\
		inColor = color;\n\
	}\n\
	";

	static const char* fragmentShader_code = 
	"#version 330\n\
	\n\
	in vec3 inColor;\n\
	\n\
	out vec4 color;\n\
	\n\
	void main() {\n\
		color = vec4(inColor, 1.0f);\n\
	}\n\
	";

	GLuint vao = 0,
		vbo_position = 0,
		vbo_color = 0, 
		ebo = 0;

	GLuint shader = 0;

	void Init() {
		try {
			shader = loadShader( vertexShader_code, fragmentShader_code );
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
	
	int sizeX=800, sizeY=800;
	void SetResolution( int w, int h ) {
		sizeX = w;
		sizeY = h;
	}

	void Rect(int x, int y, int w, int h, GLuint color) {
		glUseProgram(shader);
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
		
		float cr = (float)((color >> 24) & 0xff) / 255;
		float cg = (float)((color >> 16) & 0xff) / 255;
		float cb = (float)((color >> 8) & 0xff) / 255;
		float ca = (float)(color & 0xff) / 255;

		GLfloat colors[] = {
			 cr, cg, cb,
			 cr, cg, cb,
			 cr, cg, cb,
			 cr, cg, cb,
		};
	   
		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);    
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);    
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		glBindVertexArray(0);
		glUseProgram(0);
	}

	void FillRect(int x, int y, int w, int h, GLuint color) {
		glUseProgram(shader);
		glBindVertexArray(vao);

		float x1 = (float)(x) / sizeX * 2.0 - 1.0;
		float y1 = (float)(y-1) / sizeY * 2.0 - 1.0;
		float x2 = (float)(x+w) / sizeX * 2.0 - 1.0;
		float y2 = (float)(y+h) / sizeY * 2.0 - 1.0;

		GLfloat positions[] = {
			x1, -y1,
			x1, -y2,
			x2, -y2,
			x2, -y1,
		};
		
		float cr = (float)((color >> 24) & 0xff) / 255;
		float cg = (float)((color >> 16) & 0xff) / 255;
		float cb = (float)((color >> 8) & 0xff) / 255;
		float ca = (float)(color & 0xff) / 255;

		GLfloat colors[] = {
			 cr, cg, cb,
			 cr, cg, cb,
			 cr, cg, cb,
			 cr, cg, cb,
		};

		GLuint indices[] = {
			0, 1, 3,
			1, 2, 3,
		};
	   
		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);    
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);    
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		
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
		
		float cr = (float)((color >> 24) & 0xff) / 255;
		float cg = (float)((color >> 16) & 0xff) / 255;
		float cb = (float)((color >> 8) & 0xff) / 255;
		float ca = (float)(color & 0xff) / 255;
		
		std::vector<glm::vec3> colors;
		std::vector<glm::vec2> positions;
		
		for(int i = 0; i <= triangleAmount;i++) { 
			glm::vec2 pos = glm::vec2(
				x1 + (radius * cos(i *  twicePi / triangleAmount) / sizeX), 
				-y1 + (radius * sin(i * twicePi / triangleAmount) / sizeY)
			);
			
			positions.push_back(pos);
			colors.push_back(glm::vec3(cr, cg, cb));
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);    
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colors.size(), &colors[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);    
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		
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
		
		float cr = (float)((color >> 24) & 0xff) / 255;
		float cg = (float)((color >> 16) & 0xff) / 255;
		float cb = (float)((color >> 8) & 0xff) / 255;
		float ca = (float)(color & 0xff) / 255;
		
		std::vector<glm::vec3> colors;
		std::vector<glm::vec2> positions;
		
		for(int i = 0; i <= triangleAmount;i++) { 
			glm::vec2 pos = glm::vec2(
				x1 + (radius * cos(i *  twicePi / triangleAmount) / sizeX), 
				-y1 + (radius * sin(i * twicePi / triangleAmount) / sizeY)
			);
			
			positions.push_back(pos);
			colors.push_back(glm::vec3(cr, cg, cb));
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);    
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colors.size(), &colors[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);    
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		
		glDrawArrays(GL_LINE_LOOP, 0, triangleAmount+1);

		glBindVertexArray(0);
		glUseProgram(0);
	}


	void Line(int xA, int yA, int xB, int yB, GLuint color) {
		glUseProgram(shader);
		glBindVertexArray(vao);
		
		float x1 = (float)(xA) / sizeX * 2.0 - 1.0;
		float y1 = (float)(yA) / sizeY * 2.0 - 1.0;
		float x2 = (float)(xB) / sizeX * 2.0 - 1.0;
		float y2 = (float)(yB) / sizeY * 2.0 - 1.0;
		
		float cr = (float)((color >> 24) & 0xff) / 255;
		float cg = (float)((color >> 16) & 0xff) / 255;
		float cb = (float)((color >> 8) & 0xff) / 255;
		float ca = (float)(color & 0xff) / 255;
		
		GLfloat positions[] = {
			x1, -y1,
			x2, -y2,
		};
		
		GLfloat colors[] = {
			 cr, cg, cb,
			 cr, cg, cb,
		};

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);    
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);    
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		
		glDrawArrays(GL_LINES, 0, 2);

		glBindVertexArray(0);
		glUseProgram(0);
	}
	
	void VLine(int x1, int y1, int y2, GLuint color) {
		Line(x1, y1, x1, y2, color);
	}

	
}
}
