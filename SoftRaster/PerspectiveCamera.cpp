#include "PerspectiveCamera.hpp"

#include "Camera.hpp"

void PerspectiveCamera::perspectiveParams(float thetaInDegree, float aspectRatio, float zn, float zf) {
	thetaInDegree_ = thetaInDegree;
	aspectRatio_ = aspectRatio;
	zn_ = zn;
	zf_ = zf;
	isProjDirty_ = true;
}

void PerspectiveCamera::updateProj() {
	Math::perspectiveProjMatrix(thetaInDegree_, aspectRatio_, zn_, zf_, projMatrix_);
}
