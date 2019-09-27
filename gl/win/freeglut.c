#include <GL\glew.h>
#include <GL\freeglut.h>
#include <cstdio>
#include <iostream>

using namespace std;

void changeViewport(int w, int h) {
	glViewport(0, 0, w, h);
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
}

int main(int argc, char * argv[])
{
	// Initialize GLUT
	glutInit(&argc, argv);

	// Set up some memory buffers for our display
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	// Set the window size
	glutInitWindowSize(800, 600);
	// Create the window with the title "Hello, GL"
	glutCreateWindow("Hello GL");
	// Bind the two functions (above) to respond when necessary
	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);

	// Very important! This initializes the entry points in the OpenGL driver so we can
	// call the function in the API.
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW error");
		return 1;
	}

	// Start up a loop that runs in the background (you never see it)
	glutMainLoop();
	return 0;
}
