#pragma once

typedef unsigned short region_dtype;

namespace TerrainConfig{
	const int ChunkSize = 16;
	const int RegionSize = 4;
};

const region_dtype BLOCK_NOT_FOUND = 0xff;