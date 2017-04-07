#include "sph.h"
///////////////////////////////////////////////////////////////////////////////
// This is an extensively reworked version of Example 3-1
// from the OpenGL Red Book:
//
// http://www.glprogramming.com/red/chapter03.html
//
///////////////////////////////////////////////////////////////////////////////


// GUI interaction stuff
GLVU glvu;

ParticleSystem * particleSystem;

double dt    = 1.0 / 100.0;
bool animate = false;

int iterationCount = 0;

char *windowTitle;
int windowHeight = 500;
int windowWidth = 800;

double arUtilTimer(void);
void arUtilTimerReset(void);


///////////////////////////////////////////////////////////////////////
// draw coordinate axes
///////////////////////////////////////////////////////////////////////
void drawAxes(){
    // glDisable(GL_COLOR_MATERIAL);
    // draw coordinate axes
    glPushMatrix();
    glTranslatef(-0.1f, -0.1f, -0.1f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    // x axis is red
    glColor4f(10.0f, 0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glColor4f(10.0f, 0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);

    // y axis is green
    glColor4f(0.0f, 10.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glColor4f(0.0f, 10.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);

    // z axis is blue
    glColor4f(0.0f, 0.0f, 10.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glColor4f(0.0f, 0.0f, 10.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
    glLineWidth(1.0f);
    glPopMatrix();
}

void showText(char *text, int x, int y){
    glColor3f(1,1,1);

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();

    gluOrtho2D( 0, windowWidth, 0, windowHeight );

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    glRasterPos2i(x, y);

    int len;
    len = strlen(text);
    for ( int i = 0; i < len; ++i ) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);
    }

    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
}

int textX = windowWidth - (windowWidth/5);

void showStats(){
    int textY = windowHeight - (windowHeight/4);
    int textSpace = 15;

    char fps[15];
    sprintf(fps, "FPS: %0.4f", (double) iterationCount / arUtilTimer());
    showText(fps, textX, textY);

    textY -= textSpace;
    char particleNo[30];
    sprintf(particleNo, "particleNo: %d", Particle::count);
    showText(particleNo, textX, textY);

}

///////////////////////////////////////////////////////////////////////////////
// The drawing function
///////////////////////////////////////////////////////////////////////////////
void displayCallback(){
    glvu.BeginFrame();

    // clear away the previous frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    showStats();
    // drawAxes();

    // draw the particle system and walls
    particleSystem->draw();

    // swap the buffers
    // glutSwapBuffers();

    glvu.EndFrame();
}

///////////////////////////////////////////////////////////////////////////////
// The projection function
///////////////////////////////////////////////////////////////////////////////
void reshapeCallback(int width, int height){
    cout << "reshape" << endl;
    // set the viewport resolution (w x h)
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);

    // Make ensuing transforms affect the projection matrix
    glMatrixMode(GL_PROJECTION);

    // set the projection matrix to an orthographic view
    glLoadIdentity();
    gluPerspective(65.0, (float) width / height, 0.01, 1000.0);

    // set the matric mode back to modelview
    glMatrixMode(GL_MODELVIEW);

    // set the lookat transform
    glLoadIdentity();
    gluLookAt(0, 0, 0.5, 0, 0, 0, 0, 1, 0);
}

///////////////////////////////////////////////////////////////////////////////
// Keyboard command processing function
///////////////////////////////////////////////////////////////////////////////
void keyboardCallback(unsigned char key, int x, int y){
    switch (key) {
        // quit entirely
        case 'q':
        case 'Q':
            exit(0);
            break;

        case 'a':
            animate = !animate;
            break;

        case ' ':
#ifdef BRUTE
            particleSystem->stepVerletBrute(dt);
#else
            particleSystem->stepVerlet(dt);
#endif
            glutPostRedisplay();
            break;

        //    case 't':
        //      for (int i = 0; i < 200; i++) {
        // #ifdef BRUTE
        //        particleSystem->stepVerletBrute(dt);
        // #else
        //        particleSystem->stepVerlet(dt);
        // #endif
        //        glutPostRedisplay();
        //      }
        //      break;

        case 'g':
#ifndef BRUTE
            particleSystem->toggleGridVisble();
#endif
            break;
        case 't':
#ifndef BRUTE
            particleSystem->toggleParticleTransparent();
#endif
            break;
        case 'h':
            particleSystem->toggleSurfaceVisible();
            break;

        case '=':
            particleSystem->surfaceThreshold += 0.1;
            cout << "surface threshold: " << particleSystem->surfaceThreshold << endl;
            break;

        case '-':
            particleSystem->surfaceThreshold -= 0.1;
            cout << "surface threshold: " << particleSystem->surfaceThreshold << endl;
            break;

        // case 's':
        //     particleSystem->toggleSurfaceVisible();
        //     break;

        case '/':
            particleSystem->toggleGravity();
            break;

        //    case '.':
        //      particleSystem->toggleArrows();
        //      break;

        case 'T':
            particleSystem->toggleTumble();
            break;

        case '1':
            particleSystem->loadScenario(SCENARIO_DAM);
            iterationCount = 0;
            arUtilTimerReset();
            break;

        case '2':
            particleSystem->loadScenario(SCENARIO_CUBE);
            iterationCount = 0;
            arUtilTimerReset();
            break;
        case '3':
            particleSystem->loadScenario(SCENARIO_FAUCET);
            iterationCount = 0;
            arUtilTimerReset();
            break;

        case 'f':
            printf("*** %f (frame/sec)\n", (double) iterationCount / arUtilTimer());
            break;
    }
    glvu.Keyboard(key, x, y);

    glutPostRedisplay();
} // keyboardCallback

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void glutMouseClick(int button, int state, int x, int y){
    glvu.Mouse(button, state, x, y);
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void glutMouseMotion(int x, int y){
    glvu.Motion(x, y);
    glvuVec3f viewVector = glvu.GetCurrentCam()->Y;
    particleSystem->setGravityVectorWithViewVector(VEC3D(viewVector[0], viewVector[1], viewVector[2]));
}

///////////////////////////////////////////////////////////////////////////////
// Idle command processing function
///////////////////////////////////////////////////////////////////////////////
void idleCallback(){
    if (!animate) return;

    // particleSystem.stepEuler(dt);
    // particleSystem.stepMidpoint(dt);
    // particleSystem.stepRK4(dt);
#ifdef BRUTE
    particleSystem->stepVerletBrute(dt);
#else
    if (iterationCount == 0) arUtilTimerReset();
    particleSystem->stepVerlet(dt);
    iterationCount++;
#endif

    glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char ** argv){
	windowTitle = (char *)malloc(sizeof(char)*50);
    sprintf(windowTitle, "SPH");

    glutInit(&argc, argv);
    glvu.Init(windowTitle,
            GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH,
            0, 0,
            windowWidth, windowHeight);
    glShadeModel(GL_SMOOTH);

    glvu.SetInertiaEnabled(0);

    // point GLUT to our callback functions
    glutDisplayFunc(displayCallback);
    glutIdleFunc(idleCallback);
    // glutReshapeFunc(reshapeCallback);
    glutKeyboardFunc(keyboardCallback);
    glutMouseFunc(glutMouseClick);
    glutMotionFunc(glutMouseMotion);


    // set background to black
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // enable lights
    GLfloat ambient[]  = { 0.7, 0.7, 0.7 };
    GLfloat diffuse[]  = { 1.0, 1.0, 1.0 };
    GLfloat specular[] = { 0.0, 0.0, 0.0 };
    // GLfloat lightPosition[] = { 0.0, 2.0, 0.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    // glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glvuVec3f ModelMin(-10, -10, -10), ModelMax(10, 10, 10),
    Eye(0, 0, 1.5), LookAtCntr(0.1, 0, 0), Up(0, 1, 0);

    float Yfov   = 30;
    float Aspect = 1;
    float Near   = 0.001f;
    float Far    = 10.0f;
    glvu.SetAllCams(ModelMin, ModelMax, Eye, LookAtCntr, Up, Yfov, Aspect, Near, Far);

    glvuVec3f center(0.0, 0.0, 0.0);
    glvu.SetWorldCenter(center);

    particleSystem = new ParticleSystem();

    // Let GLUT take over
    glutMainLoop();

    return 0;
} // main

static int ss, sms;

double arUtilTimer(void){
#ifdef _WIN32
    struct _timeb sys_time;
    double tt;
    int s1, s2;

    _ftime(&sys_time);
    s1 = sys_time.time - ss;
    s2 = sys_time.millitm - sms;
#else
    struct timeval time;
    double tt;
    int s1, s2;

# if defined(__linux) || defined(__APPLE__)
    gettimeofday(&time, NULL);
# else
    gettimeofday(&time);
# endif
    s1 = time.tv_sec - ss;
    s2 = time.tv_usec / 1000 - sms;
#endif // ifdef _WIN32

    tt = (double) s1 + (double) s2 / 1000.0;

    return ( tt );
}

void arUtilTimerReset(void){
#ifdef _WIN32
    struct _timeb sys_time;

    _ftime(&sys_time);
    ss  = sys_time.time;
    sms = sys_time.millitm;
#else
    struct timeval time;

# if defined(__linux) || defined(__APPLE__)
    gettimeofday(&time, NULL);
# else
    gettimeofday(&time);
# endif
    ss  = time.tv_sec;
    sms = time.tv_usec / 1000;
#endif // ifdef _WIN32
}
