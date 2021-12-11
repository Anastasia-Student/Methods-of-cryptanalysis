#pragma once
#include "stat.h"
#include "file.h"
#include "criterions.h"
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

using namespace std;

typedef vector<unsigned char> Code;

double rangeSumm(const vector<pair<Letter, Frequancy>>& counters, int start, int end)  // Сумма частот на промежутке
{
	double summ = 0;
	for (int i = start; i < end; i++)  // От start до end
		summ += counters[i].second.frequancy;  // Суммирование частот
	return summ;
}

void shannonFanoCodesGenerate(const vector<pair<Letter, Frequancy>>& counters,  // Генерация двоичных кодов
	int start, int end, map<Letter, Code>& codes, vector<unsigned char> bits)
{
	if (end - start == 1)  // Если промежуток состоит из одного символа
		codes[counters[start].first] = bits;  // Сохранение битов
	else
	{
		int minDifferensPosition = start + 1;  // Позиция середины сумм
		double previousMinDifferens = abs(rangeSumm(counters, start, start + 1) - // Подсчет середины сумм
			rangeSumm(counters, start + 1, end));
		for (int i = start + 1; i < end; i++)  // Проход по промежутку
		{
			double defferent = abs(rangeSumm(counters, start, i) - rangeSumm(counters, i, end)); // Подсчет середины сумм
			if (defferent > previousMinDifferens) // Если зашли дальше середины
				break;  // Выход из цикла
			previousMinDifferens = defferent;
			minDifferensPosition = i;
		}
		bits.push_back(1);
		shannonFanoCodesGenerate(counters, start, minDifferensPosition, codes, bits);  // Влево
		bits[bits.size() - 1] = 0;
		shannonFanoCodesGenerate(counters, minDifferensPosition, end, codes, bits);  // Вправо
	}
}

ComressionResult shannonFano(const string& text)  // Алгоритм Шеннона-Фано 
{
	auto countersMap = letterFrequency(text);  // Подсчет частот букв
	vector<pair<Letter, Frequancy>> counters(countersMap.size());
	for (auto counter : countersMap)
		counters.push_back(counter);
	copy(countersMap.begin(), countersMap.end(), counters.begin());  // Копирование частот в вектор
	sort(counters.begin(), counters.end(), [](const pair<Letter, Frequancy>& a,  // Сортировка по частотам
		const pair<Letter, Frequancy>& b) -> bool { return a.second.frequancy > b.second.frequancy; });
	map<Letter, Code> codes;
	shannonFanoCodesGenerate(counters, 0, (int)counters.size(), codes, vector<unsigned char>());  // Генерация кодов
	stringstream stream;  // Поток для записи
	ComressionResult result;  // Результат
	result.size = text.size() * sizeof(Letter) * 8;  // Размер текста
	for (size_t i = 0; i < text.size(); i++)  // Проход по тексту
		for (size_t j = 0; j < codes[text[i]].size(); j++, result.compressedSize++)  // Проход по битам и их запись
			stream << (char)('0' + codes[text[i]][j]);
	result.complited = true;
	return result;
}

ComressionResult shannonFano(string input, string output)  // Алгорит Шеннона-Фано (из файла input в файл output)
{
	auto text = load(input);
	auto countersMap = letterFrequency(text);  // Подсчет частот букв
	vector<pair<Letter, Frequancy>> counters(countersMap.size());
	for (auto counter : countersMap)
		counters.push_back(counter);
	copy(countersMap.begin(), countersMap.end(), counters.begin());  // Копирование частот в вектор
	sort(counters.begin(), counters.end(), [](const pair<Letter, Frequancy>& a,  // Сортировка по частотам
		const pair<Letter, Frequancy>& b) -> bool { return a.second.frequancy > b.second.frequancy; });
	map<Letter, Code> codes;
	shannonFanoCodesGenerate(counters, 0, (int)counters.size(), codes, vector<unsigned char>());  // Генерация кодов
	ComressionResult result;  // Результат
	result.size = text.size() * sizeof(Letter) * 8;  // Размер текста
	ofstream outputFile(output);
	if (!outputFile.fail())
	{
		for (size_t i = 0; i < text.size(); i++)  // Проход по тексту
			for (size_t j = 0; j < codes[text[i]].size(); j++, result.compressedSize++)  // Проход по битам и их запись
				outputFile << (char)('0' + codes[text[i]][j]);
		outputFile.close();
		result.complited = true;
		result.size = text.size() * 8;
	}
	return result;
}
