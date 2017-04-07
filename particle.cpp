#include "particle.h"

VEC3F red(1, 0, 0);
VEC3F blue(0, 0, 1);
VEC3F black(0, 0, 0);
VEC3F green(0, 1, 0);
VEC3F lightBlueColor(0.01, 0.25, 1.0);
VEC3F purpleColor(0.88, 0.08, 0.88);

int count = 0;

#define PARTICLE_DRAW_RADIUS 0.015 // 0.01 //0.006

bool Particle::isSurfaceVisible = true;
bool Particle::isTransparent    = true;
bool Particle::showArrows       = false;
unsigned int Particle::count    = 0;

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////

Particle::Particle(): type(0)
{ }

Particle::Particle(const VEC3D& position) :
    _position(position), type(0){
    myQuadric = NULL;
    _id       = count++;
}

Particle::Particle(const VEC3D& position, const VEC3D& velocity) :
    _position(position), _velocity(velocity), type(0){
    myQuadric = NULL;
    _id       = count++;
}

///////////////////////////////////////////////////////////////////////////////
// OGL drawing
///////////////////////////////////////////////////////////////////////////////
void Particle::draw(){

    // if (_flag && isSurfaceVisible)
    //     glMaterialfv(GL_FRONT, GL_DIFFUSE, lightBlueColor);

    GLfloat materialColor[] = {0.0f, 0.0f, 1.0f, 0.2f};
    if (type == 1){
        materialColor[0] = 1.0f;
        materialColor[2] = 0.0f;
    }
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColor);

    glPushMatrix();
    glTranslated(_position[0], _position[1], _position[2]);

    if (showArrows) {
        // scale

        // glColor3f(0.2f,0.3f,0.6f);
        if (!myQuadric) {
            myQuadric = gluNewQuadric();
            gluQuadricDrawStyle(myQuadric, GLU_FILL);
            gluQuadricNormals(myQuadric, GLU_SMOOTH);
        }


        double angle1 = asin(_velocity[0]) * 180.0 / M_PI;
        double angle2 = asin(_velocity[1]) * 180.0 / M_PI;
        // double angle3 = asin(_velocity[2]) * 180.0 / M_PI;


        glRotatef(-angle1, 0, 1, 0);
        glRotatef(-angle2, 1, 0, 0);
        // glRotatef(-angle3, 0, 0, 1);

        gluCylinder(myQuadric, 0.001, 0.001, 0.01, 10, 10);
        glTranslated(0.00, 0.01, 0.00);
        glutSolidCone(0.003, 0.01, 10, 10);

        glFlush();


        // ;
    } else   {

        //NOTE: by using _flag show only the surface
        if (type == 1) {
            glutSolidSphere(PARTICLE_DRAW_RADIUS, 10, 10);
        } else {
            if (isTransparent){
                glEnable(GL_BLEND);
            }
            if ((isSurfaceVisible && _flag) || !isSurfaceVisible){
                glutSolidSphere(PARTICLE_DRAW_RADIUS, 10, 10);
            }
            if (isTransparent){
                glDisable(GL_BLEND);
            }
        }

    }

    glPopMatrix();
} // Particle::draw

void Particle::clearParameters(){
    _position     = VEC3D();
    _velocity     = VEC3D();
    _acceleration = VEC3D();
    _density      = 0.0;
    _pressure     = 0.0;
}
