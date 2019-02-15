#ifndef BWDECOMPRESSION_H_INCLUDED
#define BWDECOMPRESSION_H_INCLUDED

#include "EasyBMP.h"

class GrayscaleDecompression
{
public:
	static void writeFromListToBmpg(BMP &image, std::vector<uint8_t> &bw)
	{
		RGBApixel temp = { 0, 0, 0, 0 };

		for (auto y = 0; y < image.TellHeight(); y++)
			for (auto x = 0; x < image.TellWidth(); x++)
			{
				temp.Red = bw.back();
				temp.Green = bw.back();
				temp.Blue = bw.back();
				bw.pop_back();

				image.SetPixel(x, y, temp);
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

	static void lz77OutG(std::vector<uint8_t> &bw, const int codesize, std::ifstream &file)
	{
		lz77Decompression(bw, file, codesize);
	}
};


#endif // BWDECOMPRESSION_H_INCLUDED
