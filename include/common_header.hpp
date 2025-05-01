#ifndef _COMMONHEADER_HPP_
#define _COMMONHEADER_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <config.hpp>
#include <input.hpp>
#include <screen.hpp>
#include <sound.hpp>
#include <system.hpp>

#define PI 3.1415926536
#define DEG2RAD (2.0f * PI / 360.0f)
#define RAD2DEG (360.0f / (2.0f * PI))

#define MAX(a,b) ((a)>(b) ? (a) : (b))

#define ASSERT(x, m) { if(!(x)) { printf(m); exit(-1); } }

#define MAX_PATH_LENGTH 256

#endif