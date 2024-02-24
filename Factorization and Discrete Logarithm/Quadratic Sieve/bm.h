#pragma once

#include<iostream>
#include<vector>
#include<map>

using namespace std;

struct FactorizeResult  // Результат факторизации
{
	long long x, y;  // X и Y
	vector<long long> indc;  // Индексы подходящих векторов
	// Ïåðåìåííûå äëÿ àëãîðèòìû
	vector<long long> a;
	vector<long long> u;
	vector<long long> v;
	vector<long long> b;
	vector<long long> b2;
	// Ïðîñòûå ÷èñëà (-1 2 ...)
	vector<long long> base;
	map<long long, map<long, long>> ks;  // Коэффициенты (b:base)
	vector<long long> multipliers;
	bool root = false;
};

FactorizeResult bmFactorize(long long n, long long maxIteration = 50);  // Факторизация
