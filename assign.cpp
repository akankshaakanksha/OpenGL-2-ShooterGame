#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <glm/gtc/matrix_transform.hpp>
#include "vec3f.cpp"
#include "vec3f.h"
#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
#define RAD2DEG(rad) (rad * 180/ PI)


using namespace std;

struct Mouse { int x = 0; int y = 0; };
int enter = 0;

float findHeight(float , float );
struct Mouse mouse; struct Target { float x = 0.0f, y = findHeight(0,0.5f), z = 0.5f; }; struct Target targett;
struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;


GLuint programID, programID2;
GLuint VertexArrayID[2];
GLuint vertexbuffer[2];
GLuint colorbuffer[2];
int numVerticesOfPyramid = 24;
//int numVerticesOfCube = 36;
#define terrainSize 150
#define pyramidScale 0.02f
GLuint VertexArrayIDterrain[terrainSize];
GLuint vertexbufferterrain[terrainSize];
GLuint VertexArrayIDtarget[1];
GLuint vertexbuffertarget[1];
GLuint colorbuffertarget[1];
int collisionTarget();
int score=10;
//float heightmap[terrainSize][terrainSize];
struct Pyramid
{
	float rotation = 0;
	float x = 0.0f, y = 0.0f, z = 0.0f, sx = 0.2f, sy = 0.2f, sz = 0.1f;
	float velx = 0.0f, velz = 0.0f, vel=0.0f,angle=0.0f;
	float anglex=0.0f,anglez=0.0f, angley=0.0f;
};
struct Pyramid pyramid;

int collisionTarget() { float tx,ty,tz; tx = targett.x; ty = targett.y - 0.5f; tz = targett.z; float px,py,pz; px = pyramid.x * pyramid.sx; py = (pyramid.y-1.0f) * pyramid.sy; pz = pyramid.z * pyramid.sz; float d = (tx - px)*(tx - px)+(ty - py)*(ty - py)+(tz - pz)*(tz - pz); if (d < 0.1f) return 1; else return 0; }

float findHeight(float x,float z)
{
	float finy;
	float t = -(x*x*x*x- 4*x*x*x -z*x +15*x - z*3 )/4 ;
	finy = 0.05f * t + 0.5f;
	return finy;
}

Vec3f  compute_normal(){
	Vec3f sum(0.0f, 0.0f, 0.0f);	
	float x = pyramid.x;
	float z = pyramid.z;			
	Vec3f out;
	if (z > 0) {
		out = Vec3f(0.0f, findHeight(x,z - 1) - findHeight(x,z), -1.0f);
	}
	Vec3f in;
	if (z < terrainSize - 1 ) {
		in = Vec3f(0.0f, findHeight(x,z+1) - findHeight(x,z), 1.0f);
	}
	Vec3f left;
	if (x > 0) {
		left = Vec3f(-1.0f, findHeight(x-1,z) - findHeight(x,z), 0.0f);
	}
	Vec3f right;
	if (x < terrainSize -1 ) {
		right = Vec3f(1.0f, findHeight(x+1,z) - findHeight(x,z), 0.0f);
	}

	if (x > 0 && z > 0) {
		sum += out.cross(left).normalize();
	}
	if (x > 0 && z < terrainSize-1) {
		sum += left.cross(in).normalize();
	}
	if (x < terrainSize -1 && z < terrainSize-1) {
		sum += in.cross(right).normalize();
	}
	if (x < terrainSize - 1 && terrainSize -1) {
		sum += right.cross(out).normalize();
	}

	return sum;
}


void update_posn(int val){

	if( collisionTarget() == 1 && enter ){
		
		pyramid.velx = 0.0f;pyramid.velz=0.0f;
		pyramid.x = terrainSize*pyramidScale;
		pyramid.z = terrainSize*pyramidScale/2;
		pyramid.y = findHeight(pyramid.x,pyramid.z);
		enter = 0;
		pyramid.vel=0.0f;

		score+=10;
		printf("Your Score : %d\n",score);
		exit(0);

	}
	if(fabs(pyramid.x) > terrainSize*pyramidScale || fabs(pyramid.z) > terrainSize*pyramidScale || pyramid.x < 0 || pyramid.z < 0 )
	{
		if( enter ){
			pyramid.velx = 0.0f;pyramid.velz=0.0f;
			pyramid.x = terrainSize*pyramidScale;
			pyramid.z = terrainSize*pyramidScale/2;
			pyramid.y = findHeight(pyramid.x,pyramid.z);
			enter=0;
			pyramid.vel=0.0f;
			score-=5;
		}
	}
	Vec3f gravity;
	Vec3f normal = compute_normal();
	
	float a=normal[1]/sqrt(pow(normal[0],2)+pow(normal[1],2)+pow(normal[2],2)) ;
	gravity[0]=-a*normal[0]/sqrt(pow(normal[0],2)+pow(normal[1],2)+pow(normal[2],2));
	gravity[1]=1-a*normal[1]/sqrt(pow(normal[0],2)+pow(normal[1],2)+pow(normal[2],2));
	gravity[2]=-a*normal[2]/sqrt(pow(normal[0],2)+pow(normal[1],2)+pow(normal[2],2));
	if(pyramid.velx && pyramid.velz){
		pyramid.velx-= gravity[0]/50;
		pyramid.velz-= gravity[2]/50;
	}
	//cout << gravity[0]/50 << endl;
	float last=findHeight(pyramid.x,pyramid.z);
	pyramid.x+=pyramid.velx;
	pyramid.z+=pyramid.velz;
	pyramid.y = findHeight(pyramid.x, pyramid.z);
	
	pyramid.anglex= RAD2DEG(acos(normal[0]/sqrt(pow(normal[0],2)+pow(normal[1],2)+pow(normal[2],2)) ));
	pyramid.anglez= RAD2DEG(acos(normal[2]/sqrt(pow(normal[0],2)+pow(normal[1],2)+pow(normal[2],2)) ));
	pyramid.angley= RAD2DEG(acos(normal[1]/sqrt(pow(normal[0],2)+pow(normal[1],2)+pow(normal[2],2)) ));
	if(last < findHeight(pyramid.x,pyramid.z))
	{
		if(pyramid.velx > 0)
		{
			pyramid.velx-=0.01;
		}
		else
		{
			pyramid.velx+=0.01;
		}
		if(pyramid.z>0)
		{
			pyramid.z-=0.01;
		}
		else
		{
			pyramid.z+=0.01;
		}
	}
	glutTimerFunc(25, update_posn, 0);
}


/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void keyboardSpecialUp(int key, int x, int y)
{
	if(enter==0)
	{
		switch (key) {
		case GLUT_KEY_DOWN :
			//printf("imhere");
			if( pyramid.vel >-2.0f && pyramid.vel < 0.0f )
				pyramid.vel+=0.04200f;
			//pyramid.velx=pyramid.vel*cos(DEG2RAD(pyramid.angle));
			//pyramid.velz=pyramid.vel*sin(DEG2RAD(pyramid.angle));
			break;
		case GLUT_KEY_UP :
			if( pyramid.vel > -002.60f )
				pyramid.vel-=0.0600f;
			//pyramid.velx=pyramid.vel*cos(DEG2RAD(pyramid.angle));
			//pyramid.velz=pyramid.vel*sin(DEG2RAD(pyramid.angle));
			break;
		case GLUT_KEY_LEFT :
			pyramid.angle-=2.0f;
			//pyramid.velx=pyramid.vel*cos(DEG2RAD(pyramid.angle));
			//pyramid.velz=pyramid.vel*sin(DEG2RAD(pyramid.angle));
			break;
		case GLUT_KEY_RIGHT :
			pyramid.angle+=1.0f;
			//pyramid.velx=pyramid.vel*cos(DEG2RAD(pyramid.angle));
			//pyramid.velz=pyramid.vel*sin(DEG2RAD(pyramid.angle));
			break;
		}
	}
}
float eye1=5,eye2=2,eye3=1.5,target1=1.5,target2=0,target3=1.5,up1=0,up2=1,up3=0;
/* Executed when a regular key is pressed */
void keyboardDown (unsigned char key, int x, int y)
{
	switch (key) {
		case 'Q':
		case 'w': eye1 = pyramid.x;
			  eye2 = pyramid.y + 1.5f;
			  eye3 = pyramid.z;
			  target1 = 0.0f;
			  target2 = 0.5f;
			  target3 = 0.5f;
			  up1 = 0;
			  up2 = 1;
			  up3 = 0;
			  break;
		case 'a': eye1 = 4;
			  eye2 = 2;
			  eye3 = 5;
			  target1 = 0; 
			  target2 = 0;
			  target3 = 0;
			  up1 = 0;
			  up2 = 1;
			  up3 =	0;
			  break;
		case 's' :  eye1 = 1.5f;
			    eye2 = 5.0f;
			    eye3 = 1.5f;
			    target1 = 1.5f; 
			    target2 = findHeight(75*0.02f,75*0.02f);
			    target3 = pyramid.z-1.5f;
			    up1 = -1;
			    up2 = 0;
			    up3 = 0;
			  break;
                case 'd' : 
	  		    eye1 = 5.0f;
			    eye2 = 0.5f;
			    eye3 = 0.5f;
			    target1 = pyramid.x; 
			    target2 = pyramid.y + 1.5f;
			    target3 = 1.5f;
			    up1 = 0;
			    up2 = 1;
			    up3 = 0;
                           break;				 
		case 27: //ESC
		{
			printf( "your score: %d\n",score);
			exit (0);
		}
		case 32:	
			enter = 1;
			pyramid.velx=pyramid.vel*cos(DEG2RAD(pyramid.angle));
			pyramid.velz=pyramid.vel*sin(DEG2RAD(pyramid.angle));
			break;
	}
}

/* Executed when a regular key is released */

void mouseMotion(int x, int y) 
{ if(mouse.x == 0) 
	{ mouse.x = x;
		mouse.y = y;
		return; }
	if(mouse.x > x) 
	{  
		printf("right\n");
		up1-=0.1; 
	}
	if(mouse.x < x)
	{
		printf("left\n");
		up1+=0.1;
	}
	if(mouse.y > y) 
	{  
		printf("up\n");
		up2-=0.1; 
	}
	if(mouse.y < y)
	{
		printf("down\n");
		up2+=0.1;
	}

	mouse.x = x;
	mouse.y = y; 
	//printf("%d %d\n",x,y); 
}

/* Executed when window is resized to 'width' and 'height' */
void reshapeWindow (int width, int height)
{
	GLfloat fov = 90.0f;

	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) width, (GLsizei) height);

	// set the projection matrix as perspective
	/* glMatrixMode (GL_PROJECTION);
	   glLoadIdentity ();
	   gluPerspective (fov, (GLfloat) width / (GLfloat) height, 0.1, 500.0); */
	// Store the projection matrix in a variable for future use
	Matrices.projection = glm::perspective (fov, (GLfloat) width / (GLfloat) height, 0.1f, 500.0f);
}

void createTarget()
{
	static const GLfloat vertex_buffer_data [] =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		 0.0f, 0.0f, 1.0f,
		 0.0f, 1.0f, 1.0f,
	};

	static const GLfloat color_buffer_data [] = 
	{
		1,0,0, // color 0
		1,0,0, // color 1
		1,0,0, // color 2
		1,0,0  // color 1
	};

	glBindVertexArray (VertexArrayIDtarget[0]); // Bind the VAO for pyramid
	glBindBuffer (GL_ARRAY_BUFFER, vertexbuffertarget[0]); // Bind the VBO vertices for pyramid
	glBufferData (GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
	glVertexAttribPointer(
			0,                  // attribute 0. Vertices 
			3,                  // size (x,y,z)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
	glBindBuffer (GL_ARRAY_BUFFER, colorbuffertarget[0]); // Bind the VBO colors for pyramid
	glBufferData (GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	glVertexAttribPointer(
			1,                  // attribute 1. Color
			3,                  // size (r,g,b)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
}


void createTerrain()
{
	//vertex_buffer_data[terrainSize*3][terrainSize];
	int i,j,y;
	float finx,finy,finz;
	for (i=0;i<terrainSize;i++)
	{
		//printf("%d\n",i);
		GLfloat vertex_buffer_data [terrainSize*6]; 
		for (j=0;j<terrainSize;j++)
		{

			finx = ((float)i)*pyramidScale;
			//finy = 1.0f;
			finz = ((float)j)*pyramidScale;
			//int t = (i * i * 2 + j * 3 + i * j);
			int jp = j % 60;
			if (jp > 30)
			jp = 60 - jp;
			float x = ((float)i)/(float)terrainSize*5.0f- 1.3f;
			float z = ((float)jp)/(float)terrainSize*25.0f;

			finy = findHeight(x,z);
			
			vertex_buffer_data[j*6+0] = finx;
			vertex_buffer_data[j*6+1] = 0.0f;
			vertex_buffer_data[j*6+2] = finz;
			vertex_buffer_data[j*6+3] = finx;
			vertex_buffer_data[j*6+4] = finy;
			vertex_buffer_data[j*6+5] = finz;
		}
		glBindVertexArray (VertexArrayIDterrain[i]); // Bind the VAO for pyramid
		glBindBuffer (GL_ARRAY_BUFFER, vertexbufferterrain[i]); // Bind the VBO vertices for pyramid
		glBufferData (GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
		glVertexAttribPointer(
			0,                  // attribute 0. Vertices 
			3,                  // size (x,y,z)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
	glBindBuffer (GL_ARRAY_BUFFER, vertexbufferterrain[i]); // Bind the VBO colors for pyramid
	glBufferData (GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	glVertexAttribPointer(
			1,                  // attribute 1. Color
			3,                  // size (r,g,b)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
	}


}

void setPyramid(int x, int z)
{
	pyramid.x = x*pyramidScale;
	pyramid.z = z*pyramidScale;
	pyramid.y = findHeight(x,z);
	//printf("%f %f %f\n",pyramid.x,pyramid.y,pyramid.z);
}
void createPyramid ()
{

	/* Define vertex array as used in glBegin (GL_TRIANGLES) */
	int z = 75, x= 150;
	setPyramid(x,z);
	static const GLfloat vertex_buffer_data [] =
	{
		-1.0f, 0.0f, 1.0f,
		 1.0f, 0.0f, 1.0f,
		 0.0f, 2.0f, 0.0f,
                
		 -1.0f, 0.0f, 1.0f,
		 -1.0f, 0.0f, -1.0f,
		 0.0f, 2.0f, 0.0f,
		               
		 1.0f, 0.0f, -1.0f,
		 -1.0f, 0.0f, -1.0f,
		 0.0f, 2.0f, 0.0f,
                
		 1.0f, 0.0f, 1.0f,
		 1.0f, 0.0f, -1.0f,
		 0.0f, 2.0f, 0.0f,
//pyr 2
		-1.0f, 0.0f, 1.0f,
		 1.0f, 0.0f, 1.0f,
		 0.0f, -1.0f, 0.0f,
                
		 -1.0f, 0.0f, 1.0f,
		 -1.0f, 0.0f, -1.0f,
		 0.0f, -1.0f, 0.0f,
		               
		 1.0f, 0.0f, -1.0f,
		 -1.0f, 0.0f, -1.0f,
		 0.0f, -1.0f, 0.0f,
                
		 1.0f, 0.0f, 1.0f,
		 1.0f, 0.0f, -1.0f,
		 0.0f, -1.0f, 0.0f

		 /*-1.0f, 1.0f, 0.0f,
		 1.0f, 1.0f, 0.0f,
		 0.0f, 0.0f, 2.0f,
                
		 -1.0f, 1.0f, 0.0f,
		 -1.0f, -1.0f, 0.0f,
		 0.0f, 0.0f, 2.0f,
		               
		 1.0f, -1.0f, 0.0f,
		 -1.0f, -1.0f, 0.0f,
		 0.0f, 0.0f, 2.0f,
                
		 1.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f, 0.0f, 2.0f,
//pyr 2
		-1.0f, 1.0f, 0.0f,
		 1.0f, 1.0f, 0.0f,
		 0.0f, 0.0f, -1.0f,
                
		 -1.0f, 1.0f, 0.0f,
		 -1.0f, -1.0f, 0.0f,
		 0.0f, 0.0f, -1.0f,
		               
		 1.0f, -1.0f, 0.0f,
		 -1.0f, -1.0f, 0.0f,
		 0.0f, 0.0f, -1.0f,
                
		 1.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f, 0.0f, -1.0f*/

	};

	static const GLfloat color_buffer_data [] = 
	{
		1,0,0, // color 0
		0,1,0, // color 1
		0,0,1, // color 2
				
		1,0,0, // color 0
		0,0,1, // color 2
		0,1,1, // color 4
				
		1,0,0, // color 0
		0,1,1, // color 4
		1,1,0, // color 3
				
		1,0,0, // color 0
		1,1,0, // color 3
		0,1,0, // color 1
				
		0,1,0, // color 1
		1,1,0, // color 3
		0,1,1, // color 4
				
		0,1,1, // color 4
		0,0,1, // color 2
		0,1,0,  // color 1

		0,1,0, // color 1
		1,1,0, // color 3
		0,1,1, // color 4
				
		0,1,1, // color 4
		0,0,1, // color 2
		0,1,0  // color 1
	};

	glBindVertexArray (VertexArrayID[0]); // Bind the VAO for pyramid
	glBindBuffer (GL_ARRAY_BUFFER, vertexbuffer[0]); // Bind the VBO vertices for pyramid
	glBufferData (GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
	glVertexAttribPointer(
			0,                  // attribute 0. Vertices 
			3,                  // size (x,y,z)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
	glBindBuffer (GL_ARRAY_BUFFER, colorbuffer[0]); // Bind the VBO colors for pyramid
	glBufferData (GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	glVertexAttribPointer(
			1,                  // attribute 1. Color
			3,                  // size (r,g,b)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
}

float camera_rotation_angle = 0;
void drawTerrain()
{
	
	// clear the color and depth in the frame buffer
	//glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use the loaded shader program
	//glUseProgram (programID);
	//glUseProgram (programID2);
	//glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 4, 5*sin(camera_rotation_angle*M_PI/180.0f) );
	glm::vec3 eye ( eye1,eye2,eye3);
	glm::vec3 target (target1,target2,target3);
	glm::vec3 up (up1,up2,up3);

	// Compute Camera matrix (view)
	Matrices.view = glm::lookAt( eye, target, up ); // ***need to generate own matrices. opengl wont do.

	// Load identity to model matrix
	Matrices.model = glm::mat4(1.0f);
	//Matrices.model = glm::scale(glm::mat4(1.0f),glm::vec3(2.0f));

	// Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
	glm::mat4 VP = Matrices.projection * Matrices.view;
	
	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// For each model you render, since the MVP will be different (at least the M part)
	glUseProgram (programID2);
	
	glm::mat4 MVP;	// MVP = Projection * View * Model

	// Implement & use Push Matrix to save intermediate transformation matrices & reduce computations 
	// glPushMatrix ();

	/* Render your scene */

//	glm::mat4 translatePyramid = glm::translate (glm::vec3(0.0f, 0.0f, 0.0f)); // glTranslatef
	//glm::mat4 rotPyramid; //= glm::rotate(90.0f, glm::vec3(1,0,0));
//	glm::mat4 rotatePyramid = glm::rotate(pyramid.rotation, glm::vec3(0,1,0));  // glRotatef

//	glm::mat4 pyramidTransform = translatePyramid ;//* rotatePyramid ;//* rotPyramid;
//	Matrices.model *= pyramidTransform; 
	//glm::mat4 test = glm::vec4(2.0f, 2.0f, 2.0f,1.0f);
	//Matrices.model = Matrices.model * test;
	MVP = VP * Matrices.model; // MVP = p * V * M
	
	//glUseProgram (programID2);
	
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	//glUseProgram (programID2);
	int i = 0;
	for (i = 0;i< terrainSize ;i++)
	{
	glBindVertexArray (VertexArrayIDterrain[i]);
	
	// Enable Vertex Attribute 0 - 3d Vertices
	glEnableVertexAttribArray(0);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferterrain[i]);

	// Enable Vertex Attribute 1 - Color
	glEnableVertexAttribArray(1);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferterrain[i]);

	// Draw the pyramid !
	//glDrawArrays(GL_LINES, 0, terrainSize*terrainSize*2);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, terrainSize*2);
	//glDrawArrays(GL_LINES, 0, terrainSize);
	}
}

void drawTarget ()
{
	// use the loaded shader program
	glUseProgram (programID);
	//glUseProgram (programID2);
	//glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 4, 5*sin(camera_rotation_angle*M_PI/180.0f) );
	glm::vec3 eye ( eye1,eye2,eye3);
	glm::vec3 target (target1,target2,target3);
	glm::vec3 up (up1,up2,up3);
	// Compute Camera matrix (view)
	Matrices.view = glm::lookAt( eye, target, up ); // ***need to generate own matrices. opengl wont do. */ //-----------

	// Load identity to model matrix
	Matrices.model = glm::mat4(1.0f);
	//Matrices.model = glm::scale(glm::mat4(1.0f),glm::vec3(pyramid.sx, pyramid.sy, pyramid.sz));

	// Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
	glm::mat4 VP = Matrices.projection * Matrices.view;
	
	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// For each model you render, since the MVP will be different (at least the M part)
	glm::mat4 MVP;	// MVP = Projection * View * Model

	// Implement & use Push Matrix to save intermediate transformation matrices & reduce computations 
	// glPushMatrix ();

	/* Render your scene */


//	glm::mat4 translate = glm::translate (glm::vec3(-01.f, 1.0f, -0.5f)); // glTranslatef
	//printf("%f %f %f\n",pyramid.x,pyramid.y,pyramid.z);
	//glm::mat4 rotPyramid; //= glm::rotate(90.0f, glm::vec3(1,0,0));
	//glm::mat4 rotatePyramid = glm::rotate(pyramid.rotation, glm::vec3(0,1,0));  // glRotatef

//	glm::mat4 pyramidTransform = translate;
//	Matrices.model *= pyramidTransform; 

	MVP = VP * Matrices.model; // MVP = p * V * M
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);


	// Bind the VAO to use
	glBindVertexArray (VertexArrayIDtarget[0]);
	
	// Enable Vertex Attribute 0 - 3d Vertices
	glEnableVertexAttribArray(0);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffertarget[0]);

	// Enable Vertex Attribute 1 - Color
	glEnableVertexAttribArray(1);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffertarget[0]);

	// Draw the pyramid !
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

void drawPyramid ()
{
	// use the loaded shader program
	glUseProgram (programID);
	//glUseProgram (programID2);
	//glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 4, 5*sin(camera_rotation_angle*M_PI/180.0f) );
	glm::vec3 eye ( eye1,eye2,eye3);
	glm::vec3 target (target1,target2, target3);
	glm::vec3 up (up1,up2,up3);


	// Compute Camera matrix (view)
	Matrices.view = glm::lookAt( eye, target, up ); // ***need to generate own matrices. opengl wont do.

	// Load identity to model matrix
	Matrices.model = glm::mat4(1.0f);
	Matrices.model = glm::scale(glm::mat4(1.0f),glm::vec3(pyramid.sx, pyramid.sy, pyramid.sz));

	// Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
	glm::mat4 VP = Matrices.projection * Matrices.view;
	
	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// For each model you render, since the MVP will be different (at least the M part)
	glm::mat4 MVP;	// MVP = Projection * View * Model

	// Implement & use Push Matrix to save intermediate transformation matrices & reduce computations 
	// glPushMatrix ();

	/* Render your scene */


	glm::mat4 translatePyramid = glm::translate (glm::vec3(pyramid.x/pyramid.sx, pyramid.y/pyramid.sy + 1.0f, pyramid.z/pyramid.sz)); // glTranslatef
	//printf("%f %f %f\n",pyramid.x,pyramid.y,pyramid.z);
	glm::mat4 rotPyramidx = glm::rotate(pyramid.anglex, glm::vec3(1,0,0));
	glm::mat4 rotPyramidz = glm::rotate(pyramid.anglez, glm::vec3(0,0,1));  // glRotatef
	glm::mat4 rotPyramidy = glm::rotate(pyramid.anglez, glm::vec3(0,1,0));  // glRotatef

	glm::mat4 pyramidTransform = translatePyramid * rotPyramidx * rotPyramidy * rotPyramidz;
	Matrices.model *= pyramidTransform; 
	//glm::mat4 test = glm::vec4(2.0f, 2.0f, 2.0f,1.0f);
	//Matrices.model = Matrices.model * test;
	MVP = VP * Matrices.model; // MVP = p * V * M
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);


	// Bind the VAO to use
	glBindVertexArray (VertexArrayID[0]);
	
	// Enable Vertex Attribute 0 - 3d Vertices
	glEnableVertexAttribArray(0);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);

	// Enable Vertex Attribute 1 - Color
	glEnableVertexAttribArray(1);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer[0]);

	// Draw the pyramid !
	glDrawArrays(GL_TRIANGLES, 0, numVerticesOfPyramid); // Starting from vertex 0; 3 vertices total -> 1 triangle
}




/* Render the scene with openGL */
void draw ()
{
	// clear the color and depth in the frame buffer
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*
	// use the loaded shader program
	glUseProgram (programID);
	//glUseProgram (programID2);
	glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 4, 5*sin(camera_rotation_angle*M_PI/180.0f) );
	//glm::vec3 eye ( 4, 2, 4 );
	glm::vec3 target (0, 0, 0);
	glm::vec3 up (0, 1, 0);

	// Compute Camera matrix (view)
	Matrices.view = glm::lookAt( eye, target, up ); // ***need to generate own matrices. opengl wont do.

	// Load identity to model matrix
	Matrices.model = glm::mat4(1.0f);
	Matrices.model = glm::scale(glm::mat4(1.0f),glm::vec3(pyramid.sx, pyramid.sy, pyramid.sz));

	// Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
	glm::mat4 VP = Matrices.projection * Matrices.view;
	
	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// For each model you render, since the MVP will be different (at least the M part)
	glm::mat4 MVP;	// MVP = Projection * View * Model

	// Implement & use Push Matrix to save intermediate transformation matrices & reduce computations 
	// glPushMatrix ();

	


	glm::mat4 translatePyramid = glm::translate (glm::vec3(pyramid.x/pyramid.sx, pyramid.y/pyramid.sy + 1.0f, pyramid.z/pyramid.sz)); // glTranslatef
	//printf("%f %f %f\n",pyramid.x,pyramid.y,pyramid.z);
	glm::mat4 rotPyramid; //= glm::rotate(90.0f, glm::vec3(1,0,0));
	glm::mat4 rotatePyramid = glm::rotate(pyramid.rotation, glm::vec3(0,1,0));  // glRotatef

	glm::mat4 pyramidTransform = translatePyramid * rotatePyramid * rotPyramid;
	Matrices.model *= pyramidTransform; 
	//glm::mat4 test = glm::vec4(2.0f, 2.0f, 2.0f,1.0f);
	//Matrices.model = Matrices.model * test;
	MVP = VP * Matrices.model; // MVP = p * V * M
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	
	*/
	//glUseProgram (programID2);
	drawTarget();
	drawPyramid();
	drawTerrain();
	// Pop matrix to undo transformations till last push matrix instead of recomputing model matrix
	// glPopMatrix ();
	// swap the frame buffers
	glutSwapBuffers ();

	// Increment angles
	//camera_rotation_angle++;
	pyramid.rotation+=2;
	//cube_rotation++;
}


/* Executed when the program is idle (no I/O activity) */
void idle () {
	// OpenGL should never stop drawing
	// can draw the same scene or a modified scene
	draw (); // drawing same scene
}


/* Initialise glut window, I/O callbacks and the renderer to use */
void initGLUT (int& argc, char** argv, int width, int height)
{
	// Init glut
	glutInit (&argc, argv);
	
	// Init glut window
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitContextVersion (3, 3); // Init GL 3.3
	glutInitContextFlags (GLUT_CORE_PROFILE); // Use Core profile - older functions are deprecated
	glutInitWindowSize (width, height);
	glutCreateWindow ("Sample OpenGL3.3 Application");
	
	// Initialize GLEW, Needed in Core profile
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		cout << "Error: Failed to initialise GLEW : "<< glewGetErrorString(err) << endl;
		exit (1);
	}

	// register glut callbacks
	glutKeyboardFunc (keyboardDown);
	//glutKeyboardUpFunc (keyboardUp);

	//glutSpecialFunc (keyboardSpecialDown);
	glutSpecialFunc (keyboardSpecialUp);

	//glutMouseFunc (mouseClick);
	glutMotionFunc (mouseMotion);

	glutReshapeFunc (reshapeWindow);

	glutDisplayFunc (draw); // function to draw when active
	glutIdleFunc (idle); // function to draw when idle (no I/O activity)

	glutIgnoreKeyRepeat (true); // Ignore keys held down
}

/* Process menu option 'op' */


void initGL (int width, int height)
{
	// Create Vertex Array Object
	// Should be done after CreateWindow and before any other GL calls
	glGenVertexArrays(2, &VertexArrayID[0]); // VAO
	glGenBuffers (2, &vertexbuffer[0]); // VBO - vertices
	glGenBuffers (2, &colorbuffer[0]);  // VBO - colors

	glGenVertexArrays(terrainSize, &VertexArrayIDterrain[0]); // VAO
	glGenBuffers (terrainSize, &vertexbufferterrain[0]); // VBO - vertices

	glGenVertexArrays(1, &VertexArrayIDtarget[0]); // VAO
	glGenBuffers (1, &vertexbuffertarget[0]); // VBO - vertices
	glGenBuffers (1, &colorbuffertarget[0]);  // VBO - colors

	// Create the models
	createTarget();
	createTerrain();
	createPyramid (); // Generate the vertices data & copy into the array buffer
	//createTerrain();
	//createCube ();
	
	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "Sample_GL3.vert", "Sample_GL3.frag" );
	programID2 = LoadShaders( "terrain.vert", "terrain.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");

	
	reshapeWindow (width, height);

	glClearColor (0.0f, 1.0f, 1.0f, 0.0f);
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
}



int main (int argc, char** argv)
{
	int width = 800;
	int height = 600;

	initGLUT (argc, argv, width, height);

	initGL (width, height);
 glutTimerFunc(25, update_posn, 0);

	glutMainLoop ();

	return 0;
}
