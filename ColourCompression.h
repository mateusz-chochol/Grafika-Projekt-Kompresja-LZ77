#ifndef COLORCOMPRESSION_H_INCLUDED
#define COLORCOMPRESSION_H_INCLUDED

#include <vector>
#include <fstream>
#include "EasyBMP.h"

class ColourCompression
{
public:
	static void readFromBmPtoList (BMP &image, std::vector<uint8_t> &pixelRed, std::vector<uint8_t> &pixelGreen, std::vector<uint8_t> &pixelBlue)
	{
		for (auto y = 0; y < image.TellHeight(); y++)
			for (auto x = 0; x < image.TellWidth(); x++)
			{
				const auto temp = image.GetPixel(x, y);
				pixelRed.push_back(temp.Red >> 7 << 7);
				pixelGreen.push_back(temp.Green >> 6 << 6);
				pixelBlue.push_back(temp.Blue >> 7 << 7);
			}

		reverse(pixelRed.begin(), pixelRed.end());
		reverse(pixelGreen.begin(), pixelGreen.end());
		reverse(pixelBlue.begin(), pixelBlue.end());
	}

	static int powerModuloFast(const int a, const int b, const int m)
	{
		auto result = 1;
		long int x = a % m;

		for (auto i = 1; i <= b; i <<= 1)
		{
			x %= m;

			if ((b&i) != 0)
			{
				result *= x;
				result %= m;
			}

			x *= x;
		}

		return result % m;
	}

	static void lz77(std::vector<uint8_t> &code, std::ofstream &file)
	{
		const auto r = 256;
		const auto q = 9551;
		
		uint8_t text[32];
		uint8_t buffer[32];
		
		int i, j;

		for (auto& z : text)
			z = code[0];

		file.write(reinterpret_cast<char*>(&code[0]), 1);
		
		while (!code.empty())
		{

			uint8_t length = 0, position = 0;
			
			auto l = 1;
			while (l < 32 && l < code.size())
			{
				auto k = 0;
				
				while (k < l)
				{
					buffer[k] = code[k];
					k++;
				}

				const auto n = 32;
				const auto m = k;

				auto h2 = 0;
				auto h1 = 0;

				for (i = 0; i < m; i++)
				{
					h2 = h2*r + text[i];
					h2 %= q;
				}

				for (i = 0; i < m; i++)
				{
					h1 = h1*r + buffer[i];
					h1 %= q;
				}

				const auto rm = powerModuloFast(r, m - 1, q);
				i = 0;

				while (i < n - m)
				{
					j = 0;

					if (h1 == h2) 
						while (j < m && buffer[j] == text[i + j]) 
							j++;

					if (j == m)
					{
						if (l > length)
						{
							position = i;
							length = l;
						}
					}

					h2 = (h2 - text[i] * rm) * r + text[i + m];
					h2 %= q;

					if (h2 < 0) 
						h2 += q;

					i++;
				}

				j = 0;

				if (h1 == h2) 
					while (j < m && buffer[j] == text[i + j]) 
						j++;

				if (j == m)
				{
					if (l > length)
					{
						position = i;
						length = l;
					}
				}

				l++;
			}

			file.write(reinterpret_cast<char*>(&code[length]), 1);
			file.write(reinterpret_cast<char*>(&position), 1);
			file.write(reinterpret_cast<char*>(&length), 1);

			j = 0;

			for (i = length + 1; i < 32; i++)
			{
				text[j] = text[i];
				j++;
			}

			i = 0;

			while (j < 32)
			{
				text[j] = code[i];
				j++; i++;
			}

			code.erase(code.begin(), code.begin() + length + 1);
		}
	}

	static void lz77Compression(std::vector<uint8_t> &pixelRed, std::vector<uint8_t> &pixelGreen, std::vector<uint8_t> &pixelBlue, BMP &image)
	{
		std::string outputName;

		std::cout << "Output file name: ";
		std::cin >> outputName;

		std::ofstream file(outputName.c_str(), std::ifstream::binary);

		if (!file)
		{
			std::cout << "Can't create the file. Error." << std::endl;
			exit(1);
		}

		const auto kkamg = new char[5]{ 'k', 'k', 'a', 'm', 'g' };
		auto colour = 2;
		auto width = image.TellWidth();
		auto height = image.TellHeight();
		auto fileSize = width * height + 48;
		auto imageSize = width * height;
		auto infoHeaderSize = 32;
		auto offset = 4;
		auto spaceForColoursTable = 16;

		file.write(&kkamg[0], 1);
		file.write(&kkamg[1], 1);
		file.write(&kkamg[2], 1);
		file.write(&kkamg[3], 1);
		file.write(&kkamg[4], 1);
		
		file.write(reinterpret_cast<char*>(&fileSize), 4);
		file.write(reinterpret_cast<char*>(&colour), 4);
		file.write(reinterpret_cast<char*>(&offset), 4);

		file.write(reinterpret_cast<char*>(&infoHeaderSize), 4);
		file.write(reinterpret_cast<char*>(&width), 4);
		file.write(reinterpret_cast<char*>(&height), 4);
		file.write(reinterpret_cast<char*>(&imageSize), 4);
		file.write(reinterpret_cast<char*>(&spaceForColoursTable), 16);

		lz77(pixelRed, file);
		lz77(pixelGreen, file);
		lz77(pixelBlue, file);

		pixelBlue.clear();
		pixelGreen.clear();
		pixelRed.clear();

		file.close();
	}
};


#endif // COLORCOMPRESSION_H_INCLUDED
