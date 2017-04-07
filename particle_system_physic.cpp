#include "particle_system.h"


// to update the grid cells particles are located in
// should be called right after particle positions are updated
void ParticleSystem::updateGrid(){
    for (unsigned int x = 0; x < (*grid).xRes(); x++) {
        for (unsigned int y = 0; y < (*grid).yRes(); y++) {
            for (unsigned int z = 0; z < (*grid).zRes(); z++) {
                vector<Particle>& particles = (*grid)(x, y, z);

                // cout << particles.size() << "p's in this grid" << endl;

                for (int p = 0; p < particles.size(); p++) {
                    Particle& particle = particles[p];

                    int newGridCellX = (int) floor((particle.position().x + BOX_SIZE / 2.0) / h);
                    int newGridCellY = (int) floor((particle.position().y + BOX_SIZE / 2.0) / h);
                    int newGridCellZ = (int) floor((particle.position().z + BOX_SIZE / 2.0) / h);

                    // cout << "particle position: " << particle.position() << endl;
                    // cout << "particle cell pos: " << newGridCellX << " " << newGridCellY << " " << newGridCellZ << endl;

                    if (newGridCellX < 0)
                        newGridCellX = 0;
                    else if (newGridCellX >= (*grid).xRes())
                        newGridCellX = (*grid).xRes() - 1;
                    if (newGridCellY < 0)
                        newGridCellY = 0;
                    else if (newGridCellY >= (*grid).yRes())
                        newGridCellY = (*grid).yRes() - 1;
                    if (newGridCellZ < 0)
                        newGridCellZ = 0;
                    else if (newGridCellZ >= (*grid).zRes())
                        newGridCellZ = (*grid).zRes() - 1;

                    // cout << "particle cell pos: " << newGridCellX << " " << newGridCellY << " " << newGridCellZ << endl;


                    // check if particle has moved

                    if (x != newGridCellX || y != newGridCellY || z != newGridCellZ) {
                        // move the particle to the new grid cell

                        (*grid)(newGridCellX, newGridCellY, newGridCellZ).push_back(particle);

                        // remove it from it's previous grid cell

                        particles[p] = particles.back();
                        particles.pop_back();
                        p--; // important! make sure to redo this index, since a new particle will (probably) be there
                    }
                }
            }
        }
    }
} // ParticleSystem::updateGrid


/*
 * Calculate the acceleration of each particle using a grid optimized approach.
 * For each particle, only particles in the same grid cell and the (26) neighboring grid cells must be considered,
 * since any particle beyond a grid cell distance away contributes no force.
 */
void ParticleSystem::calculateAcceleration(){
    ///////////////////
    // STEP 1: UPDATE DENSITY & PRESSURE OF EACH Particle

    for (int x = 0; x < (*grid).xRes(); x++) {
        for (int y = 0; y < (*grid).yRes(); y++) {
            for (int z = 0; z < (*grid).zRes(); z++) {
                vector<Particle>& particles = (*grid)(x, y, z);

                for (int p = 0; p < particles.size(); p++) {
                    Particle& particle = particles[p];

                    particle.density() = 0.0;

                    // now iteratate through neighbors

                    for (int offsetX = -1; offsetX <= 1; offsetX++) {
                        if (x + offsetX < 0) continue;
                        if (x + offsetX >= (*grid).xRes()) break;

                        for (int offsetY = -1; offsetY <= 1; offsetY++) {
                            if (y + offsetY < 0) continue;
                            if (y + offsetY >= (*grid).yRes()) break;

                            for (int offsetZ = -1; offsetZ <= 1; offsetZ++) {
                                if (z + offsetZ < 0) continue;
                                if (z + offsetZ >= (*grid).zRes()) break;

                                vector<Particle>& neighborGridCellParticles =
                                  (*grid)(x + offsetX, y + offsetY, z + offsetZ);

                                for (int i = 0; i < neighborGridCellParticles.size(); i++) {
                                    Particle& neighborParticle = neighborGridCellParticles[i];

                                    VEC3D diffPosition = particle.position() - neighborParticle.position();

                                    double radiusSquared = diffPosition.dot(diffPosition);

                                    if (radiusSquared <= h * h)
                                        particle.density() += Wpoly6(radiusSquared);
                                }
                            }
                        }
                    }

                    particle.density() *= particle.mass();

                    // p = k(density - density_rest)

                    particle.pressure() = GAS_STIFFNESS * (particle.density() - REST_DENSITY);
                }
            }
        }
    }

    ///////////////////
    // STEP 2: COMPUTE FORCES FOR ALL ParticleS

    for (int x = 0; x < (*grid).xRes(); x++) {
        for (int y = 0; y < (*grid).yRes(); y++) {
            for (int z = 0; z < (*grid).zRes(); z++) {
                vector<Particle>& particles = (*grid)(x, y, z);

                for (int p = 0; p < particles.size(); p++) {
                    Particle& particle = particles[p];

                    // cout << "particle id: " << particle.id() << endl;

                    VEC3D f_pressure,
                      f_viscosity,
                      f_surface,
                      f_gravity = gravityVector * particle.density(),
                      colorFieldNormal;

                    double colorFieldLaplacian;

                    // now iteratate through neighbors

                    for (int offsetX = -1; offsetX <= 1; offsetX++) {
                        if (x + offsetX < 0) continue;
                        if (x + offsetX >= (*grid).xRes()) break;

                        for (int offsetY = -1; offsetY <= 1; offsetY++) {
                            if (y + offsetY < 0) continue;
                            if (y + offsetY >= (*grid).yRes()) break;

                            for (int offsetZ = -1; offsetZ <= 1; offsetZ++) {
                                if (z + offsetZ < 0) continue;
                                if (z + offsetZ >= (*grid).zRes()) break;

                                vector<Particle>& neighborGridCellParticles =
                                  (*grid)(x + offsetX, y + offsetY, z + offsetZ);

                                for (int i = 0; i < neighborGridCellParticles.size(); i++) {
                                    Particle& neighbor = neighborGridCellParticles[i];

                                    // if (particle.id() == neighbor.id()) continue; // SKIPPING COMPARISON OF THE SAME Particle

                                    VEC3D diffPosition   = particle.position() - neighbor.position();
                                    double radiusSquared = diffPosition.dot(diffPosition);

                                    if (radiusSquared <= h * h) {
                                        VEC3D poly6Gradient, spikyGradient;

                                        Wpoly6Gradient(diffPosition, radiusSquared, poly6Gradient);

                                        WspikyGradient(diffPosition, radiusSquared, spikyGradient);

                                        if (particle.id() != neighbor.id()) {
                                            f_pressure +=
                                              (particle.pressure()
                                              / pow(particle.density(),
                                              2) + neighbor.pressure() / pow(neighbor.density(), 2)) * spikyGradient;

                                            f_viscosity += (neighbor.velocity() - particle.velocity())
                                              * WviscosityLaplacian(radiusSquared) / neighbor.density();
                                        }


                                        colorFieldNormal += poly6Gradient / neighbor.density();

                                        colorFieldLaplacian += Wpoly6Laplacian(radiusSquared) / neighbor.density();
                                    }
                                }
                            }
                        }
                    } // end of neighbor grid cell iteration

                    f_pressure *= -particle.mass() * particle.density();

                    f_viscosity *= VISCOSITY * particle.mass();

                    colorFieldNormal *= particle.mass();


                    particle.normal = -1.0 * colorFieldNormal;

                    colorFieldLaplacian *= particle.mass();


                    // surface tension force

                    double colorFieldNormalMagnitude = colorFieldNormal.magnitude();

                    if (colorFieldNormalMagnitude > SURFACE_THRESHOLD) {
                        particle.flag() = true;
                        f_surface       = -SURFACE_TENSION * colorFieldNormal / colorFieldNormalMagnitude
                          * colorFieldLaplacian;
                    } else {
                        particle.flag() = false;
                    }

                    // ADD IN SPH FORCES

                    particle.acceleration() = (f_pressure + f_viscosity + f_surface + f_gravity) / particle.density();

                    // EXTERNAL FORCES HERE (USER INTERACTION, SWIRL)

                    VEC3D f_collision;
                    collisionForce(particle, f_collision);
                }
            }
        }
    }
} // ParticleSystem::calculateAcceleration

void ParticleSystem::collisionForce(Particle& particle, VEC3D& f_collision){
    for (unsigned int i = 0; i < _walls.size(); i++) {
        WALL& wall = _walls[i];

        double d = (wall.point() - particle.position()).dot(wall.normal()) + 0.01; // particle radius

        if (d > 0.0) {
            // This is an alernate way of calculating collisions of particles against walls, but produces some jitter at boundaries
            // particle.position() += d * wall.normal();
            // particle.velocity() -= particle.velocity().dot(wall.normal()) * 1.9 * wall.normal();

            particle.acceleration() += WALL_K * wall.normal() * d;
            particle.acceleration() += WALL_DAMPING * particle.velocity().dot(wall.normal()) * wall.normal();
        }
    }
}

double ParticleSystem::Wpoly6(double radiusSquared){
    static double coefficient = 315.0 / (64.0 * M_PI * pow(h, 9));
    static double hSquared    = h * h;

    return coefficient * pow(hSquared - radiusSquared, 3);
}

void ParticleSystem::Wpoly6Gradient(VEC3D& diffPosition, double radiusSquared, VEC3D& gradient){
    static double coefficient = -945.0 / (32.0 * M_PI * pow(h, 9));
    static double hSquared    = h * h;

    gradient = coefficient * pow(hSquared - radiusSquared, 2) * diffPosition;
}

double ParticleSystem::Wpoly6Laplacian(double radiusSquared){
    static double coefficient = -945.0 / (32.0 * M_PI * pow(h, 9));
    static double hSquared    = h * h;

    return coefficient * (hSquared - radiusSquared) * (3.0 * hSquared - 7.0 * radiusSquared);
}

void ParticleSystem::WspikyGradient(VEC3D& diffPosition, double radiusSquared, VEC3D& gradient){ //
    static double coefficient = -45.0 / (M_PI * pow(h, 6));

    double radius = sqrt(radiusSquared);

    gradient = coefficient * pow(h - radius, 2) * diffPosition / radius;
}

double ParticleSystem::WviscosityLaplacian(double radiusSquared){
    static double coefficient = 45.0 / (M_PI * pow(h, 6));

    double radius = sqrt(radiusSquared);

    return coefficient * (h - radius);
}



/*
 * Calculate the acceleration of every particle in a brute force manner (n^2).
 * Used for debugging.
 */
void ParticleSystem::calculateAccelerationBrute(){
    ///////////////////
    // STEP 1: UPDATE DENSITY & PRESSURE OF EACH Particle

    for (int i = 0; i < Particle::count; i++) {
        // grab ith particle reference

        Particle& particle = _particles[i];

        // now iteratate through neighbors

        particle.density() = 0.0;

        for (int j = 0; j < Particle::count; j++) {
            Particle& neighborParticle = _particles[j];

            VEC3D diffPosition = particle.position() - neighborParticle.position();

            double radiusSquared = diffPosition.dot(diffPosition);

            if (radiusSquared <= h * h)
                particle.density() += Wpoly6(radiusSquared);
        }

        particle.density() *= particle.mass();

        // p = k(density - density_rest)

        particle.pressure() = GAS_STIFFNESS * (particle.density() - REST_DENSITY);

        // totalDensity += particle.density();
    }

    ///////////////////
    // STEP 2: COMPUTE FORCES FOR ALL ParticleS

    for (int i = 0; i < Particle::count; i++) {
        Particle& particle = _particles[i];

        // cout << "particle id: " << particle.id() << endl;

        VEC3D f_pressure,
          f_viscosity,
          f_surface,
        f_gravity(0.0, particle.density() * -9.80665, 0.0),
        n,
        colorFieldNormal,
        colorFieldLaplacian; // n is gradient of colorfield
        // double n_mag;

        for (int j = 0; j < Particle::count; j++) {
            Particle& neighbor = _particles[j];

            VEC3D diffPosition = particle.position() - neighbor.position();
            VEC3D diffPositionNormalized = diffPosition.normal(); // need?
            double radiusSquared         = diffPosition.dot(diffPosition);

            if (radiusSquared <= h * h) {
                if (radiusSquared > 0.0) {
                    // neighborsVisited++;
                    // cout << neighborsVisited << endl;

                    // cout << neighbor.id() << endl;

                    VEC3D gradient;

                    Wpoly6Gradient(diffPosition, radiusSquared, gradient);

                    f_pressure += (particle.pressure() + neighbor.pressure()) / (2.0 * neighbor.density()) * gradient;

                    colorFieldNormal += gradient / neighbor.density();
                }

                f_viscosity += (neighbor.velocity() - particle.velocity()) * WviscosityLaplacian(radiusSquared)
                  / neighbor.density();

                colorFieldLaplacian += Wpoly6Laplacian(radiusSquared) / neighbor.density();
            }
        }

        f_pressure *= -particle.mass();

        // totalPressure += f_pressure;

        f_viscosity *= VISCOSITY * particle.mass();

        colorFieldNormal *= particle.mass();

        particle.normal = -1.0 * colorFieldNormal;

        colorFieldLaplacian *= particle.mass();

        // surface tension force

        double colorFieldNormalMagnitude = colorFieldNormal.magnitude();

        if (colorFieldNormalMagnitude > surfaceThreshold) {
            particle.flag() = true;
            f_surface       = -SURFACE_TENSION * colorFieldLaplacian * colorFieldNormal / colorFieldNormalMagnitude;
        } else {
            particle.flag() = false;
        }

        // ADD IN SPH FORCES

        particle.acceleration() = (f_pressure + f_viscosity + f_surface + f_gravity) / particle.density();


        // EXTERNAL FORCES HERE (USER INTERACTION, SWIRL)

        VEC3D f_collision;

        collisionForce(particle, f_collision);
    }
} // ParticleSystem::calculateAccelerationBrute
