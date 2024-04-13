#include "bm.h"

#include <iostream> 
#include <locale>
#include <iomanip>
#include <sstream>
#include <string>

using namespace std;

void main()
{
	setlocale(LC_ALL, "Rus");
	stringstream ss;

	long number;
	cout << "Введите число: ";
	cin >> number;

	cout << endl;
	auto result = bmFactorize(number);
	if (!result.root)
	{
		cout << "----------------------------------------------------" << endl;
		cout << "|   i |      a |    b%n |  b^2%n |      u |      v |" << endl;
		cout << "----------------------------------------------------" << endl;
		for (long ix = 0; ix < result.a.size(); ix++)
			cout << "| " << setw(3) << ix << " | " << setw(6) << result.a[ix] << " | "
			<< setw(6) << result.b[ix] << " | " << setw(6) << result.b2[ix] << " | "
			<< setw(6) << result.u[ix] << " | " << setw(6) << result.v[ix] << " | " << endl;
		cout << "----------------------------------------------------" << endl;
		ss << "|    B | ";
		for (long i = 0; i < result.base.size(); i++)
			ss << setw(3) << result.base[i] << " | ";
		cout << endl;
		string header = ss.str();
		cout << string(header.size() - 1, '-') << endl;
		cout << header << endl;
		cout << string(header.size() - 1, '-') << endl;
		for (long it = 0; it < result.b.size(); it++)
		{
			cout << "| " << setw(4) << result.b2[it] << " | ";
			for (long j = 0; j < result.base.size(); j++)
				cout << setw(3) << result.ks[result.b[it]][result.base[j]] << " | ";
			cout << endl;
		}
		cout << string(header.size() - 1, '-') << endl;
		cout << endl;

		cout << "Подходящие векторы: ";
		for (auto v : result.indc)
			cout << v + 1 << " ";
		cout << endl << endl;

		cout << "X = " << result.x << endl;
		cout << "Y = " << result.y << endl;
		cout << endl;

		cout << "Множители: ";
		for (long i = 0; i < result.multipliers.size(); i++)
			cout << result.multipliers[i] << " ";
	}
	else
	{
		cout << "Множители: ";
		for (long i = 0; i < result.multipliers.size(); i++)
			cout << result.multipliers[i] << " ";
	}
	cin.get();
	cin.get();
}
