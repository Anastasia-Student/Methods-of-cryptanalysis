#include "distortion.h"
#include "file.h"
#include "stat.h"
#include "ShannonFano.h"
#include "criterions.h"
#include "Huffman.h"
#include "GZip.h"
#include "Deflate.h"

#include <algorithm>
#include <iostream>
#include <iomanip>

using namespace std;

// Вывести энтропию и индекс соответствия букв
void printBigramFrequencyHeader(const map<Letter, Frequancy>& counters, size_t lenght, ostream& stream = cout)
{
	stream << "Энтропия на символ источника (l = 1) = " << setprecision(3) << fixed << entropy(counters, 1) << endl;
	stream << "Индекс соответствия украинского языка для одного символа = " << setprecision(3) << fixed
		<< complianceIndex(counters, (int)lenght) << ", L = " << lenght << endl;
}

// Вывести частоты букв
void printLetterFrequency(const map<Letter, Frequancy> &counters, size_t lenght, ostream& stream = cout)
{
	printBigramFrequencyHeader(counters, lenght, stream);
	vector<pair<Letter, Frequancy>> vectorCounters(counters.size());
	copy(counters.begin(), counters.end(), vectorCounters.begin());
	sort(vectorCounters.begin(), vectorCounters.end(), [](const pair<Letter, Frequancy>& a,
		const pair<Letter, Frequancy>& b) -> bool { return a.second.frequancy > b.second.frequancy; });

	stream << "-------------------\n";
	stream << "| буква | частота |\n";
	stream << "-------------------\n";

	for (const auto& counter : vectorCounters)
		if (counter.second.number)
			stream << "| " << setw(5) << left << counter.first <<
			" | " << setw(7) << left << setprecision(5) << fixed << counter.second.frequancy << " |\n";
	stream << "-------------------\n";
}

// Вывести энтропию и индекс соответствия биграмм
void printBigramFrequencyHeader(const map<Bigram, Frequancy>& counters, size_t lenght, ostream& stream = cout)
{
	stream << "Энтропия на символ источника (l = 2) = " << setprecision(3) << fixed << entropy(counters, 2) << endl;
	stream << "Индекс соответствия украинского языка для биграмм = " << setprecision(3) << fixed <<
		complianceIndex(counters, (int)lenght) << ", L = " << lenght << endl;
}

// Вывести частоты биграмм
void printBigramFrequency(const map<Bigram, Frequancy>& counters, size_t lenght, ostream& stream = cout)
{
	printBigramFrequencyHeader(counters, lenght, stream);
	vector<pair<Bigram, Frequancy>> vectorCounters(counters.size());
	copy(counters.begin(), counters.end(), vectorCounters.begin());
	sort(vectorCounters.begin(), vectorCounters.end(), [](const pair<Bigram, Frequancy>& a,
		const pair<Bigram, Frequancy>& b) -> bool { return a.second.frequancy > b.second.frequancy; });

	stream << "----------------------\n";
	stream << "| биграмма | частота |\n";
	stream << "----------------------\n";

	for (auto counter : vectorCounters)
		if (counter.second.number)
			stream << "| " << counter.first[0] << setw(7) << left << counter.first[1] <<
			" | " << setw(7) << left << setprecision(5) << fixed << counter.second.frequancy << " |\n";
	stream << "----------------------\n";
}

// Генерация случайного текста
void generateRandomText(string& text)
{
	for (size_t j = 0; j < text.length(); j++)
		text[j] = letter(rand());
}

// Шифр Виженера
void vigener(string& text, int r)
{
	for (size_t j = 0; j < text.size(); j++)
		text[j] = letter(code(text[j]) + j % r);
}

// Афинная подстановка
void affineSubstitution(string& text, Letter a = rand() % 32, Letter b = rand() % 32)
{
	for (int j = 0; j < (int)text.size(); j++)
		text[j] = letter(a * code(text[j]) + b);
}

// Равномерное заполнение
void uniformDistribution(string& text)
{
	for (int j = 0; j < (int)text.size(); j++)
		text[j] = letter(j);
}

// Текст на основе соотношения
void distribution(string& text)
{
	if (text.size() >= 1)
		text[0] = letter(rand());
	if (text.size() >= 2)
		text[1] = letter(rand());
	for (size_t j = 2; j < text.size(); j++)
		text[j] = letter(code(text[j - 1]) + code(text[j - 2]));
}

int main()
{
	setlocale(LC_ALL, "rus");

	filter("text.txt", "filtered_text.txt");  // Фильтрация текста
	auto text = load("filtered_text.txt");  // Загрузка отфильтрованного текста
	if (text.length() == 0)
	{
		cerr << "Неудалось найти файл \"filtered_text.txt\" либо он пуст!";
		return 1;
	}

	auto letterCounters = letterFrequency(text);  // Частота букв
	auto bigramCounters = bigramFrequencyWithIntersections(text);  // Частота биграмм

	ofstream letterFrequencyFile("letter_frequency.txt");  // Запись частоты букв
	printLetterFrequency(letterCounters, text.size(), letterFrequencyFile);
	letterFrequencyFile.close();
	printBigramFrequencyHeader(letterCounters, text.size());

	ofstream bigrammFrequencyFile("frequency_of_bigrams.txt");  // Запись частоты биграмм
	printBigramFrequency(bigramCounters, text.size(), bigrammFrequencyFile);
	bigrammFrequencyFile.close();
	printBigramFrequencyHeader(bigramCounters, text.size());

	vector<int> rs = { 1, 5, 10 };  // Значения r
	vector<int> ls = { 10, 100, 1000, 10000 };  // Значения L
	map<int, int> cicles = { {10, 10000}, {100, 10000}, {1000, 10000}, {10000, 1000} };  // Количество циклов для кадой L

	ofstream criteriaPassingResultsFile("criteria_passing_results.txt");  // Файл для записи критериев

	return 0;
}
