#pragma once

#include "stat.h"

#include <algorithm>

using namespace std;

struct Criterion20Result  // Структура результата критерия 2.0
{
	bool h1 = false;  // Гипотеза H1
	int kh = 0;  // Kh
	string unit;  // Биграмма или буква
};

template <typename Type>
Criterion20Result criterion20(const string& text, // Критерий 2.0
	const map<Type, Frequancy>& counters, int kh = 5, int lenght = -1)
{
	Criterion20Result result;
	result.kh = kh;
	if (lenght < 0 || lenght >(int)text.size())
		lenght = (int)text.size();
	vector<pair<Type, Frequancy>> afrg(counters.size());  // Afrg
	for (auto counter : counters)  // Копирование map в vector
		afrg.push_back(counter);  // Копирование map в vector
	sort(afrg.begin(), afrg.end(), [](const pair<Type, Frequancy>& a,  // Сортировка по частоте
		const pair<Type, Frequancy>& b) -> bool { return a.second.frequancy > b.second.frequancy; });

	for (int i = 0; i < kh; i++)  // Первые Kh биграммы/буквы по частоте
	{
		if (text.find(afrg[i].first) == -1)  // Если текст содержит Afrg_i
		{
			result.unit = afrg[i].first;
			result.h1 = true;  // Гипотеза H1 верна
			break;
		}
	}

	return result;
}


struct Criterion21Result  // Структура результата критерия 2.1
{
	bool h1 = false;  // Гипотеза H1
	int kh = 0;  // Kh
	int kf = 0;  // Kf
};

template <typename Type>
Criterion21Result criterion21(const string& text, map<Type, Frequancy>& counters,  // Критерий 2.1
	int kh = 5, int kf = 2, int lenght = -1)
{
	Criterion21Result result;
	result.kh = kh;
	result.kf = kf;
	if (lenght < 0 || lenght >(int)text.size())
		lenght = (int)text.size();
	vector<pair<Type, Frequancy>> afrg(counters.size());  // Afrg
	for (auto counter : counters)  // Копирование map в vector
		afrg.push_back(counter);  // Копирование map в vector
	sort(afrg.begin(), afrg.end(), [](const pair<Type, Frequancy>& a,  // Сортировка по частоте
		const pair<Type, Frequancy>& b) -> bool { return a.second.frequancy > b.second.frequancy; });

	vector<pair<Type, Frequancy>> af;  // Для сохранения Afrg_i, которые содержит текст 
	for (int i = 0; i < kh; i++)  // Первые Kh биграммы/буквы по частоте
		if (text.find(afrg[i].first) != -1)  // Если текст содержит
			af.push_back(afrg[i]);  // Сохраняем в af

	if (1 <= kf && kf < kh && (int)af.size() <= kf)  // |Aaf пересечение Afrq| < kf , 1 < kf < hf
		result.h1 = true;  // Гипотеза H1 верна

	return result;
}
