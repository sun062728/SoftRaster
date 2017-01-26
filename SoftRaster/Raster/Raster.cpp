#include "Raster.hpp"
#include "../Utils/Math.hpp"

#include <fstream>

Raster::Raster()
{
	init();
}

void Raster::init()
{
	viewport_.zn = 0.0f;
	viewport_.zf = 1.0f;

	mWorld_.identity();
	mView_.identity();
	mProj_.identity();

	memset(&drawBuffer_, 0, sizeof(drawBuffer_));
}

void Raster::InputAssembler()
{
	IA2VS ia2vs;

	if (vIndex_.empty()) {
		auto it_p = vPosition_.cbegin();
		auto it_n = vNormal_.cbegin();
		auto it_t = vTexcoord_.cbegin();

		auto fill_vtx_prop = [&](VtxProps &v) {
			v.position.x = *it_p++;	v.position.y = *it_p++;	v.position.z = *it_p++;	v.position.w = 1.0f;
			v.normal.x = *it_n++;	v.normal.y = *it_n++;	v.normal.z = *it_n++;
			v.texcoord.x = *it_t++;	v.texcoord.y = *it_t++;
		};

		for (unsigned int idx = 0; idx < iVertexCount_ / 3; idx++) {
			VtxProps v1;
			fill_vtx_prop(v1);
			VtxProps v2;
			fill_vtx_prop(v2);
			VtxProps v3;
			fill_vtx_prop(v3);

			if (bClockWise_) {
				ia2vs.v1 = v1;
				ia2vs.v2 = v2;
				ia2vs.v3 = v3;
			}
			else {
				ia2vs.v1 = v1;
				ia2vs.v2 = v3;
				ia2vs.v3 = v2;
			}
			VertexShader(ia2vs);
		}
	}
	else {
		int size = vIndex_.capacity(); assert(size % 3 == 0);
		for (int i = 0; i < size; ) {
			int idx = vIndex_[i++], offset = idx * 3, cur = offset;
			ia2vs.v1.position.x = vPosition_[cur++];
			ia2vs.v1.position.y = vPosition_[cur++];
			ia2vs.v1.position.z = vPosition_[cur];
			ia2vs.v1.position.w = 1.0f;
			offset = idx * 3; cur = offset;
			ia2vs.v1.normal.x = vNormal_[cur++];
			ia2vs.v1.normal.y = vNormal_[cur++];
			ia2vs.v1.normal.z = vNormal_[cur];
			offset = idx * 2; cur = offset;
			ia2vs.v1.texcoord.x = vTexcoord_[cur++];
			ia2vs.v1.texcoord.y = vTexcoord_[cur];

			idx = vIndex_[i++]; offset = idx * 3; cur = offset;
			ia2vs.v2.position.x = vPosition_[cur++];
			ia2vs.v2.position.y = vPosition_[cur++];
			ia2vs.v2.position.z = vPosition_[cur];
			ia2vs.v2.position.w = 1.0f;
			offset = idx * 3; cur = offset;
			ia2vs.v2.normal.x = vNormal_[cur++];
			ia2vs.v2.normal.y = vNormal_[cur++];
			ia2vs.v2.normal.z = vNormal_[cur];
			offset = idx * 2; cur = offset;
			ia2vs.v2.texcoord.x = vTexcoord_[cur++];
			ia2vs.v2.texcoord.y = vTexcoord_[cur];

			idx = vIndex_[i++]; offset = idx * 3; cur = offset;
			ia2vs.v3.position.x = vPosition_[cur++];
			ia2vs.v3.position.y = vPosition_[cur++];
			ia2vs.v3.position.z = vPosition_[cur];
			ia2vs.v3.position.w = 1.0f;
			offset = idx * 3; cur = offset;
			ia2vs.v3.normal.x = vNormal_[cur++];
			ia2vs.v3.normal.y = vNormal_[cur++];
			ia2vs.v3.normal.z = vNormal_[cur];
			offset = idx * 2; cur = offset;
			ia2vs.v3.texcoord.x = vTexcoord_[cur++];
			ia2vs.v3.texcoord.y = vTexcoord_[cur];

			VertexShader(ia2vs);
		}
	}
}

void Raster::VertexShader(IA2VS ia2vs)
{
	VS2TAS vs2tas;
	vs2tas.v1 = ia2vs.v1;
	vs2tas.v2 = ia2vs.v2;
	vs2tas.v3 = ia2vs.v3;

	Vector4f p1_clip_coord = ia2vs.v1.position*mWVP_;
	Vector4f p2_clip_coord = ia2vs.v2.position*mWVP_;
	Vector4f p3_clip_coord = ia2vs.v3.position*mWVP_;

	// normal won't change when:
	// 1. translation
	// 2. scale(all axis)
	// normal needs mul matWorldView:
	// 1. rotation
	Vector3f p1_normal = ia2vs.v1.normal*m33World_;
	Vector3f p2_normal = ia2vs.v2.normal*m33World_;
	Vector3f p3_normal = ia2vs.v3.normal*m33World_;

	Vector4f p1_posWorld = ia2vs.v1.position*mWorld_;
	Vector4f p2_posWorld = ia2vs.v2.position*mWorld_;
	Vector4f p3_posWorld = ia2vs.v3.position*mWorld_;

	vs2tas.v1.position = p1_clip_coord;
	vs2tas.v2.position = p2_clip_coord;
	vs2tas.v3.position = p3_clip_coord;
	vs2tas.v1.normal = p1_normal;
	vs2tas.v2.normal = p2_normal;
	vs2tas.v3.normal = p3_normal;
	vs2tas.v1.posWorld = p1_posWorld;
	vs2tas.v2.posWorld = p2_posWorld;
	vs2tas.v3.posWorld = p3_posWorld;

	TriangleSetup_Clip(vs2tas);
}

void Raster::TriangleSetup_Clip(VS2TAS vs2tas)
{
	VtxProps &v1 = vs2tas.v1;
	VtxProps &v2 = vs2tas.v2;
	VtxProps &v3 = vs2tas.v3;

	// Clipping
	// 1. All 3 vertices on one side should be clipped.
	Vector4f &v1c = v1.position, &v2c = v2.position, &v3c = v3.position;
	if (v1c.x > v1c.w &&
		v2c.x > v2c.w &&
		v3c.x > v3c.w) {
		return;
	}
	if (v1c.x < -v1c.w &&
		v2c.x < -v2c.w &&
		v3c.x < -v3c.w) {
		return;
	}
	if (v1c.y > v1c.w &&
		v2c.y > v2c.w &&
		v3c.y > v3c.w) {
		return;
	}
	if (v1c.y < -v1c.w &&
		v2c.y < -v2c.w &&
		v3c.y < -v3c.w) {
		return;
	}
	if (v1c.z > v1c.w &&
		v2c.z > v2c.w &&
		v3c.z > v3c.w) {
		return;
	}
	if (v1c.z < -v1c.w &&
		v2c.z < -v2c.w &&
		v3c.z < -v3c.w) {
		return;
	}

	// 2. Do zNear plane clipping if needed
	//
	// alpha for zNear clipping: ( while zn = (1 - a)*z1 + a*z2 )
	//   (zn + zc1) / (zc1 - zc2)
	//
	// alpha for right plane clipping:
	// In clipping coordinate:
	//   x0 = w							}
	//   A + B*w = z0 => w = (z0 - A)/B	} => x0 = (z0 - A)/B	}
	//   x0 = (1 - a)*x1 + a*x2									} => a = (A + B*x1 - z1)/(z2 - z1 + B*x1 - B*x2)
	//   z0 = (1 - a)*z1 + a*z2									}
	if (v1c.z < -v1c.w) {
		if (v2c.z < -v2c.w) {
			// v1 v2 out, v3 in
			VtxProps v1_v3, v2_v3;
			doZNearClip(v1, v3, v1_v3);
			doZNearClip(v2, v3, v2_v3);

			TASInternal tasInternal;
			tasInternal.v1 = v1_v3;
			tasInternal.v2 = v2_v3;
			tasInternal.v3 = v3;
			TriangleSetup_PostClip(tasInternal);
		}
		else {
			if (v3c.z < -v3c.w) {
				// v1 v3 out, v2 in
				VtxProps v1_v2, v3_v2;
				doZNearClip(v1, v2, v1_v2);
				doZNearClip(v3, v2, v3_v2);

				TASInternal tasInternal;
				tasInternal.v1 = v1_v2;
				tasInternal.v2 = v2;
				tasInternal.v3 = v3_v2;
				TriangleSetup_PostClip(tasInternal);
			}
			else {
				// v1 out, v2 v3 in
				VtxProps v1_v2, v1_v3; // 1, 4
				doZNearClip(v1, v2, v1_v2);
				doZNearClip(v1, v3, v1_v3);

				TASInternal tasInternal;
				tasInternal.v1 = v1_v2;	// 1
				tasInternal.v2 = v2;	// 2
				tasInternal.v3 = v3;	// 3
				TriangleSetup_PostClip(tasInternal);

				tasInternal.v1 = v1_v2;	// 1
				tasInternal.v2 = v3;	// 3
				tasInternal.v3 = v1_v3;	// 4
				TriangleSetup_PostClip(tasInternal);
			}
		}
	}
	else {
		if (v2c.z < -v2c.w) {
			if (v3c.z < -v3c.w) {
				// v2 v3 out, v1 in
				VtxProps v2_v1, v3_v1;
				doZNearClip(v2, v1, v2_v1);
				doZNearClip(v3, v1, v3_v1);

				TASInternal tasInternal;
				tasInternal.v1 = v1;
				tasInternal.v2 = v2_v1;
				tasInternal.v3 = v3_v1;
				TriangleSetup_PostClip(tasInternal);
			}
			else {
				// v2 out, v1 v3 in
				VtxProps v2_v1, v2_v3;
				doZNearClip(v2, v1, v2_v1);
				doZNearClip(v2, v3, v2_v3);

				TASInternal tasInternal;
				tasInternal.v1 = v2_v1;
				tasInternal.v2 = v3;
				tasInternal.v3 = v1;
				TriangleSetup_PostClip(tasInternal);

				tasInternal.v1 = v2_v1;
				tasInternal.v2 = v2_v3;
				tasInternal.v3 = v3;
				TriangleSetup_PostClip(tasInternal);
			}
		}
		else {
			if (v3c.z < -v3c.w) {
				// v3 out, v1 v2 in
				VtxProps v3_v1, v3_v2;
				doZNearClip(v3, v1, v3_v1);
				doZNearClip(v3, v2, v3_v2);

				TASInternal tasInternal;
				tasInternal.v1 = v3_v1;
				tasInternal.v2 = v2;
				tasInternal.v3 = v1;
				TriangleSetup_PostClip(tasInternal);

				tasInternal.v1 = v3_v1;
				tasInternal.v2 = v3_v2;
				tasInternal.v3 = v2;
				TriangleSetup_PostClip(tasInternal);
			}
			else {
				// No clipping
				TASInternal tasInternal;
				tasInternal.v1 = v1;
				tasInternal.v2 = v2;
				tasInternal.v3 = v3;
				TriangleSetup_PostClip(tasInternal);
			}
		}
	}
}

void Raster::TriangleSetup_PostClip(TASInternal tasInternal)
{
	// Perspective division
	Vector4f p1_device_coord = tasInternal.v1.position;
	Vector4f p2_device_coord = tasInternal.v2.position;
	Vector4f p3_device_coord = tasInternal.v3.position;
	p1_device_coord.x /= p1_device_coord.w;
	p1_device_coord.y /= p1_device_coord.w;
	p1_device_coord.z /= p1_device_coord.w;
	p1_device_coord.w = 1.0f;
	p2_device_coord.x /= p2_device_coord.w;
	p2_device_coord.y /= p2_device_coord.w;
	p2_device_coord.z /= p2_device_coord.w;
	p2_device_coord.w = 1.0f;
	p3_device_coord.x /= p3_device_coord.w;
	p3_device_coord.y /= p3_device_coord.w;
	p3_device_coord.z /= p3_device_coord.w;
	p3_device_coord.w = 1.0f;

	// Viewport transformation
	// TODO: pos_deviceCoord * matViewport ?
	TAS2RAS tas2ras;
	tas2ras.v1 = tasInternal.v1;
	tas2ras.v2 = tasInternal.v2;
	tas2ras.v3 = tasInternal.v3;
	tas2ras.v1.position.x = consts_.xk*p1_device_coord.x + consts_.xb;
	tas2ras.v1.position.y = consts_.yk*p1_device_coord.y + consts_.yb;
	tas2ras.v1.position.z = consts_.zk*p1_device_coord.z + consts_.zb;
	tas2ras.v2.position.x = consts_.xk*p2_device_coord.x + consts_.xb;
	tas2ras.v2.position.y = consts_.yk*p2_device_coord.y + consts_.yb;
	tas2ras.v2.position.z = consts_.zk*p2_device_coord.z + consts_.zb;
	tas2ras.v3.position.x = consts_.xk*p3_device_coord.x + consts_.xb;
	tas2ras.v3.position.y = consts_.yk*p3_device_coord.y + consts_.yb;
	tas2ras.v3.position.z = consts_.zk*p3_device_coord.z + consts_.zb;

	Rasterization_float(tas2ras);
	//Rasterization_integer(tas2ras);
}

/* Edge equation */
void Raster::Rasterization_float(TAS2RAS tas2ras) 
{
	const float &x1 = tas2ras.v1.position.x;
	const float &y1 = tas2ras.v1.position.y;
	const float &x2 = tas2ras.v2.position.x;
	const float &y2 = tas2ras.v2.position.y;
	const float &x3 = tas2ras.v3.position.x;
	const float &y3 = tas2ras.v3.position.y;

	// http://groups.csail.mit.edu/graphics/classes/6.837/F98/Lecture7/triangles.html
	// Vector 12 right side:
	// (x1-x2)(y-y1) - (y1-y2)(x-x1) > 0
	// => Dx12*(y-y1) - Dy12*(x-x1) > 0
	// => Dx12*y - Dy12*x - (Dx12*y1 - Dy12*x1) > 0
	// => Dx12*y - Dy12*x - C12 > 0
	// => F12 > 0 at (x,y)
	// F12 - Dy12 > 0 at (x+1,y)
	// F12 + Dx12 > 0 at (x,y+1)

	// bbox
	int iMinX = static_cast<int>(floor(min(min(x1, x2), x3)));	iMinX = max(iMinX, viewport_.x); // it works if whole bbox is clipped at TAS stage
	int iMaxX = static_cast<int>(ceil(max(max(x1, x2), x3)));	iMaxX = min(iMaxX, viewport_.x + viewport_.w - 1);
	int iMinY = static_cast<int>(floor(min(min(y1, y2), y3)));	iMinY = max(iMinY, viewport_.y);
	int iMaxY = static_cast<int>(ceil(max(max(y1, y2), y3)));	iMaxY = min(iMaxY, viewport_.y + viewport_.h - 1);
	// pixel center
	float minX = (float)iMinX + 0.5f, maxX = (float)iMaxX - 0.5f;
	float minY = (float)iMinY + 0.5f, maxY = (float)iMaxY - 0.5f;
	// constants for edge function
	// (y1 - y2)*x - (x1 - x2)*y + (y1*x2 - x1*y2) = 0;
	float Dx12 = x1 - x2, Dy12 = y1 - y2, C12 = x2*y1 - y2*x1; // C12 = Dy12*x1 - Dx12*y1
	float Dx23 = x2 - x3, Dy23 = y2 - y3, C23 = x3*y2 - y3*x2; // C23 = Dy23*x2 - Dx23*y2
	float Dx31 = x3 - x1, Dy31 = y3 - y1, C31 = x1*y3 - y1*x3; // C31 = Dy31*x3 - Dx31*y3
	// for lower-left pixel
	float PllL12 = Dx12*minY - Dy12*minX + C12, row12 = PllL12, col12;
	float PllL23 = Dx23*minY - Dy23*minX + C23, row23 = PllL23, col23;
	float PllL31 = Dx31*minY - Dy31*minX + C31, row31 = PllL31, col31;
	// pX in lineYZ
	float P3L12 = Dx12*y3 - Dy12*x3 + C12;
	float P1L23 = Dx23*y1 - Dy23*x1 + C23;
	///float P2L31 = Dx31*y2 - Dy31*x2 + C31; // useless, omitted

	// walk through bbox
	for (int j = iMinY; j <= iMaxY; j++) {
		col12 = row12;
		col23 = row23;
		col31 = row31;
		for (int i = iMinX; i <= iMaxX; i++) {
			// clock-wise is F > 0, right side of the vector
			if (col12 > 0.0f && col23 > 0.0f && col31 > 0.0f) {
				float x = (float)i + 0.5f, y = (float)j + 0.5f;
				// Barycentric coordinate:
				// a + b + c = 1
				// a = A(pp1p2)/A(p1p2p3) = h/h3
				// distance from point to line: h = |(ax+by+c)/sqrt(a*a+b*b)|
				// a = h/h3 = |(ax+by+c)/(ax3+by3+c)|
				float PL12 = Dx12*y - Dy12*x + C12;
				float PL23 = Dx23*y - Dy23*x + C23;
				float a = abs(PL12 / P3L12);	// p3
				float b = abs(PL23 / P1L23);	// p1
				float c = 1.0f - a - b;			// p2

				VtxProps &v1 = tas2ras.v1;
				VtxProps &v2 = tas2ras.v2;
				VtxProps &v3 = tas2ras.v3;
				// Depth test
				float depth = a*v3.position.z + b*v1.position.z + c*v2.position.z;
				if (depth < drawBuffer_.pDepthbuffer[i + j*drawBuffer_.width]) {
					// Draw depth
					drawBuffer_.pDepthbuffer[i + j*drawBuffer_.width] = depth;

					// Interp props
					// f = (a*fa / wa + b*fb / wb + c*fc / wc) / (a / wa + b / wb + c / wc)
					// f = a2*f3 + b2*f1 + c2*f2
					float aDivWa = a / v3.position.w;
					float bDivWb = b / v1.position.w;
					float cDivWc = c / v2.position.w;
					float rcp_sum = 1.0f / (aDivWa + bDivWb + cDivWc);
					float a2 = aDivWa*rcp_sum;
					float b2 = bDivWb*rcp_sum;
					float c2 = cDivWc*rcp_sum;

					// other props
					RAS2PS ras2ps;
					ras2ps.p.coord.x = i;
					ras2ps.p.coord.y = j;
					ras2ps.p.normal = v3.normal*a2 + v1.normal*b2 + v2.normal*c2;
					ras2ps.p.texcoord = v3.texcoord*a2 + v1.texcoord*b2 + v2.texcoord*c2;
					ras2ps.p.posWorld = v3.posWorld*a2 + v1.posWorld*b2 + v2.posWorld*c2;

					PixelShader(ras2ps);
					///PixelShader_phong(ras2ps);
					///PixelShader_fresnel(ras2ps);
					///PixelShader_cook_torrance(ras2ps);
				}
				else {
					;
				}
			}
			col12 -= Dy12;
			col23 -= Dy23;
			col31 -= Dy31;
		}
		row12 += Dx12;
		row23 += Dx23;
		row31 += Dx31;
	}
}

// reference: http://forum.devmaster.net/t/advanced-rasterization/6145
void Raster::Rasterization_integer(TAS2RAS tas2ras)
{
	const float &x1 = tas2ras.v1.position.x;
	const float &y1 = tas2ras.v1.position.y;
	const float &x2 = tas2ras.v2.position.x;
	const float &y2 = tas2ras.v2.position.y;
	const float &x3 = tas2ras.v3.position.x;
	const float &y3 = tas2ras.v3.position.y;

	// 28.4 fixed-point coordinates
	int X1 = Math::iRound(16.0f * tas2ras.v1.position.x);
	int Y1 = Math::iRound(16.0f * tas2ras.v1.position.y);
	int X2 = Math::iRound(16.0f * tas2ras.v2.position.x);
	int Y2 = Math::iRound(16.0f * tas2ras.v2.position.y);
	int X3 = Math::iRound(16.0f * tas2ras.v3.position.x);
	int Y3 = Math::iRound(16.0f * tas2ras.v3.position.y);

	// Deltas
	const int DX12 = X1 - X2;
	const int DX23 = X2 - X3;
	const int DX31 = X3 - X1;

	const int DY12 = Y1 - Y2;
	const int DY23 = Y2 - Y3;
	const int DY31 = Y3 - Y1;

	// Fixed-point deltas
	const int FDX12 = DX12 << 4;
	const int FDX23 = DX23 << 4;
	const int FDX31 = DX31 << 4;

	const int FDY12 = DY12 << 4;
	const int FDY23 = DY23 << 4;
	const int FDY31 = DY31 << 4;

	// Bounding rectangle
	int minx = (min(min(X1, X2), X3) + 0xF) >> 4; minx = max(minx, viewport_.x);
	int maxx = (max(max(X1, X2), X3) + 0xF) >> 4; maxx = min(maxx, viewport_.x + viewport_.w - 1);
	int miny = (min(min(Y1, Y2), Y3) + 0xF) >> 4; miny = max(miny, viewport_.y);
	int maxy = (max(max(Y1, Y2), Y3) + 0xF) >> 4; maxy = min(maxy, viewport_.y + viewport_.h - 1);

	///(char*&)colorBuffer += miny * stride;

	// Half-edge constants
	int C1 = DY12 * X1 - DX12 * Y1;
	int C2 = DY23 * X2 - DX23 * Y2;
	int C3 = DY31 * X3 - DX31 * Y3;

	// Correct for fill convention
	if (DY12 > 0 || (DY12 == 0 && DX12 < 0)) C1--;
	if (DY23 > 0 || (DY23 == 0 && DX23 < 0)) C2--;
	if (DY31 > 0 || (DY31 == 0 && DX31 < 0)) C3--;

	int CY1 = C1 + DX12 * (miny << 4) - DY12 * (minx << 4);
	int CY2 = C2 + DX23 * (miny << 4) - DY23 * (minx << 4);
	int CY3 = C3 + DX31 * (miny << 4) - DY31 * (minx << 4);

	for (int y = miny; y < maxy; y++)
	{
		int CX1 = CY1;
		int CX2 = CY2;
		int CX3 = CY3;

		for (int x = minx; x < maxx; x++)
		{
			if (CX1 < 0 && CX2 < 0 && CX3 < 0)	// clock-wise
			{
				///colorBuffer[x] = 0x00FFFFFF;
				drawBuffer_.pColorbuffer[x + y*drawBuffer_.width] = Color4f(0.0f, 1.0f, 0.0f, 1.0f);
			}

			CX1 -= FDY12;
			CX2 -= FDY23;
			CX3 -= FDY31;
		}

		CY1 += FDX12;
		CY2 += FDX23;
		CY3 += FDX31;

		///(char*&)colorBuffer += stride;
	}
}

void Raster::PixelShader(RAS2PS ras2ps) 
{
	int x = ras2ps.p.coord.x;
	int y = ras2ps.p.coord.y;
	Color4f tex_color = textures_[0].sample(Sampler2D(FM_NEAREST), ras2ps.p.texcoord);
	drawBuffer_.pColorbuffer[x + y*drawBuffer_.width] = tex_color;
	///drawBuffer_.pColorbuffer[x + y*drawBuffer_.width] = Color4f(0.0f, 1.0f, 0.0f, 1.0f);
}

void Raster::PixelShader_phong(RAS2PS ras2ps)
{
	int x = ras2ps.p.coord.x;
	int y = ras2ps.p.coord.y;
	Vector3f normal = ras2ps.p.normal;
	Vector2f texCoord = ras2ps.p.texcoord;
	Vector3f lightDir(0.0f, -1.0f, -1.0f); // World coordinate
	Vector3f minusLightDir = -lightDir;
	Color4f diffuse;
	Color4f lightColor(1.0f, 1.0f, 1.0f, 1.0f);
	static const float K = 0.3f;
	static const int SPEC = 20;
	//diffuse = tex2D(texCoord, 0);
	diffuse = Color4f(0.0f, 0.1f, 0.3f, 1.0f);

	normal = normal.normalize();
	minusLightDir = minusLightDir.normalize();
	float cos = normal.dot(minusLightDir);
	float lambertian = clamp(cos, 0.0f, 1.0f);
	float phong = clamp(pow(lambertian, SPEC), 0.0f, 1.0f);

	Color4f color = diffuse;
	color += lightColor*lambertian*K;
	color += lightColor*phong*K;

	color.r = clamp(color.r, 0.0f, 1.0f);
	color.g = clamp(color.g, 0.0f, 1.0f);
	color.b = clamp(color.b, 0.0f, 1.0f);
	color.a = clamp(color.a, 0.0f, 1.0f);

	// OM
	drawBuffer_.pColorbuffer[x + y*drawBuffer_.width] = color;
}

void Raster::PixelShader_fresnel(RAS2PS ras2ps)
{
	int x = ras2ps.p.coord.x;
	int y = ras2ps.p.coord.y;
	Vector3f normal = ras2ps.p.normal;
	Vector2f texCoord = ras2ps.p.texcoord;
	Vector3f lightDir(0.0f, -1.0f, -1.0f); // World coordinate
	lightDir = -lightDir;
	Color4f lightColor(1.0f, 1.0f, 1.0f, 1.0f);

	normal = normal.normalize();
	lightDir = lightDir.normalize();
	float NDotL = clamp(normal.dot(lightDir), 0.0f, 1.0f);
	//float NDotL = normal.dot(lightDir);

	Color4f BaseColor;
	//BaseColor = tex2D(texCoord, 0);
	BaseColor = Color4f(0.2f, 0.5f, 0.9f, 1.0f);

	Color4f TotalAmbient = BaseColor*0.0f;
	Color4f TotalDiffuse = BaseColor*1.0f*NDotL;

	// fresnel constants
	const float ni = 1.00029f;
	const float nt = 2.216f;
	const float R0 = ((ni - nt)*(ni - nt)) / ((ni + nt)*(ni + nt));
	float Fresnel = R0 + (1.0f - R0) * pow((1.0f - NDotL), 5);

	Color4f color = TotalAmbient + TotalDiffuse + lightColor*Fresnel;

	color.r = clamp(color.r, 0.0f, 1.0f);
	color.g = clamp(color.g, 0.0f, 1.0f);
	color.b = clamp(color.b, 0.0f, 1.0f);
	color.a = clamp(color.a, 0.0f, 1.0f);

	// OM
	drawBuffer_.pColorbuffer[x + y*drawBuffer_.width] = color;
}

void Raster::PixelShader_cook_torrance(RAS2PS ras2ps) 
{
	/*************** Geometry **************/
	int x = ras2ps.p.coord.x;
	int y = ras2ps.p.coord.y;
	Vector3f posWorld(ras2ps.p.posWorld.x, ras2ps.p.posWorld.y, ras2ps.p.posWorld.z); // save pos in world for later use
	Vector3f normal = ras2ps.p.normal;
	Vector2f texCoord = ras2ps.p.texcoord;
	Vector3f lightDir = psLighting_.lightDir;
	lightDir = -lightDir;
	Color4f lightColor = psLighting_.lightColor;

	Vector3f eyePos = psLighting_.eyePos;
	Vector3f viewDir = eyePos - posWorld;
	viewDir = viewDir.normalize();

	normal = normal.normalize();
	lightDir = lightDir.normalize();
	//float NDotL = clamp(normal.dot(lightDir), 0.0f, 1.0f);
	float NDotL = normal.dot(lightDir);

	Vector3f Reflection = normal*2.0f*NDotL - lightDir;
	Reflection = Reflection.normalize();
	float RDotV = max(0.0f, Reflection.dot(viewDir));

	/*************** Color *****************/
	Color4f BaseColor = psLighting_.baseColor; //BaseColor = tex2D(texCoord, 0);
	Color4f TotalAmbient = BaseColor*0.0f;
	Color4f Diffuse = psLighting_.diffuseColor;
	Color4f TotalSpecular = lightColor*pow(RDotV, psLighting_.specularPower);

	// Cook-torrance
	Vector3f Half = lightDir + viewDir;
	Half = Half.normalize();
	float NDotH = normal.dot(Half);
	float NDotV = normal.dot(viewDir);
	float VDotH = viewDir.dot(Half);

	// fresnel constants
	const float ni = 1.00029f;
	const float nt = 2.216f;
	const float R0 = ((ni - nt)*(ni - nt)) / ((ni + nt)*(ni + nt));
	float Fresnel = R0 + (1.0f - R0) * pow((1.0f - NDotL), 5);

	float Geomectric = min(1.0f, min(2.0f * NDotH*NDotV / VDotH, 2.0f * NDotH*NDotL / VDotH));
	const float m = 0.2f;
	float Roughtness = (1.0f / (m*m*pow(NDotH, 4)))*exp((NDotH*NDotH - 1.0f) / (m*m*NDotH*NDotH));
	float Rs = Fresnel*Roughtness*Geomectric / (NDotV*NDotL);
	const float I = 0.15f;
	Color4f CT = (lightColor*Rs + Diffuse)*I*max(0.0f, NDotL);

	Color4f color = CT + BaseColor;

	color.r = clamp(color.r, 0.0f, 1.0f);
	color.g = clamp(color.g, 0.0f, 1.0f);
	color.b = clamp(color.b, 0.0f, 1.0f);
	color.a = clamp(color.a, 0.0f, 1.0f);

	// OM
	drawBuffer_.pColorbuffer[x + y*drawBuffer_.width] = color;
}


void Raster::OutputMerger(PS2OM ps2om)
{

}

void Raster::clearColor(float r, float g, float b, float a)
{
	for (int y = 0; y < drawBuffer_.height; y++)
		for (int x = 0; x < drawBuffer_.width; x++) {
			drawBuffer_.pColorbuffer[x + y*drawBuffer_.width].r = r;
			drawBuffer_.pColorbuffer[x + y*drawBuffer_.width].g = g;
			drawBuffer_.pColorbuffer[x + y*drawBuffer_.width].b = b;
			drawBuffer_.pColorbuffer[x + y*drawBuffer_.width].a = a;
		}
}

void Raster::clearDepthf(float d)
{
	for (int y = 0; y < drawBuffer_.height; y++)
		for (int x = 0; x < drawBuffer_.width; x++)
			drawBuffer_.pDepthbuffer[x + y*drawBuffer_.width] = d;
}

void Raster::draw()
{
	initStatesAndInternalConstants();
	// first stage
	InputAssembler();
}

void Raster::initStatesAndInternalConstants()
{
	// setup matrices
	mWVP_ = mWorld_*mView_*mProj_;
	Matrix4x4 mat4x4_worldView = mWorld_*mView_;
	m33WV_ = mat4x4_worldView.toMatrix3x3();
	m33World_ = mWorld_.toMatrix3x3();
	// get zNear
	float B = mProj_.m33, A = mProj_.m43;
	zNear_ = -(A / (1.0f + B));
	// set viewport consts
	consts_.xk = static_cast<float>(viewport_.w / 2);
	consts_.xb = static_cast<float>(viewport_.x + viewport_.w / 2);
	consts_.yk = static_cast<float>(viewport_.h / 2);
	consts_.yb = static_cast<float>(viewport_.y + viewport_.h / 2);
	consts_.zk = 0.5f*(viewport_.zf - viewport_.zn);
	consts_.zb = 0.5f*(viewport_.zn + viewport_.zf);
}

void Raster::setViewport(int x, int y, int w, int h)
{
	viewport_.x = x;
	viewport_.y = y;
	viewport_.w = w;
	viewport_.h = h;

	if (viewport_.w != drawBuffer_.width ||
		viewport_.h != drawBuffer_.height) {
		delete[]drawBuffer_.pColorbuffer;
		delete[]drawBuffer_.pDepthbuffer;
		drawBuffer_.width = viewport_.w;
		drawBuffer_.height = viewport_.h;
		drawBuffer_.pColorbuffer = new Color4f[drawBuffer_.width*drawBuffer_.height];
		clearColor(0.0f, 0.0f, 0.0f, 0.0f);
		drawBuffer_.pDepthbuffer = new float[drawBuffer_.width*drawBuffer_.height];
		clearDepthf(1.0f);
	}
}

void Raster::setModelMat(Matrix4x4 m)
{
	mWorld_ = m;
}

void Raster::setViewMat(Matrix4x4 m)
{
	mView_ = m;
}

void Raster::setProjMat(Matrix4x4 m)
{
	mProj_ = m;
}

void Raster::setVertexAttribs(
	std::vector<float> const &vPos, int iPosChn,
	std::vector<float> const &vNorm, int iNormChn,
	std::vector<float> const &vTC, int iTCChn,
	std::vector<int> const &vIdx)
{
	vPosition_ = vPos;	iPosChn_ = iPosChn;
	vNormal_ = vNorm;	iNormChn_ = iNormChn;
	vTexcoord_ = vTC;	iTCChn_ = iTCChn;
	vIndex_ = vIdx;

	if (iPosChn != 3) assert(0);
	if (!(iNormChn != 3 || iNormChn != 0)) assert(0);
	if (!(iTCChn != 2 || iTCChn != 0)) assert(0);

	assert(!vPos.empty());
	iVertexCount_ = vPos.size() / 3;

	assert(vIdx.size() % 3 == 0);
	if (vIdx.empty())
		iPrimitiveCount_ = vPos.size() / 3;
	else
		iPrimitiveCount_ = vIdx.size() / 3;

	// fill empty vectors
	if (vNorm.empty()) {
		vNormal_.reserve(iVertexCount_ * 3);
		for (unsigned int i = 0; i < iVertexCount_; i++) {
			vNormal_.push_back(0.0f);
			vNormal_.push_back(0.0f);
			vNormal_.push_back(0.0f);
		}
	}

	if (vTC.empty()) {
		vTexcoord_.reserve(iVertexCount_ * 2);
		for (unsigned int i = 0; i < iVertexCount_; i++) {
			vTexcoord_.push_back(0.0f);
			vTexcoord_.push_back(0.0f);
		}
	}
}

void Raster::setTexture(std::vector<int> const &tex, int iTexChn, int w, int h, int idx)
{
	textures_[idx].setTexture(w, h, Texture2D::TF_RGBA32F, Texture2D::TF_RGB888, (void*)tex.data());
}

void Raster::dumpRT2BMP(const char *path)
{
	const int bitCount = 24;
	const int width = drawBuffer_.width;
	const int height = drawBuffer_.height;
	const int pitchInBytes = bitCount / 8;

	char *color = new char[width*height * 3];
	char *p = color;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// reverse
			*p++ = static_cast<char>(drawBuffer_.pColorbuffer[x + y*drawBuffer_.width].b*255.0f);
			*p++ = static_cast<char>(drawBuffer_.pColorbuffer[x + y*drawBuffer_.width].g*255.0f);
			*p++ = static_cast<char>(drawBuffer_.pColorbuffer[x + y*drawBuffer_.width].r*255.0f);
		}
	}

	// bmp header
	std::fstream bmpOutput(path, std::ios::out | std::ios::binary);
	if (true == bmpOutput.fail()) {
		OutputDebugString("Open output file failed!\n");
		return;
	}

	BITMAPFILEHEADER bmpFileHeader;
	memset(&bmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
	bmpFileHeader.bfType = 0x4D42;
	bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + width * height * pitchInBytes;
	bmpFileHeader.bfReserved1 = 0;
	bmpFileHeader.bfReserved2 = 0;
	bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	BITMAPINFOHEADER bmpInfoHeader;
	memset(&bmpInfoHeader, 0, sizeof(BITMAPINFOHEADER));
	bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfoHeader.biWidth = width;
	bmpInfoHeader.biHeight = height;
	bmpInfoHeader.biPlanes = 1;
	bmpInfoHeader.biBitCount = 24;
	bmpInfoHeader.biCompression = BI_RGB;
	bmpInfoHeader.biSizeImage = width * height * pitchInBytes;
	bmpInfoHeader.biXPelsPerMeter = 0;
	bmpInfoHeader.biYPelsPerMeter = 0;
	bmpInfoHeader.biClrUsed = 0;
	bmpInfoHeader.biClrImportant = 0;

	bmpOutput.write(reinterpret_cast<char*>(&bmpFileHeader), sizeof(BITMAPFILEHEADER));
	bmpOutput.write(reinterpret_cast<char*>(&bmpInfoHeader), sizeof(BITMAPINFOHEADER));
	bmpOutput.write(color, width * height * pitchInBytes);
	bmpOutput.close();
	return;
}

void Raster::setPSEyePos(Vector3f v)
{
	psLighting_.eyePos = v;
}

void Raster::setPSLightDir(Vector3f v)
{
	psLighting_.lightDir = v;
}

void Raster::setPSBaseColor(Color4f c) 
{
	psLighting_.baseColor = c;
}

void Raster::setPSDiffuseColor(Color4f c)
{
	psLighting_.diffuseColor = c;
}

void Raster::setPSLightColor(Color4f c)
{
	psLighting_.lightColor = c;
}

void Raster::setPSSpecularPower(int f)
{
	psLighting_.specularPower = f;
}

void Raster::isClockWise(bool b)
{
	bClockWise_ = b;
}