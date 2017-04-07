#include "particle_system.h"


unsigned int iteration = 0;
int scenario;

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////
ParticleSystem::ParticleSystem() :
    _isGridVisible(false), surfaceThreshold(0.01), gravityVector(0.0, GRAVITY_ACCELERATION, 0.0), grid(NULL){
    loadScenario(INITIAL_SCENARIO);
}

void ParticleSystem::loadScenario(int newScenario){
    scenario = newScenario;

    // remove all particles
    if (grid) {
        delete grid;
    }
    _walls.clear();

    // reset params
    Particle::count = 0;
    iteration = 0;

    if (scenario == SCENARIO_DAM) {
        // create long grid
        boxSize.x = BOX_SIZE * 2.0;
        boxSize.y = BOX_SIZE;
        boxSize.z = BOX_SIZE / 2.0;

        int gridXRes = (int) ceil(boxSize.x / h);
        int gridYRes = (int) ceil(boxSize.y / h);
        int gridZRes = (int) ceil(boxSize.z / h);

        grid = new FIELD_3D(gridXRes, gridYRes, gridZRes);

        // add walls
        _walls.push_back(WALL(VEC3D(0, 0, 1), VEC3D(0, 0, -boxSize.z / 2.0))); // back
        _walls.push_back(WALL(VEC3D(0, 0, -1), VEC3D(0, 0, boxSize.z / 2.0))); // front
        _walls.push_back(WALL(VEC3D(1, 0, 0), VEC3D(-boxSize.x / 2.0, 0, 0))); // left
        _walls.push_back(WALL(VEC3D(-1, 0, 0), VEC3D(boxSize.x / 2.0, 0, 0))); // right
        _walls.push_back(WALL(VEC3D(0, 1, 0), VEC3D(0, -boxSize.y / 2.0, 0))); // bottom

        vector<Particle>& firstGridCell = (*grid)(0, 0, 0);

        // add particles
        for (double y = -boxSize.y / 2.0; y < boxSize.y / 4.0; y += h / 2.0) {
            for (double x = -boxSize.x / 2.0; x < boxSize.x / 4.0; x += h / 2.0) {
                for (double z = -boxSize.z / 2.0; z < boxSize.z / 2.0; z += h / 2.0) {
                    firstGridCell.push_back(Particle(VEC3D(x, y, z)));
                }
            }
        }

        // TODO:
        Particle p = Particle(VEC3D(0.0,-boxSize.y / 4.0,0.0));
        p.type = 1;
        p._mass = 0.1;
        firstGridCell.push_back(p);

        cout << "Loaded dam scenario" << endl;
        cout << "Grid size is " << (*grid).xRes() << "x" << (*grid).yRes() << "x" << (*grid).zRes() << endl;
        cout << "Simulating " << Particle::count << " particles" << endl;
    } else if (scenario == SCENARIO_CUBE) {
        // create cubed grid

        boxSize.x = BOX_SIZE;
        boxSize.y = BOX_SIZE;
        boxSize.z = BOX_SIZE;

        int gridXRes = (int) ceil(boxSize.x / h);
        int gridYRes = (int) ceil(boxSize.y / h);
        int gridZRes = (int) ceil(boxSize.z / h);

        grid = new FIELD_3D(gridXRes, gridYRes, gridZRes);

        // add walls

        _walls.push_back(WALL(VEC3D(0, 0, 1), VEC3D(0, 0, -boxSize.z / 2.0))); // back
        _walls.push_back(WALL(VEC3D(0, 0, -1), VEC3D(0, 0, boxSize.z / 2.0))); // front
        _walls.push_back(WALL(VEC3D(1, 0, 0), VEC3D(-boxSize.x / 2.0, 0, 0))); // left
        _walls.push_back(WALL(VEC3D(-1, 0, 0), VEC3D(boxSize.x / 2.0, 0, 0))); // right
        _walls.push_back(WALL(VEC3D(0, 1, 0), VEC3D(0, -boxSize.y / 2.0, 0))); // bottom

        vector<Particle>& firstGridCell = (*grid)(0, 0, 0);

        // add particles

        for (double y = 0; y < boxSize.y; y += h / 2.0) {
            for (double x = -boxSize.x / 4.0; x < boxSize.x / 4.0; x += h / 2.0) {
                for (double z = -boxSize.z / 4.0; z < boxSize.z / 4.0; z += h / 2.0) {
                    firstGridCell.push_back(Particle(VEC3D(x, y, z)));
                }
            }
        }

        cout << "Loaded cube scenario" << endl;
        cout << "Grid size is " << (*grid).xRes() << "x" << (*grid).yRes() << "x" << (*grid).zRes() << endl;
        cout << "Simulating " << Particle::count << " particles" << endl;
    } else if (scenario == SCENARIO_FAUCET) {
        // create cubed grid

        boxSize.x = BOX_SIZE;
        boxSize.y = BOX_SIZE;
        boxSize.z = BOX_SIZE;

        int gridXRes = (int) ceil(boxSize.x / h);
        int gridYRes = (int) ceil(boxSize.y / h);
        int gridZRes = (int) ceil(boxSize.z / h);

        grid = new FIELD_3D(gridXRes, gridYRes, gridZRes);

        // add walls

        _walls.push_back(WALL(VEC3D(0, 0, 1), VEC3D(0, 0, -boxSize.z / 2.0))); // back
        _walls.push_back(WALL(VEC3D(0, 0, -1), VEC3D(0, 0, boxSize.z / 2.0))); // front
        _walls.push_back(WALL(VEC3D(1, 0, 0), VEC3D(-boxSize.x / 2.0, 0, 0))); // left
        _walls.push_back(WALL(VEC3D(-1, 0, 0), VEC3D(boxSize.x / 2.0, 0, 0))); // right
        _walls.push_back(WALL(VEC3D(0, 1, 0), VEC3D(0, -boxSize.y / 2.0, 0))); // bottom

        cout << "Loaded faucet scenario" << endl;
        cout << "Grid size is " << (*grid).xRes() << "x" << (*grid).yRes() << "x" << (*grid).zRes() << endl;
    }


    updateGrid();
} // ParticleSystem::loadScenario

void ParticleSystem::generateFaucetParticleSet(){
    VEC3D initialVelocity(-1.8, -1.8, 0);

    addParticle(VEC3D(BOX_SIZE / 2.0 - h / 2.0, BOX_SIZE + h * 0.6, 0), initialVelocity);
    addParticle(VEC3D(BOX_SIZE / 2.0 - h / 2.0, BOX_SIZE, 0), initialVelocity);
    addParticle(VEC3D(BOX_SIZE / 2.0 - h / 2.0, BOX_SIZE + h * -0.6, 0), initialVelocity);

    addParticle(VEC3D(BOX_SIZE / 2.0 - h / 2.0, BOX_SIZE + h * 0.3, h * 0.6), initialVelocity);
    addParticle(VEC3D(BOX_SIZE / 2.0 - h / 2.0, BOX_SIZE + h * 0.3, h * -0.6), initialVelocity);

    addParticle(VEC3D(BOX_SIZE / 2.0 - h / 2.0, BOX_SIZE + h * -0.3, h * 0.6), initialVelocity);
    addParticle(VEC3D(BOX_SIZE / 2.0 - h / 2.0, BOX_SIZE + h * -0.3, h * -0.6), initialVelocity);
}

void ParticleSystem::addParticle(const VEC3D& position, const VEC3D& velocity){
    #ifdef BRUTE
    _particles.push_back(Particle(position, velocity));
    #else
    (*grid)(0, 0, 0).push_back(Particle(position, velocity));
    #endif

    // _particleCount++;
}

void ParticleSystem::addParticle(const VEC3D& position){
    addParticle(position, VEC3D());
}

ParticleSystem::~ParticleSystem(){
    if (grid) delete grid;
}

void ParticleSystem::toggleGridVisble(){
    _isGridVisible = !_isGridVisible;
}

void ParticleSystem::toggleSurfaceVisible(){
    Particle::isSurfaceVisible = !Particle::isSurfaceVisible;
}

void ParticleSystem::toggleParticleTransparent(){
    Particle::isTransparent = !Particle::isTransparent;
}

void ParticleSystem::toggleTumble(){
    _tumble = !_tumble;
}

void ParticleSystem::toggleGravity(){
    if (gravityVector.magnitude() > 0.0) {
        gravityVector = VEC3D(0, 0, 0);
    } else {
        gravityVector = VEC3D(0, GRAVITY_ACCELERATION, 0);
    }
}

void ParticleSystem::toggleArrows(){
    Particle::showArrows = !Particle::showArrows;
}

void ParticleSystem::setGravityVectorWithViewVector(VEC3D viewVector){
    if (_tumble)
        gravityVector = viewVector * GRAVITY_ACCELERATION;
}

///////////////////////////////////////////////////////////////////////////////
// OGL drawing
///////////////////////////////////////////////////////////////////////////////
void ParticleSystem::draw(){
    static VEC3F blackColor(0, 0, 0);
    static VEC3F blueColor(0, 0, 1);
    static VEC3F whiteColor(1, 1, 1);
    static VEC3F greyColor(0.2, 0.2, 0.2);
    static VEC3F lightGreyColor(0.8, 0.8, 0.8);
    // static VEC3F greenColor(34.0 / 255, 139.0 / 255, 34.0 / 255);
    static float shininess = 10.0;

    // glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw the particles
    glEnable(GL_LIGHTING);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, blueColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, whiteColor);
    glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);

    // for (unsigned int x = 0; x < _particles.size(); x++)
    //  _particles[x].draw();

#ifdef BRUTE

    for (unsigned int x = 0; x < _particles.size(); x++)
        _particles[x].draw();

#else

    for (int gridCellIndex = 0; gridCellIndex < (*grid).cellCount(); gridCellIndex++) {
        vector<Particle>& particles = (*grid).data()[gridCellIndex];

        for (int p = 0; p < particles.size(); p++) {
            Particle& particle = particles[p];

            particle.draw();
        }
    }

    glDisable(GL_LIGHTING);

    if (_isGridVisible) {
        // draw the grid

        glColor3fv(lightGreyColor);

        // double offset = -BOX_SIZE/2.0+h/2.0;

        for (int x = 0; x < grid->xRes(); x++) {
            for (int y = 0; y < grid->yRes(); y++) {
                for (int z = 0; z < grid->zRes(); z++) {
                    glPushMatrix();

                    glTranslated(x * h - boxSize.x / 2.0 + h / 2.0, y * h - boxSize.y / 2.0 + h / 2.0,
                      z * h - boxSize.z / 2.0 + h / 2.0);
                    glutWireCube(h);

                    glPopMatrix();
                }
            }
        }
    }


#endif // ifdef BRUTE

    glColor3fv(greyColor);

    glPopMatrix();
    glScaled(boxSize.x, boxSize.y, boxSize.z);
    glutWireCube(1.0);
    glPopMatrix();
} // ParticleSystem::draw




///////////////////////////////////////////////////////////////////////////////
// Verlet integration
///////////////////////////////////////////////////////////////////////////////
void ParticleSystem::stepVerlet(double dt){
    calculateAcceleration();

    for (unsigned int gridCellIndex = 0; gridCellIndex < (*grid).cellCount(); gridCellIndex++) {
        vector<Particle>& particles = (*grid).data()[gridCellIndex];

        for (unsigned int p = 0; p < particles.size(); p++) {
            Particle& particle = particles[p];

            VEC3D newPosition = particle.position() + particle.velocity() * dt + particle.acceleration() * dt * dt;
            VEC3D newVelocity = (newPosition - particle.position()) / dt;

            particle.position() = newPosition;
            particle.velocity() = newVelocity;
        }
    }

    if (scenario == SCENARIO_FAUCET && Particle::count < MAX_ParticleS) {
        generateFaucetParticleSet();
    }

    updateGrid();

    iteration++;
}

///////////////////////////////////////////////////////////////////////////////
// Verlet integration
///////////////////////////////////////////////////////////////////////////////
void ParticleSystem::stepVerletBrute(double dt){
    if (Particle::count < MAX_ParticleS && iteration % 12 == 0) {
        generateFaucetParticleSet();
    }

    calculateAccelerationBrute();

    for (unsigned int i = 0; i < Particle::count; i++) {
        Particle& particle = _particles[i];

        VEC3D newPosition = particle.position() + particle.velocity() * dt + particle.acceleration() * dt * dt;
        VEC3D newVelocity = (newPosition - particle.position()) / dt;

        particle.position() = newPosition;
        particle.velocity() = newVelocity;
    }

    iteration++;
}
