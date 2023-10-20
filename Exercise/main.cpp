#include <GL/glut.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <iostream>

static int slices = 16;
static int stacks = 16;

glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);      // Initial pos
glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);   // Initial target
glm::vec3 upVector(0.0f, 1.0f, 0.0f);       // Up vector
glm::mat4 viewMatrix;

float moveSpeed = 0.09f;
float rotateSpeed = 0.09f;

float yawAngle = 0.0f;      // n axis
float pitchAngle = 0.0f;    // u axis
float rollAngle = 0.0f;     // v axis

/* GLUT callback Handlers */
void updateView()
{
    glm::vec3 n = glm::normalize(cameraPos - cameraTarget);
    glm::vec3 u = glm::normalize(glm::cross(upVector, n));
    glm::vec3 v = glm::cross(n, u);

    glm::mat4 yawRot = glm::rotate(glm::mat4(1.0f), yawAngle, upVector);
    glm::mat4 pitchRot = glm::rotate(glm::mat4(1.0f), pitchAngle, u);
    glm::mat4 rollRot = glm::rotate(glm::mat4(1.0f), rollAngle, n);

    glm::mat4 rotationMat = yawRot * pitchRot * rollRot;

    cameraTarget = cameraPos - glm::vec3(rotationMat * glm::vec4(n, 0.0f));
    upVector = glm::vec3(rotationMat * glm::vec4(upVector, 0.0f));

    glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(viewMatrix));

    yawAngle = 0.0f;
    pitchAngle = 0.0f;
    rollAngle = 0.0f;

    //std::cout << "n - " << glm::to_string(n) << " u - " << glm::to_string(u) << " v - " << glm::to_string(v) << "\n";
}


static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glm::dvec3 cameraPosD(cameraPos.x, cameraPos.y, cameraPos.z);
    glm::dvec3 cameraTargetD(cameraTarget.x, cameraTarget.y, cameraTarget.z);
    glm::dvec3 upVectorD(upVector.x, upVector.y, upVector.z);

    gluLookAt(
        cameraPosD.x, cameraPosD.y, cameraPosD.z,
        cameraTargetD.x, cameraTargetD.y, cameraTargetD.z,
        upVectorD.x, upVectorD.y, upVectorD.z
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

static void display(void)
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double a = t*90.0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(1,0,0);

    glPushMatrix();
        glTranslated(-2.4,1.2,-6);
        glRotated(60,1,0,0);
        glRotated(a,0,0,1);
        glutSolidSphere(1,slices,stacks);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0,1.2,-6);
        glRotated(60,1,0,0);
        glRotated(a,0,0,1);
        glutSolidCone(1,1,slices,stacks);
    glPopMatrix();

    glPushMatrix();
        glTranslated(2.4,1.2,-6);
        glRotated(60,1,0,0);
        glRotated(a,0,0,1);
        glutSolidTorus(0.2,0.8,slices,stacks);
    glPopMatrix();

    glPushMatrix();
        glTranslated(-2.4,-1.2,-6);
        glRotated(60,1,0,0);
        glRotated(a,0,0,1);
        glutWireSphere(1,slices,stacks);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0,-1.2,-6);
        glRotated(60,1,0,0);
        glRotated(a,0,0,1);
        glutWireCone(1,1,slices,stacks);
    glPopMatrix();

    glPushMatrix();
        glTranslated(2.4,-1.2,-6);
        glRotated(60,1,0,0);
        glRotated(a,0,0,1);
        glutWireTorus(0.2,0.8,slices,stacks);
    glPopMatrix();

    glutSwapBuffers();
}


static void key(unsigned char key, int x, int y)
{
    glm::vec3 moveBy;
    glm::mat4 viewMatrix;

    switch (key)
    {
        case 27 :
        case 'q':
            exit(0);
            break;

        case 'w':
            moveBy = moveSpeed * (cameraTarget - cameraPos);
            cameraPos += moveBy;
            cameraTarget += moveBy;
            upVector += moveBy;

            viewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(glm::value_ptr(viewMatrix));
            //updateView();
            break;
        case 's':
            moveBy = moveSpeed * (cameraTarget - cameraPos);
            cameraPos -= moveBy;
            cameraTarget -= moveBy;
            upVector -= moveBy;

            viewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(glm::value_ptr(viewMatrix));
            //updateView();
            break;
        case 'd':
            moveBy = moveSpeed * glm::normalize(glm::cross(upVector, cameraTarget - cameraPos));
            cameraPos -= moveBy;
            cameraTarget -= moveBy;
            //upVector -= moveBy;

            viewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(glm::value_ptr(viewMatrix));
            //updateView();
            break;
        case 'a':
            moveBy = moveSpeed * glm::normalize(glm::cross(upVector, cameraTarget - cameraPos));
            cameraPos += moveBy;
            cameraTarget += moveBy;
            //upVector += moveBy;

            viewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(glm::value_ptr(viewMatrix));
            //updateView();
            break;

        case '1':
            yawAngle = rotateSpeed;
            updateView();
            break;
        case '4':
            yawAngle = -rotateSpeed;
            updateView();
            break;

        case '2':
            pitchAngle = rotateSpeed;
            updateView();
            break;
        case '5':
            pitchAngle = -rotateSpeed;
            updateView();
            break;
        case '3':
            rollAngle = rotateSpeed;
            updateView();
            break;
        case '6':
            rollAngle = -rotateSpeed;
            updateView();
            break;
    }

    //std::cout << "Camera pos = " << glm::to_string(cameraPos) << " camera tar = " << glm::to_string(cameraTarget) << std::endl;
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
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

    updateView();
    glutMainLoop();

    return EXIT_SUCCESS;
}
