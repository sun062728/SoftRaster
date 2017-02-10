#pragma once

#include "Prerequesite.hpp"
#include "Utils\Matrix.hpp"
#include "Utils\Vector.hpp"
#include "Mesh.hpp"

class Entity {
public:
	Entity(std::string const &meshFileName) {
		mesh_ = std::make_shared<Mesh>(meshFileName);
	}
	void draw();
	void update(float fElapsedTime) {}
	//void LoadMaterial(std::string const &filename) {}
	inline Matrix4x4 getWorldMatrix() const { return worldMatrix_; }
	void setWorldMatrix(Matrix4x4 const &m) { worldMatrix_ = m; }
	void SetVisible(bool visible) { isVisible_ = visible; }
	bool IsVisible() { return isVisible_; }
	MaterialPtr getMaterial(unsigned int index = 0) {}
	unsigned int getMaterialCount() {}
private:
	Matrix4x4			worldMatrix_;
	bool				isVisible_;
	bool				isDirty_;
	MeshPtr				mesh_;
};