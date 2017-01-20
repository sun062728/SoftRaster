#pragma once

#include <windows.h>
#include <vector>
#include <cassert>
#include <fstream>

class ObjLoader
{
public:
	ObjLoader(std::string filePath);
	int getObjectNum() const
	{
		return this->m_vObject.size();
	}
	int getGroupNum(unsigned int o) const
	{
		if (o >= m_vObject.size() || o < 0)
			return -1;
		return m_vObject[o].vGroup.size();
	}
	bool getMeshName(unsigned int o, unsigned int g, char *p, int len)
	{
		if (o >= m_vObject.size() || o < 0)
			return false;
		if (g >= m_vObject[o].vGroup.size() || g < 0)
			return false;
		char const *s = m_vObject[o].vGroup[g].sGroupName.c_str();
		int size = m_vObject[o].vGroup[g].sGroupName.size();
		strncpy_s(p, len, s, size);
		return true;
	}
	bool getMesh(
		unsigned int o, unsigned int g,
		std::vector<float> &p, int &iPC,
		std::vector<float> &n, int &iNC,
		std::vector<float> &t, int &iTC,
		std::vector<int> &i
	)
	{
		if (o >= m_vObject.size() || o < 0)
			return false;
		if (g >= m_vObject[o].vGroup.size() || g < 0)
			return false;
		Group &grp = m_vObject[o].vGroup[g];
		p = grp.vPosition; iPC = grp.iPosChn;
		n = grp.vNormal; iNC = grp.iNormChn;
		t = grp.vTexcoord; iTC = grp.iTCChn;
		i = grp.vIndex;
		return true;
	}

private:
	class Group
	{
	public:
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

		bool empty()
		{
			return (
				vPosition.empty() &&
				vNormal.empty() &&
				vTexcoord.empty() &&
				vIndex.empty() &&
				sGroupName.empty()
				);
		}

		void checkFaceType(FaceType ft)
		{
			faceType == 0 ? faceType = ft : assert(faceType == ft);
		}
	};

	class Object
	{
	public:
		std::vector<Group>	vGroup;
		std::string			sObjectName;

		Object()
		{}
		~Object()
		{
			clear();
		}

		void clear()
		{
			vGroup.clear();
			sObjectName.clear();
		}

		bool empty()
		{
			return (
				vGroup.empty() &&
				sObjectName.empty()
				);
		}
	};

	// Output
	std::vector<Object>		m_vObject;
	// tmp
	std::vector<float>		m_vPosition;
	std::vector<float>		m_vNormal;
	std::vector<float>		m_vTexcoord;
	int						m_iPosChn;
	int						m_iNormChn;
	int						m_iTCChn;
	Object					m_obj;
	Group					m_grp;

	void parse(std::fstream &obj);

	inline int readFloats(char *p, std::vector<float> &vec)
	{
		int i = 0; --p;
		do
		{
			vec.push_back(static_cast<float>(atof(++p)));
			i++;
		} while (NULL != (p = strchr(p, ' ')));

		return i;
	}

	inline void pushPos(Group &g, int faceIdx)
	{
		int offset = (--faceIdx)*m_iPosChn;
		for (int i = 0; i < m_iPosChn; i++)
			g.vPosition.push_back(m_vPosition[offset + i]);
	}

	inline void pushNorm(Group &g, int faceIdx)
	{
		int offset = (--faceIdx)*m_iNormChn;
		for (int i = 0; i < m_iNormChn; i++)
			g.vNormal.push_back(m_vNormal[offset + i]);
	}

	inline void pushTexcoord(Group &g, int faceIdx)
	{
		int offset = (--faceIdx)*m_iTCChn;
		for (int i = 0; i < m_iTCChn; i++)
			g.vTexcoord.push_back(m_vTexcoord[offset + i]);
	}

	void readFaceElement(char *p, Group &g);

	inline void endGrp()
	{
		Group &g = m_grp;
		Object &o = m_obj;

		g.iPosChn = m_iPosChn;
		g.iNormChn = m_iNormChn;
		g.iTCChn = m_iTCChn;
		o.vGroup.push_back(g);
		g.clear();
	}

	inline void endObj()
	{
		Object &o = m_obj;

		m_vObject.push_back(o);
		o.clear();
	}
};