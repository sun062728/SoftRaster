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
		std::fstream bmp(path, std::fstream::in);
		if (!bmp) {
			assert(0);
			return;
		}
		std::fstream loader(path, std::fstream::in | std::fstream::binary);
		loader.read(reinterpret_cast<char*>(&fileHeader_), sizeof(fileHeader_));
		loader.read(reinterpret_cast<char*>(&infoHeader_), sizeof(infoHeader_));
		if (infoHeader_.biCompression != BI_RGB)
			assert(0);
		unsigned int w = infoHeader_.biWidth;
		unsigned int h = infoHeader_.biHeight;
		data_.reserve(w*h * 3);
		for (unsigned int i = 0; i < w*h; i++) {
			unsigned char pixel[3];
			loader.read((char*)pixel, 3);
			data_.push_back(pixel[2]);
			data_.push_back(pixel[1]);
			data_.push_back(pixel[0]);
		}
		bmp.close();
	}
	BITMAPINFOHEADER getInfo() const
	{
		return infoHeader_;
	}
	void* getData() const
	{
		return (void*)data_.data();
	}
	unsigned int getWidth() const
	{
		return infoHeader_.biWidth;
	}
	unsigned int getHeight() const
	{
		return infoHeader_.biHeight;
	}
private:
	BITMAPFILEHEADER fileHeader_;
	BITMAPINFOHEADER infoHeader_;
	std::vector<unsigned int> data_;
};