#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <ratio>
#include <chrono>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "GL\glew.h"
#include "GL\glut.h"

using namespace std;

void mat4Perspective(float out[16], float fovy, float aspect, float nears, float fars)
{
	float f = 1.0 / tan(fovy / 2),
		nf = 1 / (nears - fars);
	out[0] = f / aspect;
	out[1] = 0;
	out[2] = 0;
	out[3] = 0;
	out[4] = 0;
	out[5] = f;
	out[6] = 0;
	out[7] = 0;
	out[8] = 0;
	out[9] = 0;
	out[10] = (fars + nears) * nf;
	out[11] = -1;
	out[12] = 0;
	out[13] = 0;
	out[14] = (2 * fars * nears) * nf;
	out[15] = 0;
};

void mat4LookAt(float out[16], float eyex, float eyey, float eyez, float centerx, float centery, float centerz, float upx, float upy, float upz) {
	float x0, x1, x2, y0, y1, y2, z0, z1, z2, len;

	if (abs(eyex - centerx) < 0.000001 &&
		abs(eyey - centery) < 0.000001 &&
		abs(eyez - centerz) < 0.000001) {
		out[0] = out[5] = out[10] = out[15] = 1.0; 
		out[1] = out[2] = out[3] = out[4] = out[6] = out[7] = out[8] = out[9] = out[11] = out[12] = out[13] = out[14] = 0.0;
	}

	z0 = eyex - centerx;
	z1 = eyey - centery;
	z2 = eyez - centerz;

	len = 1 / sqrt(z0 * z0 + z1 * z1 + z2 * z2);
	z0 *= len;
	z1 *= len;
	z2 *= len;

	x0 = upy * z2 - upz * z1;
	x1 = upz * z0 - upx * z2;
	x2 = upx * z1 - upy * z0;
	len = sqrt(x0 * x0 + x1 * x1 + x2 * x2);
	if (!len) {
		x0 = 0.0;
		x1 = 0.0;
		x2 = 0.0;
	}
	else {
		len = 1 / len;
		x0 *= len;
		x1 *= len;
		x2 *= len;
	}

	y0 = z1 * x2 - z2 * x1;
	y1 = z2 * x0 - z0 * x2;
	y2 = z0 * x1 - z1 * x0;

	len = sqrt(y0 * y0 + y1 * y1 + y2 * y2);
	if (!len) {
		y0 = 0.0;
		y1 = 0.0;
		y2 = 0.0;
	}
	else {
		len = 1 / len;
		y0 *= len;
		y1 *= len;
		y2 *= len;
	}

	out[0] = x0;
	out[1] = y0;
	out[2] = z0;
	out[3] = 0.0;
	out[4] = x1;
	out[5] = y1;
	out[6] = z1;
	out[7] = 0.0;
	out[8] = x2;
	out[9] = y2;
	out[10] = z2;
	out[11] = 0.0;
	out[12] = -(x0 * eyex + x1 * eyey + x2 * eyez);
	out[13] = -(y0 * eyex + y1 * eyey + y2 * eyez);
	out[14] = -(z0 * eyex + z1 * eyey + z2 * eyez);
	out[15] = 1.0;

}

void mat4Translate(float out[16], float a[16], float x, float y, float z) {
	float //x = v[0], y = v[1], z = v[2],
		a00, a01, a02, a03,
		a10, a11, a12, a13,
		a20, a21, a22, a23;

	if (a == out) {
		out[12] = a[0] * x + a[4] * y + a[8] * z + a[12];
		out[13] = a[1] * x + a[5] * y + a[9] * z + a[13];
		out[14] = a[2] * x + a[6] * y + a[10] * z + a[14];
		out[15] = a[3] * x + a[7] * y + a[11] * z + a[15];
	}
	else {
		a00 = a[0]; a01 = a[1]; a02 = a[2]; a03 = a[3];
		a10 = a[4]; a11 = a[5]; a12 = a[6]; a13 = a[7];
		a20 = a[8]; a21 = a[9]; a22 = a[10]; a23 = a[11];

		out[0] = a00; out[1] = a01; out[2] = a02; out[3] = a03;
		out[4] = a10; out[5] = a11; out[6] = a12; out[7] = a13;
		out[8] = a20; out[9] = a21; out[10] = a22; out[11] = a23;

		out[12] = a00 * x + a10 * y + a20 * z + a[12];
		out[13] = a01 * x + a11 * y + a21 * z + a[13];
		out[14] = a02 * x + a12 * y + a22 * z + a[14];
		out[15] = a03 * x + a13 * y + a23 * z + a[15];
	}
}

void mat4Rotate(float out[16], float a[16], float rad, float x, float y, float z) {
	float len = sqrt(x * x + y * y + z * z),
		s, c, t,
		a00, a01, a02, a03,
		a10, a11, a12, a13,
		a20, a21, a22, a23,
		b00, b01, b02,
		b10, b11, b12,
		b20, b21, b22;

	if (abs(len) >= 0.000001) 
	{

		len = 1 / len;
		x *= len;
		y *= len;
		z *= len;

		s = sin(rad);
		c = cos(rad);
		t = 1 - c;

		a00 = a[0]; a01 = a[1]; a02 = a[2]; a03 = a[3];
		a10 = a[4]; a11 = a[5]; a12 = a[6]; a13 = a[7];
		a20 = a[8]; a21 = a[9]; a22 = a[10]; a23 = a[11];

		// Construct the elements of the rotation matrix
		b00 = x * x * t + c; b01 = y * x * t + z * s; b02 = z * x * t - y * s;
		b10 = x * y * t - z * s; b11 = y * y * t + c; b12 = z * y * t + x * s;
		b20 = x * z * t + y * s; b21 = y * z * t - x * s; b22 = z * z * t + c;

		// Perform rotation-specific matrix multiplication
		out[0] = a00 * b00 + a10 * b01 + a20 * b02;
		out[1] = a01 * b00 + a11 * b01 + a21 * b02;
		out[2] = a02 * b00 + a12 * b01 + a22 * b02;
		out[3] = a03 * b00 + a13 * b01 + a23 * b02;
		out[4] = a00 * b10 + a10 * b11 + a20 * b12;
		out[5] = a01 * b10 + a11 * b11 + a21 * b12;
		out[6] = a02 * b10 + a12 * b11 + a22 * b12;
		out[7] = a03 * b10 + a13 * b11 + a23 * b12;
		out[8] = a00 * b20 + a10 * b21 + a20 * b22;
		out[9] = a01 * b20 + a11 * b21 + a21 * b22;
		out[10] = a02 * b20 + a12 * b21 + a22 * b22;
		out[11] = a03 * b20 + a13 * b21 + a23 * b22;

		if (a != out) { // If the source and destination differ, copy the unchanged last row
			out[12] = a[12];
			out[13] = a[13];
			out[14] = a[14];
			out[15] = a[15];
		}
	}
};

void LoadFile(const char* Filename, std::string& OutputString)
{
	std::ifstream input(Filename);
	if (!input.is_open()) {
		std::cout << "Open <" << Filename << "> error." << std::endl;
		return;
	}

	char temp[300];
	while (!input.eof()) {
		input.getline(temp, 300);
		OutputString += temp;
		OutputString += '\n';
	}

	input.close();
}

unsigned int LoadShader(std::string& SourceText, GLenum ShaderType)
{
	unsigned int shaderId = 0;
	shaderId = glCreateShader(ShaderType);
	const char* csource = SourceText.c_str();
	glShaderSource(shaderId, 1, &csource, NULL);
	glCompileShader(shaderId);
	char error[1000] = "";
	glGetShaderInfoLog(shaderId, 1000, NULL, error);
	std::cout << "Complie status: \n" << error << std::endl;
	return shaderId;
}

unsigned int vs, fs, program;
GLuint VAO;
GLuint positionBuffer;
GLuint colorBuffer;
GLuint indicesBuffer;
float Rotation = 0.0;

void InitShader(const char* VertexShaderFileName, const char* FragmentShaderFileName)
{
	std::string source;

	LoadFile(VertexShaderFileName, source);
	vs = LoadShader(source, GL_VERTEX_SHADER);
	source = "";
	LoadFile(FragmentShaderFileName, source);
	fs = LoadShader(source, GL_FRAGMENT_SHADER);

	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glUseProgram(program);
}

void Clean()
{
	glDetachShader(program, vs);
	glDetachShader(program, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteProgram(program);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &indicesBuffer);
}

void theCube() 
{
	InitShader("vertex.vs", "fragment.frag");

	GLfloat positionInfo[] =
	{
		// Top
		-1.0, 1.0, -1.0,	0.0,0.0,1.0,
		-1.0, 1.0, 1.0,		0.0,0.0,1.0,
		1.0, 1.0, 1.0,		0.0,0.0,1.0,
		1.0, 1.0, -1.0,		0.0,0.0,1.0,

		// Left
		-1.0, 1.0, 1.0,		-1.0,0.0,0.0,
		-1.0, -1.0, 1.0,	-1.0,0.0,0.0,
		-1.0, -1.0, -1.0,	-1.0,0.0,0.0,
		-1.0, 1.0, -1.0,	-1.0,0.0,0.0,

		// Right
		1.0, 1.0, 1.0,		1.0,0.0,0.0,
		1.0, -1.0, 1.0,		1.0,0.0,0.0,
		1.0, -1.0, -1.0,	1.0,0.0,0.0,
		1.0, 1.0, -1.0,		1.0,0.0,0.0,

		// Front
		1.0, 1.0, 1.0,		0.0,0.0,1.0,
		1.0, -1.0, 1.0,		0.0,0.0,1.0,
		-1.0, -1.0, 1.0,	0.0,0.0,1.0,
		-1.0, 1.0, 1.0,		0.0,0.0,1.0,

		// Back
		1.0, 1.0, -1.0,		0.0,0.0,-1.0,
		1.0, -1.0, -1.0,	0.0,0.0,-1.0,
		-1.0, -1.0, -1.0,	0.0,0.0,-1.0,
		-1.0, 1.0, -1.0,	0.0,0.0,-1.0,

		// Bottom
		-1.0, -1.0, -1.0,	0.0,-1.0,0.0,
		-1.0, -1.0, 1.0,	0.0,-1.0,0.0,
		1.0, -1.0, 1.0,		0.0,-1.0,0.0,
		1.0, -1.0, -1.0,	0.0,-1.0,0.0
	};
	GLfloat colorInfo[] =
	{
		//blue			//0.0,0.0
		0.0,0.0,1.0,	0.0,0.0,
		0.0,0.0,1.0,	0.0,0.25,
		0.0,0.0,1.0,	0.25,0.25,
		0.0,0.0,1.0,	0.25,0.0,
		//green			//0.25,0.0
		0.0,1.0,0.0,	0.25,0.0,
		0.0,1.0,0.0,	0.25,0.25,
		0.0,1.0,0.0,	0.5,0.25,
		0.0,1.0,0.0,	0.5,0.0,
		//red			//0.0,0.25
		1.0,0.0,0.0,	0.0,0.25,
		1.0,0.0,0.0,	0.0,0.5,
		1.0,0.0,0.0,	0.25,0.5,
		1.0,0.0,0.0,	0.25,0.25,
		//cyan			//0.25,0.5
		0.0,1.0,1.0,	0.25,0.5,
		0.0,1.0,1.0,	0.25,0.75,
		0.0,1.0,1.0,	0.5,0.75,
		0.0,1.0,1.0,	0.5,0.5,
		//magenta		//0.5,0.5
		1.0,0.0,1.0,	0.5,0.5,
		1.0,0.0,1.0,	0.5,0.75,
		1.0,0.0,1.0,	0.75,0.75,
		1.0,0.0,1.0,	0.75,0.5,
		//yellow		//0.75,0.5
		1.0,1.0,0.0,	0.75,0.5,
		1.0,1.0,0.0,	0.75,0.75,
		1.0,1.0,0.0,	1.0,0.75,
		1.0,1.0,0.0,	1.0,0.5
	};
	GLuint indices[] =
	{
		// Top
		0, 1, 2,
		0, 2, 3,

		// Left
		5, 4, 6,
		6, 4, 7,

		// Right
		8, 9, 10,
		8, 10, 11,

		// Front
		13, 12, 14,
		15, 14, 12,

		// Back
		16, 17, 18,
		16, 18, 19,

		// Bottom
		21, 20, 22,
		22, 20, 23
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positionInfo), positionInfo, GL_STATIC_DRAW);
	GLint positionLocation = glGetAttribLocation(program, "vertPosition");
	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	GLint normalLocation = glGetAttribLocation(program, "normalVert");
	glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(float)));

	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorInfo), colorInfo, GL_STATIC_DRAW);
	GLint colorLocation = glGetAttribLocation(program, "vertColor");
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	GLint textureLocation = glGetAttribLocation(program, "vertTexture");
	glVertexAttribPointer(textureLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));

	glGenBuffers(1, &indicesBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(positionLocation);
	glEnableVertexAttribArray(normalLocation);
	glEnableVertexAttribArray(colorLocation);
	glEnableVertexAttribArray(textureLocation);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 加载并生成纹理
	int width, height, nrChannels;
	unsigned char* data = stbi_load("texture8.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO);
}
void myDisplay(void)
{
	theCube();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	GLint lightPos = glGetUniformLocation(program, "lightPos");
	GLint lightColor = glGetUniformLocation(program, "lightColor");
	GLint viewPos = glGetUniformLocation(program, "viewPos");
	glUniform3f(lightPos, 3.0, 3.0, 3.0);
	glUniform3f(lightColor, 1.0, 1.0, 1.0);
	glUniform3f(viewPos, 0.0, 0.0, 5.0);
	
	Rotation += 0.05;
	if (Rotation >= 360) { Rotation -= 360; }
	//cout << Rotation << endl;

	GLint matCubeUniformLocation = glGetUniformLocation(program, "mCube");
	GLint matViewUniformLocation = glGetUniformLocation(program, "mView");
	GLint matProjUniformLocation = glGetUniformLocation(program, "mProj");
	float projMatrix[16];
	mat4Perspective(projMatrix, 45*acos(-1)/180, 1.0, 0.1, 1000.0);
	float viewMatrix[16];
	mat4LookAt(viewMatrix, 0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);//眼睛位置，看向位置，攝影機旋轉
	float cubeMatrix[16] = { 1.0,0.0,0.0,0.0,
							0.0,1.0,0.0,0.0, 
							0.0,0.0,1.0,0.0,
							0.0,0.0,0.0,1.0};
	//mat4Translate(cubeMatrix,     // destination matrix
	//	cubeMatrix,     // matrix to translate
	//	0.0, 0.0, -5.0 );  // amount to translate
	mat4Rotate(cubeMatrix,  // destination matrix
		cubeMatrix,  // matrix to rotate
		Rotation,     // amount to rotate in radians
		0, 0, 1);       // axis to rotate around (Z)
	mat4Rotate(cubeMatrix,  // destination matrix
		cubeMatrix,  // matrix to rotate
		Rotation*0.3 ,// amount to rotate in radians
		0, 1, 0);
	mat4Rotate(cubeMatrix,  // destination matrix
		cubeMatrix,  // matrix to rotate
		Rotation*0.7,// amount to rotate in radians
		1, 0, 0);
	glUniformMatrix4fv(matCubeUniformLocation, 1, GL_FALSE, cubeMatrix);
	glUniformMatrix4fv(matViewUniformLocation, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(matProjUniformLocation, 1, GL_FALSE, projMatrix);
	
	glDrawElements(GL_TRIANGLES, 6*6, GL_UNSIGNED_INT, 0);
	glFlush();
	Clean();
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	printf("Success!");
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(400, 400);
	glutCreateWindow("Test");
	
	GLenum err = glewInit();
	char* errMsg = (char*)glewGetErrorString(err);
	//while (true) {
	glutDisplayFunc(&myDisplay);
	//}
	glutMainLoop();
	return 0;
}