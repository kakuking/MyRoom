#include <GL/glut.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <iostream>


glm::vec3 initialCameraPos(0.0f, 0.0f, 3.0f);      // Initial pos
glm::vec3 initialCameraTarget(0.0f, 0.0f, 0.0f);   // Initial target
glm::vec3 initialUpVector(0.0f, 1.0f, 0.0f);       // Initial Up vector

glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);      // pos
glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);   // target
glm::vec3 upVector(0.0f, 1.0f, 0.0f);       // Up vector
glm::mat4 viewMatrix;

float moveSpeed = 0.5f;
float rotateSpeed = 0.09f;

float yawAngle = 0.0f;      // n axis
float pitchAngle = 0.0f;    // u axis
float rollAngle = 0.0f;     // v axis

/* GLUT callback Handlers */
void initCam()
{
    yawAngle = 0.0f;
    pitchAngle = 0.0f;
    rollAngle = 0.0f;

    cameraPos = initialCameraPos;
    cameraTarget = initialCameraTarget;
    upVector = initialUpVector;

    viewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(viewMatrix));
}

// Takes increment in yawAngle, pitchAngle, and rollAngle and then calculates new position of camera
// Calls lookAt()
void updateView()
{
    glm::vec3 n = glm::normalize(cameraPos - cameraTarget);
    glm::vec3 u = glm::normalize(glm::cross(upVector, n));
    glm::vec3 v = glm::cross(n, u);

    glm::quat yawQuat = glm::angleAxis(yawAngle, upVector);
    glm::quat pitchQuat = glm::angleAxis(pitchAngle, u);
    glm::quat rollQuat = glm::angleAxis(rollAngle, n);

    glm::quat rotationQuat = yawQuat * pitchQuat * rollQuat;
    rotationQuat = glm::normalize(rotationQuat);

    glm::mat4 rotationMat = glm::mat4_cast(rotationQuat);

    cameraTarget = cameraPos - glm::vec3(rotationMat * glm::vec4(n, 0.0f));
    upVector = glm::vec3(rotationMat * glm::vec4(upVector, 0.0f));

    glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(viewMatrix));

    // Resets angles so that angles dont add up
    yawAngle = 0.0f;
    pitchAngle = 0.0f;
    rollAngle = 0.0f;
}

// Takes bottom-left-front vertex and top-right-back vertex and draws a cuboid
// normal = 1 is outward normals, normal = -1 is inwards
void drawCuboid(float x1, float y1, float z1, float x2, float y2, float z2, int normal)
{
    glBegin(GL_QUADS);

        //Front Face
        glNormal3f(0.0, 0.0, normal*1.0);
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y1, z1);
        glVertex3f(x2, y2, z1);
        glVertex3f(x1, y2, z1);

        // Right Face
        glNormal3f(normal*1.0, 0.0, 0.0);
        glVertex3f(x2, y1, z1);
        glVertex3f(x2, y1, z2);
        glVertex3f(x2, y2, z2);
        glVertex3f(x2, y2, z1);

        // Back Face
        glNormal3f(0.0, 0.0, normal*-1.0);
        glVertex3f(x1, y1, z2);
        glVertex3f(x2, y1, z2);
        glVertex3f(x2, y2, z2);
        glVertex3f(x1, y2, z2);

        // Left Face
        glNormal3f(normal*-1.0, 0.0, 0.0);
        glVertex3f(x1, y1, z1);
        glVertex3f(x1, y1, z2);
        glVertex3f(x1, y2, z2);
        glVertex3f(x1, y2, z1);

        // Bottom Face
        glNormal3f(0.0, normal*-1.0, 0.0);
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y1, z1);
        glVertex3f(x2, y1, z2);
        glVertex3f(x1, y1, z2);

        // Top Face
        glNormal3f(0.0, normal*1.0, 0.0);
        glVertex3f(x1, y2, z1);
        glVertex3f(x2, y2, z1);
        glVertex3f(x2, y2, z2);
        glVertex3f(x1, y2, z2);
    glEnd();
}


// Draws the room, a modified drawCuboid where a wall is not rendered if it obstructs view
// of camera into the room
void drawRoom(float x1, float y1, float z1, float x2, float y2, float z2)
{
    int normal = -1;
    float xOffset = -10;
    float yOffset = -10;
    float zOffset = 0;

    glBegin(GL_QUADS);

        //Front Face
        if(cameraPos.z < z1 + zOffset){
            glNormal3f(0.0, 0.0, normal*1.0);
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y1, z1);
            glVertex3f(x2, y2, z1);
            glVertex3f(x1, y2, z1);
        }

        // Right Face
        if(cameraPos.x < x2 + xOffset){
            glNormal3f(normal*1.0, 0.0, 0.0);
            glVertex3f(x2, y1, z1);
            glVertex3f(x2, y1, z2);
            glVertex3f(x2, y2, z2);
            glVertex3f(x2, y2, z1);
        }

        // Back Face
        if(cameraPos.z > z2 + zOffset){
            glNormal3f(0.0, 0.0, normal*-1.0);
            glVertex3f(x1, y1, z2);
            glVertex3f(x2, y1, z2);
            glVertex3f(x2, y2, z2);
            glVertex3f(x1, y2, z2);
        }

        // Left Face
        if(cameraPos.x > x1 + xOffset){
            glNormal3f(normal*-1.0, 0.0, 0.0);
            glVertex3f(x1, y1, z1);
            glVertex3f(x1, y1, z2);
            glVertex3f(x1, y2, z2);
            glVertex3f(x1, y2, z1);
        }

        // Bottom Face
        if(cameraPos.y > y1 + yOffset){
            glNormal3f(0.0, normal*-1.0, 0.0);
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y1, z1);
            glVertex3f(x2, y1, z2);
            glVertex3f(x1, y1, z2);
        }

        // Top Face
        if(cameraPos.y < y2 + yOffset){
            glNormal3f(0.0, normal*1.0, 0.0);
            glVertex3f(x1, y2, z1);
            glVertex3f(x2, y2, z1);
            glVertex3f(x2, y2, z2);
            glVertex3f(x1, y2, z2);
        }
    glEnd();
}

static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);  // For Perspective
    //glOrtho(-20, 20, -20, 30, 0, 100);        // For parallel

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

// Takes rgb [0-255, 3] and calls glColor3f with it
void setColorRGB(int R, int G, int B)
{
    glColor3f(R/255.0, G/255.0, B/255.0);
}

static void display(void)
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double a = t*90.0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // We translate so that all coordinates are easier to keep track of, bottom left corner of room is 0, 0, 0 and top right is 20, 15, -20
    glPushMatrix();
        glTranslatef(-10, -10, 0);

        // Draw the room
        setColorRGB(244, 247, 208);
        drawRoom(0.0, 0, 0, 25.0, 30.0, -30.0);

        // Draw the bed
        setColorRGB(112, 65, 0);
        drawCuboid(0.0, 5.0, -5.0, 12, 5.5, -24.5, 1);  //  Platform

        setColorRGB(122, 122, 122);
        drawCuboid(0.5, 0, -5.5, 1, 5, -6, 1);          // The legs
        drawCuboid(11, 0, -5.5, 11.5, 5, -6, 1);
        drawCuboid(0, 0, -24.5, 12, 8.5, -25, 1);       // The backboard

        // Next, the table
        setColorRGB(255, 203, 143);
        drawCuboid(17, 9, -10, 25, 9.5, -20, 1);        // Platform

        setColorRGB(194, 194, 194);
        drawCuboid(17.5, 0, -10.5, 18, 9, -11, 1);      // The legs
        drawCuboid(24, 0, -10.5, 24.5, 9, -11, 1);
        drawCuboid(24, 0, -19, 24.5, 9, -19.5, 1);
        drawCuboid(17.5, 0, -19, 18, 9, -19.5, 1);


        // The window
        setColorRGB(79, 79, 79);
        drawCuboid(15, 10, -29.9, 25, 21, -30.3, 1); // The window frame
        drawCuboid(20, 10, -29.84, 20.5, 21, -30.36, 1); // Slat in the middle

        setColorRGB(181, 255, 240);
        drawCuboid(15.3, 10.3, -29.85, 24.7, 20.7, -30.35, 1);

        // The whiteboard
        setColorRGB(224, 224, 224);
        drawCuboid(24.8, 13, -10, 25, 22, -20, 1);  // The background

        setColorRGB(250, 250, 250);
        drawCuboid(24.78, 13.3, -10.3, 24.99, 21.7, -19.7, 1); // The board itself

        // The cupboard
        setColorRGB(41, 24, 0); // For the dark insides
        drawCuboid(1, 0.3, -29.9, 8, 21.7, -31.9, 1);

        setColorRGB(255, 191, 99); // The frame
        drawCuboid(0.5, 0, -29.95, 8.5, 22, -32, 1);
        drawCuboid(0.5, 0, -29.7, 4.5, 22, -29.95, 1);

        // For the door that is ajar
        glPushMatrix();
            glTranslatef(9, 0, -29.95);
            glRotatef(30, 0, 1, 0);
            glTranslatef(-9, 0, 29.95);
            drawCuboid(4.5, 0, -29.7, 9, 22, -29.95, 1);
        glPopMatrix();

        // door
        glPushMatrix();
            glTranslatef(24.6, 0, -0.1);
            glRotatef(-10, 0, 1, 0);
            glTranslatef(-24.6, 0, 0.1);
            drawCuboid(16.9, 0, 0.1, 24.6, 22, -0.1, 1);
        glPopMatrix();

    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}


static void key(unsigned char key, int x, int y)
{
    glm::vec3 moveBy;
    glm::mat4 viewMatrix;

    glm::vec3 n = glm::normalize(cameraPos - cameraTarget);
    glm::vec3 u = glm::normalize(glm::cross(upVector, n));
    glm::vec3 v = glm::cross(n, u);

    switch (key)
    {
        case 27 :
        case 'q':
            exit(0);
            break;

        // Forwards or backwards
        case 'w':
            moveBy = moveSpeed * n;//(cameraTarget - cameraPos);
            cameraPos -= moveBy;
            cameraTarget -= moveBy;
            //upVector -= moveBy;

            viewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(glm::value_ptr(viewMatrix));
            //updateView();
            break;
        case 's':
            moveBy = moveSpeed * n;// (cameraTarget - cameraPos);
            cameraPos += moveBy;
            cameraTarget += moveBy;
            //upVector += moveBy;

            viewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(glm::value_ptr(viewMatrix));
            //updateView();
            break;

        // Left and right
        case 'a':
            moveBy = moveSpeed * u;//glm::normalize(glm::cross(upVector, cameraTarget - cameraPos));
            cameraPos -= moveBy;
            cameraTarget -= moveBy;
            //upVector -= moveBy;

            viewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(glm::value_ptr(viewMatrix));
            //updateView();
            break;
        case 'd':
            moveBy = moveSpeed * u;//glm::normalize(glm::cross(upVector, cameraTarget - cameraPos));
            cameraPos += moveBy;
            cameraTarget += moveBy;
            //upVector += moveBy;

            viewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(glm::value_ptr(viewMatrix));
            //updateView();
            break;

        // Up and Down
        case 't':
            moveBy = moveSpeed * v;//glm::normalize(glm::cross(upVector, cameraTarget - cameraPos));
            cameraPos += moveBy;
            cameraTarget += moveBy;
            //upVector += moveBy;

            viewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(glm::value_ptr(viewMatrix));
            //updateView();
            break;
        case 'g':
            moveBy = moveSpeed * v;//glm::normalize(glm::cross(upVector, cameraTarget - cameraPos));
            cameraPos -= moveBy;
            cameraTarget -= moveBy;
            //upVector -= moveBy;

            viewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(glm::value_ptr(viewMatrix));
            //updateView();
            break;

        // Yaw Angle
        case '1':
            yawAngle = rotateSpeed;
            updateView();
            break;
        case '4':
            yawAngle = -rotateSpeed;
            updateView();
            break;

        // Pitch rotation
        case '2':
            pitchAngle = rotateSpeed;
            updateView();
            break;
        case '5':
            pitchAngle = -rotateSpeed;
            updateView();
            break;

        // Roll Rotation
        case '3':
            rollAngle = rotateSpeed;
            updateView();
            break;
        case '6':
            rollAngle = -rotateSpeed;
            updateView();
            break;

        // Resets camera
        case 'r':
            initCam();
            break;

    }

    glutPostRedisplay();
}


static void idle(void)
{
    glutPostRedisplay();
}

/* Program entry point */

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("GLUT Shapes");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    glClearColor(1,1,1,1);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glutMainLoop();

    return EXIT_SUCCESS;
}
