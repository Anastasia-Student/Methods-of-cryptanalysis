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

struct Criterion22Result  // Структура результата критерия 2.2
{
	bool h1 = false;  // Гипотеза H1
	int kh = 0;  // Kh
	double fx = 0;  // Fx
	double kx = 0;  // Kx
	string unit;  // Биграмма или буква
};

template <typename Type>
Criterion22Result criterion22(const string& text, map<Type, Frequancy>& counters, int kh = 5, int lenght = -1)  // Критерий 2.2
{
	Criterion22Result result;
	result.kh = kh;
	if (lenght < 0 || lenght >(int)text.size())
		lenght = (int)text.size();
	vector<pair<Type, Frequancy>> afrg(counters.size());  // Afrg
	for (auto counter : counters)  // Копирование map в vector
		afrg.push_back(counter);
	sort(afrg.begin(), afrg.end(), [](const pair<Type, Frequancy>& a,  // Сортировка по частоте
		const pair<Type, Frequancy>& b) -> bool { return a.second.frequancy > b.second.frequancy; });
	for (int i = 0; i < kh; i++)  // Первые Kh биграммы/буквы по частоте
	{
		double kx = afrg[i].second.frequancy;
		int offset = 0;
		double fx = 0;
		while ((offset = (int)text.find(afrg[i].first, offset + 1)) != -1)
			fx++;
		fx /= (double)lenght;
		if (fx < kx)  // Если fx < kx
		{
			result.unit = afrg[i].first;
			result.fx = fx;
			result.kx = kx;
			result.h1 = true;  // Гипотеза H1 верна
			break;
		}
	}
	return result;
}


struct Criterion23Result  // Структура результата критерия 2.3
{
	bool h1 = false;  // Гипотеза H1
	double ff = 0;  // Ff
	double kf = 0;  // Kf
};

template <typename Type>
Criterion23Result criterion23(const string& text, map<Type, Frequancy>& counters,  // Критерий 2.3
	int kh = 5, int lenght = -1)
{
	Criterion23Result result;
	if (lenght < 0 || lenght >(int)text.size())
		lenght = (int)text.size();
	vector<pair<Type, Frequancy>> afrg(counters.size());  // Afrg
	for (auto counter : counters)  // Копирование map в vector
		afrg.push_back(counter);  // Копирование map в vector
	sort(afrg.begin(), afrg.end(), [](const pair<Type, Frequancy>& a,  // Сортировка по частоте
		const pair<Type, Frequancy>& b) -> bool { return a.second.frequancy > b.second.frequancy; });

	double ff = 0;
	double kf = 0;

	for (int i = 0; i < kh; i++)  // Первые Kh биграммы/буквы по частоте
	{
		kf += afrg[i].second.frequancy;
		double kx = 0;
		int offset = 0;
		while ((offset = (int)text.find(afrg[i].first, offset + 1)) != -1)
			kx++;
		kf += kx / (double)lenght;  // Суммирование kx
	}

	result.ff = ff;
	result.kf = kf;

	if (ff < kf)  // Если ff < kf
		result.h1 = true;  // Гипотеза H1 верна

	return result;
}


struct Criterion40Result  // Структура результата критерия 4.0
{
	bool h1 = false;  // Гипотеза H1
	double k = 0.0;
	double kl = 0.0;
};

template <typename Type>
Criterion40Result criterion40(map<Type, Frequancy> partFrequencies,   // Критерий 4.0
	int partLenght, map<Type, Frequancy>& frequencies,  int textLenght, double kl = 0.009, int lenght = -1)
{
	Criterion40Result result;
	auto k = abs(complianceIndex(partFrequencies, partLenght) - // |I - I'|
		complianceIndex(frequencies, textLenght));
	result.k = k;
	result.kl = kl;
	if (k > kl)  // Если |I - I'| > kl
		result.h1 = true;  // Гипотеза H1 верна
	return result;
}

struct Criterion50Result  // Структура результата критерия 5.0
{
	bool h1 = false;  // Гипотеза H1
	int fempt = 0;  // Fempt
	int kempt = 0;  // Kempt
};

template <typename Type>
Criterion50Result criterion50(const string& text, map<Type, Frequancy>& frequencies,  // Критерий 5.0
	int j = 4, int kempt = 1, int lenght = -1)
{
	Criterion50Result result;
	if (lenght < 0 || lenght >(int)text.size())
		lenght = (int)text.size();
	vector<pair<Type, Frequancy>> bprh(frequencies.size());  // Bprh
	for (auto counter : frequencies)  // Копирование map в vector
		bprh.push_back(counter);  // Копирование map в vector
	sort(bprh.begin(), bprh.end(), [](const pair<Type, Frequancy>& a,  // Сортировка по частоте (по убыванию)
		const pair<Type, Frequancy>& b) -> bool { return a.second.frequancy < b.second.frequancy; });
	map<Type, int> boxes;  // Коробки
	j = j < (int)bprh.size() ? j : (int)bprh.size();
	for (int i = 0; i < j; i++)  // J Букв биграмм с наименьшей частотой
	{
		int offset = 0;
		while ((offset = (int)text.find(bprh[i].first, offset + 1)) != -1)  // Подcчет количества Bprh_i в тексте
			boxes[bprh[i].first]++;
	}
	int fempt = 0;
	for (auto box : boxes)
		if (box.second == 0)  // Если коробка пуста
			fempt++;  // Считаем
	result.fempt = fempt;
	result.kempt = kempt;
	if (fempt < kempt)  // Если Fempt < Kempt
		result.h1 = true;  // Гипотеза H1 верна
	return result;
}
