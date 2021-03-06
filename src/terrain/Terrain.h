#ifndef TERRAIN_H
#define TERRAIN_H

#include "PerlinNoise.hpp"

const siv::PerlinNoise perlin(1234);

char terrainAt(int x, int y, int z);

#endif