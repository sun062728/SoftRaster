#pragma once

#include <vector>
#include <cassert>
#include "../Utils/Vector.hpp"
#include <cmath>
#include "../Utils/Math.hpp"

enum AddressMode
{
	AM_WRAP,
};
enum FilterMode
{
	FM_NEAREST,
	FM_LINEAR,
};

struct Sampler2D
{
	FilterMode	fm;
	AddressMode	am_u;
	AddressMode	am_v;
	Sampler2D(FilterMode _fm, AddressMode _am_u = AM_WRAP, AddressMode _am_v = AM_WRAP)
		:fm(_fm), am_u(_am_u), am_v(_am_v){}
};

class Texture2D {
public:
	enum TexFmt 
	{
		TF_RGBA32F,
		TF_RGB888,
	};

	Texture2D() = default;
	void setTexture(unsigned int w, unsigned int h, TexFmt internalFormat, TexFmt dataFormat, void *pData)
	{
		if (internalFormat == TF_RGBA32F && dataFormat == TF_RGB888) {
			int *p = reinterpret_cast<int*>(pData);
			data_.reserve(w*h);
			for (unsigned int i = 0; i < w*h; i++) {
				Color4f c;
				c.r = static_cast<float>(*p++) / 255.0f;
				c.g = static_cast<float>(*p++) / 255.0f;
				c.b = static_cast<float>(*p++) / 255.0f;
				c.a = 1.0f;
				data_.push_back(c);
			}
		}
		else {
			assert(0);
		}
		format_ = internalFormat;
		w_ = w;
		h_ = h;
	}

	// need test negative tc
	Color4f sample(Sampler2D sampler, Vector2f tc)
	{
		float u, v;
		Color4f ret;

		switch (sampler.am_u)
		{
		case AM_WRAP:
		{
			u = tc.x - floorf(tc.x);
			break;
		}
		default:
			assert(0);
			break;
		}

		switch (sampler.am_v)
		{
		case AM_WRAP:
		{
			v = tc.y - floorf(tc.y);
			break;
		}
		default:
			assert(0);
			break;
		}

		switch (sampler.fm)
		{
		case FM_NEAREST:
		{
			u *= w_;
			v *= h_;
			int x = (int)u;
			int y = (int)v;
			x %= w_;
			y %= h_;
			assert(x >= 0 && x < (int)w_);
			assert(y >= 0 && y < (int)h_);
			ret = data_[x + y*w_];
			break;
		}
		case FM_LINEAR:
		{
			u *= (w_ - 1);
			v *= (h_ - 1);
			int x = (int)(u);
			int y = (int)(v);
			float du = u - x;
			float dv = v - y;
			int left = (x) % w_;
			int right = (x + 1) % w_;
			int up = (y) % h_;
			int down = (y + 1) % h_;

			assert(left >= 0 && left < (int)w_);
			assert(right >= 0 && right < (int)w_);
			assert(up >= 0 && up < (int)h_);
			assert(down >= 0 && down < (int)h_);

			ret = (data_[left + down*w_] + data_[right + down*w_] + data_[left + up*w_] + data_[left + up*w_]) / 4.0f;
			break;
		}
		default:
			assert(0);
			break;
		}

		return ret;
	}

private:
	std::vector<Color4f> data_;
	TexFmt format_;
	unsigned int w_;
	unsigned int h_;
};