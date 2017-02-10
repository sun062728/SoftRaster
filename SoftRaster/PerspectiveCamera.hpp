#pragma once

#include "Camera.hpp"

class PerspectiveCamera :public Camera {
public:
	void perspectiveParams(float thetaInDegree, float aspectRatio, float zn, float zf);
protected:
	virtual void updateProj() override;
	float thetaInDegree_;	// in degree
	float aspectRatio_;		// w/h
	float zn_;
	float zf_;
};