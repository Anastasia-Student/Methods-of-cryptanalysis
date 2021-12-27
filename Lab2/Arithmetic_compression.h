#pragma once
#include "stat.h"
#include "file.h"
#include "criterions.h"
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

string doubleToString(double number)  // Преобразование double в строку
{
	stringstream stream;
	stream << fixed << setprecision(64) << number;  // 64 знака после запятой
	return stream.str();
}

struct Range  // Диaпазон
{
	double from;  // От
	double to;  // До
};

ComressionResult arithmeticCoding(const string& text, map<Letter, Frequancy>& letterCounters, int lenght = -1)  // Арифмитическое кодирование
{
	if (lenght == -1 || lenght > text.size()) 
		lenght = (int)text.size(); 
	stringstream stream;
	int bitCount = 0; 
	for (int t = 0; t < text.size(); t += 128)  // Разделение текста на блоки по 128 символов (так как double имеет ограниченную точность)
	{
		double start = 0.0;
		double end = 1.0;
		string part = text.substr(t, min(128, (int)text.size() - t));  // Кусок 128 символов
		map<char, Range> ranges;  // Диапазоны для букв
		double from = 0.0;  // С 0
		for (int i = 0; i < part.size(); i++)  // Пройтись по буквам
		{
			if (ranges.find(part[i]) == ranges.end())  // Если буквы нет в map
			{
				ranges[part[i]] = { from, from + letterCounters[part[i]].frequancy };  // Добавить диапазон
				from += letterCounters[part[i]].frequancy;  // Сдвинуть на частоту
			}
		}
		for (int i = 0; i < part.size(); i++)  // Пройтись по буквам
		{
			start = ranges[part[i]].from;  // Начало дипазона
			end = ranges[part[i]].to;  // Конец диапазона
			auto from = start;  // С начала
			map<char, Range> localRanges;  // Новые диапазоны
			for (int j = 0; j < part.size(); j++)   // Пройтись по буквам
			{
				if (localRanges.find(part[j]) == localRanges.end())  // Если буквы нет в map
				{
					auto frequancy = letterCounters[part[j]].frequancy * (end - start);  // Пересчет частоты
					localRanges[part[j]] = { from, from + frequancy };  // Добавить диапазон
					from += frequancy;  // Сдвинуть на частоту
				}
			}
			ranges = localRanges;  // Новые диапазоны
		}
		string startCode = doubleToString(start);  // double в строку
		if (startCode.size() >= 2 && startCode.substr(0, 2) == "0.")  // Убрать 0.
			startCode = startCode.substr(2, startCode.length() - 2);  // Убрать 0.
		string endCode = doubleToString(end);  // double в строку
		if (endCode.size() >= 2 && endCode.substr(0, 2) == "0.")  // Убрать 0.
			endCode = endCode.substr(2, endCode.length() - 2);  // Убрать 0.
		for (int i = 0; i < min(startCode.length(), endCode.length()); i++)  // Пройтись по строкам
			if (startCode[i] != endCode[i])  // Найти первую несовпадающую цифру
				startCode = startCode.substr(0, i);  // Обрезать лишнее
		stream << startCode;  // Сохранить строку в поток
		bitCount += startCode.length() * 8;  // Подсчет бит
	}
	ComressionResult result;
	result.size = lenght * 8;
	result.compressedSize = bitCount;
	result.complited = true;
	return result;
}

ComressionResult arithmeticCoding(string input, string output, map<Letter, Frequancy>& letterCounters, int lenght = 100000)  // Арифмитическое кодирование (из файла input в файл output)
{
	string text = load(input);  
	if (lenght < 0 || lenght > text.size()) 
		lenght = (int)text.size();
	ofstream outputFile(output); 
	int bitCount = 0;
	for (int t = 0; t < lenght; t += 128)  // Разделение текста на боли по 128 символов (так как double имеет ограниченную точность)
	{
		double start = 0.0;
		double end = 1.0;
		string part = text.substr(t, min(128, lenght - t));  // Кусок 128 символов
		map<char, Range> ranges;  // Диапазоны для букв
		double from = 0.0;  // С 0
		for (int i = 0; i < part.size(); i++)  // Пройтись по буквам
		{
			if (ranges.find(part[i]) == ranges.end())  // Если буквы нет в map
			{
				ranges[part[i]] = { from, from + letterCounters[part[i]].frequancy };  // Добавить диапазон
				from += letterCounters[part[i]].frequancy;  // Сдвинуть на частоту
			}
		}
		for (int i = 0; i < part.size(); i++)  // Пройтись по буквам
		{
			start = ranges[part[i]].from;  // Начало диапазона
			end = ranges[part[i]].to;  // Конец диапазона
			auto from = start;  // С начала
			map<char, Range> localRanges;  // Новые диапазоны
			for (int j = 0; j < part.size(); j++)   // Пройтись по буквам
			{
				if (localRanges.find(part[j]) == localRanges.end())  // Если буквы нет в map
				{
					auto frequancy = letterCounters[part[j]].frequancy * (end - start);  // Пересчет частоты
					localRanges[part[j]] = { from, from + frequancy };  // Добавить диапазон
					from += frequancy;  // Сдвинуть на частоту
				}
			}
			ranges = localRanges;  // Новые диапазоны
		}
		string startCode = doubleToString(start);  // double в строку
		if (startCode.size() >= 2 && startCode.substr(0, 2) == "0.")  // Убрать 0.
			startCode = startCode.substr(2, startCode.length() - 2);  // Убрать 0.
		string endCode = doubleToString(end);  // double в строку
		if (endCode.size() >= 2 && endCode.substr(0, 2) == "0.")  // Убрать 0.
			endCode = endCode.substr(2, endCode.length() - 2);  // Убрать 0.
		for (int i = 0; i < min(startCode.length(), endCode.length()); i++)  // Пройтись по строкам
			if (startCode[i] != endCode[i])  // Найти первую несовпадающую цифру
				startCode = startCode.substr(0, i);  // Обрезать лишнее
		for (int i = 0; i < startCode.size(); i++)  // Пройтись по строкам
			for (int j = 7; j >= 0; j--)  // Пройтись по битам 
				outputFile << (((startCode[i] >> j) & 1) ? '1' : '0');  // Запись бита
		bitCount += startCode.length() * 8;  // Подсчет бит
	}
	outputFile.close(); 
	ComressionResult result;
	result.size = lenght * 8;
	result.compressedSize = bitCount;
	result.complited = true;
	return result; 
}
