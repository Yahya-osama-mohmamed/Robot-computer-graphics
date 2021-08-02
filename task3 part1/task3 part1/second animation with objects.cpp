#include <math.h>
#include "imageloader.h"
#include <cstdlib>
#include <GL/glut.h>
#include "glm.h"
#define PI 3.14
Image* image = loadBMP("img6.bmp");
char object[] = "data/palm.obj";
char object2[] = "data/basketball.obj";
char object3[] = "data/siameseCat.obj";
char object4[] = "data/horse.obj";

double eye[] = { 0, 0, 3 };
double center[] = { 0, 0, 2 };
double up[] = { 0, 1, 0 };
GLfloat angle = 0.0;   /* in degrees */
GLfloat angle2 = 0.0;   /* in degrees */
GLfloat BALL = 0.01;
static int shoulder = 0, LA = -90, LA2 = 0, LAR = 0, RA = -90, RA2 = 0, RAR = 0, LL = 0, LL2 = 0, RL = 0, RL2 = 0;
int moving, startx, starty;

// RGBA
GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
GLfloat light_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
// x , y, z, w
GLfloat light_position[] = { 0.5, 5.0, 0.0, 1.0 };
GLfloat lightPos1[] = { -0.5, -5.0, -2.0, 1.0 };

//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture(Image* image)
{
    GLuint textureId;
    glGenTextures(1, &textureId);			 //Make room for our texture
    glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
    //Map the image to the texture
    glTexImage2D(GL_TEXTURE_2D,				  //Always GL_TEXTURE_2D
        0,							  //0 for now
        GL_RGB,					  //Format OpenGL uses for image
        image->width, image->height, //Width and height
        0,							  //The border of the image
        GL_RGB,					  //GL_RGB, because pixels are stored in RGB format
        GL_UNSIGNED_BYTE,			  //GL_UNSIGNED_BYTE, because pixels are stored
                                     //as unsigned numbers
        image->pixels);			  //The actual pixel data
    return textureId;						  //Returns the id of the texture
}

GLuint _textureId; //The id of the texture

void drawmodel(char* filename)
{
    GLMmodel* model = glmReadOBJ(filename);
    glmUnitize(model);
    glmFacetNormals(model);
    glmVertexNormals(model, 90.0);
    glmScale(model, .15);
    glmDraw(model, GLM_SMOOTH | GLM_MATERIAL);
}

float poses[5][12] = {
    {0,90,0,0,90,0,0,0,0,0,0,0.5},
    {0,90,0,0,90,0,0,45,0,0,0,0},
    {0,90,0,45,90,0,-45,60,40,-60,40,0.5},
    {0,90,0,0,90,0,0,0,0,45,0,0},
    {0,90,0,-45,90,0,45,-60,40,60,40,0.5}

};


void SetPoses(int frameNum) {
    shoulder = poses[frameNum][0];
    LA = poses[frameNum][1];
    LA2 = poses[frameNum][2];
    LAR = poses[frameNum][3];
    RA = poses[frameNum][4];
    RA2 = poses[frameNum][5];
    RAR = poses[frameNum][6];
    LL = poses[frameNum][7];
    LL2 = poses[frameNum][8];
    RL = poses[frameNum][9];
    RL2 = poses[frameNum][10];
    BALL = poses[frameNum][11];






}
static int f = 0;
void timer(int value) {
    f = f % 5;
    SetPoses(f);
    f++;
    glutPostRedisplay();
    glutTimerFunc(150, timer, 0);
}

//Initializes 3D rendering
void initRendering()
{
    _textureId = loadTexture(image);
    delete image;
    // Turn on the power
    glEnable(GL_LIGHTING);
    // Flip light switch
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    // assign light parameters
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
    // Material Properties
    GLfloat lightColor1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor1);
    glEnable(GL_NORMALIZE);
    //Enable smooth shading
    glShadeModel(GL_SMOOTH);
    // Enable Depth buffer
    glEnable(GL_DEPTH_TEST);
}
void screen_menu(int value)
{
    switch (value)
    {
    case '1':
        image = loadBMP("img4.bmp");
        break;
    case '2':
        image = loadBMP("img5.bmp");
        break;
    case '3':
        image = loadBMP("img6.bmp");
        break;
    }
    initRendering();
    glutPostRedisplay();
}
void attachMenu()
{
    glutCreateMenu(screen_menu);
    glutAddMenuEntry("floor1", '1');
    glutAddMenuEntry("floor2", '2');
    glutAddMenuEntry("floor3", '3');
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void rotatePoint(double a[], double theta, double p[])
{

    double temp[3];
    temp[0] = p[0];
    temp[1] = p[1];
    temp[2] = p[2];

    temp[0] = -a[2] * p[1] + a[1] * p[2];
    temp[1] = a[2] * p[0] - a[0] * p[2];
    temp[2] = -a[1] * p[0] + a[0] * p[1];

    temp[0] *= sin(theta);
    temp[1] *= sin(theta);
    temp[2] *= sin(theta);

    temp[0] += (1 - cos(theta)) * (a[0] * a[0] * p[0] + a[0] * a[1] * p[1] + a[0] * a[2] * p[2]);
    temp[1] += (1 - cos(theta)) * (a[0] * a[1] * p[0] + a[1] * a[1] * p[1] + a[1] * a[2] * p[2]);
    temp[2] += (1 - cos(theta)) * (a[0] * a[2] * p[0] + a[1] * a[2] * p[1] + a[2] * a[2] * p[2]);

    temp[0] += cos(theta) * p[0];
    temp[1] += cos(theta) * p[1];
    temp[2] += cos(theta) * p[2];

    p[0] = temp[0];
    p[1] = temp[1];
    p[2] = temp[2];
}

void crossProduct(double a[], double b[], double c[])
{
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
}


void normalize(double a[])
{
    double norm;
    norm = a[0] * a[0] + a[1] * a[1] + a[2] * a[2];
    norm = sqrt(norm);
    a[0] /= norm;
    a[1] /= norm;
    a[2] /= norm;
}
void turnLeft()
{
    double theta = -PI / 100;
    rotatePoint(up, theta, eye);
}

void turnRight()
{
    double theta = PI / 100;
    rotatePoint(up, theta, eye);
}

void moveUp()
{
    double horizontal[3];
    double look[] = { center[0] - eye[0], center[1] - eye[1], center[2] - eye[2] };
    crossProduct(up, look, horizontal);
    normalize(horizontal);
    rotatePoint(horizontal, PI / 100, eye);
    rotatePoint(horizontal, PI / 100, up);
}


void moveDown()
{

    double horizontal[3];
    double look[] = { center[0] - eye[0], center[1] - eye[1], center[2] - eye[2] };
    crossProduct(up, look, horizontal);
    normalize(horizontal);
    rotatePoint(horizontal, -PI / 100, eye);
    rotatePoint(horizontal, -PI / 100, up);
}

void moveForward()
{

    double speed = 0.1;
    double direction[3];
    direction[0] = center[0] - eye[0];
    direction[1] = center[1] - eye[1];
    direction[2] = center[2] - eye[2];
    normalize(direction);

    eye[0] += direction[0] * speed;
    eye[2] += direction[2] * speed;

    center[0] += direction[0] * speed;
    center[2] += direction[2] * speed;
}

void moveBack()
{

    double speed = -0.1;
    double direction[3];
    direction[0] = center[0] - eye[0];
    direction[1] = center[1] - eye[1];
    direction[2] = center[2] - eye[2];

    normalize(direction);

    eye[0] += direction[0] * speed;
    eye[2] += direction[2] * speed;

    center[0] += direction[0] * speed;
    center[2] += direction[2] * speed;
}

void display(void)
{
    glPushMatrix();
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // Clear Depth and Color buffers

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

    glPushMatrix();
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glPopMatrix();

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS);
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-5, -4, 5);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(5, -4, 5);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(5, -4, -5);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-5, -4, -5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();


    glRotatef(angle2, 1.0, 0.0, 0.0);
    glRotatef(angle, 0.0, 1.0, 0.0);
    glPushMatrix();
    glScalef(1.0, 2.0, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    //  create Left arm

    //glTranslatef(-1.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(0.5, 0.875, 0.0);
    glRotatef((GLfloat)LAR, 1, 0, 0);
    glTranslatef(-0.5, -0.875, 0);
    glPushMatrix();
    glTranslatef(0.5, 0.875, 0.0);
    glRotatef((GLfloat)LA, 0, 0, -1.0);
    glTranslatef(-0.5, -0.875, 0);
    glTranslatef(1, 0.875, 0.0);
    glPushMatrix();
    glScalef(1.0, 0.25, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    glTranslatef(0.5, 0, 0.0);
    glRotatef((GLfloat)LA2, 0.0, 0.0, 1.0);
    glTranslatef(-0.5, 0, 0.0);
    glTranslatef(1, 0, 0.0);
    glPushMatrix();
    glScalef(1.0, 0.25, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    // finger 1

    glPushMatrix();

    glTranslatef(0.575, 0.0, 0.0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(0.15, 0.0, 0.0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    // finger 2

    glPushMatrix();

    glTranslatef(0.575, 0.0, 0.08);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(0.15, 0.0, 0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    // finger 3

    glPushMatrix();

    glTranslatef(0.575, 0.0, 0.155);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(0.15, 0.0, 0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    // finger 4
    glPushMatrix();

    glTranslatef(0.575, 0.0, -0.08);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(0.15, 0.0, 0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    // finger 5

    glPushMatrix();
    glTranslatef(0.575, 0.0, -0.155);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(0.15, 0.0, 0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();

    //Create Right Arm
    glPushMatrix();
    glTranslatef(-0.5, 0.875, 0.0);
    glRotatef((GLfloat)RAR, 1.0, 0.0, 0);
    glTranslatef(0.5, -0.875, 0);
    glPushMatrix();
    glTranslatef(-0.5, 0.875, 0.0);
    glRotatef((GLfloat)RA, 0.0, 0.0, 1.0);
    glTranslatef(0.5, -0.875, 0);
    glTranslatef(-1, 0.875, 0.0);
    glPushMatrix();
    glScalef(1.0, 0.25, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    glTranslatef(-0.5, 0, 0.0);
    glRotatef((GLfloat)RA2, 0.0, 0.0, 1.0);
    glTranslatef(0.5, 0, 0.0);
    glTranslatef(-1, 0, 0.0);
    glPushMatrix();
    glScalef(1.0, 0.25, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    // finger 1
    glPushMatrix();

    glTranslatef(-0.575, 0.0, 0.0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(-0.15, 0.0, 0.0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    // finger 2

    glPushMatrix();

    glTranslatef(-0.575, 0.0, 0.08);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(-0.15, 0.0, 0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    // finger 3

    glPushMatrix();

    glTranslatef(-0.575, 0.0, 0.155);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(-0.15, 0.0, 0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    // finger 4
    glPushMatrix();

    glTranslatef(-0.575, 0.0, -0.08);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(-0.15, 0.0, 0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    // finger 5

    glPushMatrix();

    glTranslatef(-0.575, 0.0, -0.155);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(-0.15, 0.0, 0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();
    //Create left Leg
    glPushMatrix();
    glTranslatef(0.375, -1.25, 0.0);
    glRotatef((GLfloat)LL, 1.0, 0.0, 0.0);
    glTranslatef(-0.375, 1.25, 0.0);
    glTranslatef(0.375, -1.5, 0.0);
    glPushMatrix();
    glScalef(0.25, 1, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    glTranslatef(0, -0.5, 0.0);
    glRotatef((GLfloat)LL2, 1.0, 0.0, 0.0);
    glTranslatef(0, 0.5, 0.0);
    glTranslatef(0, -1, 0.0);
    glPushMatrix();
    glScalef(0.25, 1, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    glTranslatef(0, -0.65, 0.25);
    glPushMatrix();
    glScalef(0.25, 0.3, 1);
    glutWireCube(1.0);
    glPopMatrix();

    glPopMatrix();

    //Create Right Leg
    glPushMatrix();
    glTranslatef(-0.375, -1.25, 0.0);
    glRotatef((GLfloat)RL, 1.0, 0.0, 0);
    glTranslatef(0.375, 1.25, 0.0);
    glTranslatef(-0.375, -1.5, 0.0);
    glPushMatrix();
    glScalef(0.25, 1, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    glTranslatef(0, -0.5, 0.0);
    glRotatef((GLfloat)RL2, 1.0, 0.0, 0);
    glTranslatef(0, 0.5, 0.0);
    glTranslatef(0, -1, 0.0);
    glPushMatrix();
    glScalef(0.25, 1, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    glTranslatef(0, -0.65, 0.25);
    glPushMatrix();
    glScalef(0.25, 0.3, 1);
    glutWireCube(1.0);
    glPopMatrix();

    glPopMatrix();

    //Create Head
    glPushMatrix();
    glTranslatef(0, 1.5, 0.0);
    glPushMatrix();
    glutSolidSphere(0.5, 20, 20);
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glScalef(100, 100, 100);
    glTranslatef(0.01, 0, 0);

    
    drawmodel(object);
    glPopMatrix();



    glPushMatrix();
    glScalef(2, 2, 2);
    glTranslatef(1, -1, 0.01);


    drawmodel(object3);
    glPopMatrix();

  

    glPushMatrix();
    
    glTranslatef(0.1, -3, BALL);
    glScalef(2, 2, 2);



    drawmodel(object2);
    glPopMatrix();

    glPushMatrix();

    glTranslatef(-3, -0.11, -3);
    glScalef(20, 20, 20);



    drawmodel(object4);
    glPopMatrix();



    

    glPopMatrix();

    glutSwapBuffers();
}

void specialKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        turnLeft();
        break;
    case GLUT_KEY_RIGHT:
        turnRight();
        break;
    case GLUT_KEY_UP:
        if (center[1] <= 1.5)
            moveUp();
        break;
    case GLUT_KEY_DOWN:
        if (center[1] >= -1.5)
            moveDown();
        break;
    }
    glutPostRedisplay();
}

void Keyboard(unsigned char Key, int x, int y)
{
    switch (Key)
    {
    case 'f':
        moveForward();
        break;
    case 'b':
        moveBack();
        break;

    case 27:
        exit(0);
        break;

    default:
        break;
    }
    glutPostRedisplay();
}
static void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            moving = 1;
            startx = x;
            starty = y;
        }
        if (state == GLUT_UP) {
            moving = 0;
        }
    }
}
static void motion(int x, int y)
{
    if (moving) {
        angle = angle + (x - startx);
        angle2 = angle2 + (y - starty);
        startx = x;
        starty = y;
        glutPostRedisplay();
    }
}
int main(int argc, char** argv)
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Floor");
    initRendering();
    glMatrixMode(GL_PROJECTION);
    gluPerspective(60, 1.0, 0.1, 10);
    attachMenu();
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(specialKeys);
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}