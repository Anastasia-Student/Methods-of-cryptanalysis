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

struct HaffmaneNode  // Узел дерева для Хаффман сжатия
{
	Letter letter;  // Буква
	HaffmaneNode* left;  // Левый ребенок
	HaffmaneNode* right;  // Правый ребенок
	double frequancy;  // Частота

	HaffmaneNode(HaffmaneNode* left, HaffmaneNode* right)  // Конструктор из детей
	{
		letter = 0;
		this->left = left;
		this->right = right;
		frequancy = left->frequancy + right->frequancy;
	}

	HaffmaneNode(pair<Letter, Frequancy> leaf)  // Конструктор листа (из буквы или биграммы)
	{
		letter = leaf.first;
		this->left = nullptr;
		this->right = nullptr;
		frequancy = leaf.second.frequancy;
	}
};

void haffmaneCodesGenerate(HaffmaneNode* node, map<Letter, Code>& codes,  // Генерация биарных кодов для символов
	vector<unsigned char> bits = vector<unsigned char>())
{
	if (node != nullptr)  // Если узел существует
	{
		if (node->left == nullptr && node->right == nullptr)  // Если лист
			codes[node->letter] = bits;  // Сохранение бит
		bits.push_back(1);
		haffmaneCodesGenerate(node->left, codes, bits);  // Для левого ребенка
		bits[bits.size() - 1] = 0;
		haffmaneCodesGenerate(node->right, codes, bits);  // Для правого ребенка
		delete node;
	}
}

int findMinFrequncy(const vector<HaffmaneNode*>& rootes, int excludeIntex = -1)  // Найти узел с минимальной частотой
{
	int minIndex = -1;
	double minFrequncy = 1.0;
	for (size_t i = 0; i < rootes.size(); i++)  // Пройтись по узлам
	{
		if (rootes[i] != nullptr && rootes[i]->frequancy < minFrequncy && i != excludeIntex)  // Проверка на минимальность
		{
			minIndex = (int)i;
			minFrequncy = rootes[i]->frequancy;  // Замена на новый минимальный
		}
	}
	return minIndex;  // Возврат минимального
}

void haffmaneCodesGenerate(const vector<pair<Letter, Frequancy>>& counters,  // Генерация бинарных кодов для символов
	map<Letter, Code>& codes)
{
	vector<HaffmaneNode*> rootes;  // Узла дерева
	for (size_t i = 0; i < counters.size(); i++)  // Добавление листьев дерева
		rootes.push_back(new HaffmaneNode(counters[i]));
	int inTree = 0;
	HaffmaneNode* root = nullptr;
	while (inTree < rootes.size() - 1)  // Пока не останется один узел
	{
		int firstMinIndex = findMinFrequncy(rootes);  // Первый минимальный элемент
		int secondMinIndex = findMinFrequncy(rootes, firstMinIndex);  // Второй минимальный элемент
		if (firstMinIndex == -1 || secondMinIndex == -1)
			break;
		auto left = rootes[firstMinIndex];
		auto right = rootes[secondMinIndex];
		auto node = new HaffmaneNode(left, right);  // Создание узла
		root = node;
		rootes[firstMinIndex] = node;  // Замена на родителя
		rootes[secondMinIndex] = nullptr;  // Удаление
	}
	haffmaneCodesGenerate(root, codes);  // Генерация кодов по дереву
}

ComressionResult haffman(const string& text)  // Алгорит Хаффмана
{
	auto countersMap = letterFrequency(text);  // Подсчет частот букв
	vector<pair<Letter, Frequancy>> counters(countersMap.size());
	for (auto counter : countersMap)
		counters.push_back(counter);  // Копирование частот в вектор
	map<Letter, Code> codes;
	haffmaneCodesGenerate(counters, codes);  // Генерация кодов
	stringstream stream;  // Поток для записи
	ComressionResult result;  // Результат
	result.size = text.size() * sizeof(Letter) * 8;  // Размер текста
	for (size_t i = 0; i < text.size(); i++)  // Проход по тексту
		for (size_t j = 0; j < codes[text[i]].size(); j++, result.compressedSize++)  // Проход по битам и их запись
			stream << (char)('0' + codes[text[i]][j]);
	result.complited = true;
	return result;
}


ComressionResult haffman(string input, string output)  // Алгорит Хаффмана (из файла input в файл output)
{
	auto text = load(input);
	auto countersMap = letterFrequency(text);  // Подcчет частот букв
	vector<pair<Letter, Frequancy>> counters(countersMap.size());
	for (auto counter : countersMap)
		counters.push_back(counter);  // Копирование частот в вектор
	map<Letter, Code> codes;
	haffmaneCodesGenerate(counters, codes);  // Генерация кодов
	ComressionResult result;
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
