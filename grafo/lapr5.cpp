#pragma warning(disable:4996)
#pragma warning(disable:4838)

#define _WINSOCKAPI_ 
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>     
#include <GL/glut.h>
#include <GL/GLAux.h>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <tchar.h>
#include <vector>
#include "grafos.h"
#include "httpClient.h"
#include "httpRequest.h"
#include "irrKlang-1.5.0\include\irrKlang.h"

using namespace std;
using namespace irrklang;

#define NOME_TEXTURA_CHAO	"cobble.jpg"
#define NOME_TEXTURE_CHAO_NO "calcada.jpg"
#define NOME_TEXTURA_PAREDE "parede.jpg"
#define NOME_TEXTURA_SKYBOX_BK "miramar_bk.jpg"
#define NOME_TEXTURA_SKYBOX_FT "miramar_ft.jpg"
#define NOME_TEXTURA_SKYBOX_RT "miramar_rt.jpg"
#define NOME_TEXTURA_SKYBOX_DN "miramar_dn.jpg"
#define NOME_TEXTURA_SKYBOX_LF "miramar_lf.jpg"
#define NOME_TEXTURA_SKYBOX_UP "miramar_up.jpg"
#define NOME_TEXTURA_SKYBOX_STORM_BK "stormydays_bk.jpg"
#define NOME_TEXTURA_SKYBOX_STORM_FT "stormydays_ft.jpg"
#define NOME_TEXTURA_SKYBOX_STORM_RT "stormydays_rt.jpg"
#define NOME_TEXTURA_SKYBOX_STORM_DN "stormydays_dn.jpg"
#define NOME_TEXTURA_SKYBOX_STORM_LF "stormydays_lf.jpg"
#define NOME_TEXTURA_SKYBOX_STORM_UP "stormydays_up.jpg"
#define NOME_TEXTURA_SKYBOX_NIGHT_BK "space_bk.jpg"
#define NOME_TEXTURA_SKYBOX_NIGHT_FT "space_ft.jpg"
#define NOME_TEXTURA_SKYBOX_NIGHT_RT "space_rt.jpg"
#define NOME_TEXTURA_SKYBOX_NIGHT_DN "space_dn.jpg"
#define NOME_TEXTURA_SKYBOX_NIGHT_LF "space_lf.jpg"
#define NOME_TEXTURA_SKYBOX_NIGHT_UP "space_up.jpg"

#define NUM_TEXTURAS		20
#define MAX_PARTICLES 5000
#define WCX		640
#define WCY		480

#define graus(X) (double)((X)*180/M_PI)
#define rad(X)   (double)((X)*M_PI/180)

// luzes e materiais
typedef struct {
	int		sizeX, sizeY, bpp;
	char	*data;
}JPGImage;

#pragma comment (lib,"glaux.lib")
extern "C" int read_JPEG_file(const char *, char **, int *, int *, int *);


const GLfloat mat_ambient[][4] = {{0.33, 0.22, 0.03, 1.0},	// brass
								  {0.0, 0.0, 0.0},			// red plastic
								  {0.0215, 0.1745, 0.0215},	// emerald
								  {0.02, 0.02, 0.02},		// slate
								  {0.0, 0.0, 0.1745},		// azul
								  {0.02, 0.02, 0.02},		// preto
								  {0.1745, 0.1745, 0.1745}};// cinza

const GLfloat mat_diffuse[][4] = {{0.78, 0.57, 0.11, 1.0},		// brass
								  {0.5, 0.0, 0.0},				// red plastic
								  {0.07568, 0.61424, 0.07568},	// emerald
								  {0.78, 0.78, 0.78},			// slate
								  {0.0, 0.0,  0.61424},			// azul
								  {0.08, 0.08, 0.08},			// preto
								  {0.61424, 0.61424, 0.61424}};	// cinza

const GLfloat mat_specular[][4] = {{0.99, 0.91, 0.81, 1.0},			// brass
								   {0.7, 0.6, 0.6},					// red plastic
								   {0.633, 0.727811, 0.633},		// emerald
								   {0.14, 0.14, 0.14},				// slate
								   {0.0, 0.0, 0.727811},			// azul
								   {0.03, 0.03, 0.03},				// preto
								   {0.727811, 0.727811, 0.727811}};	// cinza

const GLfloat mat_shininess[] = {27.8,	// brass
								 32.0,	// red plastic
								 76.8,	// emerald
								 18.78,	// slate
								 30.0,	// azul
								 75.0,	// preto
								 60.0};	// cinza

enum tipo_material {brass, red_plastic, emerald, slate, azul, preto, cinza};

#ifdef __cplusplus
	inline tipo_material operator++(tipo_material &rs, int ) {
		return rs = (tipo_material)(rs + 1);
	}
#endif

typedef	GLfloat Vertice[3];
typedef	GLfloat Vector[4];


typedef struct Camera{
	GLfloat fov;
	GLfloat dir_lat;
	GLfloat dir_long;
	GLfloat dist;
	Vertice center;

}Camera;


typedef struct Estado{
	Camera		camera;
	int			xMouse,yMouse;
	GLboolean	light;
	GLboolean	apresentaNormais;
	GLint		lightViewer;
	GLint		eixoTranslaccao;
	GLfloat	eixo[3];
	GLboolean	paredes;
}Estado;

typedef struct Modelo {
	#ifdef __cplusplus
		tipo_material cor_cubo;
	#else
		enum tipo_material cor_cubo;
	#endif

	GLfloat g_pos_luz1[4];
	GLfloat g_pos_luz2[4];

	GLfloat escala;
	GLUquadric *quad;
}Modelo;

Estado estado;
Modelo modelo;
Visita visita;
GLuint texture[21];
ISoundEngine* engine = createIrrKlangDevice();

GLboolean TESTES;
string nome;
GLboolean RAIN;
GLboolean SNOW;
GLboolean HAIL;
int weather;


float slowdown = 2.0;
float velocity = 0.0;
float zoom = -40.0;
float pan = 0.0;
float tilt = 0.0;
float hailsize = 0.1;

int loop;
int fall;

//floor colors
float r = 0.0;
float g = 1.0;
float b = 0.0;
float ground_points[21][21][3];
float ground_colors[21][21][4];
float accum = -10.0;

typedef struct {
	// Life
	bool alive;	// is the particle alive?
	float life;	// particle lifespan
	float fade; // decay
				// color
	float red;
	float green;
	float blue;
	// Position/direction
	float xpos;
	float ypos;
	float zpos;
	// Velocity/Direction, only goes down in y dir
	float vel;
	// Gravity
	float gravity;
}particles;

// Paticle System
particles par_sys[MAX_PARTICLES];

void initEstado(){
	estado.camera.dir_lat=M_PI/4;
	estado.camera.dir_long=-M_PI/4;
	estado.camera.fov=60;
	estado.camera.dist=100;
	estado.eixo[0]=0;
	estado.eixo[1]=0;
	estado.eixo[2]=0;
	estado.camera.center[0]=0;
	estado.camera.center[1]=0;
	estado.camera.center[2]=0;
	estado.light=GL_FALSE;
	estado.paredes = GL_TRUE;
	estado.apresentaNormais=GL_FALSE;
	estado.lightViewer=1;
	TESTES = GL_FALSE;
	weather = 0;

}

void initParticles(int i) {
	par_sys[i].alive = true;
	par_sys[i].life = 1.0;
	par_sys[i].fade = float(rand() % 100) / 1000.0f + 0.003f;

	par_sys[i].xpos = (float)(rand() % -100) + -50;
	par_sys[i].ypos = 30.0;
	par_sys[i].zpos = (float)(rand() % -150) + -50;

	par_sys[i].red = 0.5;
	par_sys[i].green = 0.5;
	par_sys[i].blue = 1.0;

	par_sys[i].vel = velocity;
	par_sys[i].gravity = -5.0;//-0.8;



}

unsigned int split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
	unsigned int pos = txt.find(ch);
	unsigned int initialPos = 0;
	strs.clear();

	// Decompose statement
	while (pos != std::string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos + 1));
		initialPos = pos + 1;

		pos = txt.find(ch, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, min(pos, txt.size()) - initialPos + 1));

	return strs.size();
}

void initModelo(){
	modelo.escala=0.2;

	modelo.cor_cubo = brass;
	modelo.g_pos_luz1[0]=-5.0;
	modelo.g_pos_luz1[1]= 5.0;
	modelo.g_pos_luz1[2]= 5.0;
	modelo.g_pos_luz1[3]= 0.0;
	modelo.g_pos_luz2[0]= 5.0;
	modelo.g_pos_luz2[1]= -15.0;
	modelo.g_pos_luz2[2]= 5.0;
	modelo.g_pos_luz2[3]= 0.0;
}

void FreeTexture(GLuint texture) {
	glDeleteTextures(21, &texture);
}


void loadTexture(GLuint texture, const char* filename)
{
	JPGImage image;
	read_JPEG_file(filename, &image.data, &image.sizeX, &image.sizeY, &image.bpp);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image.sizeX, image.sizeY, GL_RGB, GL_UNSIGNED_BYTE, image.data);
}


void myInit()
{
	

	GLfloat LuzAmbiente[]={0.5,0.5,0.5, 0.0};

	glClearColor (0.0, 0.0, 0.0, 0.0);

	glEnable(GL_SMOOTH); /*enable smooth shading */
	glEnable(GL_LIGHTING); /* enable lighting */
	glEnable(GL_DEPTH_TEST); /* enable z buffer */
	glEnable(GL_NORMALIZE);

	glDepthFunc(GL_LESS);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LuzAmbiente); 
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, estado.lightViewer); 
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); 

	glGenTextures(21, texture);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	loadTexture(texture[0], NOME_TEXTURA_CHAO);
	loadTexture(texture[1], NOME_TEXTURA_PAREDE);
	loadTexture(texture[2], NOME_TEXTURE_CHAO_NO);

	loadTexture(texture[3], NOME_TEXTURA_SKYBOX_BK);
	loadTexture(texture[4], NOME_TEXTURA_SKYBOX_DN);
	loadTexture(texture[5], NOME_TEXTURA_SKYBOX_FT);
	loadTexture(texture[6], NOME_TEXTURA_SKYBOX_LF);
	loadTexture(texture[7], NOME_TEXTURA_SKYBOX_RT);
	loadTexture(texture[8], NOME_TEXTURA_SKYBOX_UP);

	loadTexture(texture[9], NOME_TEXTURA_SKYBOX_STORM_BK);
	loadTexture(texture[10], NOME_TEXTURA_SKYBOX_STORM_DN);
	loadTexture(texture[11], NOME_TEXTURA_SKYBOX_STORM_FT);
	loadTexture(texture[12], NOME_TEXTURA_SKYBOX_STORM_LF);
	loadTexture(texture[13], NOME_TEXTURA_SKYBOX_STORM_RT);
	loadTexture(texture[14], NOME_TEXTURA_SKYBOX_STORM_UP);

	loadTexture(texture[15], NOME_TEXTURA_SKYBOX_NIGHT_BK);
	loadTexture(texture[16], NOME_TEXTURA_SKYBOX_NIGHT_DN);
	loadTexture(texture[17], NOME_TEXTURA_SKYBOX_NIGHT_FT);
	loadTexture(texture[18], NOME_TEXTURA_SKYBOX_NIGHT_LF);
	loadTexture(texture[19], NOME_TEXTURA_SKYBOX_NIGHT_RT);
	loadTexture(texture[20], NOME_TEXTURA_SKYBOX_NIGHT_UP);

	int x, z;

	for (z = 0; z < 21; z++) {
		for (x = 0; x < 21; x++) {
			ground_points[x][z][0] = x - 10.0;
			ground_points[x][z][1] = accum;
			ground_points[x][z][2] = z - 10.0;

			ground_colors[z][x][0] = r; // red value
			ground_colors[z][x][1] = g; // green value
			ground_colors[z][x][2] = b; // blue value
			ground_colors[z][x][3] = 0.0; // acummulation factor
		}
	}

	// Initialize particles
	for (loop = 0; loop < MAX_PARTICLES; loop++) {
		initParticles(loop);
	}

	initModelo();
	initEstado();
	modelo.quad=gluNewQuadric();
	gluQuadricDrawStyle(modelo.quad, GLU_FILL);
	gluQuadricNormals(modelo.quad, GLU_OUTSIDE);

	//Chamar HTTP
	//leGrafo();
}

// For Hail
void drawHail() {
	float x, y, z;

	for (loop = 0; loop < MAX_PARTICLES; loop = loop + 2) {
		if (par_sys[loop].alive == true) {
			x = par_sys[loop].xpos;
			y = par_sys[loop].zpos + zoom;
			z = par_sys[loop].ypos;

			// Draw particles
			glColor3f(0.8, 0.8, 0.9);
			glBegin(GL_QUADS);
			// Front
			glVertex3f(x - hailsize, y - hailsize, z + hailsize); // lower left
			glVertex3f(x - hailsize, y + hailsize, z + hailsize); // upper left
			glVertex3f(x + hailsize, y + hailsize, z + hailsize); // upper right
			glVertex3f(x + hailsize, y - hailsize, z + hailsize); // lower left
																  //Left
			glVertex3f(x - hailsize, y - hailsize, z + hailsize);
			glVertex3f(x - hailsize, y - hailsize, z - hailsize);
			glVertex3f(x - hailsize, y + hailsize, z - hailsize);
			glVertex3f(x - hailsize, y + hailsize, z + hailsize);
			// Back
			glVertex3f(x - hailsize, y - hailsize, z - hailsize);
			glVertex3f(x - hailsize, y + hailsize, z - hailsize);
			glVertex3f(x + hailsize, y + hailsize, z - hailsize);
			glVertex3f(x + hailsize, y - hailsize, z - hailsize);
			//Right
			glVertex3f(x + hailsize, y + hailsize, z + hailsize);
			glVertex3f(x + hailsize, y + hailsize, z - hailsize);
			glVertex3f(x + hailsize, y - hailsize, z - hailsize);
			glVertex3f(x + hailsize, y - hailsize, z + hailsize);
			//Top 
			glVertex3f(x - hailsize, y + hailsize, z + hailsize);
			glVertex3f(x - hailsize, y + hailsize, z - hailsize);
			glVertex3f(x + hailsize, y + hailsize, z - hailsize);
			glVertex3f(x + hailsize, y + hailsize, z + hailsize);
			//Bottom 
			glVertex3f(x - hailsize, y - hailsize, z + hailsize);
			glVertex3f(x - hailsize, y - hailsize, z - hailsize);
			glVertex3f(x + hailsize, y - hailsize, z - hailsize);
			glVertex3f(x + hailsize, y - hailsize, z + hailsize);
			glEnd();

			// Update values
			//Move
			/*if (par_sys[loop].ypos <= -10) {
				par_sys[loop].vel = par_sys[loop].vel * -1.0;
			}*/
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000); // * 1000
			par_sys[loop].vel += par_sys[loop].gravity;

			// Decay
			par_sys[loop].life -= par_sys[loop].fade;

			//Revive 
			if (par_sys[loop].life < 0.0) {
				initParticles(loop);
			}
		}
	}
}

// For Snow
void drawSnow() {
	float x, y, z;
	for (loop = 0; loop < MAX_PARTICLES; loop = loop + 2) {
		if (par_sys[loop].alive == true) {
			x = par_sys[loop].xpos;
			y = par_sys[loop].zpos + zoom;
			z = par_sys[loop].ypos;

			// Draw particles
			glColor3f(1.0, 1.0, 1.0);
			glPushMatrix();
			glTranslatef(x, y, z);
			glutSolidSphere(0.2, 16, 16);
			glPopMatrix();

			// Update values
			//Move
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000);
			par_sys[loop].vel += par_sys[loop].gravity;
			// Decay
			par_sys[loop].life -= par_sys[loop].fade;

			if (par_sys[loop].ypos <= -10) {
				int zi = z - zoom + 10;
				int xi = x + 10;
				ground_colors[zi][xi][0] = 1.0;
				ground_colors[zi][xi][2] = 1.0;
				ground_colors[zi][xi][3] += 1.0;
				if (ground_colors[zi][xi][3] > 1.0) {
					ground_points[xi][zi][1] += 0.1;
				}
				par_sys[loop].life = -1.0;
			}

			//Revive 
			if (par_sys[loop].life < 0.0) {
				initParticles(loop);
			}
		}
	}
}

void drawRain() {
	float x, y, z;
	for (loop = 0; loop < MAX_PARTICLES; loop = loop + 2) {
		if (par_sys[loop].alive == true) {
			x = par_sys[loop].xpos;
			y = par_sys[loop].ypos;
			z = par_sys[loop].zpos+zoom;

			// Draw particles
			glColor3f(0, 0, 1.0);
			glBegin(GL_LINES);
			glVertex3f(x, z, y);
			glVertex3f(x, z, y + 0.5);
			glEnd();

			// Update values
			//Move
			// Adjust slowdown for speed!
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000);
			par_sys[loop].vel += par_sys[loop].gravity;
			// Decay
			par_sys[loop].life -= par_sys[loop].fade;

			if (par_sys[loop].ypos <= -10) {
				par_sys[loop].life = -1.0;
			}
			//Revive 
			if (par_sys[loop].life < 0.0) {
				initParticles(loop);
			}
		}
	}
}

void imprime_ajuda(void)
{

  printf("\n\nDesenho de um labirinto a partir de um grafo\n");
  printf("h,H - Ajuda \n");
  printf("i,I - Reset dos Valores \n");
  printf("******* Diversos ******* \n");
  printf("l,L - Alterna o calculo luz entre Z e eye (GL_LIGHT_MODEL_LOCAL_VIEWER)\n");
  printf("k,K - Alerna luz de camera com luz global \n");
  printf("s,S - PolygonMode Fill \n");
  printf("w,W - PolygonMode Wireframe \n");
  printf("p,P - PolygonMode Point \n");
  printf("c,C - Liga/Desliga Cull Face \n");
  printf("n,N - Liga/Desliga apresentação das normais \n");
  printf("******* grafos ******* \n");
  printf("F1  - Grava grafo do ficheiro \n");
  printf("F2  - Lê grafo para ficheiro \n");
  printf("F6  - Cria novo grafo\n");
  printf("******* Camera ******* \n");
  printf("Botão esquerdo - Arrastar os eixos (centro da camera)\n");
  printf("Botão direito  - Rodar camera\n");
  printf("Botão direito com CTRL - Zoom-in/out\n");
  printf("PAGE_UP, PAGE_DOWN - Altera distância da camara \n");
  printf("ESC - Sair\n");
}


void material(enum tipo_material mat)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient[mat]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse[mat]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular[mat]);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess[mat]);
}

const GLfloat red_light[] = {1.0, 0.0, 0.0, 1.0};
const GLfloat green_light[] = {0.0, 1.0, 0.0, 1.0};
const GLfloat blue_light[] = {0.0, 0.0, 1.0, 1.0};
const GLfloat white_light[] = {1.0, 1.0, 1.0, 1.0};


void putLights(GLfloat* diffuse)
{
	const GLfloat white_amb[] = {0.2, 0.2, 0.2, 1.0};

	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white_amb);
	glLightfv(GL_LIGHT0, GL_POSITION, modelo.g_pos_luz1);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT1, GL_AMBIENT, white_amb);
	glLightfv(GL_LIGHT1, GL_POSITION, modelo.g_pos_luz2);

	/* desenhar luz */
	//material(red_plastic);
	//glPushMatrix();
	//	glTranslatef(modelo.g_pos_luz1[0], modelo.g_pos_luz1[1], modelo.g_pos_luz1[2]);
	//	glDisable(GL_LIGHTING);
	//	glColor3f(1.0, 1.0, 1.0);
	//	glutSolidCube(0.1);
	//	glEnable(GL_LIGHTING);
	//glPopMatrix();
	//glPushMatrix();
	//	glTranslatef(modelo.g_pos_luz2[0], modelo.g_pos_luz2[1], modelo.g_pos_luz2[2]);
	//	glDisable(GL_LIGHTING);
	//	glColor3f(1.0, 1.0, 1.0);
	//	glutSolidCube(0.1);
	//	glEnable(GL_LIGHTING);
	//glPopMatrix();

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
}

void desenhaSolo(){
#define STEP 10
	glBegin(GL_QUADS);
		glNormal3f(0,0,1);
		for(int i=-300;i<300;i+=STEP)
			for(int j=-300;j<300;j+=STEP){
				glVertex2f(i,j);
				glVertex2f(i+STEP,j);
				glVertex2f(i+STEP,j+STEP);
				glVertex2f(i,j+STEP);
			}
	glEnd();
}

void CrossProduct (GLdouble v1[], GLdouble v2[], GLdouble cross[])
{
	cross[0] = v1[1]*v2[2] - v1[2]*v2[1];
	cross[1] = v1[2]*v2[0] - v1[0]*v2[2];
	cross[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

GLdouble VectorNormalize (GLdouble v[])
{
	int		i;
	GLdouble	length;

	if ( fabs(v[1] - 0.000215956) < 0.0001)
		i=1;

	length = 0;
	for (i=0 ; i< 3 ; i++)
		length += v[i]*v[i];
	length = sqrt (length);
	if (length == 0)
		return 0;
		
	for (i=0 ; i< 3 ; i++)
		v[i] /= length;	

	return length;
}

void desenhaNormal(GLdouble x, GLdouble y, GLdouble z, GLdouble normal[], tipo_material mat){

	switch (mat){
		case red_plastic:
				glColor3f(1,0,0);
			break;
		case azul:
				glColor3f(0,0,1);
			break;
		case emerald:
				glColor3f(0,1,0);
			break;
		default:
				glColor3f(1,1,0);
	}
	glDisable(GL_LIGHTING);
	glPushMatrix();
		glTranslated(x,y,z);
		glScaled(0.4,0.4,0.4);
		glBegin(GL_LINES);
			glVertex3d(0,0,0);
			glVertex3dv(normal);
		glEnd();
		glPopMatrix();
	glEnable(GL_LIGHTING);
}

void desenhaParede(GLfloat xi, GLfloat yi, GLfloat zi, GLfloat xf, GLfloat yf, GLfloat zf){

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);

	GLdouble v1[3],v2[3],cross[3];
	GLdouble length;
	v1[0]=xf-xi;
	v1[1]=yf-yi;
	v1[2]=0;
	v2[0]=0;
	v2[1]=0;
	v2[2]=1;
	CrossProduct(v1,v2,cross);
	//printf("cross x=%lf y=%lf z=%lf",cross[0],cross[1],cross[2]);
	length=VectorNormalize(cross);
	//printf("Normal x=%lf y=%lf z=%lf length=%lf\n",cross[0],cross[1],cross[2]);

	material(emerald);
	glBegin(GL_QUADS);
		glNormal3dv(cross);
		glTexCoord2f(1, 1);
		glVertex3f(xi,yi,zi);
		glTexCoord2f(0, 1);
		glVertex3f(xf,yf,zf+0);
		glTexCoord2f(0, 0);
		glVertex3f(xf,yf,zf+1);
		glTexCoord2f(1, 0);
		glVertex3f(xi,yi,zi+1);
	glEnd();

	if(estado.apresentaNormais) {
		desenhaNormal(xi,yi,zi,cross,emerald);
		desenhaNormal(xf,yf,zf,cross,emerald);
		desenhaNormal(xf,yf,zf+1,cross,emerald);
		desenhaNormal(xi,yi,zi+1,cross,emerald);
	}

	glDisable(GL_TEXTURE_2D);

}

void printtext(int x, int y, string string)
{
	//(x,y) is from the bottom left of the window
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 500, 0, 500, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	glRasterPos2i(x, y);
	for (unsigned int i = 0; i<string.size(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void desenhaChao(GLfloat xi, GLfloat yi, GLfloat zi, GLfloat xf, GLfloat yf, GLfloat zf, int orient){

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	GLdouble v1[3],v2[3],cross[3];
	GLdouble length;
	v1[0]=xf-xi;
	v1[1]=0;
	v2[0]=0;
	v2[1]=yf-yi;

	switch(orient) {
		case NORTE_SUL :
				v1[2]=0;
				v2[2]=zf-zi;
				CrossProduct(v1,v2,cross);
				//printf("cross x=%lf y=%lf z=%lf",cross[0],cross[1],cross[2]);
				length=VectorNormalize(cross);
				//printf("Normal x=%lf y=%lf z=%lf length=%lf\n",cross[0],cross[1],cross[2]);

				material(red_plastic);
				glBegin(GL_QUADS);
					glNormal3dv(cross);
					glTexCoord2f(0, 0.1);
					glVertex3f(xi,yi,zi);
					glTexCoord2f(0.1, 0);
					glVertex3f(xf,yi,zi);
					glTexCoord2f(0.1, 1);
					glVertex3f(xf,yf,zf);
					glTexCoord2f(0, 1);
					glVertex3f(xi,yf,zf);
				glEnd();
				if(estado.apresentaNormais) {
					desenhaNormal(xi,yi,zi,cross,red_plastic);
					desenhaNormal(xf,yi,zi,cross,red_plastic);
					desenhaNormal(xf,yf,zf,cross,red_plastic);
					desenhaNormal(xi,yi,zf,cross,red_plastic);
				}
			break;
		case ESTE_OESTE :
				v1[2]=zf-zi;
				v2[2]=0;
				CrossProduct(v1,v2,cross);
				//printf("cross x=%lf y=%lf z=%lf",cross[0],cross[1],cross[2]);
				length=VectorNormalize(cross);
				//printf("Normal x=%lf y=%lf z=%lf length=%lf\n",cross[0],cross[1],cross[2]);
				material(red_plastic);
				glBegin(GL_QUADS);
					glNormal3dv(cross);
					glTexCoord2f(0, 0.1);
					glVertex3f(xi,yi,zi);
					glTexCoord2f(1, 0);
					glVertex3f(xf,yi,zf);
					glTexCoord2f(1, 1);
					glVertex3f(xf,yf,zf);
					glTexCoord2f(0, 1);
					glVertex3f(xi,yf,zi);
				glEnd();
				if(estado.apresentaNormais) {
					desenhaNormal(xi,yi,zi,cross,red_plastic);
					desenhaNormal(xf,yi,zf,cross,red_plastic);
					desenhaNormal(xf,yf,zf,cross,red_plastic);
					desenhaNormal(xi,yi,zi,cross,red_plastic);
				}
			break;
		default:
				glBindTexture(GL_TEXTURE_2D, texture[2]);
				cross[0]=0;
				cross[1]=0;
				cross[2]=1;
				material(azul);
				glBegin(GL_QUADS);
					glNormal3f(0,0,1);
					glTexCoord2f(0, 1);
					glVertex3f(xi,yi,zi);
					glTexCoord2f(1, 1);
					glVertex3f(xf,yi,zf);
					glTexCoord2f(1, 0);
					glVertex3f(xf,yf,zf);
					glTexCoord2f(0, 0);
					glVertex3f(xi,yf,zi);
				glEnd();
				if(estado.apresentaNormais) {
					desenhaNormal(xi,yi,zi,cross,azul);
					desenhaNormal(xf,yi,zf,cross,azul);
					desenhaNormal(xf,yf,zf,cross,azul);
					desenhaNormal(xi,yi,zi,cross,azul);
				}
			break;
	}

	glDisable(GL_TEXTURE_2D);
}

void desenhaNo(int no){
	GLboolean norte,sul,este,oeste;
	GLfloat larguraNorte,larguraSul,larguraEste,larguraOeste;
	Arco arco=arcos[0];
	No *noi=&nos[no],*nof;
	norte=sul=este=oeste=GL_TRUE;
	desenhaChao(nos[no].x-0.5*noi->largura,nos[no].y-0.5*noi->largura,nos[no].z,nos[no].x+0.5*noi->largura,nos[no].y+0.5*noi->largura,nos[no].z,PLANO);
	for(int i=0;i<numArcos; arco=arcos[++i]){
		if(arco.noi==no)
			nof=&nos[arco.nof];
		else 
			if(arco.nof==no)
				nof=&nos[arco.noi];
			else
				continue;
		if(noi->x==nof->x)
			if(noi->y<nof->y){
				norte=GL_FALSE;
				larguraNorte=arco.largura;
			}
			else{
				sul=GL_FALSE;
				larguraSul=arco.largura;
			}
		else 
			if(noi->y==nof->y)
				if(noi->x<nof->x){
					oeste=GL_FALSE;
					larguraOeste=arco.largura;
				}
				else{
					este=GL_FALSE;
					larguraEste=arco.largura;
				}
			else
				cout << "Arco dioagonal: " << arco.noi << " " << arco.nof << endl;
		if (norte && sul && este && oeste)
			return;
	}	
	if (estado.paredes) {
		if (norte)
			desenhaParede(nos[no].x - 0.5*noi->largura, nos[no].y + 0.5*noi->largura, nos[no].z, nos[no].x + 0.5*noi->largura, nos[no].y + 0.5*noi->largura, nos[no].z);
		else
			if (larguraNorte < noi->largura) {
				desenhaParede(nos[no].x - 0.5*noi->largura, nos[no].y + 0.5*noi->largura, nos[no].z, nos[no].x - 0.5*larguraNorte, nos[no].y + 0.5*noi->largura, nos[no].z);
				desenhaParede(nos[no].x + 0.5*larguraNorte, nos[no].y + 0.5*noi->largura, nos[no].z, nos[no].x + 0.5*noi->largura, nos[no].y + 0.5*noi->largura, nos[no].z);
			}
		if (sul)
			desenhaParede(nos[no].x + 0.5*noi->largura, nos[no].y - 0.5*noi->largura, nos[no].z, nos[no].x - 0.5*noi->largura, nos[no].y - 0.5*noi->largura, nos[no].z);
		else
			if (larguraSul < noi->largura) {
				desenhaParede(nos[no].x + 0.5*noi->largura, nos[no].y - 0.5*noi->largura, nos[no].z, nos[no].x + 0.5*larguraSul, nos[no].y - 0.5*noi->largura, nos[no].z);
				desenhaParede(nos[no].x - 0.5*larguraSul, nos[no].y - 0.5*noi->largura, nos[no].z, nos[no].x - 0.5*noi->largura, nos[no].y - 0.5*noi->largura, nos[no].z);
			}
		if (este)
			desenhaParede(nos[no].x - 0.5*noi->largura, nos[no].y - 0.5*noi->largura, nos[no].z, nos[no].x - 0.5*noi->largura, nos[no].y + 0.5*noi->largura, nos[no].z);
		else
			if (larguraEste < noi->largura) {
				desenhaParede(nos[no].x - 0.5*noi->largura, nos[no].y - 0.5*noi->largura, nos[no].z, nos[no].x - 0.5*noi->largura, nos[no].y - 0.5*larguraEste, nos[no].z);
				desenhaParede(nos[no].x - 0.5*noi->largura, nos[no].y + 0.5*larguraEste, nos[no].z, nos[no].x - 0.5*noi->largura, nos[no].y + 0.5*noi->largura, nos[no].z);
			}
		if (oeste)
			desenhaParede(nos[no].x + 0.5*noi->largura, nos[no].y + 0.5*noi->largura, nos[no].z, nos[no].x + 0.5*noi->largura, nos[no].y - 0.5*noi->largura, nos[no].z);
		else
			if (larguraOeste < noi->largura) {
				desenhaParede(nos[no].x + 0.5*noi->largura, nos[no].y + 0.5*noi->largura, nos[no].z, nos[no].x + 0.5*noi->largura, nos[no].y + 0.5*larguraOeste, nos[no].z);
				desenhaParede(nos[no].x + 0.5*noi->largura, nos[no].y - 0.5*larguraOeste, nos[no].z, nos[no].x + 0.5*noi->largura, nos[no].y - 0.5*noi->largura, nos[no].z);
			}
	}
}


void desenhaArco(Arco arco){
	No *noi,*nof;

	if(nos[arco.noi].x==nos[arco.nof].x){
		// arco vertical
		if(nos[arco.noi].y<nos[arco.nof].y){
			noi=&nos[arco.noi];
			nof=&nos[arco.nof];
		}else{
			nof=&nos[arco.noi];
			noi=&nos[arco.nof];
		}

		desenhaChao(noi->x-0.5*arco.largura,noi->y+0.5*noi->largura,noi->z,nof->x+0.5*arco.largura,nof->y-0.5*nof->largura,nof->z, NORTE_SUL);
		if (estado.paredes) {
			desenhaParede(noi->x - 0.5*arco.largura, noi->y + 0.5*noi->largura, noi->z, nof->x - 0.5*arco.largura, nof->y - 0.5*nof->largura, nof->z);
			desenhaParede(nof->x + 0.5*arco.largura, nof->y - 0.5*nof->largura, nof->z, noi->x + 0.5*arco.largura, noi->y + 0.5*noi->largura, noi->z);
		}
	}else{
		if(nos[arco.noi].y==nos[arco.nof].y){
			//arco horizontal
			if(nos[arco.noi].x<nos[arco.nof].x){
				noi=&nos[arco.noi];
				nof=&nos[arco.nof];
			}else{
				nof=&nos[arco.noi];
				noi=&nos[arco.nof];
			}
			desenhaChao(noi->x+0.5*noi->largura,noi->y-0.5*arco.largura,noi->z,nof->x-0.5*nof->largura,nof->y+0.5*arco.largura,nof->z, ESTE_OESTE);
			if (estado.paredes) {
				desenhaParede(noi->x + 0.5*noi->largura, noi->y + 0.5*arco.largura, noi->z, nof->x - 0.5*nof->largura, nof->y + 0.5*arco.largura, nof->z);
				desenhaParede(nof->x - 0.5*nof->largura, nof->y - 0.5*arco.largura, nof->z, noi->x + 0.5*noi->largura, noi->y - 0.5*arco.largura, noi->z);
			}
		}
		else{
			cout << "arco diagonal... não será desenhado";
		}
	}
}

void desenhaLabirinto(){
	glPushMatrix();
		glTranslatef(0,0,0.05);
		glScalef(5,5,5);
		material(red_plastic);
		for(int i=0; i<numNos; i++){
			glPushMatrix();
				material(emerald);
				glTranslatef(nos[i].x,nos[i].y,nos[i].z+0.25);
				glutSolidCube(0.5);
			glPopMatrix();
			desenhaNo(i);
		}
		material(emerald);
		for(int i=0; i<numArcos; i++)
			desenhaArco(arcos[i]);
	glPopMatrix();
}

void desenhaEixo(){
	gluCylinder(modelo.quad,0.5,0.5,20,16,15);
	glPushMatrix();
		glTranslatef(0,0,20);
		glPushMatrix();
			glRotatef(180,0,1,0);
			gluDisk(modelo.quad,0.5,2,16,6);
		glPopMatrix();
		gluCylinder(modelo.quad,2,0,5,16,15);
	glPopMatrix();
}



#define EIXO_X		1
#define EIXO_Y		2
#define EIXO_Z		3

void desenhaPlanoDrag(int eixo){
	glPushMatrix();
		glTranslated(estado.eixo[0],estado.eixo[1],estado.eixo[2]);
		switch (eixo) {
			case EIXO_Y :
					if(abs(estado.camera.dir_lat)<M_PI/4)
						glRotatef(-90,0,0,1);
					else
						glRotatef(90,1,0,0);
					material(red_plastic);
				break;
			case EIXO_X :
					if(abs(estado.camera.dir_lat)>M_PI/6)
						glRotatef(90,1,0,0);
					material(azul);
				break;
			case EIXO_Z :
					if(abs(cos(estado.camera.dir_long))>0.5)
						glRotatef(90,0,0,1);

					material(emerald);
				break;
		}
		glBegin(GL_QUADS);
			glNormal3f(0,1,0);
			glVertex3f(-100,0,-100);
			glVertex3f(100,0,-100);
			glVertex3f(100,0,100);
			glVertex3f(-100,0,100);
		glEnd();
	glPopMatrix();
}

void skybox() {


	// Store the current matrix
	glPushMatrix();
	
	// Reset and transform the matrix.
	//glLoadIdentity();
	/*gluLookAt(
		0, 0, 0,
		estado.camera.center[0], estado.camera.center[1], estado.camera.center[2],
		0, 1, 0);

	// Enable/Disable features
	/*glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);*/
	glEnable(GL_TEXTURE_2D);
	// Just in case we set all vertices to white.
	glColor4f(1, 1, 1, 1);

	//tamanho da skybox
	float tamanhoSky = 100;
	int i = weather;
	// Render the front quad

	glBindTexture(GL_TEXTURE_2D, texture[4 + (i * 6)]);
	
	
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); 
	glVertex3f(tamanhoSky, -tamanhoSky, -tamanhoSky);
	glTexCoord2f(1, 0); 
	glVertex3f(-tamanhoSky, -tamanhoSky, -tamanhoSky);
	glTexCoord2f(1, 1); 
	glVertex3f(-tamanhoSky, tamanhoSky, -tamanhoSky);
	glTexCoord2f(0, 1); 
	glVertex3f(tamanhoSky, tamanhoSky, -tamanhoSky);
	glEnd();

	// Render the left quad
	
		glBindTexture(GL_TEXTURE_2D, texture[3 + (i * 6)]);
	
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0); glVertex3f(tamanhoSky, -tamanhoSky, tamanhoSky);
	glTexCoord2f(1, 1); glVertex3f(tamanhoSky, -tamanhoSky, -tamanhoSky);
	glTexCoord2f(0, 1); glVertex3f(tamanhoSky, tamanhoSky, -tamanhoSky);
	glTexCoord2f(0, 0); glVertex3f(tamanhoSky, tamanhoSky, tamanhoSky);
	glEnd();

	// Render the back quad
	
		glBindTexture(GL_TEXTURE_2D, texture[8 + (i * 6)]);
	
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(-tamanhoSky, -tamanhoSky, tamanhoSky);
	glTexCoord2f(0, 1); glVertex3f(tamanhoSky, -tamanhoSky, tamanhoSky);
	glTexCoord2f(0, 0); glVertex3f(tamanhoSky, tamanhoSky, tamanhoSky);
	glTexCoord2f(1, 0); glVertex3f(-tamanhoSky, tamanhoSky, tamanhoSky);

	glEnd();

	// Render the right quad
	
		glBindTexture(GL_TEXTURE_2D, texture[5 + (i * 6)]);
	
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(-tamanhoSky, -tamanhoSky, -tamanhoSky);
	glTexCoord2f(0, 0); glVertex3f(-tamanhoSky, -tamanhoSky, tamanhoSky );
	glTexCoord2f(1, 0); glVertex3f(-tamanhoSky, tamanhoSky, tamanhoSky);
	glTexCoord2f(1, 1); glVertex3f(-tamanhoSky, tamanhoSky, -tamanhoSky);
	glEnd();

	// Render the top quad
	
		glBindTexture(GL_TEXTURE_2D, texture[6 + (i * 6)]);
	
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(-tamanhoSky, tamanhoSky, -tamanhoSky);
	glTexCoord2f(0, 0); glVertex3f(-tamanhoSky, tamanhoSky, tamanhoSky);
	glTexCoord2f(1, 0); glVertex3f(tamanhoSky, tamanhoSky, tamanhoSky);
	glTexCoord2f(1, 1); glVertex3f(tamanhoSky, tamanhoSky, -tamanhoSky);
	glEnd();

	// Render the bottom quad

		glBindTexture(GL_TEXTURE_2D, texture[7 + (i * 6)]);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(-tamanhoSky, -tamanhoSky, -tamanhoSky);
	glTexCoord2f(0, 0); glVertex3f(-tamanhoSky, -tamanhoSky, tamanhoSky);
	glTexCoord2f(1, 0); glVertex3f(tamanhoSky, -tamanhoSky, tamanhoSky);
	glTexCoord2f(1, 1); glVertex3f(tamanhoSky, -tamanhoSky, -tamanhoSky);
	glEnd();

	// Restore enable bits and matrix
	//glPopAttrib();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);


}

void desenhaEixos(){

	glPushMatrix();
		glTranslated(estado.eixo[0],estado.eixo[1],estado.eixo[2]);
		material(emerald);
		glPushName(EIXO_Z);
			desenhaEixo();
		glPopName();
		glPushName(EIXO_Y);
			glPushMatrix();
				glRotatef(-90,1,0,0);
				material(red_plastic);
				desenhaEixo();
			glPopMatrix();
		glPopName();
		glPushName(EIXO_X);
			glPushMatrix();
				glRotatef(90,0,1,0);
				material(azul);
				desenhaEixo();
			glPopMatrix();
		glPopName();
	glPopMatrix();
}

void setCamera(){
	Vertice eye;
	eye[0]=estado.camera.center[0]+estado.camera.dist*cos(estado.camera.dir_long)*cos(estado.camera.dir_lat);
	eye[1]=estado.camera.center[1]+estado.camera.dist*sin(estado.camera.dir_long)*cos(estado.camera.dir_lat);
	eye[2]=estado.camera.center[2]+estado.camera.dist*sin(estado.camera.dir_lat);

	if(estado.light){
		gluLookAt(eye[0],eye[1],eye[2],estado.camera.center[0],estado.camera.center[1],estado.camera.center[2],0,0,1);
		putLights((GLfloat*)white_light);
	}else{
		putLights((GLfloat*)white_light);
		gluLookAt(eye[0],eye[1],eye[2],estado.camera.center[0],estado.camera.center[1],estado.camera.center[2],0,0,1);
	}
}

void display(void)
{


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	setCamera();

	material(cinza);
	desenhaSolo();

	material(emerald);
	printtext(50,50,"Login : "+nome);
	
	desenhaEixos();
	
	desenhaLabirinto();
 
	if (TESTES) {
		
	
	}
	if (RAIN) {
		material(azul);
		drawRain();
	}
	else if(SNOW) {
		material(cinza);
		drawSnow();
	}
	else if(HAIL){
		material(azul);
		drawHail();
	}

	if(estado.eixoTranslaccao) {
		// desenha plano de translacção
		cout << "Translate... " << estado.eixoTranslaccao << endl; 
		desenhaPlanoDrag(estado.eixoTranslaccao);

	}
	vector<string> vecHoras;
	split(visita.horaInicio,vecHoras,':');
	int hora = atoi(vecHoras.at(0).c_str());
	
	if(hora>14 && hora<18) {
		weather = 1;
	}
	else if (hora>8 && hora<14) {
		weather = 0;
	}
	else {
		weather = 2;
	}
	skybox();


	glFlush();
	glutSwapBuffers();

}



void keyboard(unsigned char key, int x, int y)
{

    switch (key)
    {
		case 27 :
				exit(0);
			break;
		case 'h':
		case 'H':
				imprime_ajuda();
			break;
		case 'l':
		case 'L':
				if(estado.lightViewer)
					estado.lightViewer=0;
				else
					estado.lightViewer=1;
				glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, estado.lightViewer);
				glutPostRedisplay();
			break;
		case 'k':
		case 'K':
				estado.light=!estado.light;
				glutPostRedisplay();
			break;
		case 'w':
		case 'W':
				glDisable(GL_LIGHTING);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glutPostRedisplay();
			break;
		case 'p':
		case 'P':
				glDisable(GL_LIGHTING);
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				glutPostRedisplay();
			break;
		case 's':
		case 'S':
				glEnable(GL_LIGHTING);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glutPostRedisplay();
			break;
		case 'c':
		case 'C':
				if(glIsEnabled(GL_CULL_FACE))
					glDisable(GL_CULL_FACE);
				else
					glEnable(GL_CULL_FACE);
				glutPostRedisplay();
			break;    
		case 'n':
		case 'N':
				estado.apresentaNormais=!estado.apresentaNormais;
				glutPostRedisplay();
			break;    		
		case 'i':
		case 'I':
				initEstado();
				initModelo();
				glutPostRedisplay();
			break;   
		case 'q':
		case 'Q':
				printf("Insira Nome de Utilizador:");
				cin >> nome;
				glutPostRedisplay();
				break;
		case 'r':
		case 'R':
				printf("Reloading...");
				glutPostRedisplay();
				break;
		case 'y':
		case 'Y':
			printf("Rain...");
			if (!RAIN && !HAIL && !SNOW) {
				RAIN = !RAIN;
			}
			else {
				RAIN = !RAIN;
				if (SNOW) {
					SNOW = !SNOW;
				}
				else if (HAIL) {
					HAIL = !HAIL;
				}
			}
			glutPostRedisplay();
			break;
		case 'x':
		case 'X':
			printf("Hail...");
			if (!RAIN && !HAIL && !SNOW) {
				HAIL = !HAIL;
			}
			else {
				HAIL = !HAIL;
				if (SNOW) {
					SNOW = !SNOW;
				}
				else if (RAIN) {
					RAIN = !RAIN;
				}
			}
			glutPostRedisplay();
			break;
		case 'z':
		case 'Z':
			printf("Snow...");
			if (!RAIN && !HAIL && !SNOW) {
				SNOW = !SNOW;
			}
			else {
				SNOW = !SNOW;
				if (RAIN) {
					RAIN = !RAIN;
				}
				else if (HAIL) {
					HAIL = !HAIL;
				}
			}
			glutPostRedisplay();
			break;
		case 'b':
		case 'B':
				estado.paredes = !estado.paredes;
				glutPostRedisplay();
				break;
		case 'v':
		case 'V':
			if (weather == 2) {
				weather = 0;
			}
			else {
				weather++;
			}
			
			glutPostRedisplay();
			break;
		case 'u':
		case 'U':
				if (TESTES) {
					TESTES = !TESTES;
				}
				else {
					TESTES = !TESTES;
				}
				
				glutPostRedisplay();
				break;

	}
}

void Special(int key, int x, int y){

	switch(key){
		case GLUT_KEY_F1 :
				gravaGrafo();
			break;
		case GLUT_KEY_F2 :
				//Chamar HTTP
				leGrafo();
				glutPostRedisplay();
			break;	

		case GLUT_KEY_F6 :
				numNos=numArcos=0;
				addNo(criaNo( 0, 10,0));  // 0
				addNo(criaNo( 0,  5,0));  // 1
				addNo(criaNo(-5,  5,0));  // 2
				addNo(criaNo( 5,  5,0));  // 3
				addNo(criaNo(-5,  0,0));  // 4
				addNo(criaNo( 5,  0,0));  // 5
				addNo(criaNo(-5, -5,0));  // 6
				addArco(criaArco(0,1,1,1));  // 0 - 1
				addArco(criaArco(1,2,1,1));  // 1 - 2
				addArco(criaArco(1,3,1,1));  // 1 - 3
				addArco(criaArco(2,4,1,1));  // 2 - 4
				addArco(criaArco(3,5,1,1));  // 3 - 5
				addArco(criaArco(4,5,1,1));  // 4 - 5
				addArco(criaArco(4,6,1,1));  // 4 - 6
				glutPostRedisplay();
			break;	
		case GLUT_KEY_UP:
				estado.camera.dist-=1;
				glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:
				estado.camera.dist+=1;
				glutPostRedisplay();
			break;	}
}


void setProjection(int x, int y, GLboolean picking){
    glLoadIdentity();
	if (picking) { // se está no modo picking, lê viewport e define zona de picking
		GLint vport[4];
		glGetIntegerv(GL_VIEWPORT, vport);
		gluPickMatrix(x, glutGet(GLUT_WINDOW_HEIGHT)  - y, 4, 4, vport); // Inverte o y do rato para corresponder à jana
	}
	    
	gluPerspective(estado.camera.fov,(GLfloat)glutGet(GLUT_WINDOW_WIDTH) /glutGet(GLUT_WINDOW_HEIGHT) ,1,500);

}

void myReshape(int w, int h){	
	glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
	setProjection(0,0,GL_FALSE);
	glMatrixMode(GL_MODELVIEW);
}


void motionRotate(int x, int y){
#define DRAG_SCALE	0.01
	double lim=M_PI/2-0.1;
	estado.camera.dir_long+=(estado.xMouse-x)*DRAG_SCALE;
	estado.camera.dir_lat-=(estado.yMouse-y)*DRAG_SCALE*0.5;
	if(estado.camera.dir_lat>lim)
		estado.camera.dir_lat=lim;
	else 
		if(estado.camera.dir_lat<-lim)
			estado.camera.dir_lat=-lim;
	estado.xMouse=x;
	estado.yMouse=y;
	glutPostRedisplay();
}

void motionZoom(int x, int y){
#define ZOOM_SCALE	0.5
	estado.camera.dist-=(estado.yMouse-y)*ZOOM_SCALE;
	if(estado.camera.dist<5)
		estado.camera.dist=5;
	else 
		if(estado.camera.dist>200)
			estado.camera.dist=200;
	estado.yMouse=y;
	glutPostRedisplay();
}

void motionDrag(int x, int y){
	GLuint buffer[100];
	GLint vp[4];
	GLdouble proj[16], mv[16];
	int n;
	GLdouble newx, newy, newz;

	glSelectBuffer(100, buffer);
	glRenderMode(GL_SELECT);
	glInitNames();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix(); // guarda a projecção
		glLoadIdentity();
		setProjection(x,y,GL_TRUE);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	setCamera();
	desenhaPlanoDrag(estado.eixoTranslaccao);
	
	n = glRenderMode(GL_RENDER);
	if (n > 0) {
		glGetIntegerv(GL_VIEWPORT, vp);
		glGetDoublev(GL_PROJECTION_MATRIX, proj);
		glGetDoublev(GL_MODELVIEW_MATRIX, mv);
		gluUnProject(x, glutGet(GLUT_WINDOW_HEIGHT) - y, (double) buffer[2] / UINT_MAX, mv, proj, vp, &newx, &newy, &newz);
		printf("Novo x:%lf, y:%lf, z:%lf\n\n", newx, newy, newz);
		switch (estado.eixoTranslaccao) {
			case EIXO_X :
					estado.eixo[0]=newx;
					//estado.eixo[1]=newy;
				break;
			case EIXO_Y :
					estado.eixo[1]=newy;
					//estado.eixo[2]=newz;
				break;
			case EIXO_Z :
					//estado.eixo[0]=newx;
					estado.eixo[2]=newz;
				break;		
		}
		glutPostRedisplay();
	}


	glMatrixMode(GL_PROJECTION); //repõe matriz projecção
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

int picking(int x, int y){
	int i, n, objid=0;
	double zmin = 10.0;
	GLuint buffer[100], *ptr;

	glSelectBuffer(100, buffer);
	glRenderMode(GL_SELECT);
	glInitNames();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix(); // guarda a projecção
		glLoadIdentity();
		setProjection(x,y,GL_TRUE);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	setCamera();
	desenhaEixos();
	
	n = glRenderMode(GL_RENDER);
	if (n > 0)
	{
		ptr = buffer;
		for (i = 0; i < n; i++)
		{
			if (zmin > (double) ptr[1] / UINT_MAX) {
				zmin = (double) ptr[1] / UINT_MAX;
				objid = ptr[3];
			}
			ptr += 3 + ptr[0]; // ptr[0] contem o número de nomes (normalmente 1); 3 corresponde a numnomes, zmin e zmax
		}
	}


	glMatrixMode(GL_PROJECTION); //repõe matriz projecção
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	return objid;
}
void mouse(int btn, int state, int x, int y){
	switch(btn) {
		case GLUT_RIGHT_BUTTON :
					if(state == GLUT_DOWN){
						estado.xMouse=x;
						estado.yMouse=y;
						if(glutGetModifiers() & GLUT_ACTIVE_CTRL)
							glutMotionFunc(motionZoom);
						else
							glutMotionFunc(motionRotate);
						cout << "Left down\n";
					}
					else{
						glutMotionFunc(NULL);
						cout << "Left up\n";
					}
				break;
		case GLUT_LEFT_BUTTON :
					if(state == GLUT_DOWN){
						estado.eixoTranslaccao=picking(x,y);
						if(estado.eixoTranslaccao)
							glutMotionFunc(motionDrag);
						cout << "Right down - objecto:" << estado.eixoTranslaccao << endl;
					}
					else{
						if(estado.eixoTranslaccao!=0) {
							estado.camera.center[0]=estado.eixo[0];
							estado.camera.center[1]=estado.eixo[1];
							estado.camera.center[2]=estado.eixo[2];
							glutMotionFunc(NULL);
							estado.eixoTranslaccao=0;
							glutPostRedisplay();
						}
						cout << "Right up\n";
					}
				break;
	}
}


void static testeSom() {

	ISoundEngine* engineTest = createIrrKlangDevice();
	if (!engineTest)
		exit(0);
	engineTest->stopAllSounds();
	engineTest->play2D("teste.mp3", true);
	engineTest->drop();
	printf("Teste de Som Concluido");
}

void static testeHttp() {

	//You create the request
	httpRequest r;
	//Set the host,uri and headers
	
	r.setHost("localhost");
	r.setUri("/Api/meteorologias");
	r.addHeader("Connection: close");
	
	//Build the request
	r.buildRequest();
	
	//Create the client
	httpClient *cl = new httpClient();
	//Send the request
	cl->sendRequest(r);
	//Get the Response
	printf(r.getHost().c_str());
	printf(r.toString().c_str());
	printf("%s",cl->getResponse().c_str());
	printf("Teste de HTTP Concluido");

}

void static getPercursoHttp() {

	vector<Arco> ligacoes;

	//You create the request
	httpRequest r;

	r.setHost("localhost");
	r.setUri("/api/visitas");
	r.addHeader("Connection: close");

	//Build the request
	r.buildRequest();
	printf(r.toString().c_str());
	//Create the client
	httpClient *clr = new httpClient();
	//Send the request

	clr->sendRequest(r);
	//Get the Response

	string resposta = clr->getResponse().c_str();

	

	if (resposta.find("Visita") != std::string::npos) {

		unsigned first = resposta.find("{");
		unsigned last = resposta.find_last_of("}]");
		string novaRes = resposta.substr(first, last - first);
		

		vector<string> vectt;
		split(novaRes, vectt, '}');

		for (vector<string>::iterator it = vectt.begin(); it != vectt.end(); ++it) {
			string op = *it;
			replace(op.begin(), op.end(), '"', ' ');
			replace(op.begin(), op.end(), '{', ' ');
			replace(op.begin(), op.end(), '}', ' ');
			replace(op.begin(), op.end(), ',', '\n');
			cout << "----------------------\n";
			cout << op << "\n";
			*it = op;

		}

		string escolha;
		cout << "Qual a visita pretendida?\n";
		cin >> escolha;
		string numP;

		for (vector<string>::iterator it = vectt.begin(); it != vectt.end(); ++it) {
			string op = *it;
			
			string vis = "idVisita :" + escolha;
			if (op.find(vis) != std::string::npos) {
		
			

				vector<string> vectV;
				split(op, vectV, '\n');
				
				string dataVisita=vectV.at(1);
				string descricaoVisita = vectV.at(2);
				string horaInicioVisita = vectV.at(3);
				string idPercurso= vectV.at(4);
				string idUser= vectV.at(5);
				string idVisita= vectV.at(6);


				first = dataVisita.find(":");
				last = dataVisita.find_last_of(",");
				string novaDataV = dataVisita.substr(first, last - first);
				replace(novaDataV.begin(), novaDataV.end(), '"', ' ');
				replace(novaDataV.begin(), novaDataV.end(), ':', ' ');
				visita.visitaId = atoi(novaDataV.c_str());


				first = descricaoVisita.find(":");
				last = descricaoVisita.find_last_of(",");
				string novaDescricaoV = descricaoVisita.substr(first, last - first);
				replace(novaDescricaoV.begin(), novaDescricaoV.end(), '"', ' ');
				replace(novaDescricaoV.begin(), novaDescricaoV.end(), ':', ' ');
				visita.data = novaDescricaoV;

				first = horaInicioVisita.find(":");
				last = horaInicioVisita.find_last_of(",");
				string novoHorario = horaInicioVisita.substr(first, last - first);
				replace(novoHorario.begin(), novoHorario.end(), '"', ' ');
				replace(novoHorario.begin(), novoHorario.end(), ':', ' ');
				visita.descricao= novoHorario;


				first = idPercurso.find(":");
				last = idPercurso.find_last_of(",");
				string novoIDPercurso = idPercurso.substr(first, last - first);
				replace(novoIDPercurso.begin(), novoIDPercurso.end(), '"', ' ');
				novoIDPercurso.erase(0,1);
				//replace(novoIDPercurso.begin(), novoIDPercurso.end(), ':', ' ');
				visita.horaInicio=novoIDPercurso;


				first = idUser.find(":");
				last = idUser.find_last_of(",");
				string novoIDUser = idUser.substr(first, last - first);
				replace(novoIDUser.begin(), novoIDUser.end(), '"', ' ');
				replace(novoIDUser.begin(), novoIDUser.end(), ':', ' ');
				replace(novoIDUser.begin(), novoIDUser.end(), '\n', ' ');
				visita.percursoId=atoi(novoIDUser.c_str());
				novoIDUser.erase(0,1);
				numP = novoIDUser;


				first = idVisita.find(":");
				last = idVisita.find_last_of(",");
				string novoIDVisita = idVisita.substr(first, last - first);
				replace(novoIDVisita.begin(), novoIDVisita.end(), '"', ' ');
				replace(novoIDVisita.begin(), novoIDVisita.end(), ':', ' ');
				visita.UserId = atoi(novoIDVisita.c_str());
				

			}

		}
		
		//Set the host,uri and headers

		r.setHost("localhost");
		string ur = "/api/percursoes/" + numP;
	
		r.setUri(ur);
		r.addHeader("Connection: close");

		//Build the request
		r.buildRequest();
		printf(r.toString().c_str());
		//Create the client
		httpClient *cl = new httpClient();
		//Send the request

		cl->sendRequest(r);
		//Get the Response

		resposta = cl->getResponse().c_str();

		if (resposta.find("404 Not Found") == std::string::npos) {


			first = resposta.find("{");
			last = resposta.find_last_of("}]");
			novaRes = resposta.substr(first, last - first);


			vector<string> vect;
			split(novaRes, vect, '[');

			string res = vect.at(0);
			string pois = vect.at(1);
			split(res, vect, ',');
			string id = vect.at(0);
			string nome = vect.at(1);
			string descricao = vect.at(2);;


			first = id.find(":");
			last = id.find_last_of(",");
			string novoID = id.substr(first, last - first);
			replace(novoID.begin(), novoID.end(), ':', ' ');

			first = nome.find(":");
			last = nome.find_last_of(",");
			string novoNome = nome.substr(first, last - first);
			replace(novoNome.begin(), novoNome.end(), ':', ' ');
			replace(novoNome.begin(), novoNome.end(), '"', ' ');

			first = descricao.find(":");
			last = descricao.find_last_of(",");
			string novaDescricao = descricao.substr(first, last - first);
			replace(novaDescricao.begin(), novaDescricao.end(), ':', ' ');


			replace(pois.begin(), pois.end(), ']', ' ');
			
			visita.rota.id = atoi(novoID.c_str());
			visita.rota.descricao = novaDescricao;
			visita.rota.nome = novoNome;

			//Set the host,uri and headers

			vector<string> lista;
			split(pois, lista, ',');
			
			string latitude;
			string localID;
			string longitude;
			string nomeLocal;
			int numP = 0;
			int soma = 0;
			for (vector<string>::iterator it = lista.begin(); it != lista.end(); ++it) {
				soma++;
				r.setHost("localhost");
				string num = *it;
				replace(num.begin(), num.end(), ',', ' ');
				string uri = "/api/pois/" + num;

				r.setUri(uri);
				r.addHeader("Connection: close");

				//Build the request
				r.buildRequest();
				//Create the client
				httpClient *cl2 = new httpClient();
				//Send the request

				cl2->sendRequest(r);

				resposta = cl2->getResponse().c_str();

				if (resposta.find("404 Not Found") == std::string::npos) {


					first = resposta.find("{");
					last = resposta.find_last_of("}]");
					novaRes = resposta.substr(first, last - first);


					vector<string> vect2;
					split(novaRes, vect2, ',');

					id = vect2.at(0);
					nome = vect2.at(1);
					descricao = vect2.at(2);
					pois = vect2.at(3);
					string descricao2 = vect2.at(4);
					string duracaoVisita = vect2.at(5);


					first = id.find(":");
					last = id.find_last_of(",");
					novoID = id.substr(first, last - first);
					replace(novoID.begin(), novoID.end(), ':', ' ');
					replace(novoID.begin(), novoID.end(), '"', ' ');

					first = nome.find(":");
					last = nome.find_last_of(",");
					novoNome = nome.substr(first, last - first);
					replace(novoNome.begin(), novoNome.end(), ':', ' ');
					replace(novoNome.begin(), novoNome.end(), '"', ' ');

					first = descricao.find(":");
					last = descricao.find_last_of(",");
					novaDescricao = descricao.substr(first, last - first);
					replace(novaDescricao.begin(), novaDescricao.end(), ':', ' ');

					first = pois.find(":");
					last = pois.find_last_of(",");
					string novoPois = pois.substr(first, last - first);
					replace(novoPois.begin(), novoPois.end(), ':', ' ');
					replace(novoPois.begin(), novoPois.end(), '"', ' ');

					first = descricao2.find(":");
					last = descricao2.find_last_of(",");
					string novaDescricao2 = descricao2.substr(first, last - first);
					replace(novaDescricao2.begin(), novaDescricao2.end(), ':', ' ');
					replace(novaDescricao2.begin(), novaDescricao2.end(), '"', ' ');

					first = duracaoVisita.find(":");
					last = duracaoVisita.find_last_of(",");
					string novaDuracao = duracaoVisita.substr(first, last - first);
					replace(novaDuracao.begin(), novaDuracao.end(), ':', ' ');
					replace(novaDuracao.begin(), novaDuracao.end(), '"', ' ');


					r.setHost("localhost");
				
					std::string::iterator end_pos = std::remove(novaDescricao.begin(), novaDescricao.end(), ' ');
					novaDescricao.erase(end_pos, novaDescricao.end());
					novoID.erase(0, 1);
					ur = "/api/locals/" + novoID;
				

					r.setUri(ur);
					r.addHeader("Connection: close");

					//Build the request
					r.buildRequest();
					printf(r.toString().c_str());
					//Create the client
					httpClient *clLocal = new httpClient();
					//Send the request

					clLocal->sendRequest(r);
					//Get the Response

					resposta = clLocal->getResponse().c_str();

					if (resposta.find("404 Not Found") == std::string::npos) {
						
						first = resposta.find("{");
						last = resposta.find_last_of("}]");
						novaRes = resposta.substr(first, last - first);


						vector<string> vectL;
						split(novaRes, vectL, ',');

						latitude = vectL.at(0);
						localID = vectL.at(1);
						longitude = vectL.at(2);
						nomeLocal = vectL.at(3);

						first = latitude.find(":");
						last = latitude.find_last_of(",");
						string novaLatitude = latitude.substr(first, last - first);
						replace(novaLatitude.begin(), novaLatitude.end(), ':', ' ');
						replace(novaLatitude.begin(), novaLatitude.end(), '"', ' ');

						first = localID.find(":");
						last = localID.find_last_of(",");
						string novoLocal = localID.substr(first, last - first);
						replace(novoLocal.begin(), novoLocal.end(), ':', ' ');
						replace(novoLocal.begin(), novoLocal.end(), '"', ' ');


						first = longitude.find(":");
						last = longitude.find_last_of(",");
						string novaLongitude = longitude.substr(first, last - first);
						replace(novaLongitude.begin(), novaLongitude.end(), ':', ' ');
						replace(novaLongitude.begin(), novaLongitude.end(), '"', ' ');

						first = nomeLocal.find(":");
						last = nomeLocal.find_last_of(",");
						string novoNomeL = nomeLocal.substr(first, last - first);
						replace(novoNomeL.begin(), novoNomeL.end(), ':', ' ');
						replace(novoNomeL.begin(), novoNomeL.end(), '"', ' ');
					
						Poi a;
						a.categoria = novoNome;
						a.localId = novoID;
						a.id = atoi(novaDescricao.c_str());
						a.nome = novoPois;
						a.descricao = novaDescricao2;
						a.duracaoVisita = atoi(novaDuracao.c_str());
						a.nomeLocal = novoLocal;
						a.x = 5 * soma;//stof(novaLongitude)/10+ rand() % 10+ -10; 
							a.y = 0 * soma;//stof(novoNomeL)/10 + rand() % 10 + -10;

						Arco b;
						
						if (numP<lista.size()) {
							b.noi = numP;
							numP++;
							b.nof = numP;
							
							b.largura = 2;
							b.peso = 1;
							ligacoes.push_back(b);
						}
						
						


						visita.rota.pois.push_back(a);

					}
					else {
						cout << "Pagina Nao Existe";
						leGrafo();
					}


					
				}
				else {
					cout << "Pagina Nao Existe";
					leGrafo();
				}

			}

		
			cout << "\nVisita: " << visita.descricao << "Data: " << visita.data << "Horario: " << visita.horaInicio << "ID Percurso: " << visita.percursoId << "\nID User: " << visita.UserId << "\nId Visita: " << visita.visitaId << "\n";
			cout << "\nRota :\n" << "Id: " << visita.rota.id << "\nNome: " << visita.rota.nome << "\nDescricao: " << visita.rota.descricao << "\n";
			cout << "Pois:\n";
			for (vector<Poi>::iterator it = visita.rota.pois.begin(); it != visita.rota.pois.end(); ++it) {
				cout << "Id: " << it->id << "\nNome: " << it->nome << "\nDescricao: " << it->descricao << "\nDuracao: " << it->duracaoVisita << "\nCategoria: " << it->categoria << "\nLocalID: " << it->localId <<"\nX: "<<it->x<<"\nY: "<<it->y<<"\nNome: "<<it->nomeLocal<<"\n" ;
				cout << "--------------------\n";
			}
			
			
			
			int numArcos=visita.rota.pois.size()-1;
			int numNos = visita.rota.pois.size();
			leGrafoHTTP(numNos,numArcos,visita.rota.pois,ligacoes);
			//leGrafo();
			

		}
		else {
			cout << "Pagina Nao Existe";
		}
	}
else {
	cout << "Pagina Nao Existe";
}
	
}

void static testePrintToScreen() {

	printtext(100,50,"Teste");

	printf("Teste de PrintToScreen Concluido");
}

void static runTests() {

	testeSom();
	testeHttp();
	testePrintToScreen();

}

void timer(int n) {

	glutPostRedisplay();
	
	glutTimerFunc(25, timer, 0);
}


void main(int argc, char **argv)
{
	nome = "";
	if (!engine)
		exit(0);

	engine->play2D("teste.mp3",true);

    glutInit(&argc, argv);

/* need both double buffering and z buffer */

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("OpenGL");
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(Special);
	glutMouseFunc(mouse);
	glutTimerFunc(25, timer, 0);

	RAIN = GL_FALSE;
	SNOW = GL_FALSE;
	HAIL = GL_FALSE;

	myInit();
	getPercursoHttp();
	//testeHttp();
	imprime_ajuda();

    glutMainLoop();

}


