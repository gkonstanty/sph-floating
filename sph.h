#ifndef HW1_H
#define HW1_H

#define BRUTE1

#define SCENARIO_DAM      0
#define SCENARIO_FAUCET   1
#define SCENARIO_CUBE     2


#include <cstdlib>
#include <sys/time.h>
#include <string.h>

#ifdef __APPLE__
# include <GLUT/glut.h>
#else
# include <GL/glut.h>
# include <GL/gl.h>
# include <GL/glu.h>
#endif

#include "glvu.h"
#include "glvuVec3f.h"

#include "wall.h"
#include "particle.h"
#include "particle_system.h"
#include <vector>

#endif
