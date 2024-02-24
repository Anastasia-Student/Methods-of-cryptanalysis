#include "bm.h"
#include <set>
#include <algorithm>  
#include <numeric>

long mod(long long a, long long b)  // Функция реализация mod (a mod b)
{
	return (b + (a % b)) % b;
}

long devide(long long n, long long a)
{
	long i = 0;
	if (a == -1)
		return n / a > 0;
	while (abs(n) % a == 0)
	{
		n /= a;
		i++;
	}
	return i;
}


int gcd(long long a, long long b)  // Наименьший общий делитель
{
	return b == 0 ? a : gcd(b, mod(a, b));
}

long lejandrSymbol(long long a, long long p)  // Функция рассчета символа Лежанра
{
	while (a < 0)
		a += p;
	if (a % p == 0)
		return 0;
	if (a == 1)
		return 1;
	if (a == 2)
		return pow(-1, (p * p - 1) / 8);
	if (a < p)
		return pow(-1, (a - 1) * (p - 1) / 4) * lejandrSymbol(p, a);
	return
		lejandrSymbol(a % p, p);
}

bool nextSet(long long* a, long long n, long long m)  // Функция для сочетаний (m из n)
{
	long long k = m;
	for (long long i = k - 1; i >= 0; --i)
		if (a[i] < n - k + i + 1)
		{
			++a[i];
			for (long long j = i + 1; j < k; ++j)
				a[j] = a[j - 1] + 1;
			return true;
		}
	return false;
}

void removeClient(FactorizeResult& result)  // Очистка лишних столбцов (dij)
{
	for (int i = result.base.size() - 1; i >= 0; i--)
	{
		bool flag = false;
		for (int j = 0; j < result.b.size(); j++)
		{
			if (result.ks[result.b[j]][result.base[i]])
			{
				flag = true;
				break;
			}
		}
		if (!flag)
			result.base.erase(result.base.begin() + i);
	}
}

FactorizeResult bmFactorize(long long n, long long maxIteration)  // Факторизация
{
	FactorizeResult result;  // Результат файторизации
	long long root = (long long)sqrt(n);  // Корень из n
	long long buffer;  // Буффер для рассчетов
	bool* primes;  // Простые числа
	long long multiplier1;  // Первый множитель
	long long multiplier2;  // Второй множитель

	result.base.push_back(-1);  // Первый элемент b это -1
	primes = new bool[n + 1];  // Выделение памяти под простые числа
	// Генерация простых чисел
	primes[0] = primes[1] = false;
	for (long i = 2; i <= n; i++)
		primes[i] = true;
	long sq_rt = sqrt(n);
	for (long i = 2; i <= sq_rt; i++)
		if (primes[i])
			for (long j = i * i; j <= n; j += i)
				primes[j] = false;
	// Создание базы
	for (long i = 0; i <= 1000; i++)
		if (primes[i])
			if (lejandrSymbol(n, i) == 1)
				result.base.push_back(i);
	delete[] primes;  // Освобождение памяти

	// Алгоритм
	result.v.push_back(1);
	result.a.push_back(root);
	result.u.push_back(root);
	result.b.push_back(root);
	buffer = (long long)pow(root, 2) % n;
	if (buffer > n / 2)
		buffer -= n;
	result.b2.push_back(long(buffer));
	for (long ix = 1; ix <= maxIteration; ix++)
	{
		//result.v.push_back(abs(result.b2[ix - 1]));
		result.v.push_back((n - result.u[ix - 1]* result.u[ix - 1])/ result.v[ix - 1]);
		result.a.push_back(long long((sqrt(n) + result.u[ix - 1]) / result.v[ix]));
		result.u.push_back(result.a[ix] * result.v[ix] - result.u[ix - 1]);
		if (ix > 1)
		{
			buffer = mod((result.a[ix] * result.b[ix - 1] + result.b[ix - 2]), n);
			if (buffer > n / 2)
				buffer -= n;
			result.b.push_back(buffer);
		}
		else
		{
			buffer = mod((result.a[ix] * result.b[ix - 1] + 1), n);
			result.b.push_back(buffer);
		}
		buffer = (result.b[ix] * result.b[ix]) % n;
		if (buffer > n / 2)
			buffer -= n;
		result.b2.push_back(buffer);

		// Поиск подходящих векторов
		for (int i = 0; i < result.b.size(); i++)
			for (int j = 0; j < result.base.size(); j++)
				result.ks[result.b[i]][result.base[j]] = devide(result.b2[i], result.base[j]);

		// Поиск подходящих векторов
		for (long i = 2; i <= result.b.size(); i++)
		{
			long long* r = new long long[i];
			for (long j = 0; j < i; j++)
				r[j] = j;
			do
			{
				long long* vec = new long long[result.base.size()];
				memset(vec, 0, result.base.size() * sizeof(long long));
				vector<long long> indc;
				for (long j = 0; j < i; j++)
				{
					bool need = false;
					for (int l = 0; l < result.base.size(); l++)
					{
						if (result.ks[result.b[r[j]]][result.base[l]] != 0)
						{
							need = true;
							break;
						}
					}
					if (need)
					{
						indc.push_back(r[j]);
						for (int l = 0; l < result.base.size(); l++)
						{
							vec[l] += result.ks[result.b[r[j]]][result.base[l]] % 2;
							vec[l] %= 2;
						}
					}
				}

				if (indc.size() >= 2)
				{
					bool flag = true;
					for (int j = 0; j < result.base.size(); j++)
					{
						if (vec[j] != 0)
						{
							flag = false;
							break;
						}
					}
					if (flag)
					{
						// Подсчет X и Y
						result.x = 1;
						result.y = 1;
						long long* powers = new long long[result.base.size()];
						memset(powers, 0, result.base.size() * sizeof(long long));
						for (int iz = 0; iz < indc.size(); iz++)
						{
							result.x *= result.b[indc[iz]];
							for (long j = 0; j < result.base.size(); j++)
								powers[j] += result.ks[result.b[indc[iz]]][result.base[j]];
						}
						result.x = mod(result.x, n);
						for (int j = 0; j < result.base.size(); j++)
							result.y *= pow(result.base[j], powers[j] / 2);
						result.y = mod(result.y, n);
						if (result.x != result.y && result.x != -result.y)
						{
							multiplier1 = gcd(result.x + result.y, n);
							multiplier2 = gcd(result.x - result.y, n);
							if (multiplier1 != 1 && multiplier2 != 1)
							{
								result.indc = indc;
								result.multipliers.push_back(multiplier1);
								result.multipliers.push_back(multiplier2);
								removeClient(result);
								return result; // Возврат результата
							}
						}
					}
				}

				delete[] vec;  // Освобождение памяти

			} while (nextSet(r, result.b.size() - 1, i));  // Следующая комбинация

			delete[] r;  // Освобождение памяти
		}
	}

	return result;
}
