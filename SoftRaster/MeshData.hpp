#pragma once

#include <vector>

#include "Prerequesite.hpp"

struct VBData {
	std::vector<float>	vPos;
	std::vector<float>	vNorm;
	std::vector<float>	vTC;
	std::vector<int>	vIdx;
	int					iPosChn;
	int					iNormChn;
	int					iTCChn;
};

struct IBData {
	std::vector<unsigned int> vIdx;
};

struct MeshData {
	VBData			vb;
	IBData			ib;
	MaterialPtr		material;
	std::string		name;
	//bool			skined;
};

