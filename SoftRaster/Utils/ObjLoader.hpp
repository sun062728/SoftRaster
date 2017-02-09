#pragma once

#include <vector>
#include <cassert>
#include <fstream>

class ObjLoader
{
public:
	ObjLoader(std::string const &filePath);
	int getObjectNum() const
	{
		return vObject_.size();
	}
	int getGroupNum(unsigned int o) const 
	{
		if (o >= vObject_.size() || o < 0)
			return -1;
		else
			return vObject_[o].vGroup.size();
	}
	bool getMeshName(unsigned int o, unsigned int g, std::string &ret) const
	{
		if (o >= vObject_.size() || o < 0)
			return false;
		if (g >= vObject_[o].vGroup.size() || g < 0)
			return false;
		ret = vObject_[o].vGroup[g].sGroupName;
		return true;
	}
	bool getMesh(
		unsigned int o, unsigned int g,
		std::vector<float> &p, int &iPC,
		std::vector<float> &n, int &iNC,
		std::vector<float> &t, int &iTC,
		std::vector<int> &i) const
	{
		if (o >= vObject_.size() || o < 0)
			return false;
		if (g >= vObject_[o].vGroup.size() || g < 0)
			return false;
		Group const &grp = vObject_[o].vGroup[g];
		p = grp.vPosition; iPC = grp.iPosChn;
		n = grp.vNormal; iNC = grp.iNormChn;
		t = grp.vTexcoord; iTC = grp.iTCChn;
		i = grp.vIndex;
		return true;
	}

private:
	struct Group
	{
		enum FaceType
		{
			FT_V = 1,
			FT_VT,
			FT_VN,
			FT_VTN,
		};
		std::vector<float>	vPosition;
		int					iPosChn;
		std::vector<float>	vNormal;
		int					iNormChn;
		std::vector<float>	vTexcoord;
		int					iTCChn;
		std::vector<int>	vIndex;
		std::string			sGroupName;
		FaceType			faceType;

		void clear()
		{
			vPosition.clear();
			vNormal.clear();
			vTexcoord.clear();
			vIndex.clear();
			sGroupName.clear();
		}

		bool empty() const
		{
			return (
				vPosition.empty() &&
				vNormal.empty() &&
				vTexcoord.empty() &&
				vIndex.empty() &&
				sGroupName.empty());
		}

		void checkFaceType(FaceType ft)
		{
			faceType == 0 ? faceType = ft : assert(faceType == ft);
		}
	};

	struct Object
	{
		std::vector<Group>	vGroup;
		std::string			sObjectName;

		void clear()
		{
			vGroup.clear();
			sObjectName.clear();
		}

		bool empty() 
		{
			return vGroup.empty() && sObjectName.empty();
		}
	};
	struct Material {
		struct float3 {
			union {
				float x, y, z;
				float v[3];
			};			
		};
		std::string fileName;
		float3 Ka, Kd, Ks, Ke;
		float Ns, Ni, d;
		int illum;
		float bump_multiplyer;
		std::string map_Ka;
		std::string map_Kd;
		std::string map_Ks;
		std::string map_Ns;
		std::string map_d;
		std::string map_bump;
		std::string map_disp;
		std::string map_decal;

		void clear() {
			fileName.clear();
			map_Ka.clear();
			map_Kd.clear();
			map_Ks.clear();
			map_Ns.clear();
			map_d.clear();
			map_bump.clear();
			map_disp.clear();
			map_decal.clear();
		}
	};
	// Output
	std::vector<Object>		vObject_;
	std::vector<Material>	vMaterial_;
	std::string				materialName_;
	// tmp
	std::vector<float>		vPosition_;
	std::vector<float>		vNormal_;
	std::vector<float>		vTexcoord_;
	int						iPosChn_;
	int						iNormChn_;
	int						iTCChn_;
	Object					obj_;
	Group					grp_;
	std::string				path_;

	void parse(std::fstream &obj);
	void parseMaterial() {
		std::fstream f(path_ + materialName_, std::fstream::in);
		if (!f) {
			assert(0);
			return;
		}
		const int MAX_LINE_SIZE = 256;
		char line[MAX_LINE_SIZE];
		Material m;
		do {
			f.getline(line, MAX_LINE_SIZE);
			char *cur = line;
			switch (*cur)
			{
			case 'n':
			{
				if (strncmp(cur, "newmtl", 6) != 0) {
					assert(0);
					break;
				}
				if (!m.fileName.empty()) {
					vMaterial_.push_back(std::move(m));
				}
				m.clear();
				m.fileName.assign(cur + 7);
				break;
			}
			case 'K':
			{
				auto readFloat3 = [](char *p, Material::float3 &in) {
					char *pRemainder = p;
					char delimiter[] = " ";
					for (int i = 0; i < 3; i++) {
						char *pFloat = strtok_s(pRemainder, delimiter, &pRemainder);
						in.v[i] = static_cast<float>(atof(pFloat));
					}					
				};
				cur++;
				switch (*cur)
				{
				case 'a':
				{
					readFloat3(cur + 2, m.Ka);
					break;
				}
				case 'd':
				{
					readFloat3(cur + 2, m.Kd);
					break;
				}
				case 's':
				{
					readFloat3(cur + 2, m.Ks);
					break;
				}
				case 'e':
				{
					readFloat3(cur + 2, m.Ke);
					break;
				}
				default:
					assert(0);
					break;
				}
				break;
			}
			case 'N':
			{
				cur++;
				switch (*cur)
				{
				case 's':
				{
					m.Ns = static_cast<float>(atof(cur + 2));
					break;
				}
				case 'i':
				{
					m.Ni = static_cast<float>(atof(cur + 2));
					break;
				}
				default:
					assert(0);
					break;
				}
				break;
			}
			case 'd':
			{
				m.d = static_cast<float>(atof(cur + 2));
				break;
			}
			case 'i':
			{
				if (strncmp(cur, "illum", 5) != 0) {
					assert(0);
					break;
				}
				m.illum = atoi(cur + 6);
				break;
			}
			case 'm':
			{
				if (strncmp(cur, "map_Ka ", 7) == 0) {
					m.map_Ka.assign(cur + 8);
				}
				else if (strncmp(cur, "map_Kd ", 7) == 0) {
					m.map_Kd.assign(cur + 8);
				}
				else if (strncmp(cur, "map_Ks ", 7) == 0) {
					m.map_Ks.assign(cur + 8);
				}
				else if (strncmp(cur, "map_Ns ", 7) == 0) {
					m.map_Ns.assign(cur + 8);
				}
				else if (strncmp(cur, "map_d ", 6) == 0) {
					m.map_d.assign(cur + 7);
				}
				else if (strncmp(cur, "map_bump ", 9) == 0) {
					cur += 10;
					char delimiter[] = " ";
					char *pRemainder;					
					char *pBeg;
					if (pBeg = strtok_s(cur, delimiter, &pRemainder)) {
						m.map_bump.assign(pBeg, pRemainder);
					}
					else {
						assert(0);
					}
					// params
					if (strncmp(pRemainder, "-bm", 3) == 0) {
						pRemainder += 4;
						if (pBeg = strtok_s(pRemainder, " ", &pRemainder)) {
							m.bump_multiplyer = static_cast<float>(atof(pBeg));
						}
						else {
							assert(0);
						}
					}
				}
				else if (strncmp(cur, "map_disp", 8) == 0) {
					m.map_disp.assign(cur + 9);
				}
				else if (strncmp(cur, "map_decal", 9) == 0) {
					m.map_decal.assign(cur + 10);
				}
				else {
					assert(0);
				}
				break;
			}
			case 'b':
			{
				if (strncmp(cur, "bump ", 5) != 0) {
					assert(0);
				}
				char delimiter[] = " ";
				char *pRemainder = cur + 6;
				char *pBeg;
				if (pBeg = strtok_s(pRemainder, delimiter, &pRemainder)) {
					m.map_bump.assign(pBeg, pRemainder);
				}
				else {
					assert(0);
				}
				// params
				if (strncmp(pRemainder, "-bm", 3) == 0) {
					pRemainder += 4;
					if (pBeg = strtok_s(pRemainder, " ", &pRemainder)) {
						m.bump_multiplyer = static_cast<float>(atof(pBeg));
					}
					else {
						assert(0);
					}
				}

				break;
			}
			case 'a':
			{
				// unknown cmd
				if (strncmp(cur, "alpha_test ", 11) != 0) {
					assert(0);
				}
				break;
			}
			case 'T':
			{
				// unknown cmd
				if (strncmp(cur, "Tf ", 3) != 0) {
					assert(0);
				}
				break;
			}
			case '\0':
				break;
			default:
			{
				assert(0);
				break;
			}
			}
		} while (!f.eof());
		vMaterial_.push_back(std::move(m));
	}

	inline int readFloats(char *p, std::vector<float> &vec)
	{
		int i = 0; --p;
		do {
			vec.push_back(static_cast<float>(atof(++p)));
			i++;
		} while (NULL != (p = strchr(p, ' ')));

		return i;
	}

	inline void pushPos(Group &g, int faceIdx)
	{
		int offset = (--faceIdx)*iPosChn_;
		for (int i = 0; i < iPosChn_; i++)
			g.vPosition.push_back(vPosition_[offset + i]);
	}

	inline void pushNorm(Group &g, int faceIdx)
	{
		int offset = (--faceIdx)*iNormChn_;
		for (int i = 0; i < iNormChn_; i++)
			g.vNormal.push_back(vNormal_[offset + i]);
	}

	inline void pushTexcoord(Group &g, int faceIdx)
	{
		int offset = (--faceIdx)*iTCChn_;
		for (int i = 0; i < iTCChn_; i++)
			g.vTexcoord.push_back(vTexcoord_[offset + i]);
	}

	void readFaceElement(char *p, Group &g);

	inline void endGrp()
	{
		Group &g = grp_;
		Object &o = obj_;

		g.iPosChn = iPosChn_;
		g.iNormChn = iNormChn_;
		g.iTCChn = iTCChn_;
		o.vGroup.push_back(g);
		g.clear();
	}

	inline void endObj() 
	{
		Object &o = obj_;

		vObject_.push_back(o);
		o.clear();
	}
};