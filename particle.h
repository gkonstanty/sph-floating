#ifndef Particle_H
#define Particle_H

#ifdef __APPLE__
# include <GLUT/glut.h>
#else
# include <GL/glut.h>
# include <GL/gl.h>
# include <GL/glu.h>
#endif

#include "VEC3F.h"
#include "VEC3D.h"
#include <vector>

using namespace std;

class Particle {
public:

    static bool isSurfaceVisible;
    static bool isTransparent;
    static bool showArrows;

    // static unsigned int count;
    Particle();
    Particle(const VEC3D& position);
    Particle(const VEC3D& position, const VEC3D& velocity);
    // ~Particle();

    // draw to OGL
    void draw();

    // clear all previous accumulated forces
    void clearForce(){ _force *= 0; };

    // accumulate forces
    void addForce(VEC3D newForce){ _force += newForce; };

    void calculateAcceleration();

    // accessors
    VEC3D& position(){ return _position; };
    VEC3D& velocity(){ return _velocity; };
    VEC3D& acceleration(){ return _acceleration; }

    VEC3D& force(){ return _force; };
    double& density(){ return _density; };
    double& pressure(){ return _pressure; };
    double& mass(){ return _mass; };
    bool& flag(){ return _flag; };
    int& id(){ return _id; };
    VEC3D normal;

    void
    clearParameters();

    //TODO:
    int type;
    static unsigned int count;
    double _mass = 0.02;

private:
    VEC3D _position;
    VEC3D _velocity;
    VEC3D _force;
    VEC3D _acceleration;
    double _density;
    double _pressure;
    bool _flag;
    int _id;
    GLUquadricObj * myQuadric;

    //TODO: all it there is, but as from #DEFINE
    // #define PARTICLE_MASS        0.02   // kg
    double _viscosity;
    // double radius; //???

};


#endif // ifndef Particle_H
