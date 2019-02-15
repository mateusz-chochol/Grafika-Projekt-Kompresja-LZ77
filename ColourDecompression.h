#ifndef COLORDECOMPRESSION_H_INCLUDED
#define COLORDECOMPRESSION_H_INCLUDED

#include <vector>
#include <fstream>
#include "EasyBMP.h"

class ColourDecompression
{
public:
	static void writeFromListToBmp(BMP &image, std::vector<uint8_t> &pixelRed, std::vector<uint8_t> &pixelGreen, std::vector<uint8_t> &pixelBlue)
	{
		RGBApixel temp { 0, 0, 0, 0 };
		
		for (auto y = 0; y < image.TellHeight(); y++)
		{
			for (auto x = 0; x < image.TellWidth(); x++)
			{
				temp.Red = pixelRed.back();
				pixelRed.pop_back();

				temp.Green = pixelGreen.back();
				pixelGreen.pop_back();

				temp.Blue = pixelBlue.back();
				pixelBlue.pop_back();

				image.SetPixel(x, y, temp);
			}
		}
	}

	static void lz77Decompression(std::vector<uint8_t> &code, std::ifstream &file, const int codesize)
	{
		int data, i;
		uint8_t text[32], position, length;

		auto ppose = 0;

		file.read(reinterpret_cast<char*>(&data), 1);

		for (auto& z : text)
			z = data;

		while (ppose < codesize)
		{
			file.read(reinterpret_cast<char*>(&data), 1);
			file.read(reinterpret_cast<char*>(&position), 1);
			file.read(reinterpret_cast<char*>(&length), 1);
			int j = position;
			
			for (i = 0; i < length; i++)
			{
				code.push_back(text[j]);
				j++;
			}

			code.push_back(data);

			j = 0;

			for (i = length + 1; i < 32; i++)
			{
				text[j] = text[i];
				j++;
			}

			for (i = ppose; i < (ppose + length + 1); i++)
			{
				text[j] = code[i];
				j++;
			}

			ppose = ppose + length + 1;
		}
	}

	static void lz77Out(std::vector<uint8_t> &pixelRed, std::vector<uint8_t> &pixelGreen, std::vector<uint8_t> &pixelBlue, const int codesize, std::ifstream &file)
	{
		lz77Decompression(pixelRed, file, codesize);
		lz77Decompression(pixelGreen, file, codesize);
		lz77Decompression(pixelBlue, file, codesize);
	}
};

#endif // COLORDECOMPRESSION_H_INCLUDED
