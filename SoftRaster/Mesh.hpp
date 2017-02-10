#pragma once

#include <string>
#include <vector>

#include "Prerequesite.hpp"
#include "MeshData.hpp"

class Mesh {
public:
	Mesh(std::string const &filename);
	void draw();
	void setMaterial(MaterialPtr m, unsigned int idx);
private:
	std::vector<MeshData> submeshes_;
	std::string filename_;
};