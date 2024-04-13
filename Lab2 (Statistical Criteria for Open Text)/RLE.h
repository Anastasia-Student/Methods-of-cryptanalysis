#pragma once

#include "file.h"
#include "criterions.h"

#include <string>
#include <sstream>
#include <iostream>

using namespace std;

ComressionResult RLE(const string& text, int lenght = -1)  // Алгоритм RLE
{
	if (lenght == -1 || lenght > text.size())
		lenght = (int)text.size();
	string rle_res = "";
	int num = 1;
	rle_res = rle_res + text[0];
	for (size_t i = 1; i < text.size(); i++) {
		if (text[i - 1] == text[i])
			num++;
		else {
			rle_res = rle_res + to_string(num);
			rle_res = rle_res + text[i];
			num = 1;
		}
	}
	rle_res = rle_res + to_string(num);
	
	ComressionResult result;
	result.size = lenght * 8;
	result.compressedSize = rle_res.size() * 8;
	result.complited = true;
	return result;
}

ComressionResult RLE(string input, string output, int lenght = 100)   // Алгорит RLE (из файла input в файл output)
{
	string text = load(input);
	if (lenght < 0 || lenght > text.size())
		lenght = (int)text.size();
	string rle_res = "";
	int num = 1;
	rle_res = rle_res + text[0];
	for (size_t i = 1; i < text.size(); i++) {
		if (text[i - 1] == text[i])
			num++;
		else {
			rle_res = rle_res + to_string(num);
			rle_res = rle_res + text[i];
			num = 1;
		}
	}
	rle_res = rle_res + to_string(num);

	ofstream outputFile(output);
	if (!outputFile.fail())
	{
		for (size_t i = 0; i < rle_res.size(); i++)  // Проход по тексту
			outputFile << rle_res[i];
	}

	ComressionResult result;
	result.size = lenght * 8;
	result.compressedSize = rle_res.size() * 8;
	result.complited = true;
	return result;
}

ComressionResult RLE_Bit(const string& text, int lenght = -1)  // Алгоритм RLE_Bit
{
	if (lenght == -1 || lenght > text.size())
		lenght = (int)text.size();

	stringstream stream;
	for (size_t j = 0; j < lenght; j++)
		for (int i = 8 - 1; i >= 0; i--)
			stream << (char)('0' + ((text[j] >> i) & 1));

	string bit_text = stream.str();
	string rle_res = "";
	int num = 1;
	rle_res = rle_res + bit_text[0];
	for (size_t i = 1; i < bit_text.size(); i++) {
		if (bit_text[i - 1] == bit_text[i])
			num++;
		else {
			rle_res = rle_res + to_string(num);
			rle_res = rle_res + bit_text[i];
			num = 1;
		}
	}
	rle_res = rle_res + to_string(num);

	ComressionResult result;
	result.size = lenght * 8;
	result.compressedSize = rle_res.size() * 8;
	result.complited = true;
	return result;
}

ComressionResult RLE_Bit(string input, string output, int lenght = 100)   // Алгорит RLE_Bit (из файла input в файл output)
{
	string text = load(input);
	if (lenght < 0 || lenght > text.size())
		lenght = (int)text.size();
	stringstream stream;
	for (size_t j = 0; j < lenght; j++)
		for (int i = 8 - 1; i >= 0; i--)
			stream << (char)('0' + ((text[j] >> i) & 1));

	string bit_text = stream.str();
	string rle_res = "";
	int num = 1;
	rle_res = rle_res + bit_text[0];
	for (size_t i = 1; i < bit_text.size(); i++) {
		if (bit_text[i - 1] == bit_text[i])
			num++;
		else {
			rle_res = rle_res + to_string(num);
			rle_res = rle_res + bit_text[i];
			num = 1;
		}
	}
	rle_res = rle_res + to_string(num);

	ofstream outputFile(output);
	if (!outputFile.fail())
	{
		for (size_t i = 0; i < rle_res.size(); i++)  // Проход по тексту
			outputFile << rle_res[i];
	}

	ComressionResult result;
	result.size = lenght * 8;
	result.compressedSize = rle_res.size() * 8;
	result.complited = true;
	return result;
}
