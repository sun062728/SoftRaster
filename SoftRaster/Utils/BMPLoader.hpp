#pragma once

#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>	// just for bmp header

class BMPLoader
{
public:
	BMPLoader(std::string const &path)
	{
		std::fstream obj(path, std::ios::in);
		if (!obj) {
			assert(0);
			return;
		}
		std::fstream loader("Bitmap2Material_3_Base_Color.bmp", std::fstream::in | std::fstream::binary);
		loader.read(reinterpret_cast<char*>(&fileHeader_), sizeof(fileHeader_));
		loader.read(reinterpret_cast<char*>(&infoHeader_), sizeof(infoHeader_));
		
	}
	BITMAPINFOHEADER getInfo() const
	{
		return infoHeader_;
	}
	std::vector<unsigned int> getData() const
	{
		return data_;
	}
private:
	BITMAPFILEHEADER fileHeader_;
	BITMAPINFOHEADER infoHeader_;
	std::vector<unsigned int> data_;
};