#include "Entity.hpp"

void Entity::draw() {
	if (mesh_ && isVisible_)
		mesh_->draw();
}
