#pragma once

#include "Utils\Vector.hpp"
#include "Utils\Matrix.hpp"
#include "Utils\Math.hpp"

class Camera {
public:
	virtual ~Camera() = default;
	void viewParams(Vector3f const &eye, Vector3f const &lookAt, Vector3f const &up) {
		eye_ = eye;
		lookAt_ = lookAt;
		up_ = up;
		isViewDirty = true;
	}
	void updateView() {
		Math::viewMatrix(eye_, lookAt_, up_, viewMatrix_);
	}
	Matrix4x4 viewMatrix() {
		if (isViewDirty) {
			updateView();
			isViewDirty = false;
		}
		return viewMatrix_;
	}
	Matrix4x4 projectionMatrix() {
		if (isProjDirty_) {
			updateProj();
			isProjDirty_ = false;
		}
		return projMatrix_;
	}
protected:
	virtual void updateProj() = 0;
	Vector3f eye_;
	Vector3f lookAt_;
	Vector3f up_;
	Matrix4x4 viewMatrix_;
	bool isViewDirty = true;
	Matrix4x4 projMatrix_;
	bool isProjDirty_ = true;
};