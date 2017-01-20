#include "ObjLoader.hpp"

ObjLoader::ObjLoader(std::string filePath)
{
	// save file text into buf
	std::fstream obj(filePath, std::ios::in);
	if (!obj)
	{
		DebugBreak();
		return;
	}

	parse(obj);
}

void ObjLoader::parse(std::fstream &obj)
{
	const int MAX_LINE_SIZE = 256;
	char line[MAX_LINE_SIZE];
	Object &o = m_obj;
	Group  &g = m_grp;

	obj.getline(line, MAX_LINE_SIZE);

	while (!obj.eof())
	{
		char *cur = line;
		switch (*cur++)
		{
		case 'v':
			switch (*cur++)
			{
			case ' ':
			{
				int i = readFloats(cur, m_vPosition);
				m_iPosChn == 0 ? m_iPosChn = i : assert(m_iPosChn == i);
				break;
			}
			case 'n':
			{
				int i = readFloats(++cur, m_vNormal);
				m_iNormChn == 0 ? m_iNormChn = i : assert(m_iNormChn == i);
				break;
			}
			case 't':
			{
				int i = readFloats(++cur, m_vTexcoord);
				m_iTCChn == 0 ? m_iTCChn = i : assert(m_iTCChn == i);
				break;
			}
			case 'p': break;
			default:
				DebugBreak();
				break;
			}
			break;
		case 'f':
		{
			char buf[MAX_LINE_SIZE];
			char *p;
			int i = 0, length = 0;
			errno_t err = 0;
			cur++;
			for (; NULL != (p = strchr(cur, ' ')); i++)
			{
				length = p - cur;
				err = strcpy_s(buf, MAX_LINE_SIZE, cur);
				buf[length] = '\0';
				readFaceElement(buf, g);
				cur = p + 1;
			}
			assert(i == 2); // only support triangle
			p = strchr(cur, '\0'); assert(p);
			length = p - cur;
			err = strcpy_s(buf, MAX_LINE_SIZE, cur);
			buf[length] = '\0';
			readFaceElement(buf, g);

			break;
		}
		case 'm': break; // material file
		case 'u': break; // material instance
		case 'o': // object name
		{
			if (!o.empty())
			{
				endGrp();
				endObj();
			}
			o.sObjectName.assign(++cur);
			break;
		}
		case 'g': // group name
		{
			if (!g.empty())
			{
				endGrp();
			}
			g.sGroupName.assign(++cur);
			break;
		}
		case 's': break; // if smooth shading
		case '#': // comment
			break;
		case '\0':
			break;
		default:
			DebugBreak();
			break;
		}
		obj.getline(line, MAX_LINE_SIZE);
	}
	endGrp();
	endObj();
}

void ObjLoader::readFaceElement(char *p, Group &g)
{
	char *pSlash1, *pSlash2;
	if (pSlash1 = strchr(p, '/'))
	{
		if (pSlash2 = strchr(pSlash1 + 1, '/'))
		{
			if (pSlash2 == pSlash1 + 1) // v//vn
			{
				g.checkFaceType(Group::FaceType::FT_VN);
				int idx = atoi(p);
				pushPos(g, idx);
				idx = atoi(pSlash2 + 1);
				pushNorm(g, idx);
			}
			else // v/vt/vn
			{
				g.checkFaceType(Group::FaceType::FT_VTN);
				int idx = atoi(p);
				pushPos(g, idx);
				idx = atoi(pSlash1 + 1);
				pushTexcoord(g, idx);
				idx = atoi(pSlash2 + 1);
				pushNorm(g, idx);
			}
		}
		else // v/vt
		{
			g.checkFaceType(Group::FaceType::FT_VT);
			int idx = atoi(p);
			pushPos(g, idx);
			idx = atoi(pSlash1 + 1);
			pushTexcoord(g, idx);
		}
	}
	else // v
	{
		g.checkFaceType(Group::FaceType::FT_V);
		int idx = atoi(p);
		g.vPosition.push_back(m_vPosition[idx - 1]);
	}
}
