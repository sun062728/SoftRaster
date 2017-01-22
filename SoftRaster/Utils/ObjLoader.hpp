#pragma once

#include <vector>
#include <cassert>
#include <fstream>

class ObjLoader
{
public:
	ObjLoader(std::string filePath);
	int getObjectNum() const
	{
		return vObject_.size();
	}
	int getGroupNum(unsigned int o) const 
	{
		if (o >= vObject_.size() || o < 0)
			return -1;
		return vObject_[o].vGroup.size();
	}
	bool getMeshName(unsigned int o, unsigned int g, char *p, int len) const
	{
		if (o >= vObject_.size() || o < 0)
			return false;
		if (g >= vObject_[o].vGroup.size() || g < 0)
			return false;
		char const *s = vObject_[o].vGroup[g].sGroupName.c_str();
		int size = vObject_[o].vGroup[g].sGroupName.size();
		strncpy_s(p, len, s, size);
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

	// Output
	std::vector<Object>		vObject_;
	// tmp
	std::vector<float>		vPosition_;
	std::vector<float>		vNormal_;
	std::vector<float>		vTexcoord_;
	int						iPosChn_;
	int						iNormChn_;
	int						iTCChn_;
	Object					obj_;
	Group					grp_;

	void parse(std::fstream &obj);

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