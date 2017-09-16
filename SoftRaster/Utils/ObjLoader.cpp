#include "ObjLoader.hpp"

ObjLoader::ObjLoader(std::string const &filePath) 
{
	std::fstream obj(filePath, std::fstream::in);
	if (!obj) {
		assert(0);
		return;
	}
	path_.assign(filePath, 0, filePath.rfind('/') + 1);
	parse(obj);
}

void ObjLoader::parse(std::fstream &obj)
{
	const int MAX_LINE_SIZE = 256;
	char line[MAX_LINE_SIZE];
	Object &o = obj_;
	Group  &g = grp_;

	do{
		obj.getline(line, MAX_LINE_SIZE);

		char *cur = line;
		switch (*cur++)	{
		case 'v':
			switch (*cur++)
			{
			case ' ':
			{
				//int i = readFloats(cur, vPosition_);
				float fv[3];
				int i = readFloats(cur, fv);
				for (int j = 0; j < 3; j++) {
					vPosition_.push_back(fv[j]);
					if (fv[j] < BBoxMin_[j]) {
						BBoxMin_[j] = fv[j];
					}
					if (fv[j] > BBoxMax_[j]) {
						BBoxMax_[j] = fv[j];
					}
				}
				iPosChn_ == 0 ? iPosChn_ = i : assert(iPosChn_ == i);
				break;
			}
			case 'n':
			{
				int i = readFloats(++cur, vNormal_);
				iNormChn_ == 0 ? iNormChn_ = i : assert(iNormChn_ == i);
				break;
			}
			case 't':
			{
				int i = readFloats(++cur, vTexcoord_);
				iTCChn_ == 0 ? iTCChn_ = i : assert(iTCChn_ == i);
				break;
			}
			case 'p': break;
			default:
				assert(0);
				break;
			}
			break;
		case 'f':
		{
			/*
			char buf[MAX_LINE_SIZE];
			char *p;
			int i = 0, length = 0;
			errno_t err = 0;
			cur++;
			for (; NULL != (p = strchr(cur, ' ')); i++) {
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
			*/
			cur++;
			char buf[MAX_LINE_SIZE];
			char *remainder = cur;
			char *delimiter = " ";
			char *pBeg = nullptr;
			errno_t err = 0;
			for (int i = 0; i < 3; i++) {
				if (pBeg = strtok_s(remainder, delimiter, &remainder)) {
					err = strncpy_s(buf, pBeg, remainder - pBeg);
					readFaceElement(buf, g);
				}
			}			
			break;
		}
		case 'm':
		{
			if (strncmp(cur, "tllib", 5) == 0) {
				cur += 6;
				this->materialName_.assign(cur);
				parseMaterial();
			}
			else {
				assert(0);
			}
			break;
		}
		case 'u':
		{
			
			break;
		}
		case 'o': // object name
		{
			if (!o.empty()) {
				endGrp();
				endObj();
			}
			o.sObjectName.assign(++cur);
			break;
		}
		case 'g': // group name
		{
			if (!g.empty()) {
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
			assert(0);
			break;
		}
	} while (!obj.eof());
	endGrp();
	endObj();
}

void ObjLoader::readFaceElement(char *p, Group &g)
{
	char *pSlash1, *pSlash2;
	if (pSlash1 = strchr(p, '/')) {
		if (pSlash2 = strchr(pSlash1 + 1, '/'))	{
			if (pSlash2 == pSlash1 + 1) { // v//vn
				g.checkFaceType(Group::FaceType::FT_VN);
				int idx = atoi(p);
				pushPos(g, idx);
				idx = atoi(pSlash2 + 1);
				pushNorm(g, idx);
			}
			else { // v/vt/vn
				g.checkFaceType(Group::FaceType::FT_VTN);
				int idx = atoi(p);
				pushPos(g, idx);
				idx = atoi(pSlash1 + 1);
				pushTexcoord(g, idx);
				idx = atoi(pSlash2 + 1);
				pushNorm(g, idx);
			}
		}
		else { // v/vt
			g.checkFaceType(Group::FaceType::FT_VT);
			int idx = atoi(p);
			pushPos(g, idx);
			idx = atoi(pSlash1 + 1);
			pushTexcoord(g, idx);
		}
	}
	else { // v
		g.checkFaceType(Group::FaceType::FT_V);
		int idx = atoi(p);
		g.vPosition.push_back(vPosition_[idx - 1]);
	}
}
