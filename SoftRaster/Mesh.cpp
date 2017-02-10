#include "Mesh.hpp"
#include "Utils\ObjLoader.hpp"
#include "GlobalContext.hpp"
#include "RenderEngine.hpp"

Mesh::Mesh(std::string const &filename)
	:filename_(filename) {
	ObjLoader loader(filename);
	unsigned int maxObjNum = loader.getObjectNum();
	assert(maxObjNum == 1);
	for (unsigned int iObj = 0; iObj < maxObjNum; iObj++) {
		unsigned int maxGrpNum = loader.getGroupNum(iObj);
		for (unsigned int iGrp = 0; iGrp < maxGrpNum; iGrp++) {
			std::string meshName;
			if (false == loader.getMeshName(iObj, iGrp, meshName))
				assert(0);

			MeshData data;
			data.name = meshName;
			if (false == loader.getMesh(iObj, iGrp, data.vb.vPos, data.vb.iPosChn, data.vb.vNorm, data.vb.iNormChn, data.vb.vTC, data.vb.iTCChn, data.vb.vIdx))
				assert(0);

			// material

			submeshes_.push_back(std::move(data));
		}
	}
}

void Mesh::draw() {
	for (auto m : submeshes_) {
		auto re = GlobalContext::instance().renderEngineInstance();
		re.setVertexBuffer();
		re.setIndexBuffer();
		if (m.material) {
			re.setTextures();
			re.setShaders();
			re.setShaderConstants();

			re.render();
		}
		else {
			assert(0);
		}
	}
}

void Mesh::setMaterial(MaterialPtr m, unsigned int idx) {
	if (submeshes_.size() > idx)
		submeshes_[idx].material = m;
}
