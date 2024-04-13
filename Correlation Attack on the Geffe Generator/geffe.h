#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <atomic>
#include <ctime>
#include <iomanip>
#include <sstream>
// #include <Windows.h>
//#include <omp.h>
#include <thread>
#include <cmath>
#include <stdio.h>
#include <mutex>

using namespace std;

typedef unsigned int Register;  // Регистр (для удобства)
typedef unsigned long long Counter;  // Счетчик (для удобства)
template <typename Stream>

class Data  // Данные (биты)
{
private:
	vector<unsigned int> data;  // Биты блоками по 32
	Counter bitsCount;  // Количество бит

public:
	Data()  // Конструктор без параметров
	{
		bitsCount = 0;
	}

	Data(Counter count)  // Конструктор с параметром - размером
	{
		resize(count);
	}

	void setBit(Counter position, bool bit)  // Установка бита по позиции
	{
		if ((position / 32) + 1 >= data.size())
			resize(position + 1);
		if (bit)
			data[position / 32] |= 1 << (position % 32);
		else
			data[position / 32] &= ~(1 << (position % 32));
	}

	bool getBit(unsigned int position) const  // Получение бита по позиции
	{
		return (data[position / 32] >> (position % 32)) & 1;
	}

	void push(bool bit)  // Добавление бита в конец
	{
		setBit(bitsCount++, bit);
	}

	Counter count() const  // Количество бит
	{
		return bitsCount;
	}

	void resize(Counter count)  // Изменение размера контейнера
	{
		bitsCount = count;
		data.resize(!count ? 0 : count / 32 + 1);
	}

	void print(Stream &stream) {
		for (int i = 0; i < bitsCount; i++) {
			stream << getBit(i);
		}
		stream << endl;
	}
};

struct Candidate  // Кандидат (начальное заполнение)
{
	Register data;  // Регистр
	unsigned int r;  // Rs
};

struct Result  // Результат
{
	bool complete;  // Найдены ли регистры
	vector <Candidate> l1Candidates, l2Candidates, l3Candidates;  // Кандитаты в регистры
	Register l1, l2, l3;  // Регистры
	double c1, c2;  // C
};

string bin(Register registr, unsigned int count)  // Преобразование регистра в строку
{
	stringstream ss;
	for (unsigned int i = 0; i < count; i++)
		ss << ((registr >> i) & 1);
	return ss.str();
}

ostream& operator << (ostream &out, const Data &data)  // Вывод данных в поток
{
	auto bitsCount = data.count();
	for (unsigned int i = 0; i < bitsCount; i++)
		out << data.getBit(i);
	return out;
}

Data load(const char* path)  // Загрузка данных из файла
{
	Data data;
	char buffer;
	ifstream file(path);
	if (file.fail())
		cout << "Ошибка открытия файла: \"" << path << "\"" << endl;
	else
	{

		while (!file.eof())
		{
			buffer = file.get();
			if (buffer == '0' || buffer == '1')
				data.push(buffer - '0');
		}
		file.close();
	}
	return data;
}

bool l1(Register & registr)  // Регистр сдвига L1
{
	bool newBit = (registr & 1) ^ ((registr >> 1) & 1) ^ ((registr >> 4) & 1) ^ ((registr >> 6) & 1);
	bool bit = registr & 1;
	registr = (newBit << 29) | (registr >> 1);
	if (newBit)
		registr |= 1 << 29;
	else
		registr &= ~(1 << 29);
	return bit;

	/*
	bool newBit = (registr & 1) ^ ((registr >> 3) & 1);
	bool bit = registr & 1;
	registr = (newBit << 24) | (registr >> 1);
	if (newBit)
		registr |= 1 << 24;
	else
		registr &= ~(1 << 24);
	return bit;
	*/
}

bool l2(Register & registr)  // Регистр сдвига L2
{
	bool newBit = (registr & 1) ^
		((registr >> 3) & 1);
	bool bit = registr & 1;
	registr = registr >> 1;
	if (newBit)
		registr |= 1 << 30;
	else
		registr &= ~(1 << 30);
	return bit;

	/*
	bool newBit = (registr & 1) ^ ((registr >> 1) & 1) ^ ((registr >> 2) & 1) ^ ((registr >> 6) & 1);
	bool bit = registr & 1;
	registr = (newBit << 25) | (registr >> 1);
	if (newBit)
		registr |= 1 << 25;
	else
		registr &= ~(1 << 25);
	return bit;
	*/
}

bool l3(Register & registr)  // Регистр сдвига L3
{
	bool newBit = (registr & 1) ^
		((registr >> 1) & 1) ^
		((registr >> 2) & 1) ^
		((registr >> 3) & 1) ^
		((registr >> 5) & 1) ^
		((registr >> 7) & 1);
	bool bit = registr & 1;
	registr = registr >> 1;
	if (newBit)
		registr |= 1 << 31;
	else
		registr &= ~(1 << 31);
	return bit;

	/*
	bool newBit = (registr & 1) ^ ((registr >> 1) & 1) ^ ((registr >> 2) & 1) ^ ((registr >> 5) & 1);
	bool bit = registr & 1;
	registr = (newBit << 26) | (registr >> 1);
	if (newBit)
		registr |= 1 << 26;
	else
		registr &= ~(1 << 26);
	return bit;
	*/
}

// Проверка регистров сдвига
bool check(const Data &data, const Data &l1, const Data &l2, const Data &l3)
{
	// т.к. длина гаммы 2048
	for (int i = 0; i < 2048; i++) {
		int out = (l3.getBit(i) & l1.getBit(i)) ^ ((1 ^ l3.getBit(i)) & l2.getBit(i));
		if (out != data.getBit(i)) {
			return false;
		}
	}
	return true;
}

// void enableCursor(bool enable)
// {
	// auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
	// CONSOLE_CURSOR_INFO structCursorInfo;
	// GetConsoleCursorInfo(handle, &structCursorInfo);
	// structCursorInfo.bVisible = enable;
	// SetConsoleCursorInfo(handle, &structCursorInfo);
// }

mutex mylock;

// Функция поиска кандидатов для L1 регистра.
// Принимает диапазон значений для начального состояние регистра через start, end аргументы.
// Вызывается из нескольких потоков, каждый из котороых работает в своем диапазоне.
void l1Candidates(Counter start, Counter end, Data data, unsigned int filterN, unsigned int filterC, Result &result)
{
	unsigned int count;  // Счетчик (si ^ xi или yi)
	unsigned int buffer;
	float progress = 0;

	// Проход по всем возможным начальным состояниям
	for (Counter i = start; i < end; i++) {
		buffer = i;  // Буфер регистра
		count = 0;
		// Проходимся по битам
		for (unsigned int j = 0; j < filterN; j++) {
			count += l1(buffer) ^ data.getBit(j);  // Считаем xi ^ si
		}
		// Если значение подходит
		if (count < filterC) {
			mylock.lock();
			result.l1Candidates.push_back({ (Register)i, count });  // Сохраняем кандидата
			mylock.unlock();
			printf("Found L1 Candidate: %llu (%d)\n", i, count);
		}

		if (i % 1000000 == 0) {
			progress = (i - start) / (float)(end - start) * 100;
			printf("L1 Progress: %.3f%% (%llu / %llu)\n", progress, i, end);
		}
	}
}

void l2Candidates(Counter start, Counter end, Data data, unsigned int filterN, unsigned int filterC, Result &result)
{
	unsigned int count;  // Счетчик (si ^ xi или yi)
	unsigned int buffer;
	float progress = 0;

	// Проход по всем возможным начальным состояниям
	for (Counter i = start; i < end; i++) {

		buffer = i;  // Буфер регистра
		count = 0;
		// Проходимся по битам
		for (unsigned int j = 0; j < filterN; j++) {
			count += l2(buffer) ^ data.getBit(j);  // Считаем xi ^ si
		}
		// Если значение подходит
		if (count < filterC) {
			mylock.lock();
			result.l2Candidates.push_back({ (Register)i, count });  // Сохраняем кандидата
			mylock.unlock();
			printf("Found L2 Candidate: %llu (%d)\n", i, count);
		}

		if (i % 1000000 == 0) {
			progress = (i - start) / (float)(end - start) * 100;
			printf("L2 Progress: %.3f%% (%llu / %llu)\n", progress, i, end);
		}
	}
}

std::atomic_bool l3Found(false);

void calculateL3(Data data, Counter gammaSize, vector<Data> &l1QueueTests, vector<Data> &l2QueueTests, Counter start, Counter end, Result &result) {
	// Последовательность si
	Data l3QueueTest(gammaSize);
	unsigned int buffer;
	float progress = 0;

	// Проход по всем возможным начальным состояниям
	for (Counter i = start; i < end; i++) {
		buffer = i;

		for (int j = 0; j < gammaSize; j++) {
			l3QueueTest.setBit(j, l3(buffer));
		}
		for (int a = 0; a < l1QueueTests.size(); a++) {
			for (int b = 0; b < l2QueueTests.size(); b++) {
				if (check(data, l1QueueTests[a], l2QueueTests[b], l3QueueTest)) {
					l3Found = true;
					mylock.lock();

					result.l3Candidates.push_back({ (Register)i, 0 });
					result.complete = true;
					result.l1 = result.l1Candidates[a].data;
					result.l2 = result.l2Candidates[b].data;
					result.l3 = (Register)i;
					cout << "i = " << i << endl;

					mylock.unlock();
					return;
				}
			}
		}

		if (i % 1000000 == 0) {
			progress = (i - start) / (float)(end - start) * 100;
			printf("L3 Progress: %.3f%% (%llu / %llu)\n", progress, i, end);
		}
	}
}

void bruteForceL1(unsigned int numThreads, Data data, Counter max, unsigned int filterN, unsigned int filterC, Result &result) {
	vector<thread> threads;

	for (int i = 0; i < numThreads; i++) {
		Counter start = (Counter)(max * ((float)i / numThreads));
		Counter end = (Counter)(max * (float(i + 1) / numThreads));
		printf("Block L1 %llu - %llu\n", start, end);
		threads.push_back(thread(l1Candidates, start, end, data, filterN, filterC, ref(result)));
	}

	for (auto &t : threads) {
		t.join();
	}
}

void bruteForceL2(unsigned int numThreads, Data data, Counter max, unsigned int filterN, unsigned int filterC, Result &result) {
	vector<thread> threads;

	for (int i = 0; i < numThreads; i++) {
		Counter start = (Counter)(max * ((float)i / numThreads));
		Counter end = (Counter)(max * (float(i + 1) / numThreads));
		printf("Block L2 %llu - %llu\n", start, end);
		threads.push_back(thread(l2Candidates, start, end, data, filterN, filterC, ref(result)));
	}

	for (auto &t : threads) {
		t.join();
	}
}

void bruteForceL3(unsigned int numThreads, Data data, Counter gammaSize, Counter max, vector<Data> &l1QueueTests, vector<Data> &l2QueueTests, Result &result) {
	vector<thread> threads;
	numThreads = 1;

	for (int i = 0; i < numThreads; i++) {
		//Counter start = (Counter)(max * ((float)i / numThreads));
		//Counter end = (Counter)(max * (float(i + 1) / numThreads));

		Counter start = 2229332000; // THIS IS CORRECT VALUE FOR l3
		Counter end = 2229332010;
		printf("Block L3 %llu - %llu\n", start, end);
		threads.push_back(thread(calculateL3, ref(data), gammaSize, ref(l1QueueTests), ref(l2QueueTests), start, end, ref(result)));
	}

	for (auto &t : threads) {
		t.join();
	}
}

template <typename Stream>
Result attack(Data data, Stream &stream)  // Атака
{
	Result result = {};  // Результат

	double n1 = 258; // 257.540225
	double n2 = 265; // 264.741057

	result.c1 = 80.55088;  // C1
	result.c2 = 82.575528;  // C2
	result.complete = false;
	// enableCursor(false);

	Counter max1 = (Counter)pow(2, 30) - 1;  // Максимальное значение количество комбинаций регистра сдвига 2 ^ 30 - 1  
	unsigned int filterN1 = (int)n1;
	unsigned int filterC1 = (int)result.c1;

	Counter max2 = (Counter)pow(2, 31) - 1;  // Максимальное значение количество комбинаций регистра сдвига 2 ^ 31
	unsigned int filterN2 = (int)n2;
	unsigned int filterC2 = (int)result.c2;

	unsigned int numThreads = std::thread::hardware_concurrency();

	cout << "Calculating L1 using " << numThreads << " threads..." << endl;
	bruteForceL1(numThreads, data, max1, filterN1, filterC1, result);

	cout << "Calculating L2 using " << numThreads << " threads..." << endl;
	bruteForceL2(numThreads, data, max2, filterN2, filterC2, result);

	//
	// L3
	//
	unsigned int buffer;
	Counter max = (Counter)pow(2, 32) - 1;  // Максимальное значение количество комбинаций регистра сдвига 2 ^ 32
	Counter gammaSize = 2048;  // Тестируемое количество бит
	vector<Data> l1QueueTests(result.l1Candidates.size());  // Последовательность x i-тых из начальных L1
	vector<Data> l2QueueTests(result.l2Candidates.size());  // Последовательность y i-тых из начальных L2

	// Заполнение последовательности x i-тых
	for (int j = 0; j < result.l1Candidates.size(); j++) {
		buffer = result.l1Candidates[j].data;
		for (Counter i = 0; i < gammaSize; i++) {
			l1QueueTests[j].setBit(i, l1(buffer));
		}
		l1QueueTests[j].print(stream);
	}

	// Заполнение последовательности y i-тых
	for (int j = 0; j < result.l2Candidates.size(); j++) {
		buffer = result.l2Candidates[j].data;
		for (int i = 0; i < gammaSize; i++) {
			l2QueueTests[j].setBit(i, l2(buffer));
		}
	}

	cout << "Calculating L3 using " << numThreads << " threads..." << endl;
	bruteForceL3(numThreads, data, gammaSize, max, l1QueueTests, l2QueueTests, result);

	return result;
