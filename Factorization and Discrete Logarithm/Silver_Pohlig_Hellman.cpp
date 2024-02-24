#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <iterator>
#include <cmath>

using namespace std;

long mod(long num, long mod)  // Остаток от деления
{
	while (num < 0)
		num += mod;
	return num % mod;
}

long gcdex(long a, long n, long &x, long &y)  // Расширенный алгоритм Эвклида GCD(n, a)
{
	if (a == 0)
	{
		x = 0; // коэффициент при a
		y = 1; // коэффициент при n
		return n; // конечно НОД = n, если а = 0 :)
	}
	long x1, y1; // объявление и определение последовательностей коэффициентов Безу
	long d = gcdex(mod(n, a), a, x1, y1); // алгоритм Эвклида базируется на этом
	x = y1 - (n / a) * x1; // из анализа последовательности формул (в частности x1 = n % a (mod(n, a)))
	y = x1;
	//cout << "x = " << x << "; y = " << y << endl;
	return d;
}

vector <long> PrimeFactorization(long n)
{
	long divider = 2;
	vector <long> PrimeBase;
	while ((divider^2) <= n)
	{
		while ((n % divider) == 0) // Если делится нацело, то...
		{
			PrimeBase.push_back(divider);
			n /= divider;
		}
		divider++;
	}
	if (n > 1) PrimeBase.push_back(n); // Если n оказалось простым
	return PrimeBase;
}

map <long, long> CountOccurences(vector <long>&PrimeBase)
{
	map<long, long> Counts;
	for (auto v : PrimeBase)
		++Counts[v];
	return Counts;
}

long Chinese_remainder_theorem(vector <long> x, map<long, long> Pair_num_counts, long p)
{
	long result_x = 0, N, a, b;
	//long M = 1; // Алгоритм, который не привязан к данной задаче, не знает М (здесь М это р-1), но мы знаем, поэтому можем его не считать
	// В общем случае (когда есть только уравнения) нужно раскомментировать и не передавать long p в функцию
	vector <long> m;
	for (auto it = Pair_num_counts.begin(); it != Pair_num_counts.end(); it++)
	{
		m.push_back(pow(it->first, it->second));
	}
	/*for (int i = 0; i < m.size(); i++)
		M *= m[i];*/
	for (int i = 0; i < x.size(); i++)
	{
		gcdex((p-1)/m[i], m[i], a, b);
		result_x += a * (p-1)/m[i] * x[i];
		result_x = mod(result_x, p-1);
	}
	return result_x;
}

long A_Pow_B_ModP(long a, long b, long p) // Возводим в степень самым простым способом
{
	long result = 1;
	long expp = 0;
	while (expp < b)
	{
		result *= a;
		result = mod(result, p);
		expp++;
	}
	return result;
}

long create_x_i(long p, long a, long b, long first, long second) // Наши 2 и 3 шаги (вместе, т.к. это чуть-чуть экономит память)
{
	long x = 0, value; // Наш х и вспомогательная переменная value для вычисления r_q,j
	map <long, long> r;
	long expp = (p-1) / first;
	cout << "Первый этап для q = " << first << "." << endl;
	for (int j = 0; j < first; j++)
	{
		value = A_Pow_B_ModP(a, expp*j, p);
		r[value] = j; // Типа наша "табличка"
		cout << "r_" << first << "," << j << " = " << value << endl;
	}
	cout << "Второй этап для q = " << first << "." << endl;
	cout << "x = x_0 ";
	for (int j = 1; j < second; j++)
		cout << "+ x_" << j << "*" << pow(first, j) << " ";
	cout << endl;
	// Ищем х_i через возведение в степень b (a уже возводили)
	for (int j = 0; j < second; j++)
	{
		value = A_Pow_B_ModP(b, (p-1)/pow(first, j+1), p); // По формуле 
		cout << "b^(p-1)/(" << first << "^" << j + 1 << ")" << " = " << value << " = " << a << "^(x_" << j << "*" << (p-1)/first << ")" << endl;
		cout << "x_" << j << " = " << pow(first, j) * r[value] << endl;
		x += pow(first, j) * r[value]; // По формуле находим х_j и прибавляем к старому 
		value = mod(-r[value], p-1);
		value = A_Pow_B_ModP(a, pow(first, j)*value, p);
		b = mod(b * value, p);
	}
	value = pow(first, second);
	x = mod(x, value);
	cout << endl << endl;
	return x;
}

void Silver_Pohlig_Hellman_algorithm(long p, long a, long b)
{
	vector <long> x;   
	vector <long> y = PrimeFactorization(p-1);
	map <long, long>Pair_num_counts = CountOccurences(y);
	map <long, long>::iterator it;
	cout << "p - 1 = " << p - 1 << " = ";
	for (it = Pair_num_counts.begin(); it != Pair_num_counts.end(); it++) //
		cout << it->first << "^" << it->second << " * ";
	cout << "1" << endl;
	for (it = Pair_num_counts.begin(); it != Pair_num_counts.end(); it++)
		x.push_back(create_x_i(p, a, b, it->first, it->second));
	cout << endl << "Третий этап." << endl;
	it = Pair_num_counts.begin();
	for (int i = 0; i < x.size() && it != Pair_num_counts.end(); i++, it++)
		cout << "x" << i+1 << " = " << x[i] << "mod" << pow(it->first, it->second) << endl;
	
	cout << "Искомый х = " << Chinese_remainder_theorem(x, Pair_num_counts, p);
}


int main()
{
	setlocale(LC_ALL, "Rus");

	long p, a, b;
	cout << "Введите число p: ";
	cin >> p;
	cout << "Введите число a: ";
	cin >> a;
	cout << "Введите число b: ";
	cin >> b;
	cout << endl;
	Silver_Pohlig_Hellman_algorithm(p, a, b);

	cin.get();
	cin.get();
}
