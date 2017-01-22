#include "Raster.hpp"

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
		const unsigned int SIZE = vPosition_.size();

		for (unsigned int idx = 0; idx < SIZE / 3;) {
			int offset = idx * 3, cur = offset;
			ia2vs.v1.position.x = vPosition_[cur++];
			ia2vs.v1.position.y = vPosition_[cur++];
			ia2vs.v1.position.z = vPosition_[cur];
			ia2vs.v1.position.w = 1.0f;
			offset = idx * 3, cur = offset;
			ia2vs.v1.normal.x = vNormal_[cur++];
			ia2vs.v1.normal.y = vNormal_[cur++];
			ia2vs.v1.normal.z = vNormal_[cur];
			offset = idx * 2, cur = offset;
			ia2vs.v1.texcoord.x = vTexcoord_[cur++];
			ia2vs.v1.texcoord.y = vTexcoord_[cur];
			idx++;

			offset = idx * 3; cur = offset;
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
			idx++;

			offset = idx * 3; cur = offset;
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
			idx++;

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
	TAS2RAS tas2ras;

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

	Rasterization(tas2ras);
}

/* Edge equation */
void Raster::Rasterization(TAS2RAS tas2ras) 
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
	//float P2L31 = Dx31*y2 - Dy31*x2 + C31;

	// walk through bbox
	for (int j = iMinY; j <= iMaxY; j++) {
		col12 = row12;
		col23 = row23;
		col31 = row31;
		for (int i = iMinX; i <= iMaxX; i++) {
			// clock-wise is F > 0, right side of the vector
			if (col12 > 0 && col23 > 0 && col31 > 0) {
				float x = (float)i + 0.5f, y = (float)j + 0.5f;
				// Barycentric coordinate:
				// a + b + c = 1
				// a = A(pp1p2)/A(p1p2p3) = h/h3
				// distance from point to line: h = |(ax+by+c)/sqrt(a*a+b*b)|
				// a = h/h3 = |(ax+by+c)/(ax3+by3+c)|
				float PL12 = Dx12*y - Dy12*x + C12;
				float PL23 = Dx23*y - Dy23*x + C23;
				float a = abs(PL12 / P3L12); // p3
				float b = abs(PL23 / P1L23); // p1
				float c = 1.0f - a - b; // p2

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

					//PixelShader(ras2ps);
					//PixelShader_fresnel(ras2ps);
					PixelShader_cook_torrance(ras2ps);
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

void Raster::PixelShader(RAS2PS ras2ps)
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

	//char str[64];
	//sprintf_s(str, 64, "x:%d y:%d\n", x, y);
	//OutputDebugString(str);
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

	//char str[64];
	//sprintf_s(str, 64, "x:%d y:%d\n", x, y);
	//OutputDebugString(str);
}

void Raster::PixelShader_cook_torrance(RAS2PS ras2ps) 
{
	int x = ras2ps.p.coord.x;
	int y = ras2ps.p.coord.y;
	Vector3f posWorld(ras2ps.p.posWorld.x, ras2ps.p.posWorld.y, ras2ps.p.posWorld.z); // save pos in world for later use
	Vector3f normal = ras2ps.p.normal;
	Vector2f texCoord = ras2ps.p.texcoord;
	Vector3f lightDir(0.0f, -1.0f, -0.0f); // World coordinate
	lightDir = -lightDir;
	Color4f lightColor(1.0f, 1.0f, 1.0f, 1.0f);

	Vector3f eyePos(0.0f, 2.0f, 2.0f);
	//Vector3f viewDir(0.0f, 1.0f, 2.0f);
	Vector3f viewDir = eyePos - posWorld;
	viewDir = viewDir.normalize();

	normal = normal.normalize();
	lightDir = lightDir.normalize();
	//float NDotL = clamp(normal.dot(lightDir), 0.0f, 1.0f);
	float NDotL = normal.dot(lightDir);

	Vector3f Reflection = normal*2.0f*NDotL - lightDir;
	Reflection = Reflection.normalize();
	float RDotV = max(0.0f, Reflection.dot(viewDir));

	Color4f BaseColor;
	//BaseColor = tex2D(texCoord, 0);
	BaseColor = Color4f(0.0f, 0.1f, 0.3f, 1.0f);

	Color4f TotalAmbient = BaseColor*0.0f;
	Color4f Diffuse(0.88f, 0.88f, 0.88f, 1.0f);
	Color4f TotalSpecular = lightColor*pow(RDotV, 25);

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

	//char str[64];
	//sprintf_s(str, 64, "x:%d y:%d\n", x, y);
	//OutputDebugString(str);
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
	// Let's go!
	InputAssembler();
}

void Raster::initStatesAndInternalConstants()
{
	mWVP_ = mWorld_*mView_*mProj_;
	Matrix4x4 mat4x4_worldView = mWorld_*mView_;
	m33WV_ = mat4x4_worldView.toMatrix3x3();
	m33World_ = mWorld_.toMatrix3x3();

	float B = mProj_.m33, A = mProj_.m43;
	zNear_ = -(A / (1.0f + B));

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

void Raster::setModelMat(
	float _m11, float _m12, float _m13, float _m14,
	float _m21, float _m22, float _m23, float _m24,
	float _m31, float _m32, float _m33, float _m34,
	float _m41, float _m42, float _m43, float _m44)
{
	Matrix4x4 m(
		_m11, _m12, _m13, _m14,
		_m21, _m22, _m23, _m24,
		_m31, _m32, _m33, _m34,
		_m41, _m42, _m43, _m44);
	mWorld_ = m;
}

void Raster::setViewMat(
	float _m11, float _m12, float _m13, float _m14,
	float _m21, float _m22, float _m23, float _m24,
	float _m31, float _m32, float _m33, float _m34,
	float _m41, float _m42, float _m43, float _m44)
{
	Matrix4x4 m(
		_m11, _m12, _m13, _m14,
		_m21, _m22, _m23, _m24,
		_m31, _m32, _m33, _m34,
		_m41, _m42, _m43, _m44);
	mView_ = m;
}

void Raster::setProjMat(
	float _m11, float _m12, float _m13, float _m14,
	float _m21, float _m22, float _m23, float _m24,
	float _m31, float _m32, float _m33, float _m34,
	float _m41, float _m42, float _m43, float _m44)
{
	Matrix4x4 m(
		_m11, _m12, _m13, _m14,
		_m21, _m22, _m23, _m24,
		_m31, _m32, _m33, _m34,
		_m41, _m42, _m43, _m44);
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

	assert(false == vPos.empty());
	iVertexCount_ = vPos.size() / 3;

	assert(vIdx.size() % 3 == 0);
	if (vIdx.empty())
		iPrimitiveCount_ = vPos.size() / 3;
	else
		iPrimitiveCount_ = vIdx.size() / 3;

	// fill empty vectors
	if (vNorm.empty()) {
		for (int i = 0; i < iVertexCount_; i++) {
			vNormal_.push_back(0.0f);
			vNormal_.push_back(0.0f);
			vNormal_.push_back(0.0f);
		}
	}

	if (vTC.empty()) {
		for (int i = 0; i < iVertexCount_; i++) {
			vTexcoord_.push_back(0.0f);
			vTexcoord_.push_back(0.0f);
		}
	}
}

void Raster::setTexture(std::vector<float> const &tex, int iTexChn, int w, int h, int idx)
{
	tex_[idx].texData = tex;
	tex_[idx].idx = iTexChn;
	tex_[idx].width = w;
	tex_[idx].height = h;
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
			// BGR for RGB
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