#include <iostream>
#include "EasyBMP.h"
#include "GrayscaleCompression.h"
#include "ColourCompression.h"
#include "GrayscaleDecompression.h"
#include "ColourDecompression.h"

void loadBmp(BMP &image);
void compression(int colortype, BMP &image);
void decompression(BMP &image);

int main()
{
	BMP image;
	int worktype;

	std::cout << "1. Compression" << std::endl;
	std::cout << "2. Decompression" << std::endl;
	std::cout << "Peek one: ";
	std::cin >> worktype;

	switch (worktype)
	{
	case 1:
	{
		int colortype;

		std::cout << "Do you want to work with grayscale?" << std::endl;
		std::cout << "1. Yes" << std::endl;
		std::cout << "2. No" << std::endl;
		std::cout << "Peek one: ";
		std::cin >> colortype;

		if (colortype < 1 || colortype > 2)
		{
			std::cout << "What part of 'peek one' is so hard to understand?" << std::endl;
			exit(0);
		}

		compression(colortype, image);
		break;
	}

	case 2:
		decompression(image);
		break;

	default:
		std::cout << "What part of 'peek one' is so hard to understand?" << std::endl;
		exit(0);
	}

	return EXIT_SUCCESS;
}

void loadBmp(BMP &image)
{
	std::string bmpname;

	std::cout << "File name (with .bmp): ";
	std::cin >> bmpname;

	if (bmpname[bmpname.size() - 1] != 'p' || bmpname[bmpname.size() - 2] != 'm' || bmpname[bmpname.size() - 3] != 'b' || bmpname[bmpname.size() - 4] != '.')
	{
		std::cout << "Not .bmp file. Bye." << std::endl;
		exit(0);
	}

	image.ReadFromFile(bmpname.c_str());
}

void compression(const int colortype, BMP &image)
{
	loadBmp(image);

	switch (colortype)
	{
	case 1:
	{
		std::vector<uint8_t> grayscale;

		GrayscaleCompression::readFromBmPtoListG(image, grayscale);
		GrayscaleCompression::lz77CompressionG(grayscale, image);

		grayscale.clear();
		break;

	}
	case 2:
	{
		std::vector<uint8_t> pixelRed;
		std::vector<uint8_t> pixelGreen;
		std::vector<uint8_t> pixelBlue;

		ColourCompression::readFromBmPtoList(image, pixelRed, pixelGreen, pixelBlue);
		ColourCompression::lz77Compression(pixelRed, pixelGreen, pixelBlue, image);

		pixelRed.clear();
		pixelGreen.clear();
		pixelBlue.clear();

		break;
	}
	default:
		break;
	}
}

void decompression(BMP &image)
{
	std::string inputName, outputName;

	std::cout << "File to decompress name: ";
	std::cin >> inputName;

	std::ifstream inputFile(inputName.c_str(), std::ifstream::binary);

	if (!inputFile)
	{
		std::cout << "Can't open the file. Error." << std::endl;
		exit(0);
	}

	std::string kkamg;
	int fileSize, colour, infoHeaderSize, offset, width, height, imageSize, character, spaceForColoursTable;

	inputFile.read(reinterpret_cast<char*>(&character), 1);
	kkamg += character;
	inputFile.read(reinterpret_cast<char*>(&character), 1);
	kkamg += character;
	inputFile.read(reinterpret_cast<char*>(&character), 1);
	kkamg += character;
	inputFile.read(reinterpret_cast<char*>(&character), 1);
	kkamg += character;
	inputFile.read(reinterpret_cast<char*>(&character), 1);
	kkamg += character;

	if (kkamg != "kkamg")
	{
		std::cout << "Its not *.kkamg file. Go home." << std::endl;
		exit(1);
	}

	inputFile.read(reinterpret_cast<char*>(&fileSize), 4);
	inputFile.read(reinterpret_cast<char*>(&colour), 4);
	inputFile.read(reinterpret_cast<char*>(&offset), 4);

	inputFile.read(reinterpret_cast<char*>(&infoHeaderSize), 4);
	inputFile.read(reinterpret_cast<char*>(&width), 4);
	inputFile.read(reinterpret_cast<char*>(&height), 4);
	inputFile.read(reinterpret_cast<char*>(&imageSize), 4);
	inputFile.read(reinterpret_cast<char*>(&spaceForColoursTable), 16);

	image.SetSize(width, height);
	const auto codesize = height * width;

	switch (colour)
	{
	case 1:
	{
		std::vector<uint8_t> grayscale;

		GrayscaleDecompression::lz77OutG(grayscale, codesize, inputFile);
		GrayscaleDecompression::writeFromListToBmpg(image, grayscale);

		grayscale.clear();
		break;
	}

	case 2:
	{
		std::vector<uint8_t> pixelRed;
		std::vector<uint8_t> pixelGreen;
		std::vector<uint8_t> pixelBlue;

		ColourDecompression::lz77Out(pixelRed, pixelGreen, pixelBlue, codesize, inputFile);
		ColourDecompression::writeFromListToBmp(image, pixelRed, pixelGreen, pixelBlue);

		pixelRed.clear();
		pixelGreen.clear();
		pixelBlue.clear();
		break;
	}

	default:
		break;
	}

	std::cout << "Output file name (.bmp): ";
	std::cin >> outputName;

	image.WriteToFile(outputName.c_str());

	if (outputName[outputName.size() - 1] != 'p' || outputName[outputName.size() - 2] != 'm' || outputName[outputName.size() - 3] != 'b' || outputName[outputName.size() - 4] != '.')
	{
		std::cout << "Not .bmp file. Ayyy." << std::endl;
		exit(1);
	}

	inputFile.close();
}