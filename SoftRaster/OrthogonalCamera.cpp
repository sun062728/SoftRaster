#include "OrthogonalCamera.hpp"

void OrthogonalCamera::orthoParams(float l, float r, float b, float t, float n, float f) {
	l_ = l; r_ = r; b_ = b; t_ = t; n_ = n; f_ = f;
	isProjDirty_ = true;
}

void OrthogonalCamera::updateProj() {
	Math::orthoProjMatrix(l_, r_, b_, t_, n_, f_, projMatrix_);
}

