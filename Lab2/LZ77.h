#pragma once

#include "stat.h"
#include "file.h"
#include "criterions.h"

#include <string>
#include <sstream>
#include <iostream>

using namespace std;

bool findMatching(const string& text, int startWord, int endWord, int startFind, int endFind, int& index, int& length)
{
	int count = 0;
	int startIndex = 0;
	for (int i = startFind; i < endFind; i++)
	{
		for (int j = i; j < endFind; j++)
		{
			if (text[j] == text[startWord + count])
			{
				if (count == 0)
					startIndex = j;
				count++;
			}
			else
			{
				if (count > length)
				{
					length = count;
					index = startIndex;
				}
				count = 0;
				break;
			}
		}
	}
	return index != -1;
}

void out(ostream& stream, int value, int byteLength)
{
	for (int i = byteLength - 1; i >= 0; i--)
		stream << (unsigned char)(value >> (byteLength * 8));
}

void outAsText(ostream& stream, int value, int byteLength)
{
	for (int i = byteLength * 8 - 1; i >= 0; i--)
		stream << (char)('0' + ((value >> i) & 1));
}

ComressionResult lz77(const string& text, int lenght = -1, int viewingWindow = 256, int findWindow = 256)  // Алгоритм LZ77
{
	if (lenght == -1 || lenght > text.size())
		lenght = (int)text.size();
	stringstream stream;
	int bitCount = 0;
	int shift = 1;
	int byteLength = (viewingWindow > 256 || findWindow > 256) ? 4 : 1;
	for (int viewIndex = 0; viewIndex < lenght; viewIndex += shift)
	{
		int currentFindWindow = min(lenght - viewIndex, viewingWindow);
		int currentViewingWindow = max(viewIndex - findWindow, 0);
		int index = -1;
		int wordSize = 0;
		if (findMatching(text, viewIndex, currentViewingWindow, currentFindWindow,
			viewIndex, index, wordSize))
		{
			shift = wordSize;
			int offset = viewIndex - index;
			out(stream, offset, byteLength);
			out(stream, wordSize, byteLength);
			bitCount += byteLength * 2 * 8;
		}
		else
		{
			shift = 1;
			out(stream, 0, byteLength);
			out(stream, 0, byteLength);
			out(stream, text[viewIndex], 1);
			bitCount += byteLength * 2 * 8 + 8;
		}
	}

	ComressionResult result;
	result.size = lenght * 8;
	result.compressedSize = stream.str().size(); // bitCount
	result.complited = true;
	return result;
}

ComressionResult lz77(string input, string output, int lenght = 100000,   // Алгорит LZ77 (из файла input в файл output)
	int viewingWindow = 256, int findWindow = 256)
{
	string text = load(input);
	if (lenght < 0 || lenght > text.size())
		lenght = (int)text.size();
	ComressionResult result;
	ofstream outputFile(output);
	int byteLength = (viewingWindow > 256 || findWindow > 256) ? 4 : 1;
	if (!outputFile.fail())
	{
		int bitCount = 0;
		int shift = 0;
		for (int viewIndex = 0; viewIndex < lenght; viewIndex += shift)
		{
			int currentFindWindow = min(lenght - viewIndex, viewingWindow);
			int currentViewingWindow = max(viewIndex - findWindow, 0);
			int index = -1;
			int wordSize = 0;
			if (findMatching(text, viewIndex, currentViewingWindow, currentFindWindow,
				viewIndex, index, wordSize))
			{
				shift = wordSize;
				int offset = viewIndex - index;
				outAsText(outputFile, offset, byteLength);
				outAsText(outputFile, wordSize, byteLength);
				bitCount += byteLength * 2 * 8;
			}
			else
			{
				shift = 1;
				outAsText(outputFile, 0, byteLength);
				outAsText(outputFile, 0, byteLength);
				outAsText(outputFile, text[viewIndex], 1);
				bitCount += byteLength * 2 * 8 + 8;
			}
		}

		outputFile.close();
		result.size = lenght * 8;
		result.compressedSize = bitCount;
		result.complited = true;
	}
	return result;
}
