// This example demonstrate a simple 3D Drawing and introduce matrices in our graphic program 
// It also demonstrate the use of Geometric Transformation

#include <gl/glew.h>
#include <gl/freeglut.h>
// GLM library - for matrix manipulation
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// Copy the GLM folder to the "include" folder of Visual C++
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <time.h>

#pragma warning(disable:4996)

using namespace std;
tm source;
GLint cur_x, cur_y;
int font = (int)GLUT_BITMAP_8_BY_13;	// Initial font type
GLuint VBO[9];		// ID for vertex buffer objects
GLuint VAO;			// ID for vertex array objects
GLuint program;

GLfloat Hands[] = {
	
	//Minute Hand
	   0.0, 0.5,		0.0, 0.258, 0.572,
	 0.01, -0.1,		0.0, 0.258, 0.572,
	-0.01, -0.1,		0.0, 0.258, 0.572,

	//Hour Hand
	  0.0,  0.4,		0.003, 0.796, 0.776,
	 0.005, -0.1,		0.003, 0.796, 0.776,
	-0.005, -0.1,		0.003, 0.796, 0.776,

};

//Frame of Clock
GLfloat square[] = {
	-0.6, 0.6,			0.917, 0.941, 0.945,
	-0.6, -0.6,			0.917, 0.941, 0.945,
	0.6, -0.6,			0.917, 0.941, 0.945,
	0.6, 0.6,			0.917, 0.941, 0.945,
};

/*Not Working
GLfloat Marker[] = {
	//12 oclock marker
	-0.1, 0.6,			0.909, 0.2, 0.313,
	0.1, 0.6,			0.909, 0.2, 0.313,
	-0.1, -0.4,			0.909, 0.2, 0.313,
	0.1, 0.4,			0.909, 0.2, 0.313,

	//3 oclock marker
	0.4, 0.1,			0.909, 0.2, 0.313,
	0.6, 0.1,			0.909, 0.2, 0.313,
	0.4, -0.1,			0.909, 0.2, 0.313,
	0.6, -0.1,			0.909, 0.2, 0.313,

	//6 oclock marker
	-0.1, -0.4,			0.909, 0.2, 0.313,
	 0.1, -0.4,			0.909, 0.2, 0.313,
	-0.1, -0.6,			0.909, 0.2, 0.313,
	 0.1, 0.6,			0.909, 0.2, 0.313,

	 //9 oclock marker
	-0.6, 0.1,			0.909, 0.2, 0.313,
	-0.4, 0.1,			0.909, 0.2, 0.313,
	-0.6, -0.1,			0.909, 0.2, 0.313,
	-0.4, -0.1,			0.909, 0.2, 0.313

};
*/

//predefined matrix type from GLM 
glm::mat4 trans = glm::mat4(1.0);	//set to identity matrix at the beginning

GLint object;

GLfloat MinuteHand = 360.0f;	//rotation angle for red triangle
GLfloat HourHand = 360.0f;	//rotation angle for green triangle


// function to load shaders
GLuint loadShaders(const string vertexShaderFile, const string fragmentShaderFile)
{
	GLint status;	// for checking compile and linking status

					// load vertex shader code from file
	string vertexShaderCode;	// to store shader code
	ifstream vertexShaderStream(vertexShaderFile, ios::in);	// open file stream

															// check whether file stream was successfully opened
	if (vertexShaderStream.is_open())
	{
		// read from stream line by line and append it to shader code
		string line = "";
		while (getline(vertexShaderStream, line))
			vertexShaderCode += line + "\n";

		vertexShaderStream.close();		// no longer need file stream
	}
	else
	{
		// output error message and exit
		cout << "Failed to open vertex shader file - " << vertexShaderFile << endl;
		exit(EXIT_FAILURE);
	}

	// load fragment shader code from file
	string fragmentShaderCode;	// to store shader code
	ifstream fragmentShaderStream(fragmentShaderFile, ios::in);	// open file stream

																// check whether file stream was successfully opened
	if (fragmentShaderStream.is_open())
	{
		// read from stream line by line and append it to shader code
		string line = "";
		while (getline(fragmentShaderStream, line))
			fragmentShaderCode += line + "\n";

		fragmentShaderStream.close();	// no longer need file stream
	}
	else
	{
		// output error message and exit
		cout << "Failed to open fragment shader file - " << fragmentShaderFile << endl;
		exit(EXIT_FAILURE);
	}

	// create shader objects
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// provide source code for shaders
	const GLchar* vShaderCode = vertexShaderCode.c_str();
	const GLchar* fShaderCode = fragmentShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, &vShaderCode, NULL);
	glShaderSource(fragmentShaderID, 1, &fShaderCode, NULL);

	// compile vertex shader
	glCompileShader(vertexShaderID);

	// check compile status
	status = GL_FALSE;
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		// output error message
		cout << "Failed to compile vertex shader - " << vertexShaderFile << endl;

		// output error information
		int infoLogLength;
		glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* errorMessage = new char[infoLogLength + 1];
		glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, errorMessage);
		cout << errorMessage << endl;
		delete[] errorMessage;

		exit(EXIT_FAILURE);
	}

	// compile fragment shader
	glCompileShader(fragmentShaderID);

	// check compile status
	status = GL_FALSE;
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		// output error message
		cout << "Failed to compile fragment shader - " << fragmentShaderFile << endl;

		// output error information
		int infoLogLength;
		glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* errorMessage = new char[infoLogLength + 1];
		glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, errorMessage);
		cout << errorMessage << endl;
		delete[] errorMessage;

		exit(EXIT_FAILURE);
	}

	// create program
	GLuint programID = glCreateProgram();

	// attach shaders to the program object
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);

	// flag shaders for deletion (will not be deleted until detached from program)
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	// link program object
	glLinkProgram(programID);

	// check link status
	status = GL_FALSE;
	glGetProgramiv(programID, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		// output error message
		cout << "Failed to link program object." << endl;

		// output error information
		int infoLogLength;
		glGetShaderiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* errorMessage = new char[infoLogLength + 1];
		glGetShaderInfoLog(programID, infoLogLength, NULL, errorMessage);
		cout << errorMessage << endl;
		delete[] errorMessage;

		exit(EXIT_FAILURE);
	}

	return programID;
}

void init(void)
{

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Clock Hands
	glGenBuffers(2, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Hands), Hands, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//Clock Background
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	/*Not Working
	//Clock Marker
	glGenBuffers(4, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Marker), Marker, GL_STATIC_DRAW);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2 , 0);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(4);
	*/
	
	program = loadShaders("vertexShader.glsl", "fragmentShader.glsl");
	// Activate the shader program
	glUseProgram(program);

	glClearColor(0.976, 0.866, 0.643, 0.0);	// set background color

}


void drawMinuteHand(void)
{
	glUseProgram(program);
	trans = glm::rotate(glm::mat4(1.0f), glm::radians(HourHand), glm::vec3(0.0f, 0.0f, 1.0f));
	unsigned int transformLoc = glGetUniformLocation(program, "transform");
	
	//Uniform Location, count, transpose, data
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

	// Choose object type 1 (rotation apply)
	unsigned int objLoc = glGetUniformLocation(program, "object");
	object = 1;
	glUniform1i(objLoc, object);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void drawHourHand(void)
{
	glUseProgram(program);
	trans = glm::rotate(glm::mat4(1.0f), glm::radians(MinuteHand), glm::vec3(0.0f, 0.0f, 1.0f));
	unsigned int transformLoc = glGetUniformLocation(program, "transform");
	//Uniform Location, count, transpose, data
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

	// Choose object type 1 (rotation apply)
	unsigned int objLoc = glGetUniformLocation(program, "object");
	object = 1;
	glUniform1i(objLoc, object);
	glDrawArrays(GL_TRIANGLES, 3, 3);
}

void timePointing (void)
{
	GLfloat hour;
	GLfloat min;
	int hourTime;
	time_t now = time(0);
	tm ltm;
	localtime_s(&ltm, &now);

	hourTime = ltm.tm_hour;

	switch (hourTime)
	{
	case 13: 
		hour == 1;

	case 14:
		hour == 2;

	case 15:
		hour == 3;
	
	case 16:
		hour == 4;
	
	case 17:
		hour == 5;

	case 18:
		hour == 6;

	case 19:
		hour == 7;

	case 20:
		hour == 8;

	case 21:
		hour == 9;

	case 22:
		hour == 10;

	case 23:
		hour == 11;

	case 24:
		hour == 0;

	case 0:
		hour == 0;

	default: 
		hour == hour;
	}

	//test 
	hour = (ltm.tm_hour * 30); // hour
	
	min = (ltm.tm_min * 6)*-1.0f; // minute
	
	MinuteHand = min;
	HourHand = hour;
	
	//x = (ltm.tm_hour * 30)*-1.0f; // hour
	//HourHand = (GLfloat)x;
		//y = (ltm.tm_min * 6)*-1.0f; // minute
	//MinuteHand = (GLfloat) y;
}
void drawFrame(void)
{
	glUseProgram(program);
	// Choose object type 2 (no transformation)
	unsigned int objLoc = glGetUniformLocation(program, "object");
	object = 2;
	glUniform1i(objLoc, object);
	glDrawArrays(GL_QUADS, 0, 4);
 }

/* Not working 
void drawMarker(void)
{
	unsigned int objLoc = glGetUniformLocation(program, "object");
	object = 3;
	glUniform1i(objLoc, object);
	glDrawArrays(GL_QUADS, 0, 4);
}
*/

void renderBitmapCharacter(int x, int y, void* font, char* string)
{
	glRasterPos2d(x, y);		//set the starting position of the string to render
	glColor3f(0.0, 0.0, 0.0);
	glutBitmapString(font, (const unsigned char*)string);
	GLUT_BITMAP_HELVETICA_18;
}

void drawText()
{
	glUseProgram(0);

	// Use OpenGL fixed pipeline functions 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 1280.0, 0.0, 720.0);
	

	//start_x, start_y, font_type, text_to_display
	renderBitmapCharacter(625, 525, (void*)font, (char*)"12");
	renderBitmapCharacter(925, 360, (void*)font, (char*)"3");
	renderBitmapCharacter(625, 185, (void*)font, (char*)"6");
	renderBitmapCharacter(325, 360, (void*)font, (char*)"9");
	
}


//This function is called whenever the screen is redrawn
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);		// clear screen
	

	drawFrame();

	drawText();

	drawHourHand();

	drawMinuteHand();

	timePointing();

	//drawMarker();

	glutSwapBuffers();		// use with conjunction with GLUT_DOUBLE
}

//Timer function, run every 20ms. I.e. rotate by 1 degree every 0.02s 2000ms = 2s 
void update(int m)
{/*
	GLfloat hour;
	GLfloat min;
	int hourTime;
	time_t now = time(0);
	tm ltm;
	localtime_s(&ltm, &now);

	hourTime = ltm.tm_hour;

	switch (hourTime)
	{
	case 13: 
		hour == 1;

	case 14:
		hour == 2;

	case 15:
		hour == 3;
	
	case 16:
		hour == 4;
	
	case 17:
		hour == 5;

	case 18:
		hour == 6;

	case 19:
		hour == 7;

	case 20:
		hour == 8;

	case 21:
		hour == 9;

	case 22:
		hour == 10;

	case 23:
		hour == 11;

	case 24:
		hour == 0;

	case 0:
		hour == 0;

	default: 
		hour == hour;
	}

	//test 
	hour = (ltm.tm_hour * 30); // hour
	
	min = (ltm.tm_min * 6)*-1.0f; // minute
	
	MinuteHand = min;
	HourHand = hour;
	
	//x = (ltm.tm_hour * 30)*-1.0f; // hour
	//HourHand = (GLfloat)x;
		//y = (ltm.tm_min * 6)*-1.0f; // minute
	//MinuteHand = (GLfloat) y;
	*/
	//Need to fix

	MinuteHand -= 6.0f; //Minute rotate by 1 degree 60
	if (MinuteHand <= 0.0f) {
		MinuteHand = 360.0f;
				
		HourHand -= 30.0f;
		if (HourHand <= 0.0f)
			HourHand = 360.0f;
	}

	glutTimerFunc(40, update, 0);
}

void processMenuEvents(int option) {

	font = option;
}

// Create menu items
void createMenus() {
	int fontSubMenu = glutCreateMenu(processMenuEvents);

	glutAddMenuEntry("8 by 13", (int)GLUT_BITMAP_8_BY_13);
	glutAddMenuEntry("9 by 15", (int)GLUT_BITMAP_9_BY_15);
	glutAddMenuEntry("Times Roman 24", (int)GLUT_BITMAP_TIMES_ROMAN_24);
	
	int menu = glutCreateMenu(processMenuEvents);
	glutAddSubMenu("Font", fontSubMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);	// set display mode
													//use RGB Color, double buffering
	glutInitWindowSize(600, 600);					// set window size
													// width, height
	glutInitWindowPosition(50, 25);					// set window position on screen
	glutCreateWindow("Low Meng Han Darel Clock");
	glewInit();										// Initialize and load required OpenGL components
	init();
	createMenus();

	glutDisplayFunc(display);						// register display callback
	glutIdleFunc(display);							// register idle callback
	glutTimerFunc(20, update, 0);					//Timer function, run after 25ms
													//timer, function, parameter

	glutMainLoop();									// go into a permenant loop
}















