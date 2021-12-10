#pragma once

#include "alphabet.h"

#include <string>

using namespace std;

struct Frequancy  // Частота
{
	double frequancy = 0.0;  // Частота
	int number = 0;  // Количество вхождений
};

map<Letter, Frequancy> letterFrequency(const string& text, int lenght = -1)  // Подсчет количества букв
{
	if (lenght == -1 || lenght > (int)text.size())
		lenght = (int)text.size();
	map<Letter, Frequancy> frequencies;
	for (int i = 0; i < lenght; i++)
		frequencies[text[i]].number++;  // Подсчет
	for (auto& frequancy : frequencies)
		frequancy.second.frequancy = frequancy.second.number / (double)lenght;  // Расчет частоты
	return frequencies;
}

map<Bigram, Frequancy> bigramFrequency(const string& text, int lenght = -1)  // Подсчет количества биграмм
{
	if (lenght == -1 || lenght > (int)text.size())
		lenght = (int)text.size();
	map<Bigram, Frequancy> frequencies;
	for (int i = 0; i < lenght - 1; i += 2)  // Смещение на 2 буквы
		frequencies[Bigram(text[i], text[i + 1])].number++;  // Подсчет
	for (auto& frequancy : frequencies)
		frequancy.second.frequancy = frequancy.second.number / (double)lenght;  // Расчет частоты
	return frequencies;
}

map<Bigram, Frequancy> bigramFrequencyWithIntersections(const string& text, int lenght = -1)  // Подсчет количества биграмм с пересечением
{
	if (lenght == -1 || lenght > (int)text.size())
		lenght = (int)text.size();
	map<Bigram, Frequancy> frequencies;
	for (int i = 0; i < lenght - 1; i++)  // Смещение на одну букву
		frequencies[Bigram(text[i], text[i + 1])].number++;  // Подсчет
	for (auto& frequancy : frequencies)
		frequancy.second.frequancy = frequancy.second.number / (double)lenght;  // Расчет частоты
	return frequencies;
}
