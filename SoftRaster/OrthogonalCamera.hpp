#pragma once

#include "Camera.hpp"

class OrthogonalCamera :public Camera {
public:
	void orthoParams(float l, float r, float b, float t, float n, float f);
protected:
	virtual void updateProj() override;
	float l_, r_, b_, t_, n_, f_;
};