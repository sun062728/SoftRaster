#include "Material.hpp"
#include "Shader.hpp"

void Material::setShader(ShaderPtr shader) {
	shader_ = shader;
}
void Material::applyTextures() const {
	// send textures to raster
}
