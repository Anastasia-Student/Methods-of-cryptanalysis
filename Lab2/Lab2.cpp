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
void vigener(string& text, int r = 5)
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

// Вывести данные о сжатии
void printCompressedTable(ComressionResult result, string name, ostream& stream = cout)
{
	stream << "Текст с файла filtered_text.txt сжат алгоритмом " << name << " с " << result.size
		<< " бит до " << result.compressedSize << " бит (" << setprecision(2) << fixed
		<< result.compressedSize / (float)result.size * 100.0 << "%)\n";
}

// Вывести таблицу с FP и FN значениями
void printCriterionsTableBody(map<string, double> h1Counters, ostream& stream = cout)
{
	stream << "| 2.0            | " << setprecision(3) << fixed << setw(10) << h1Counters["2.0.fp1"] << " | " << setw(10) << h1Counters["2.0.fn1"] <<
		" | " << setw(10) << h1Counters["2.0.fp2"] << " | " << setw(10) << h1Counters["2.0.fn2"] << " |" << endl;
	stream << "| 2.1            | " << setw(10) << h1Counters["2.1.fp1"] << " | " << setw(10) << h1Counters["2.1.fn1"] <<
		" | " << setw(10) << h1Counters["2.1.fp2"] << " | " << setw(10) << h1Counters["2.1.fn2"] << " |" << endl;
	stream << "| 2.2            | " << setw(10) << h1Counters["2.2.fp1"] << " | " << setw(10) << h1Counters["2.2.fn1"] <<
		" | " << setw(10) << h1Counters["2.2.fp2"] << " | " << setw(10) << h1Counters["2.2.fn2"] << " |" << endl;
	stream << "| 2.3            | " << setw(10) << h1Counters["2.3.fp1"] << " | " << setw(10) << h1Counters["2.3.fn1"] <<
		" | " << setw(10) << h1Counters["2.3.fp2"] << " | " << setw(10) << h1Counters["2.3.fn2"] << " |" << endl;
	stream << "| 4.0            | " << setw(10) << h1Counters["4.0.fp1"] << " | " << setw(10) << h1Counters["4.0.fn1"] <<
		" | " << setw(10) << h1Counters["4.0.fp2"] << " | " << setw(10) << h1Counters["4.0.fn2"] << " |" << endl;
	stream << "| 5.0 (2, 50)    | " << setw(10) << h1Counters["5.0.fp1-2"] << " | " << setw(10) << h1Counters["5.0.fn1-2"] <<
		" | " << setw(10) << h1Counters["5.0.fp2-50"] << " | " << setw(10) << h1Counters["5.0.fn2-50"] << " |" << endl;
	stream << "| 5.0 (4, 100)   | " << setw(10) << h1Counters["5.0.fp1-4"] << " | " << setw(10) << h1Counters["5.0.fn1-4"] <<
		" | " << setw(10) << h1Counters["5.0.fp2-100"] << " | " << setw(10) << h1Counters["5.0.fn2-100"] << " |" << endl;
	stream << "| 5.0 (6, 200)   | " << setw(10) << h1Counters["5.0.fp1-6"] << " | " << setw(10) << h1Counters["5.0.fn1-6"] <<
		" | " << setw(10) << h1Counters["5.0.fp2-200"] << " | " << setw(10) << h1Counters["5.0.fn2-200"] << " |" << endl;
	stream << "----------------------------------------------------------------------" << endl;
}

// Вывести заголовок таблицы с FP и FN значениями с r
void printCriterionsTable(map<string, double> h1Counters, int l, int r, string type, ostream& stream = cout)
{
	stringstream header;
	stream << "----------------------------------------------------------------------" << endl;
	header << "Искажение c помощью " << type << " (L = " << l << ", r = " << r << ")";
	stream << "| " << setw(66) << left << header.str() << " |" << endl;
	stream << "----------------------------------------------------------------------" << endl;
	stream << "| Номер критерия | FP (l = 1) | FN (l = 1) | FP (l = 2) | FN (l = 2) |" << endl;
	stream << "----------------------------------------------------------------------" << endl;
	printCriterionsTableBody(h1Counters, stream);
}

// Вывести заголовок таблицы с FP и FN значениями
void printCriterionsTable(map<string, double> h1Counters, int l, string type, ostream& stream = cout)
{
	stringstream header;
	stream << "----------------------------------------------------------------------" << endl;
	header << "Искажение c помощью " << type << " (L = " << l << ")";
	stream << "| " << setw(66) << left << header.str() << " |" << endl;
	stream << "----------------------------------------------------------------------" << endl;
	stream << "| Номер критерия | FP (l = 1) | FN (l = 1) | FP (l = 2) | FN (l = 2) |" << endl;
	stream << "----------------------------------------------------------------------" << endl;
	printCriterionsTableBody(h1Counters, stream);
}

// Вывести таблицу со значениями сжатий
void printCompressionTable(map<string, double> values, int lenght, string type, ostream& stream = cout)
{
	stream << "Результаты применения структурного критерия с разными алгоритмами сжатия(L = " << lenght <<
		", N = 10000, вид искажения = " << type << "):" << endl;
	stream << "---------------------------------------" << endl;
	stream << "| Алгоритм сжатия | FP      | FN      |" << endl;
	stream << "|-------------------------------------|" << endl;
	stream << "| DEFLATE         | " << setw(7) << left << values["DEFLATE.FP"] << " | "
		<< setw(7) << left << values["DEFLATE.FN"] << " |" << endl;
	stream << "|-------------------------------------|" << endl;
	stream << "| GZip            | " << setw(7) << left << values["GZip.FP"] << " | "
		<< setw(7) << left << values["GZip.FN"] << " |" << endl;
	stream << "|-------------------------------------|" << endl;
	stream << "| Huffman         | " << setw(7) << left << values["Huffman.FP"] << " | "
		<< setw(7) << left << values["Huffman.FN"] << " |" << endl;
	stream << "|-------------------------------------|" << endl;
	stream << "| ShannonFano     | " << setw(7) << left << values["ShannonFano.FP"] << " | "
		<< setw(7) << left << values["ShannonFano.FN"] << " |" << endl;
	stream << "---------------------------------------" << endl;
}

// Вывести таблицу для 5 задачи
void printTask5Table(vector<bool> values, ostream& stream = cout)
{
	stream << "----------------------------------------------------" << endl;
	stream << "| Номер текста | Результат применения критерия 4.0 |" << endl;
	stream << "----------------------------------------------------" << endl;
	for (size_t i = 0; i < values.size(); i++)
	{
		stream << "| " << setw(12) << left << (i + 1) << " | " << setw(33) << left <<
			(values[i] ? "случайный" : "неслучайный") << " |" << endl;
		stream << "----------------------------------------------------" << endl;
	}
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

	auto letterCounters = letterFrequency(text);  // Частоты букв
	auto bigramCounters = bigramFrequency(text);  // Частоты биграмм
	auto bigramCounterswI = bigramFrequencyWithIntersections(text);  // Частоты биграмм

	ofstream letterFrequencyFile("letter_frequency.txt");  // Запись частоты букв
	printLetterFrequency(letterCounters, text.size(), letterFrequencyFile);
	letterFrequencyFile.close();
	printBigramFrequencyHeader(letterCounters, text.size());

	ofstream bigrammFrequencyFilewI("frequency_of_bigrams_with_intersections.txt");  // Запись частоты биграмм
	printBigramFrequency(bigramCounterswI, text.size(), bigrammFrequencyFilewI);
	bigrammFrequencyFilewI.close();
	printBigramFrequencyHeader(bigramCounterswI, text.size());

	ofstream bigrammFrequencyFile("frequency_of_bigrams.txt");  // Запись частоты биграмм
	printBigramFrequency(bigramCounters, text.size(), bigrammFrequencyFile);
	bigrammFrequencyFile.close();
	printBigramFrequencyHeader(bigramCounters, text.size());

	vector<int> rs = { 1, 5, 10 };  // Значения r
	vector<int> ls = { 10, 100, 1000, 10000 };  // Значения L
	map<int, int> cicles = { {10, 10000}, {100, 10000}, {1000, 10000}, {10000, 1000} };  // Количество циклов для каждой L

	ofstream criteriaPassingResultsFile("criteria_passing_results.txt");  // Файл для записи критериев

	// Шифр Виженера

	for (auto l : ls)  // Для каждой L
	{
		string part(l, ' ');  // Строка длиной L
		for (auto r : rs)  // Для каждой r 
		{
			map<string, double> h1Counters;  // Счетчики H1

			for (int i = 0; i < cicles[l] && i * l < (int)text.size() - l; i++)  // Циклы 10000 и 1000
			{
				copy(text.begin() + (long long)i * l, text.begin() + ((long long)i + 1) * l, part.begin());
				string part_open = part;
				vigener(part, r);
				// Посчет критериев (ОТ)
				auto criterion20ResultLetters_o = criterion20(part_open, letterCounters, 5);
				h1Counters["2.0.fp1_o"] += criterion20ResultLetters_o.h1;
				auto criterion20ResultBigrams_o = criterion20(part_open, bigramCounters, 5);
				h1Counters["2.0.fp2_o"] += criterion20ResultBigrams_o.h1;

				auto criterion21ResultLetters_o = criterion21(part_open, letterCounters, 5, 2);
				h1Counters["2.1.fp1_o"] += criterion21ResultLetters_o.h1;
				auto criterion21ResultBigrams_o = criterion21(part_open, bigramCounters, 5, 2);
				h1Counters["2.1.fp2_o"] += criterion21ResultBigrams_o.h1;

				auto criterion22ResultLetters_o = criterion22(part_open, letterCounters, 5);
				h1Counters["2.2.fp1_o"] += criterion22ResultLetters_o.h1;
				auto criterion22ResultBigrams_o = criterion22(part_open, bigramCounters, 5);
				h1Counters["2.2.fp2_o"] += criterion22ResultBigrams_o.h1;

				auto criterion23ResultLetters_o = criterion23(part_open, letterCounters, 5);
				h1Counters["2.3.fp1_o"] += criterion23ResultLetters_o.h1;
				auto criterion23ResultBigrams_o = criterion23(part_open, bigramCounters, 5);
				h1Counters["2.3.fp2_o"] += criterion23ResultBigrams_o.h1;

				auto criterion40ResultLetters_o = criterion40(letterFrequency(part_open), (int)part.size(), letterCounters, (int)text.size(), 0.009);
				h1Counters["4.0.fp1_o"] += criterion40ResultLetters_o.h1;
				auto criterion40ResultBigrams_o = criterion40(bigramFrequency(part_open), (int)part.size(), bigramCounters, (int)text.size(), 0.004);
				h1Counters["4.0.fp2_o"] += criterion40ResultBigrams_o.h1;

				if (l == 10) {
					auto criterion50ResultLetters2_o = criterion50(part_open, letterCounters, 2, 1); // нет пустых ящиков, 2 редких буквы есть в последовательности
					h1Counters["5.0-2.fp1_o"] += criterion50ResultLetters2_o.h1;
					auto criterion50ResultLetters4_o = criterion50(part_open, letterCounters, 4, 2); // хотя бы 2 пустых есть
					h1Counters["5.0-4.fp1_o"] += criterion50ResultLetters4_o.h1;
					auto criterion50ResultLetters6_o = criterion50(part_open, letterCounters, 6, 3); // хотя бы 3 пустых есть
					h1Counters["5.0-6.fp1_o"] += criterion50ResultLetters6_o.h1;
					auto criterion50ResultBigrams50_o = criterion50(part_open, bigramCounters, 50, 48); // если из 50, попалась 2-1=1, то случайный (тут длина маленькая, больше нельзя)
					h1Counters["5.0.fp2_o"] += criterion50ResultBigrams50_o.h1;
					auto criterion50ResultBigrams100_o = criterion50(part_open, bigramCounters, 100, 97); // если из 100, попалось 2, то случайный (тут длина маленькая, больше нельзя)
					h1Counters["5.0.fp2_o"] += criterion50ResultBigrams100_o.h1;
					auto criterion50ResultBigrams200_o = criterion50(part_open, bigramCounters, 200, 196); // если из 200, попалось 3, то случайный (тут длина маленькая, больше нельзя)
					h1Counters["5.0.fp2_o"] += criterion50ResultBigrams200_o.h1;
				}
				if (l == 100) {
					auto criterion50ResultLetters2_o = criterion50(part_open, letterCounters, 2, 1);
					h1Counters["5.0-2.fp1_o"] += criterion50ResultLetters2_o.h1;
					auto criterion50ResultLetters4_o = criterion50(part_open, letterCounters, 4, 2);
					h1Counters["5.0-4.fp1_o"] += criterion50ResultLetters4_o.h1;
					auto criterion50ResultLetters6_o = criterion50(part_open, letterCounters, 6, 3);
					h1Counters["5.0-6.fp1_o"] += criterion50ResultLetters6_o.h1;
					auto criterion50ResultBigrams50_o = criterion50(part_open, bigramCounters, 50, 43);
					h1Counters["5.0.fp2_o"] += criterion50ResultBigrams50_o.h1;
					auto criterion50ResultBigrams100_o = criterion50(part_open, bigramCounters, 100, 91);
					h1Counters["5.0.fp2_o"] += criterion50ResultBigrams100_o.h1;
					auto criterion50ResultBigrams200_o = criterion50(part_open, bigramCounters, 200, 188);
					h1Counters["5.0.fp2_o"] += criterion50ResultBigrams200_o.h1;
				}

				if (l == 1000) {
					auto criterion50ResultLetters2_o = criterion50(part_open, letterCounters, 2, 1); // пустых нет
					h1Counters["5.0-2.fp1_o"] += criterion50ResultLetters2_o.h1;
					auto criterion50ResultLetters4_o = criterion50(part_open, letterCounters, 4, 1);
					h1Counters["5.0-4.fp1_o"] += criterion50ResultLetters4_o.h1;
					auto criterion50ResultLetters6_o = criterion50(part_open, letterCounters, 6, 2);
					h1Counters["5.0-6.fp1_o"] += criterion50ResultLetters6_o.h1;
					auto criterion50ResultBigrams50_o = criterion50(part_open, bigramCounters, 50, 34);
					h1Counters["5.0.fp2_o"] += criterion50ResultBigrams50_o.h1;
					auto criterion50ResultBigrams100_o = criterion50(part_open, bigramCounters, 100, 79);
					h1Counters["5.0.fp2_o"] += criterion50ResultBigrams100_o.h1;
					auto criterion50ResultBigrams200_o = criterion50(part_open, bigramCounters, 200, 160);
					h1Counters["5.0.fp2_o"] += criterion50ResultBigrams200_o.h1;
				}
				if (l == 10000) {
					auto criterion50ResultLetters2_o = criterion50(part_open, letterCounters, 2, 1); // пустых нет
					h1Counters["5.0-2.fp1_o"] += criterion50ResultLetters2_o.h1;
					auto criterion50ResultLetters4_o = criterion50(part_open, letterCounters, 4, 1);
					h1Counters["5.0-4.fp1_o"] += criterion50ResultLetters4_o.h1;
					auto criterion50ResultLetters6_o = criterion50(part_open, letterCounters, 6, 1);
					h1Counters["5.0-6.fp1_o"] += criterion50ResultLetters6_o.h1;
					auto criterion50ResultBigrams50_o = criterion50(part_open, bigramCounters, 50, 20); // хотя бы 30 редких в последовательности есть
					h1Counters["5.0.fp2_o"] += criterion50ResultBigrams50_o.h1;
					auto criterion50ResultBigrams100_o = criterion50(part_open, bigramCounters, 100, 67);
					h1Counters["5.0.fp2_o"] += criterion50ResultBigrams100_o.h1;
					auto criterion50ResultBigrams200_o = criterion50(part_open, bigramCounters, 200, 149);
					h1Counters["5.0.fp2_o"] += criterion50ResultBigrams200_o.h1;
				}


				// Посчет критериев (ШТ)
				auto criterion20ResultLetters = criterion20(part, letterCounters, 5);
				h1Counters["2.0.fp1"] += criterion20ResultLetters.h1;
				auto criterion20ResultBigrams = criterion20(part, bigramCounters, 5);
				h1Counters["2.0.fp2"] += criterion20ResultBigrams.h1;

				auto criterion21ResultLetters = criterion21(part, letterCounters, 5, 2);
				h1Counters["2.1.fp1"] += criterion21ResultLetters.h1;
				auto criterion21ResultBigrams = criterion21(part, bigramCounters, 5, 2);
				h1Counters["2.1.fp2"] += criterion21ResultBigrams.h1;

				auto criterion22ResultLetters = criterion22(part, letterCounters, 5);
				h1Counters["2.2.fp1"] += criterion22ResultLetters.h1;
				auto criterion22ResultBigrams = criterion22(part, bigramCounters, 5);
				h1Counters["2.2.fp2"] += criterion22ResultBigrams.h1;

				auto criterion23ResultLetters = criterion23(part, letterCounters, 5);
				h1Counters["2.3.fp1"] += criterion23ResultLetters.h1;
				auto criterion23ResultBigrams = criterion23(part, bigramCounters, 5);
				h1Counters["2.3.fp2"] += criterion23ResultBigrams.h1;

				auto criterion40ResultLetters = criterion40(letterFrequency(part), (int)part.size(), letterCounters, (int)text.size(), 0.009);
				h1Counters["4.0.fp1"] += criterion40ResultLetters.h1;
				auto criterion40ResultBigrams = criterion40(bigramFrequency(part), (int)part.size(), bigramCounters, (int)text.size(), 0.004);
				h1Counters["4.0.fp2"] += criterion40ResultBigrams.h1;

				if (l == 10) {
					auto criterion50ResultLetters2 = criterion50(part, letterCounters, 2, 1); // нет пустых ящиков, 2 редких буквы есть в последовательности
					h1Counters["5.0-2.fp1"] += criterion50ResultLetters2.h1;
					auto criterion50ResultLetters4 = criterion50(part, letterCounters, 4, 2); // хотя бы 2 пустых есть
					h1Counters["5.0-4.fp1"] += criterion50ResultLetters4.h1;
					auto criterion50ResultLetters6 = criterion50(part, letterCounters, 6, 3); // хотя бы 3 пустых есть
					h1Counters["5.0-6.fp1"] += criterion50ResultLetters6.h1;
					auto criterion50ResultBigrams50 = criterion50(part, bigramCounters, 50, 48); // если из 50, попалась 2-1=1, то случайный (тут длина маленькая, больше нельзя)
					h1Counters["5.0.fp2"] += criterion50ResultBigrams50.h1;
					auto criterion50ResultBigrams100 = criterion50(part, bigramCounters, 100, 97); // если из 100, попалось 2, то случайный (тут длина маленькая, больше нельзя)
					h1Counters["5.0.fp2"] += criterion50ResultBigrams100.h1;
					auto criterion50ResultBigrams200 = criterion50(part, bigramCounters, 200, 196); // если из 200, попалось 3, то случайный (тут длина маленькая, больше нельзя)
					h1Counters["5.0.fp2"] += criterion50ResultBigrams200.h1;
				}
				if (l == 100) {
					auto criterion50ResultLetters2 = criterion50(part, letterCounters, 2, 1);
					h1Counters["5.0-2.fp1"] += criterion50ResultLetters2.h1;
					auto criterion50ResultLetters4 = criterion50(part, letterCounters, 4, 2);
					h1Counters["5.0-4.fp1"] += criterion50ResultLetters4.h1;
					auto criterion50ResultLetters6 = criterion50(part, letterCounters, 6, 3);
					h1Counters["5.0-6.fp1"] += criterion50ResultLetters6.h1;
					auto criterion50ResultBigrams50 = criterion50(part, bigramCounters, 50, 43);
					h1Counters["5.0.fp2"] += criterion50ResultBigrams50.h1;
					auto criterion50ResultBigrams100 = criterion50(part, bigramCounters, 100, 91);
					h1Counters["5.0.fp2"] += criterion50ResultBigrams100.h1;
					auto criterion50ResultBigrams200 = criterion50(part, bigramCounters, 200, 188);
					h1Counters["5.0.fp2"] += criterion50ResultBigrams200.h1;
				}

				if (l == 1000) {
					auto criterion50ResultLetters2 = criterion50(part, letterCounters, 2, 1); // пустых нет
					h1Counters["5.0-2.fp1"] += criterion50ResultLetters2.h1;
					auto criterion50ResultLetters4 = criterion50(part, letterCounters, 4, 1);
					h1Counters["5.0-4.fp1"] += criterion50ResultLetters4.h1;
					auto criterion50ResultLetters6 = criterion50(part, letterCounters, 6, 2);
					h1Counters["5.0-6.fp1"] += criterion50ResultLetters6.h1;
					auto criterion50ResultBigrams50 = criterion50(part, bigramCounters, 50, 34);
					h1Counters["5.0.fp2"] += criterion50ResultBigrams50.h1;
					auto criterion50ResultBigrams100 = criterion50(part, bigramCounters, 100, 79);
					h1Counters["5.0.fp2"] += criterion50ResultBigrams100.h1;
					auto criterion50ResultBigrams200 = criterion50(part, bigramCounters, 200, 160);
					h1Counters["5.0.fp2"] += criterion50ResultBigrams200.h1;
				}
				if (l == 10000) {
					auto criterion50ResultLetters2 = criterion50(part, letterCounters, 2, 1); // пустых нет
					h1Counters["5.0-2.fp1"] += criterion50ResultLetters2.h1;
					auto criterion50ResultLetters4 = criterion50(part, letterCounters, 4, 1);
					h1Counters["5.0-4.fp1"] += criterion50ResultLetters4.h1;
					auto criterion50ResultLetters6 = criterion50(part, letterCounters, 6, 1);
					h1Counters["5.0-6.fp1"] += criterion50ResultLetters6.h1;
					auto criterion50ResultBigrams50 = criterion50(part, bigramCounters, 50, 20); // хотя бы 30 редких в последовательности есть
					h1Counters["5.0.fp2"] += criterion50ResultBigrams50.h1;
					auto criterion50ResultBigrams100 = criterion50(part, bigramCounters, 100, 67);
					h1Counters["5.0.fp2"] += criterion50ResultBigrams100.h1;
					auto criterion50ResultBigrams200 = criterion50(part, bigramCounters, 200, 149);
					h1Counters["5.0.fp2"] += criterion50ResultBigrams200.h1;
				}

				// Вывод критериев первого текста
				if (i == 0 && l == 10 && r == 1)
				{
					cout << "\nКритерий 2.0:" << endl;
					if (criterion20ResultLetters.h1)
					{
						cout << "Последовательность \"" << part << "\" случайна, потому что " <<
							"существует такая частая буква \"" << criterion20ResultLetters.unit << "\", которая отсутствует в " <<
							"последовательности" << endl;
					}
					else if (criterion20ResultBigrams.h1)
					{
						cout << "Последовательность \"" << part << "\" случайна, потому что " <<
							"существует такая частая биграмма \"" << criterion20ResultBigrams.unit << "\", которая отсутствует в " <<
							"последовательности" << endl;
					}
					else
					{
						cout << "Последовательность \"" << part << "\" неслучайна, потому что не существует такой " <<
							"частой буквы/биграммы, которая отсутствовала бы в последовательности" << endl;
					}

					cout << "\nКритерий 2.1:" << endl;
					if (criterion21ResultLetters.h1)
					{
						cout << "Последовательность \"" << part << "\" случайна, потому что мощность <= " <<
							criterion21ResultLetters.kf << endl;
					}
					else if (criterion21ResultBigrams.h1)
					{
						cout << "Последовательность \"" << part << "\" случайна, потому что мощность <= " <<
							criterion21ResultBigrams.kf << endl;
					}
					else
					{
						cout << "Последовательность \"" << part << "\" неслучайна, потому что мощность > " <<
							criterion21ResultLetters.kf << endl;
					}

					cout << "\nКритерий 2.2:" << endl;
					if (criterion22ResultLetters.h1)
					{
						cout << "Последовательность \"" << part << "\" случайна, потому что существует " <<
							"такая буква \"" << setprecision(3) << fixed << criterion22ResultLetters.unit << "\", для которой " <<
							criterion22ResultLetters.fx << " < " << criterion22ResultLetters.kx << endl;
					}
					else if (criterion22ResultBigrams.h1)
					{
						cout << "Последовательность \"" << part << "\" случайна, потому что существует " <<
							"такая биграмма \"" << criterion22ResultBigrams.unit << "\", для которой " <<
							criterion22ResultBigrams.fx << " < " << criterion22ResultBigrams.kx << endl;
					}
					else
					{
						cout << "Последовательность \"" << part << "\" неслучайна, потому что не существует такой " <<
							"буквы / биграммы, для которой f(x) < k(x)" << endl;
					}

					cout << "\nКритерий 2.3:" << endl;
					if (criterion23ResultLetters.h1 || criterion23ResultBigrams.h1)
					{
						cout << "Последовательность \"" << setprecision(3) << fixed << part << "\" случайна, потому что "
							<< criterion23ResultLetters.ff << " < " << criterion23ResultLetters.kf << endl;
					}
					else
					{
						cout << "Последовательность \"" << part << "\" неслучайна, потому что " << criterion23ResultLetters.ff
							<< " >= " << criterion23ResultLetters.kf << endl;
					}

					cout << "\nКритерий 4.0:" << endl;
					if (criterion40ResultLetters.h1)
					{
						cout << "Последовательность \"" << part << "\" случайна, потому что " << criterion40ResultLetters.k
							<< " > " << criterion40ResultLetters.kl << endl;
					}
					else if (criterion40ResultLetters.h1)
					{
						cout << "Последовательность \"" << part << "\" случайна, потому что " << criterion40ResultBigrams.k
							<< " > " << criterion40ResultBigrams.kl << endl;
					}
					else
					{
						cout << "Последовательность \"" << part << "\" неслучайна, потому что " << criterion40ResultLetters.k
							<< " <= " << criterion40ResultLetters.kl << endl;
					}

					/*cout << "\nКритерий 5.0:" << endl;
					if (criterion50ResultLetters2.h1)
					{
						cout << "Последовательность \"" << part << "\" случайна, потому что " << criterion50ResultLetters2.fempt
							<< " < " << criterion50ResultLetters2.kempt << endl;
					}
					else if (criterion50ResultBigrams50.h1)
					{
						cout << "Последовательность \"" << part << "\" случайна, потому что " << criterion50ResultBigrams50.fempt
							<< " < " << criterion50ResultBigrams50.kempt << endl;
					}
					else
					{
						cout << "Последовательность \"" << part << "\" неслучайна, потому что " << criterion50ResultLetters2.fempt
							<< " >= " << criterion50ResultLetters2.kempt << endl;
					}
					cout << endl;*/
				}
			}

			// Подчет FP и FN
			h1Counters["2.0.fp1"] /= (double)cicles[l];
			h1Counters["2.1.fp1"] /= (double)cicles[l];
			h1Counters["2.2.fp1"] /= (double)cicles[l];
			h1Counters["2.3.fp1"] /= (double)cicles[l];
			h1Counters["4.0.fp1"] /= (double)cicles[l];
			h1Counters["5.0.fp1-2"] /= (double)cicles[l];
			h1Counters["5.0.fp1-4"] /= (double)cicles[l];
			h1Counters["5.0.fp1-6"] /= (double)cicles[l];

			h1Counters["2.0.fn1"] = 1.0 - h1Counters["2.0.fp1"];
			h1Counters["2.1.fn1"] = 1.0 - h1Counters["2.1.fp1"];
			h1Counters["2.2.fn1"] = 1.0 - h1Counters["2.2.fp1"];
			h1Counters["2.3.fn1"] = 1.0 - h1Counters["2.3.fp1"];
			h1Counters["4.0.fn1"] = 1.0 - h1Counters["4.0.fp1"];
			h1Counters["5.0.fn1-2"] = 1.0 - h1Counters["5.0.fp1-2"];
			h1Counters["5.0.fn1-4"] = 1.0 - h1Counters["5.0.fp1-4"];
			h1Counters["5.0.fn1-6"] = 1.0 - h1Counters["5.0.fp1-6"];

			h1Counters["2.0.fp1"] = h1Counters["2.0.fp1_o"] / (double)cicles[l];
			h1Counters["2.1.fp1"] = h1Counters["2.1.fp1_o"] / (double)cicles[l];
			h1Counters["2.2.fp1"] = h1Counters["2.2.fp1_o"] / (double)cicles[l];
			h1Counters["2.3.fp1"] = h1Counters["2.3.fp1_o"] / (double)cicles[l];
			h1Counters["4.0.fp1"] = h1Counters["4.0.fp1_o"] / (double)cicles[l];
			h1Counters["5.0.fp1-2"] = h1Counters["5.0.fp1-2_o"] / (double)cicles[l];
			h1Counters["5.0.fp1-4"] = h1Counters["5.0.fp1-4_o"] / (double)cicles[l];
			h1Counters["5.0.fp1-6"] = h1Counters["5.0.fp1-6_o"] / (double)cicles[l];

			h1Counters["2.0.fp2"] /= (double)cicles[l];
			h1Counters["2.1.fp2"] /= (double)cicles[l];
			h1Counters["2.2.fp2"] /= (double)cicles[l];
			h1Counters["2.3.fp2"] /= (double)cicles[l];
			h1Counters["4.0.fp2"] /= (double)cicles[l];
			h1Counters["5.0.fp2-50"] /= (double)cicles[l];
			h1Counters["5.0.fp2-100"] /= (double)cicles[l];
			h1Counters["5.0.fp2-200"] /= (double)cicles[l];

			h1Counters["2.0.fn2"] = 1.0 - h1Counters["2.0.fp2"];
			h1Counters["2.1.fn2"] = 1.0 - h1Counters["2.1.fp2"];
			h1Counters["2.2.fn2"] = 1.0 - h1Counters["2.2.fp2"];
			h1Counters["2.3.fn2"] = 1.0 - h1Counters["2.3.fp2"];
			h1Counters["4.0.fn2"] = 1.0 - h1Counters["4.0.fp2"];
			h1Counters["5.0.fn2-50"] = 1.0 - h1Counters["5.0.fp2-50"];
			h1Counters["5.0.fn2-100"] = 1.0 - h1Counters["5.0.fp2-100"];
			h1Counters["5.0.fn2-200"] = 1.0 - h1Counters["5.0.fp2-200"];

			h1Counters["2.0.fp2"] = h1Counters["2.0.fp2_o"] / (double)cicles[l];
			h1Counters["2.1.fp2"] = h1Counters["2.1.fp2_o"] / (double)cicles[l];
			h1Counters["2.2.fp2"] = h1Counters["2.2.fp2_o"] / (double)cicles[l];
			h1Counters["2.3.fp2"] = h1Counters["2.3.fp2_o"] / (double)cicles[l];
			h1Counters["4.0.fp2"] = h1Counters["4.0.fp2_o"] / (double)cicles[l];
			h1Counters["5.0.fp2-50"] = h1Counters["5.0.fp2-50_o"] / (double)cicles[l];
			h1Counters["5.0.fp2-100"] = h1Counters["5.0.fp2-100_o"] / (double)cicles[l];
			h1Counters["5.0.fp2-200"] = h1Counters["5.0.fp2-200_o"] / (double)cicles[l];

			// Вывод таблиц критериев
			printCriterionsTable(h1Counters, l, r, "шифра Виженера");
			printCriterionsTable(h1Counters, l, r, "шифра Виженера", criteriaPassingResultsFile);
		}
	}
	cout << endl;

	// Сжатие алгоритмом Хаффмана
	auto haffmanResult = haffman("filtered_text.txt", "Huffman_res.txt");
	printCompressedTable(haffmanResult, "Хаффмана");

	// Сжатие алгоритмом Шеннона-Фано
	auto shannonFanoResult = shannonFano("filtered_text.txt", "ShannonFano_res.txt");
	printCompressedTable(shannonFanoResult, "Шеннона-Фано");

	// Подсчет структурного критерия
	int structureCriterionH1DEFLATECounter = 0;
	int structureCriterionH1GZipCounter = 0;
	int structureCriterionH1HaffmanCounter = 0;
	int structureCriterionH1ShannonFanoCounter = 0;

	int structureCriterionH1DEFLATECounterTopen = 0;
	int structureCriterionH1GZipCounterTopen = 0;
	int structureCriterionH1HaffmanCounterTopen = 0;
	int structureCriterionH1ShannonFanoCounterTopen = 0;

	int lenght = 10;
	string randomText(lenght, ' ');
	string damagedText(lenght, ' ');
	for (int i = 0; i < 10000; i++)
	{
		generateRandomText(randomText);
		copy(text.begin() + i * lenght, text.begin() + (i + 1) * lenght, damagedText.begin());

		auto randomTextHaffmane = haffman(randomText);
		auto randomTextShannonFano = shannonFano(randomText);
		auto randomTextDEFLATE = deflate(randomText);
		auto randomTextGZip = gzip(randomText);

		auto TextHaffmane = haffman(damagedText);
		auto TextShannonFano = shannonFano(damagedText);
		auto TextDEFLATE = deflate(damagedText);
		auto TextGZip = gzip(damagedText);

		auto resultTopen = structureCriterion(TextHaffmane, randomTextHaffmane);
		structureCriterionH1HaffmanCounterTopen += resultTopen.h1;

		resultTopen = structureCriterion(TextShannonFano, randomTextShannonFano);
		structureCriterionH1ShannonFanoCounterTopen += resultTopen.h1;

		resultTopen = structureCriterion(TextDEFLATE, randomTextDEFLATE);
		structureCriterionH1DEFLATECounterTopen += resultTopen.h1;

		resultTopen = structureCriterion(TextGZip, randomTextGZip);
		structureCriterionH1GZipCounterTopen += resultTopen.h1;

		vigener(damagedText);
		auto damagedTextHaffmane = haffman(damagedText);
		auto damagedTextShannonFano = shannonFano(damagedText);
		auto damagedTextDEFLATE = deflate(damagedText);
		auto damagedTextGZip = gzip(damagedText);

		auto result = structureCriterion(damagedTextHaffmane, randomTextHaffmane);
		structureCriterionH1HaffmanCounter += result.h1;

		result = structureCriterion(damagedTextShannonFano, randomTextShannonFano);
		structureCriterionH1ShannonFanoCounter += result.h1;

		result = structureCriterion(damagedTextDEFLATE, randomTextDEFLATE);
		structureCriterionH1DEFLATECounter += result.h1;

		result = structureCriterion(damagedTextGZip, randomTextGZip);
		structureCriterionH1GZipCounter += result.h1;
	}

	map<string, double> compressions;

	compressions["DEFLATE.FP"] = structureCriterionH1DEFLATECounterTopen / 10000.0;
	compressions["DEFLATE.FN"] = 1.0 - structureCriterionH1DEFLATECounter / 10000.0;

	compressions["GZip.FP"] = structureCriterionH1GZipCounterTopen / 10000.0;
	compressions["GZip.FN"] = 1.0 - structureCriterionH1GZipCounter / 10000.0;

	compressions["Huffman.FP"] = structureCriterionH1HaffmanCounterTopen / 10000.0;
	compressions["Huffman.FN"] = 1.0 - structureCriterionH1HaffmanCounter / 10000.0;

	compressions["ShannonFano.FP"] = structureCriterionH1ShannonFanoCounterTopen / 10000.0;
	compressions["ShannonFano.FN"] = 1.0 - structureCriterionH1ShannonFanoCounter / 10000.0;

	// Вывод структурного критерия
	printCompressionTable(compressions, lenght, "Виженер");
	printCompressionTable(compressions, lenght, "Виженер", criteriaPassingResultsFile);

	// Афинная подстановка (тоже самое только функция генерации текста отличается)

	for (auto l : ls)
	{
		map<string, double> h1Counters;

		for (int i = 0; i < cicles[l] && i * l < (int)text.size() - l; i++)
		{
			auto part = text.substr(i * l, l);
			string part_open = part;
			affineSubstitution(part);
			// Посчет критериев (ОТ)
			auto criterion20ResultLetters_o = criterion20(part_open, letterCounters, 5);
			h1Counters["2.0.fp1_o"] += criterion20ResultLetters_o.h1;
			auto criterion20ResultBigrams_o = criterion20(part_open, bigramCounters, 5);
			h1Counters["2.0.fp2_o"] += criterion20ResultBigrams_o.h1;

			auto criterion21ResultLetters_o = criterion21(part_open, letterCounters, 5, 2);
			h1Counters["2.1.fp1_o"] += criterion21ResultLetters_o.h1;
			auto criterion21ResultBigrams_o = criterion21(part_open, bigramCounters, 5, 2);
			h1Counters["2.1.fp2_o"] += criterion21ResultBigrams_o.h1;

			auto criterion22ResultLetters_o = criterion22(part_open, letterCounters, 5);
			h1Counters["2.2.fp1_o"] += criterion22ResultLetters_o.h1;
			auto criterion22ResultBigrams_o = criterion22(part_open, bigramCounters, 5);
			h1Counters["2.2.fp2_o"] += criterion22ResultBigrams_o.h1;

			auto criterion23ResultLetters_o = criterion23(part_open, letterCounters, 5);
			h1Counters["2.3.fp1_o"] += criterion23ResultLetters_o.h1;
			auto criterion23ResultBigrams_o = criterion23(part_open, bigramCounters, 5);
			h1Counters["2.3.fp2_o"] += criterion23ResultBigrams_o.h1;

			auto criterion40ResultLetters_o = criterion40(letterFrequency(part_open), (int)part.size(), letterCounters, (int)text.size(), 0.009);
			h1Counters["4.0.fp1_o"] += criterion40ResultLetters_o.h1;
			auto criterion40ResultBigrams_o = criterion40(bigramFrequency(part_open), (int)part.size(), bigramCounters, (int)text.size(), 0.004);
			h1Counters["4.0.fp2_o"] += criterion40ResultBigrams_o.h1;

			if (l == 10) {
				auto criterion50ResultLetters2_o = criterion50(part_open, letterCounters, 2, 1); // нет пустых ящиков, 2 редких буквы есть в последовательности
				h1Counters["5.0-2.fp1_o"] += criterion50ResultLetters2_o.h1;
				auto criterion50ResultLetters4_o = criterion50(part_open, letterCounters, 4, 2); // хотя бы 2 пустых есть
				h1Counters["5.0-4.fp1_o"] += criterion50ResultLetters4_o.h1;
				auto criterion50ResultLetters6_o = criterion50(part_open, letterCounters, 6, 3); // хотя бы 3 пустых есть
				h1Counters["5.0-6.fp1_o"] += criterion50ResultLetters6_o.h1;
				auto criterion50ResultBigrams50_o = criterion50(part_open, bigramCounters, 50, 48); // если из 50, попалась 2-1=1, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams50_o.h1;
				auto criterion50ResultBigrams100_o = criterion50(part_open, bigramCounters, 100, 97); // если из 100, попалось 2, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams100_o.h1;
				auto criterion50ResultBigrams200_o = criterion50(part_open, bigramCounters, 200, 196); // если из 200, попалось 3, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams200_o.h1;
			}
			if (l == 100) {
				auto criterion50ResultLetters2_o = criterion50(part_open, letterCounters, 2, 1);
				h1Counters["5.0-2.fp1_o"] += criterion50ResultLetters2_o.h1;
				auto criterion50ResultLetters4_o = criterion50(part_open, letterCounters, 4, 2);
				h1Counters["5.0-4.fp1_o"] += criterion50ResultLetters4_o.h1;
				auto criterion50ResultLetters6_o = criterion50(part_open, letterCounters, 6, 3);
				h1Counters["5.0-6.fp1_o"] += criterion50ResultLetters6_o.h1;
				auto criterion50ResultBigrams50_o = criterion50(part_open, bigramCounters, 50, 43);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams50_o.h1;
				auto criterion50ResultBigrams100_o = criterion50(part_open, bigramCounters, 100, 91);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams100_o.h1;
				auto criterion50ResultBigrams200_o = criterion50(part_open, bigramCounters, 200, 188);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams200_o.h1;
			}

			if (l == 1000) {
				auto criterion50ResultLetters2_o = criterion50(part_open, letterCounters, 2, 1); // пустых нет
				h1Counters["5.0-2.fp1_o"] += criterion50ResultLetters2_o.h1;
				auto criterion50ResultLetters4_o = criterion50(part_open, letterCounters, 4, 1);
				h1Counters["5.0-4.fp1_o"] += criterion50ResultLetters4_o.h1;
				auto criterion50ResultLetters6_o = criterion50(part_open, letterCounters, 6, 2);
				h1Counters["5.0-6.fp1_o"] += criterion50ResultLetters6_o.h1;
				auto criterion50ResultBigrams50_o = criterion50(part_open, bigramCounters, 50, 34);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams50_o.h1;
				auto criterion50ResultBigrams100_o = criterion50(part_open, bigramCounters, 100, 79);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams100_o.h1;
				auto criterion50ResultBigrams200_o = criterion50(part_open, bigramCounters, 200, 160);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams200_o.h1;
			}
			if (l == 10000) {
				auto criterion50ResultLetters2_o = criterion50(part_open, letterCounters, 2, 1); // пустых нет
				h1Counters["5.0-2.fp1_o"] += criterion50ResultLetters2_o.h1;
				auto criterion50ResultLetters4_o = criterion50(part_open, letterCounters, 4, 1);
				h1Counters["5.0-4.fp1_o"] += criterion50ResultLetters4_o.h1;
				auto criterion50ResultLetters6_o = criterion50(part_open, letterCounters, 6, 1);
				h1Counters["5.0-6.fp1_o"] += criterion50ResultLetters6_o.h1;
				auto criterion50ResultBigrams50_o = criterion50(part_open, bigramCounters, 50, 20); // хотя бы 30 редких в последовательности есть
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams50_o.h1;
				auto criterion50ResultBigrams100_o = criterion50(part_open, bigramCounters, 100, 67);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams100_o.h1;
				auto criterion50ResultBigrams200_o = criterion50(part_open, bigramCounters, 200, 149);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams200_o.h1;
			}

			auto criterion20ResultLetters = criterion20(part, letterCounters, 5);
			h1Counters["2.0.fp1"] += criterion20ResultLetters.h1;
			auto criterion20ResultBigrams = criterion20(part, bigramCounters, 5);
			h1Counters["2.0.fp2"] += criterion20ResultBigrams.h1;

			auto criterion21ResultLetters = criterion21(part, letterCounters, 5, 2);
			h1Counters["2.1.fp1"] += criterion21ResultLetters.h1;
			auto criterion21ResultBigrams = criterion21(part, bigramCounters, 5, 2);
			h1Counters["2.1.fp2"] += criterion21ResultBigrams.h1;

			auto criterion22ResultLetters = criterion22(part, letterCounters, (int)text.size(), 5);
			h1Counters["2.2.fp1"] += criterion22ResultLetters.h1;
			auto criterion22ResultBigrams = criterion22(part, bigramCounters, (int)text.size(), 5);
			h1Counters["2.2.fp2"] += criterion22ResultBigrams.h1;

			auto criterion23ResultLetters = criterion23(part, letterCounters, 5);
			h1Counters["2.3.fp1"] += criterion23ResultLetters.h1;
			auto criterion23ResultBigrams = criterion23(part, bigramCounters, 5);
			h1Counters["2.3.fp2"] += criterion23ResultBigrams.h1;

			auto criterion40ResultLetters = criterion40(letterFrequency(part), (int)part.size(), letterCounters, (int)text.size(), 0.009);
			h1Counters["4.0.fp1"] += criterion40ResultLetters.h1;
			auto criterion40ResultBigrams = criterion40(bigramFrequency(part), (int)part.size(), bigramCounters, (int)text.size(), 0.009);
			h1Counters["4.0.fp2"] += criterion40ResultBigrams.h1;

			if (l == 10) {
				auto criterion50ResultLetters2 = criterion50(part, letterCounters, 2, 1); // нет пустых ящиков, 2 редких буквы есть в последовательности
				h1Counters["5.0-2.fp1"] += criterion50ResultLetters2.h1;
				auto criterion50ResultLetters4 = criterion50(part, letterCounters, 4, 2); // хотя бы 2 пустых есть
				h1Counters["5.0-4.fp1"] += criterion50ResultLetters4.h1;
				auto criterion50ResultLetters6 = criterion50(part, letterCounters, 6, 3); // хотя бы 3 пустых есть
				h1Counters["5.0-6.fp1"] += criterion50ResultLetters6.h1;
				auto criterion50ResultBigrams50 = criterion50(part, bigramCounters, 50, 48); // если из 50, попалась 2-1=1, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2"] += criterion50ResultBigrams50.h1;
				auto criterion50ResultBigrams100 = criterion50(part, bigramCounters, 100, 97); // если из 100, попалось 2, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2"] += criterion50ResultBigrams100.h1;
				auto criterion50ResultBigrams200 = criterion50(part, bigramCounters, 200, 196); // если из 200, попалось 3, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2"] += criterion50ResultBigrams200.h1;
			}
			if (l == 100) {
				auto criterion50ResultLetters2 = criterion50(part, letterCounters, 2, 1);
				h1Counters["5.0-2.fp1"] += criterion50ResultLetters2.h1;
				auto criterion50ResultLetters4 = criterion50(part, letterCounters, 4, 2);
				h1Counters["5.0-4.fp1"] += criterion50ResultLetters4.h1;
				auto criterion50ResultLetters6 = criterion50(part, letterCounters, 6, 3);
				h1Counters["5.0-6.fp1"] += criterion50ResultLetters6.h1;
				auto criterion50ResultBigrams50 = criterion50(part, bigramCounters, 50, 43);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams50.h1;
				auto criterion50ResultBigrams100 = criterion50(part, bigramCounters, 100, 91);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams100.h1;
				auto criterion50ResultBigrams200 = criterion50(part, bigramCounters, 200, 188);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams200.h1;
			}

			if (l == 1000) {
				auto criterion50ResultLetters2 = criterion50(part, letterCounters, 2, 1); // пустых нет
				h1Counters["5.0-2.fp1"] += criterion50ResultLetters2.h1;
				auto criterion50ResultLetters4 = criterion50(part, letterCounters, 4, 1);
				h1Counters["5.0-4.fp1"] += criterion50ResultLetters4.h1;
				auto criterion50ResultLetters6 = criterion50(part, letterCounters, 6, 2);
				h1Counters["5.0-6.fp1"] += criterion50ResultLetters6.h1;
				auto criterion50ResultBigrams50 = criterion50(part, bigramCounters, 50, 34);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams50.h1;
				auto criterion50ResultBigrams100 = criterion50(part, bigramCounters, 100, 79);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams100.h1;
				auto criterion50ResultBigrams200 = criterion50(part, bigramCounters, 200, 160);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams200.h1;
			}
			if (l == 10000) {
				auto criterion50ResultLetters2 = criterion50(part, letterCounters, 2, 1); // пустых нет
				h1Counters["5.0-2.fp1"] += criterion50ResultLetters2.h1;
				auto criterion50ResultLetters4 = criterion50(part, letterCounters, 4, 1);
				h1Counters["5.0-4.fp1"] += criterion50ResultLetters4.h1;
				auto criterion50ResultLetters6 = criterion50(part, letterCounters, 6, 1);
				h1Counters["5.0-6.fp1"] += criterion50ResultLetters6.h1;
				auto criterion50ResultBigrams50 = criterion50(part, bigramCounters, 50, 20); // хотя бы 30 редких в последовательности есть
				h1Counters["5.0.fp2"] += criterion50ResultBigrams50.h1;
				auto criterion50ResultBigrams100 = criterion50(part, bigramCounters, 100, 67);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams100.h1;
				auto criterion50ResultBigrams200 = criterion50(part, bigramCounters, 200, 149);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams200.h1;
			}

			if (i == 0 && l == 10)
			{
				cout << "\nКритерий 2.0:" << endl;
				if (criterion20ResultLetters.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " <<
						"существует такая частая буква \"" << criterion20ResultLetters.unit << "\", которая отсутствует в " <<
						"последовательности" << endl;
				}
				else if (criterion20ResultBigrams.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " <<
						"существует такая частая биграмма \"" << criterion20ResultBigrams.unit << "\", которая отсутствует в " <<
						"последовательности" << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что не существует такой " <<
						"частой буквы/биграммы, которая отсутствовала бы в последовательности" << endl;
				}

				cout << "\nКритерий 2.1:" << endl;
				if (criterion21ResultLetters.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что мощность <= " <<
						criterion21ResultLetters.kf << endl;
				}
				else if (criterion21ResultBigrams.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что мощность <= " <<
						criterion21ResultBigrams.kf << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что мощность > " <<
						criterion21ResultLetters.kf << endl;
				}

				cout << "\nКритерий 2.2:" << endl;
				if (criterion22ResultLetters.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что существует " <<
						"такая буква \"" << setprecision(3) << fixed << criterion22ResultLetters.unit << "\", для которой " <<
						criterion22ResultLetters.fx << " < " << criterion22ResultLetters.kx << endl;
				}
				else if (criterion22ResultBigrams.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что существует " <<
						"такая биграмма \"" << criterion22ResultBigrams.unit << "\", для которой " <<
						criterion22ResultBigrams.fx << " < " << criterion22ResultBigrams.kx << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что не существует такой " <<
						"буквы / биграммы, для которой f(x) < k(x)" << endl;
				}

				cout << "\nКритерий 2.3:" << endl;
				if (criterion23ResultLetters.h1 || criterion23ResultBigrams.h1)
				{
					cout << "Последовательность \"" << setprecision(3) << fixed << part << "\" случайна, потому что "
						<< criterion23ResultLetters.ff << " < " << criterion23ResultLetters.kf << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что " << criterion23ResultLetters.ff
						<< " >= " << criterion23ResultLetters.kf << endl;
				}

				cout << "\nКритерий 4.0:" << endl;
				if (criterion40ResultLetters.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " << criterion40ResultLetters.k
						<< " > " << criterion40ResultLetters.kl << endl;
				}
				else if (criterion40ResultLetters.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " << criterion40ResultBigrams.k
						<< " > " << criterion40ResultBigrams.kl << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что " << criterion40ResultLetters.k
						<< " <= " << criterion40ResultLetters.kl << endl;
				}

				/*cout << "\nКритерий 5.0:" << endl;
				if (criterion50ResultLetters2.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " << criterion50ResultLetters2.fempt
						<< " < " << criterion50ResultLetters2.kempt << endl;
				}
				else if (criterion50ResultBigrams50.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " << criterion50ResultBigrams50.fempt
						<< " < " << criterion50ResultBigrams50.kempt << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что " << criterion50ResultLetters2.fempt
						<< " >= " << criterion50ResultLetters2.kempt << endl;
				}
				cout << endl;*/
			}
		}

		// Подчет FP и FN
		h1Counters["2.0.fp1"] /= (double)cicles[l];
		h1Counters["2.1.fp1"] /= (double)cicles[l];
		h1Counters["2.2.fp1"] /= (double)cicles[l];
		h1Counters["2.3.fp1"] /= (double)cicles[l];
		h1Counters["4.0.fp1"] /= (double)cicles[l];
		h1Counters["5.0.fp1-2"] /= (double)cicles[l];
		h1Counters["5.0.fp1-4"] /= (double)cicles[l];
		h1Counters["5.0.fp1-6"] /= (double)cicles[l];

		h1Counters["2.0.fn1"] = 1.0 - h1Counters["2.0.fp1"];
		h1Counters["2.1.fn1"] = 1.0 - h1Counters["2.1.fp1"];
		h1Counters["2.2.fn1"] = 1.0 - h1Counters["2.2.fp1"];
		h1Counters["2.3.fn1"] = 1.0 - h1Counters["2.3.fp1"];
		h1Counters["4.0.fn1"] = 1.0 - h1Counters["4.0.fp1"];
		h1Counters["5.0.fn1-2"] = 1.0 - h1Counters["5.0.fp1-2"];
		h1Counters["5.0.fn1-4"] = 1.0 - h1Counters["5.0.fp1-4"];
		h1Counters["5.0.fn1-6"] = 1.0 - h1Counters["5.0.fp1-6"];

		h1Counters["2.0.fp1"] = h1Counters["2.0.fp1_o"] / (double)cicles[l];
		h1Counters["2.1.fp1"] = h1Counters["2.1.fp1_o"] / (double)cicles[l];
		h1Counters["2.2.fp1"] = h1Counters["2.2.fp1_o"] / (double)cicles[l];
		h1Counters["2.3.fp1"] = h1Counters["2.3.fp1_o"] / (double)cicles[l];
		h1Counters["4.0.fp1"] = h1Counters["4.0.fp1_o"] / (double)cicles[l];
		h1Counters["5.0.fp1-2"] = h1Counters["5.0.fp1-2_o"] / (double)cicles[l];
		h1Counters["5.0.fp1-4"] = h1Counters["5.0.fp1-4_o"] / (double)cicles[l];
		h1Counters["5.0.fp1-6"] = h1Counters["5.0.fp1-6_o"] / (double)cicles[l];

		h1Counters["2.0.fp2"] /= (double)cicles[l];
		h1Counters["2.1.fp2"] /= (double)cicles[l];
		h1Counters["2.2.fp2"] /= (double)cicles[l];
		h1Counters["2.3.fp2"] /= (double)cicles[l];
		h1Counters["4.0.fp2"] /= (double)cicles[l];
		h1Counters["5.0.fp2-50"] /= (double)cicles[l];
		h1Counters["5.0.fp2-100"] /= (double)cicles[l];
		h1Counters["5.0.fp2-200"] /= (double)cicles[l];

		h1Counters["2.0.fn2"] = 1.0 - h1Counters["2.0.fp2"];
		h1Counters["2.1.fn2"] = 1.0 - h1Counters["2.1.fp2"];
		h1Counters["2.2.fn2"] = 1.0 - h1Counters["2.2.fp2"];
		h1Counters["2.3.fn2"] = 1.0 - h1Counters["2.3.fp2"];
		h1Counters["4.0.fn2"] = 1.0 - h1Counters["4.0.fp2"];
		h1Counters["5.0.fn2-50"] = 1.0 - h1Counters["5.0.fp2-50"];
		h1Counters["5.0.fn2-100"] = 1.0 - h1Counters["5.0.fp2-100"];
		h1Counters["5.0.fn2-200"] = 1.0 - h1Counters["5.0.fp2-200"];

		h1Counters["2.0.fp2"] = h1Counters["2.0.fp2_o"] / (double)cicles[l];
		h1Counters["2.1.fp2"] = h1Counters["2.1.fp2_o"] / (double)cicles[l];
		h1Counters["2.2.fp2"] = h1Counters["2.2.fp2_o"] / (double)cicles[l];
		h1Counters["2.3.fp2"] = h1Counters["2.3.fp2_o"] / (double)cicles[l];
		h1Counters["4.0.fp2"] = h1Counters["4.0.fp2_o"] / (double)cicles[l];
		h1Counters["5.0.fp2-50"] = h1Counters["5.0.fp2-50_o"] / (double)cicles[l];
		h1Counters["5.0.fp2-100"] = h1Counters["5.0.fp2-100_o"] / (double)cicles[l];
		h1Counters["5.0.fp2-200"] = h1Counters["5.0.fp2-200_o"] / (double)cicles[l];

		printCriterionsTable(h1Counters, l, "афинной постановки");
		printCriterionsTable(h1Counters, l, "афинной постановки", criteriaPassingResultsFile);
	}

	// Подсчет структурного критерия
	structureCriterionH1DEFLATECounter = 0;
	structureCriterionH1GZipCounter = 0;
	structureCriterionH1HaffmanCounter = 0;
	structureCriterionH1ShannonFanoCounter = 0;

	lenght = 10;
	for (int i = 0; i < 10000; i++)
	{
		generateRandomText(randomText);
		copy(text.begin() + i * lenght, text.begin() + (i + 1) * lenght, damagedText.begin());

		auto randomTextHaffmane = haffman(randomText);
		auto randomTextShannonFano = shannonFano(randomText);
		auto randomTextDEFLATE = deflate(randomText);
		auto randomTextGZip = gzip(randomText);

		auto TextHaffmane = haffman(damagedText);
		auto TextShannonFano = shannonFano(damagedText);
		auto TextDEFLATE = deflate(damagedText);
		auto TextGZip = gzip(damagedText);

		auto resultTopen = structureCriterion(TextHaffmane, randomTextHaffmane);
		structureCriterionH1HaffmanCounterTopen += resultTopen.h1;

		resultTopen = structureCriterion(TextShannonFano, randomTextShannonFano);
		structureCriterionH1ShannonFanoCounterTopen += resultTopen.h1;

		resultTopen = structureCriterion(TextDEFLATE, randomTextDEFLATE);
		structureCriterionH1DEFLATECounterTopen += resultTopen.h1;

		resultTopen = structureCriterion(TextGZip, randomTextGZip);
		structureCriterionH1GZipCounterTopen += resultTopen.h1;

		affineSubstitution(damagedText);

		auto damagedTextHaffmane = haffman(damagedText);
		auto damagedTextShannonFano = shannonFano(damagedText);
		auto damagedTextDEFLATE = deflate(damagedText);
		auto damagedTextGZip = gzip(damagedText);

		auto result = structureCriterion(damagedTextHaffmane, randomTextHaffmane);
		structureCriterionH1HaffmanCounter += result.h1;

		result = structureCriterion(damagedTextShannonFano, randomTextShannonFano);
		structureCriterionH1ShannonFanoCounter += result.h1;

		result = structureCriterion(damagedTextDEFLATE, randomTextDEFLATE);
		structureCriterionH1DEFLATECounter += result.h1;

		result = structureCriterion(damagedTextGZip, randomTextGZip);
		structureCriterionH1GZipCounter += result.h1;
	}

	compressions["DEFLATE.FP"] = structureCriterionH1DEFLATECounterTopen / 10000.0;
	compressions["DEFLATE.FN"] = 1.0 - structureCriterionH1DEFLATECounter / 10000.0;

	compressions["GZip.FP"] = structureCriterionH1GZipCounterTopen / 10000.0;
	compressions["GZip.FN"] = 1.0 - structureCriterionH1GZipCounter / 10000.0;

	compressions["Huffman.FP"] = structureCriterionH1HaffmanCounterTopen / 10000.0;
	compressions["Huffman.FN"] = 1.0 - structureCriterionH1HaffmanCounter / 10000.0;

	compressions["ShannonFano.FP"] = structureCriterionH1ShannonFanoCounterTopen / 10000.0;
	compressions["ShannonFano.FN"] = 1.0 - structureCriterionH1ShannonFanoCounter / 10000.0;

	// Вывод структурного критерия
	printCompressionTable(compressions, lenght, "Афинная подстановка");
	printCompressionTable(compressions, lenght, "Афинная подстановка", criteriaPassingResultsFile);

	// Равномерное заполнение (тоже самое только функция генерации текста отличается)

	for (auto l : ls)
	{
		map<string, double> h1Counters;
		string part(l, ' ');
		for (int i = 0; i < cicles[l]; i++)
		{
			string part_open = part;
			uniformDistribution(part);
			// Посчет критериев (ОТ)
			auto criterion20ResultLetters_o = criterion20(part_open, letterCounters, 5);
			h1Counters["2.0.fp1_o"] += criterion20ResultLetters_o.h1;
			auto criterion20ResultBigrams_o = criterion20(part_open, bigramCounters, 5);
			h1Counters["2.0.fp2_o"] += criterion20ResultBigrams_o.h1;

			auto criterion21ResultLetters_o = criterion21(part_open, letterCounters, 5, 2);
			h1Counters["2.1.fp1_o"] += criterion21ResultLetters_o.h1;
			auto criterion21ResultBigrams_o = criterion21(part_open, bigramCounters, 5, 2);
			h1Counters["2.1.fp2_o"] += criterion21ResultBigrams_o.h1;

			auto criterion22ResultLetters_o = criterion22(part_open, letterCounters, 5);
			h1Counters["2.2.fp1_o"] += criterion22ResultLetters_o.h1;
			auto criterion22ResultBigrams_o = criterion22(part_open, bigramCounters, 5);
			h1Counters["2.2.fp2_o"] += criterion22ResultBigrams_o.h1;

			auto criterion23ResultLetters_o = criterion23(part_open, letterCounters, 5);
			h1Counters["2.3.fp1_o"] += criterion23ResultLetters_o.h1;
			auto criterion23ResultBigrams_o = criterion23(part_open, bigramCounters, 5);
			h1Counters["2.3.fp2_o"] += criterion23ResultBigrams_o.h1;

			auto criterion40ResultLetters_o = criterion40(letterFrequency(part_open), (int)part.size(), letterCounters, (int)text.size(), 0.009);
			h1Counters["4.0.fp1_o"] += criterion40ResultLetters_o.h1;
			auto criterion40ResultBigrams_o = criterion40(bigramFrequency(part_open), (int)part.size(), bigramCounters, (int)text.size(), 0.004);
			h1Counters["4.0.fp2_o"] += criterion40ResultBigrams_o.h1;

			if (l == 10) {
				auto criterion50ResultLetters2_o = criterion50(part_open, letterCounters, 2, 1); // нет пустых ящиков, 2 редких буквы есть в последовательности
				h1Counters["5.0-2.fp1_o"] += criterion50ResultLetters2_o.h1;
				auto criterion50ResultLetters4_o = criterion50(part_open, letterCounters, 4, 2); // хотя бы 2 пустых есть
				h1Counters["5.0-4.fp1_o"] += criterion50ResultLetters4_o.h1;
				auto criterion50ResultLetters6_o = criterion50(part_open, letterCounters, 6, 3); // хотя бы 3 пустых есть
				h1Counters["5.0-6.fp1_o"] += criterion50ResultLetters6_o.h1;
				auto criterion50ResultBigrams50_o = criterion50(part_open, bigramCounters, 50, 48); // если из 50, попалась 2-1=1, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams50_o.h1;
				auto criterion50ResultBigrams100_o = criterion50(part_open, bigramCounters, 100, 97); // если из 100, попалось 2, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams100_o.h1;
				auto criterion50ResultBigrams200_o = criterion50(part_open, bigramCounters, 200, 196); // если из 200, попалось 3, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams200_o.h1;
			}
			if (l == 100) {
				auto criterion50ResultLetters2_o = criterion50(part_open, letterCounters, 2, 1);
				h1Counters["5.0-2.fp1_o"] += criterion50ResultLetters2_o.h1;
				auto criterion50ResultLetters4_o = criterion50(part_open, letterCounters, 4, 2);
				h1Counters["5.0-4.fp1_o"] += criterion50ResultLetters4_o.h1;
				auto criterion50ResultLetters6_o = criterion50(part_open, letterCounters, 6, 3);
				h1Counters["5.0-6.fp1_o"] += criterion50ResultLetters6_o.h1;
				auto criterion50ResultBigrams50_o = criterion50(part_open, bigramCounters, 50, 43);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams50_o.h1;
				auto criterion50ResultBigrams100_o = criterion50(part_open, bigramCounters, 100, 91);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams100_o.h1;
				auto criterion50ResultBigrams200_o = criterion50(part_open, bigramCounters, 200, 188);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams200_o.h1;
			}

			if (l == 1000) {
				auto criterion50ResultLetters2_o = criterion50(part_open, letterCounters, 2, 1); // пустых нет
				h1Counters["5.0-2.fp1_o"] += criterion50ResultLetters2_o.h1;
				auto criterion50ResultLetters4_o = criterion50(part_open, letterCounters, 4, 1);
				h1Counters["5.0-4.fp1_o"] += criterion50ResultLetters4_o.h1;
				auto criterion50ResultLetters6_o = criterion50(part_open, letterCounters, 6, 2);
				h1Counters["5.0-6.fp1_o"] += criterion50ResultLetters6_o.h1;
				auto criterion50ResultBigrams50_o = criterion50(part_open, bigramCounters, 50, 34);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams50_o.h1;
				auto criterion50ResultBigrams100_o = criterion50(part_open, bigramCounters, 100, 79);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams100_o.h1;
				auto criterion50ResultBigrams200_o = criterion50(part_open, bigramCounters, 200, 160);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams200_o.h1;
			}
			if (l == 10000) {
				auto criterion50ResultLetters2_o = criterion50(part_open, letterCounters, 2, 1); // пустых нет
				h1Counters["5.0-2.fp1_o"] += criterion50ResultLetters2_o.h1;
				auto criterion50ResultLetters4_o = criterion50(part_open, letterCounters, 4, 1);
				h1Counters["5.0-4.fp1_o"] += criterion50ResultLetters4_o.h1;
				auto criterion50ResultLetters6_o = criterion50(part_open, letterCounters, 6, 1);
				h1Counters["5.0-6.fp1_o"] += criterion50ResultLetters6_o.h1;
				auto criterion50ResultBigrams50_o = criterion50(part_open, bigramCounters, 50, 20); // хотя бы 30 редких в последовательности есть
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams50_o.h1;
				auto criterion50ResultBigrams100_o = criterion50(part_open, bigramCounters, 100, 67);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams100_o.h1;
				auto criterion50ResultBigrams200_o = criterion50(part_open, bigramCounters, 200, 149);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams200_o.h1;
			}

			auto criterion20ResultLetters = criterion20(part, letterCounters, 5);
			h1Counters["2.0.fp1"] += criterion20ResultLetters.h1;
			auto criterion20ResultBigrams = criterion20(part, bigramCounters, 5);
			h1Counters["2.0.fp2"] += criterion20ResultBigrams.h1;

			auto criterion21ResultLetters = criterion21(part, letterCounters, 5, 2);
			h1Counters["2.1.fp1"] += criterion21ResultLetters.h1;
			auto criterion21ResultBigrams = criterion21(part, bigramCounters, 5, 2);
			h1Counters["2.1.fp2"] += criterion21ResultBigrams.h1;

			auto criterion22ResultLetters = criterion22(part, letterCounters, (int)text.size(), 5);
			h1Counters["2.2.fp1"] += criterion22ResultLetters.h1;
			auto criterion22ResultBigrams = criterion22(part, bigramCounters, (int)text.size(), 5);
			h1Counters["2.2.fp2"] += criterion22ResultBigrams.h1;

			auto criterion23ResultLetters = criterion23(part, letterCounters, 5);
			h1Counters["2.3.fp1"] += criterion23ResultLetters.h1;
			auto criterion23ResultBigrams = criterion23(part, bigramCounters, 5);
			h1Counters["2.3.fp2"] += criterion23ResultBigrams.h1;

			auto criterion40ResultLetters = criterion40(letterFrequency(part), (int)part.size(), letterCounters, (int)text.size(), 0.009);
			h1Counters["4.0.fp1"] += criterion40ResultLetters.h1;
			auto criterion40ResultBigrams = criterion40(bigramFrequency(part), (int)part.size(), bigramCounters, (int)text.size(), 0.009);
			h1Counters["4.0.fp2"] += criterion40ResultBigrams.h1;

			if (l == 10) {
				auto criterion50ResultLetters2 = criterion50(part, letterCounters, 2, 1); // нет пустых ящиков, 2 редких буквы есть в последовательности
				h1Counters["5.0-2.fp1"] += criterion50ResultLetters2.h1;
				auto criterion50ResultLetters4 = criterion50(part, letterCounters, 4, 2); // хотя бы 2 пустых есть
				h1Counters["5.0-4.fp1"] += criterion50ResultLetters4.h1;
				auto criterion50ResultLetters6 = criterion50(part, letterCounters, 6, 3); // хотя бы 3 пустых есть
				h1Counters["5.0-6.fp1"] += criterion50ResultLetters6.h1;
				auto criterion50ResultBigrams50 = criterion50(part, bigramCounters, 50, 48); // если из 50, попалась 2-1=1, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2"] += criterion50ResultBigrams50.h1;
				auto criterion50ResultBigrams100 = criterion50(part, bigramCounters, 100, 97); // если из 100, попалось 2, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2"] += criterion50ResultBigrams100.h1;
				auto criterion50ResultBigrams200 = criterion50(part, bigramCounters, 200, 196); // если из 200, попалось 3, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2"] += criterion50ResultBigrams200.h1;
			}
			if (l == 100) {
				auto criterion50ResultLetters2 = criterion50(part, letterCounters, 2, 1);
				h1Counters["5.0-2.fp1"] += criterion50ResultLetters2.h1;
				auto criterion50ResultLetters4 = criterion50(part, letterCounters, 4, 2);
				h1Counters["5.0-4.fp1"] += criterion50ResultLetters4.h1;
				auto criterion50ResultLetters6 = criterion50(part, letterCounters, 6, 3);
				h1Counters["5.0-6.fp1"] += criterion50ResultLetters6.h1;
				auto criterion50ResultBigrams50 = criterion50(part, bigramCounters, 50, 43);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams50.h1;
				auto criterion50ResultBigrams100 = criterion50(part, bigramCounters, 100, 91);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams100.h1;
				auto criterion50ResultBigrams200 = criterion50(part, bigramCounters, 200, 188);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams200.h1;
			}

			if (l == 1000) {
				auto criterion50ResultLetters2 = criterion50(part, letterCounters, 2, 1); // пустых нет
				h1Counters["5.0-2.fp1"] += criterion50ResultLetters2.h1;
				auto criterion50ResultLetters4 = criterion50(part, letterCounters, 4, 1);
				h1Counters["5.0-4.fp1"] += criterion50ResultLetters4.h1;
				auto criterion50ResultLetters6 = criterion50(part, letterCounters, 6, 2);
				h1Counters["5.0-6.fp1"] += criterion50ResultLetters6.h1;
				auto criterion50ResultBigrams50 = criterion50(part, bigramCounters, 50, 34);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams50.h1;
				auto criterion50ResultBigrams100 = criterion50(part, bigramCounters, 100, 79);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams100.h1;
				auto criterion50ResultBigrams200 = criterion50(part, bigramCounters, 200, 160);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams200.h1;
			}
			if (l == 10000) {
				auto criterion50ResultLetters2 = criterion50(part, letterCounters, 2, 1); // пустых нет
				h1Counters["5.0-2.fp1"] += criterion50ResultLetters2.h1;
				auto criterion50ResultLetters4 = criterion50(part, letterCounters, 4, 1);
				h1Counters["5.0-4.fp1"] += criterion50ResultLetters4.h1;
				auto criterion50ResultLetters6 = criterion50(part, letterCounters, 6, 1);
				h1Counters["5.0-6.fp1"] += criterion50ResultLetters6.h1;
				auto criterion50ResultBigrams50 = criterion50(part, bigramCounters, 50, 20); // хотя бы 30 редких в последовательности есть
				h1Counters["5.0.fp2"] += criterion50ResultBigrams50.h1;
				auto criterion50ResultBigrams100 = criterion50(part, bigramCounters, 100, 67);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams100.h1;
				auto criterion50ResultBigrams200 = criterion50(part, bigramCounters, 200, 149);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams200.h1;
			}

			if (i == 0 && l == 10)
			{
				cout << "\nКритерий 2.0:" << endl;
				if (criterion20ResultLetters.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " <<
						"существует такая частая буква \"" << criterion20ResultLetters.unit << "\", которая отсутствует в " <<
						"последовательности" << endl;
				}
				else if (criterion20ResultBigrams.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " <<
						"существует такая частая биграмма \"" << criterion20ResultBigrams.unit << "\", которая отсутствует в " <<
						"последовательности" << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что не существует такой " <<
						"частой буквы/биграммы, которая отсутствовала бы в последовательности" << endl;
				}

				cout << "\nКритерий 2.1:" << endl;
				if (criterion21ResultLetters.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что мощность <= " <<
						criterion21ResultLetters.kf << endl;
				}
				else if (criterion21ResultBigrams.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что мощность <= " <<
						criterion21ResultBigrams.kf << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что мощность > " <<
						criterion21ResultLetters.kf << endl;
				}

				cout << "\nКритерий 2.2:" << endl;
				if (criterion22ResultLetters.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что существует " <<
						"такая буква \"" << setprecision(3) << fixed << criterion22ResultLetters.unit << "\", для которой " <<
						criterion22ResultLetters.fx << " < " << criterion22ResultLetters.kx << endl;
				}
				else if (criterion22ResultBigrams.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что существует " <<
						"такая биграмма \"" << criterion22ResultBigrams.unit << "\", для которой " <<
						criterion22ResultBigrams.fx << " < " << criterion22ResultBigrams.kx << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что не существует такой " <<
						"буквы / биграммы, для которой f(x) < k(x)" << endl;
				}

				cout << "\nКритерий 2.3:" << endl;
				if (criterion23ResultLetters.h1 || criterion23ResultBigrams.h1)
				{
					cout << "Последовательность \"" << setprecision(3) << fixed << part << "\" случайна, потому что "
						<< criterion23ResultLetters.ff << " < " << criterion23ResultLetters.kf << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что " << criterion23ResultLetters.ff
						<< " >= " << criterion23ResultLetters.kf << endl;
				}

				cout << "\nКритерий 4.0:" << endl;
				if (criterion40ResultLetters.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " << criterion40ResultLetters.k
						<< " > " << criterion40ResultLetters.kl << endl;
				}
				else if (criterion40ResultLetters.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " << criterion40ResultBigrams.k
						<< " > " << criterion40ResultBigrams.kl << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что " << criterion40ResultLetters.k
						<< " <= " << criterion40ResultLetters.kl << endl;
				}

				/*cout << "\nКритерий 5.0:" << endl;
				if (criterion50ResultLetters2.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " << criterion50ResultLetters2.fempt
						<< " < " << criterion50ResultLetters2.kempt << endl;
				}
				else if (criterion50ResultBigrams50.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " << criterion50ResultBigrams50.fempt
						<< " < " << criterion50ResultBigrams50.kempt << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что " << criterion50ResultLetters2.fempt
						<< " >= " << criterion50ResultLetters2.kempt << endl;
				}
				cout << endl;*/
			}
		}

		// Подчет FP и FN
		h1Counters["2.0.fp1"] /= (double)cicles[l];
		h1Counters["2.1.fp1"] /= (double)cicles[l];
		h1Counters["2.2.fp1"] /= (double)cicles[l];
		h1Counters["2.3.fp1"] /= (double)cicles[l];
		h1Counters["4.0.fp1"] /= (double)cicles[l];
		h1Counters["5.0.fp1-2"] /= (double)cicles[l];
		h1Counters["5.0.fp1-4"] /= (double)cicles[l];
		h1Counters["5.0.fp1-6"] /= (double)cicles[l];

		h1Counters["2.0.fn1"] = 1.0 - h1Counters["2.0.fp1"];
		h1Counters["2.1.fn1"] = 1.0 - h1Counters["2.1.fp1"];
		h1Counters["2.2.fn1"] = 1.0 - h1Counters["2.2.fp1"];
		h1Counters["2.3.fn1"] = 1.0 - h1Counters["2.3.fp1"];
		h1Counters["4.0.fn1"] = 1.0 - h1Counters["4.0.fp1"];
		h1Counters["5.0.fn1-2"] = 1.0 - h1Counters["5.0.fp1-2"];
		h1Counters["5.0.fn1-4"] = 1.0 - h1Counters["5.0.fp1-4"];
		h1Counters["5.0.fn1-6"] = 1.0 - h1Counters["5.0.fp1-6"];

		h1Counters["2.0.fp1"] = h1Counters["2.0.fp1_o"] / (double)cicles[l];
		h1Counters["2.1.fp1"] = h1Counters["2.1.fp1_o"] / (double)cicles[l];
		h1Counters["2.2.fp1"] = h1Counters["2.2.fp1_o"] / (double)cicles[l];
		h1Counters["2.3.fp1"] = h1Counters["2.3.fp1_o"] / (double)cicles[l];
		h1Counters["4.0.fp1"] = h1Counters["4.0.fp1_o"] / (double)cicles[l];
		h1Counters["5.0.fp1-2"] = h1Counters["5.0.fp1-2_o"] / (double)cicles[l];
		h1Counters["5.0.fp1-4"] = h1Counters["5.0.fp1-4_o"] / (double)cicles[l];
		h1Counters["5.0.fp1-6"] = h1Counters["5.0.fp1-6_o"] / (double)cicles[l];

		h1Counters["2.0.fp2"] /= (double)cicles[l];
		h1Counters["2.1.fp2"] /= (double)cicles[l];
		h1Counters["2.2.fp2"] /= (double)cicles[l];
		h1Counters["2.3.fp2"] /= (double)cicles[l];
		h1Counters["4.0.fp2"] /= (double)cicles[l];
		h1Counters["5.0.fp2-50"] /= (double)cicles[l];
		h1Counters["5.0.fp2-100"] /= (double)cicles[l];
		h1Counters["5.0.fp2-200"] /= (double)cicles[l];

		h1Counters["2.0.fn2"] = 1.0 - h1Counters["2.0.fp2"];
		h1Counters["2.1.fn2"] = 1.0 - h1Counters["2.1.fp2"];
		h1Counters["2.2.fn2"] = 1.0 - h1Counters["2.2.fp2"];
		h1Counters["2.3.fn2"] = 1.0 - h1Counters["2.3.fp2"];
		h1Counters["4.0.fn2"] = 1.0 - h1Counters["4.0.fp2"];
		h1Counters["5.0.fn2-50"] = 1.0 - h1Counters["5.0.fp2-50"];
		h1Counters["5.0.fn2-100"] = 1.0 - h1Counters["5.0.fp2-100"];
		h1Counters["5.0.fn2-200"] = 1.0 - h1Counters["5.0.fp2-200"];

		h1Counters["2.0.fp2"] = h1Counters["2.0.fp2_o"] / (double)cicles[l];
		h1Counters["2.1.fp2"] = h1Counters["2.1.fp2_o"] / (double)cicles[l];
		h1Counters["2.2.fp2"] = h1Counters["2.2.fp2_o"] / (double)cicles[l];
		h1Counters["2.3.fp2"] = h1Counters["2.3.fp2_o"] / (double)cicles[l];
		h1Counters["4.0.fp2"] = h1Counters["4.0.fp2_o"] / (double)cicles[l];
		h1Counters["5.0.fp2-50"] = h1Counters["5.0.fp2-50_o"] / (double)cicles[l];
		h1Counters["5.0.fp2-100"] = h1Counters["5.0.fp2-100_o"] / (double)cicles[l];
		h1Counters["5.0.fp2-200"] = h1Counters["5.0.fp2-200_o"] / (double)cicles[l];

		printCriterionsTable(h1Counters, l, "равномерного распределения");
		printCriterionsTable(h1Counters, l, "равномерного распределения", criteriaPassingResultsFile);
	}

	// Подсчет структурного критерия
	structureCriterionH1DEFLATECounter = 0;
	structureCriterionH1GZipCounter = 0;
	structureCriterionH1HaffmanCounter = 0;
	structureCriterionH1ShannonFanoCounter = 0;

	lenght = 10;
	for (int i = 0; i < 10000; i++)
	{
		generateRandomText(randomText);
		copy(text.begin() + i * lenght, text.begin() + (i + 1) * lenght, damagedText.begin());

		auto randomTextHaffmane = haffman(randomText);
		auto randomTextShannonFano = shannonFano(randomText);
		auto randomTextDEFLATE = deflate(randomText);
		auto randomTextGZip = gzip(randomText);

		auto TextHaffmane = haffman(damagedText);
		auto TextShannonFano = shannonFano(damagedText);
		auto TextDEFLATE = deflate(damagedText);
		auto TextGZip = gzip(damagedText);

		auto resultTopen = structureCriterion(TextHaffmane, randomTextHaffmane);
		structureCriterionH1HaffmanCounterTopen += resultTopen.h1;

		resultTopen = structureCriterion(TextShannonFano, randomTextShannonFano);
		structureCriterionH1ShannonFanoCounterTopen += resultTopen.h1;

		resultTopen = structureCriterion(TextDEFLATE, randomTextDEFLATE);
		structureCriterionH1DEFLATECounterTopen += resultTopen.h1;

		resultTopen = structureCriterion(TextGZip, randomTextGZip);
		structureCriterionH1GZipCounterTopen += resultTopen.h1;

		uniformDistribution(damagedText);

		auto damagedTextHaffmane = haffman(damagedText);
		auto damagedTextShannonFano = shannonFano(damagedText);
		auto damagedTextDEFLATE = deflate(damagedText);
		auto damagedTextGZip = gzip(damagedText);

		auto result = structureCriterion(damagedTextHaffmane, randomTextHaffmane);
		structureCriterionH1HaffmanCounter += result.h1;

		result = structureCriterion(damagedTextShannonFano, randomTextShannonFano);
		structureCriterionH1ShannonFanoCounter += result.h1;

		result = structureCriterion(damagedTextDEFLATE, randomTextDEFLATE);
		structureCriterionH1DEFLATECounter += result.h1;

		result = structureCriterion(damagedTextGZip, randomTextGZip);
		structureCriterionH1GZipCounter += result.h1;
	}


	compressions["DEFLATE.FP"] = structureCriterionH1DEFLATECounterTopen / 10000.0;
	compressions["DEFLATE.FN"] = 1.0 - structureCriterionH1DEFLATECounter / 10000.0;

	compressions["GZip.FP"] = structureCriterionH1GZipCounterTopen / 10000.0;
	compressions["GZip.FN"] = 1.0 - structureCriterionH1GZipCounter / 10000.0;

	compressions["Huffman.FP"] = structureCriterionH1HaffmanCounterTopen / 10000.0;
	compressions["Huffman.FN"] = 1.0 - structureCriterionH1HaffmanCounter / 10000.0;

	compressions["ShannonFano.FP"] = structureCriterionH1ShannonFanoCounterTopen / 10000.0;
	compressions["ShannonFano.FN"] = 1.0 - structureCriterionH1ShannonFanoCounter / 10000.0;

	// Вывод структурного критерия
	printCompressionTable(compressions, lenght, "Равномерного распределение");
	printCompressionTable(compressions, lenght, "Равномерного распределение", criteriaPassingResultsFile);

	// Генерция текста на осно соотношения (тоже самое только функция генерации текста отличается)

	for (auto l : ls)
	{
		map<string, double> h1Counters;
		string part(l, ' ');
		for (int i = 0; i < cicles[l]; i++)
		{
			string part_open = part;
			distribution(part);
			// Посчет критериев (ОТ)
			auto criterion20ResultLetters_o = criterion20(part_open, letterCounters, 5);
			h1Counters["2.0.fp1_o"] += criterion20ResultLetters_o.h1;
			auto criterion20ResultBigrams_o = criterion20(part_open, bigramCounters, 5);
			h1Counters["2.0.fp2_o"] += criterion20ResultBigrams_o.h1;

			auto criterion21ResultLetters_o = criterion21(part_open, letterCounters, 5, 2);
			h1Counters["2.1.fp1_o"] += criterion21ResultLetters_o.h1;
			auto criterion21ResultBigrams_o = criterion21(part_open, bigramCounters, 5, 2);
			h1Counters["2.1.fp2_o"] += criterion21ResultBigrams_o.h1;

			auto criterion22ResultLetters_o = criterion22(part_open, letterCounters, 5);
			h1Counters["2.2.fp1_o"] += criterion22ResultLetters_o.h1;
			auto criterion22ResultBigrams_o = criterion22(part_open, bigramCounters, 5);
			h1Counters["2.2.fp2_o"] += criterion22ResultBigrams_o.h1;

			auto criterion23ResultLetters_o = criterion23(part_open, letterCounters, 5);
			h1Counters["2.3.fp1_o"] += criterion23ResultLetters_o.h1;
			auto criterion23ResultBigrams_o = criterion23(part_open, bigramCounters, 5);
			h1Counters["2.3.fp2_o"] += criterion23ResultBigrams_o.h1;

			auto criterion40ResultLetters_o = criterion40(letterFrequency(part_open), (int)part.size(), letterCounters, (int)text.size(), 0.009);
			h1Counters["4.0.fp1_o"] += criterion40ResultLetters_o.h1;
			auto criterion40ResultBigrams_o = criterion40(bigramFrequency(part_open), (int)part.size(), bigramCounters, (int)text.size(), 0.004);
			h1Counters["4.0.fp2_o"] += criterion40ResultBigrams_o.h1;

			if (l == 10) {
				auto criterion50ResultLetters2_o = criterion50(part_open, letterCounters, 2, 1); // нет пустых ящиков, 2 редких буквы есть в последовательности
				h1Counters["5.0-2.fp1_o"] += criterion50ResultLetters2_o.h1;
				auto criterion50ResultLetters4_o = criterion50(part_open, letterCounters, 4, 2); // хотя бы 2 пустых есть
				h1Counters["5.0-4.fp1_o"] += criterion50ResultLetters4_o.h1;
				auto criterion50ResultLetters6_o = criterion50(part_open, letterCounters, 6, 3); // хотя бы 3 пустых есть
				h1Counters["5.0-6.fp1_o"] += criterion50ResultLetters6_o.h1;
				auto criterion50ResultBigrams50_o = criterion50(part_open, bigramCounters, 50, 48); // если из 50, попалась 2-1=1, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams50_o.h1;
				auto criterion50ResultBigrams100_o = criterion50(part_open, bigramCounters, 100, 97); // если из 100, попалось 2, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams100_o.h1;
				auto criterion50ResultBigrams200_o = criterion50(part_open, bigramCounters, 200, 196); // если из 200, попалось 3, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams200_o.h1;
			}
			if (l == 100) {
				auto criterion50ResultLetters2_o = criterion50(part_open, letterCounters, 2, 1);
				h1Counters["5.0-2.fp1_o"] += criterion50ResultLetters2_o.h1;
				auto criterion50ResultLetters4_o = criterion50(part_open, letterCounters, 4, 2);
				h1Counters["5.0-4.fp1_o"] += criterion50ResultLetters4_o.h1;
				auto criterion50ResultLetters6_o = criterion50(part_open, letterCounters, 6, 3);
				h1Counters["5.0-6.fp1_o"] += criterion50ResultLetters6_o.h1;
				auto criterion50ResultBigrams50_o = criterion50(part_open, bigramCounters, 50, 43);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams50_o.h1;
				auto criterion50ResultBigrams100_o = criterion50(part_open, bigramCounters, 100, 91);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams100_o.h1;
				auto criterion50ResultBigrams200_o = criterion50(part_open, bigramCounters, 200, 188);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams200_o.h1;
			}

			if (l == 1000) {
				auto criterion50ResultLetters2_o = criterion50(part_open, letterCounters, 2, 1); // пустых нет
				h1Counters["5.0-2.fp1_o"] += criterion50ResultLetters2_o.h1;
				auto criterion50ResultLetters4_o = criterion50(part_open, letterCounters, 4, 1);
				h1Counters["5.0-4.fp1_o"] += criterion50ResultLetters4_o.h1;
				auto criterion50ResultLetters6_o = criterion50(part_open, letterCounters, 6, 2);
				h1Counters["5.0-6.fp1_o"] += criterion50ResultLetters6_o.h1;
				auto criterion50ResultBigrams50_o = criterion50(part_open, bigramCounters, 50, 34);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams50_o.h1;
				auto criterion50ResultBigrams100_o = criterion50(part_open, bigramCounters, 100, 79);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams100_o.h1;
				auto criterion50ResultBigrams200_o = criterion50(part_open, bigramCounters, 200, 160);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams200_o.h1;
			}
			if (l == 10000) {
				auto criterion50ResultLetters2_o = criterion50(part_open, letterCounters, 2, 1); // пустых нет
				h1Counters["5.0-2.fp1_o"] += criterion50ResultLetters2_o.h1;
				auto criterion50ResultLetters4_o = criterion50(part_open, letterCounters, 4, 1);
				h1Counters["5.0-4.fp1_o"] += criterion50ResultLetters4_o.h1;
				auto criterion50ResultLetters6_o = criterion50(part_open, letterCounters, 6, 1);
				h1Counters["5.0-6.fp1_o"] += criterion50ResultLetters6_o.h1;
				auto criterion50ResultBigrams50_o = criterion50(part_open, bigramCounters, 50, 20); // хотя бы 30 редких в последовательности есть
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams50_o.h1;
				auto criterion50ResultBigrams100_o = criterion50(part_open, bigramCounters, 100, 67);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams100_o.h1;
				auto criterion50ResultBigrams200_o = criterion50(part_open, bigramCounters, 200, 149);
				h1Counters["5.0.fp2_o"] += criterion50ResultBigrams200_o.h1;
			}

			auto criterion20ResultLetters = criterion20(part, letterCounters, 5);
			h1Counters["2.0.fp1"] += criterion20ResultLetters.h1;
			auto criterion20ResultBigrams = criterion20(part, bigramCounters, 5);
			h1Counters["2.0.fp2"] += criterion20ResultBigrams.h1;

			auto criterion21ResultLetters = criterion21(part, letterCounters, 5, 2);
			h1Counters["2.1.fp1"] += criterion21ResultLetters.h1;
			auto criterion21ResultBigrams = criterion21(part, bigramCounters, 5, 2);
			h1Counters["2.1.fp2"] += criterion21ResultBigrams.h1;

			auto criterion22ResultLetters = criterion22(part, letterCounters, (int)text.size(), 5);
			h1Counters["2.2.fp1"] += criterion22ResultLetters.h1;
			auto criterion22ResultBigrams = criterion22(part, bigramCounters, (int)text.size(), 5);
			h1Counters["2.2.fp2"] += criterion22ResultBigrams.h1;

			auto criterion23ResultLetters = criterion23(part, letterCounters, 5);
			h1Counters["2.3.fp1"] += criterion23ResultLetters.h1;
			auto criterion23ResultBigrams = criterion23(part, bigramCounters, 5);
			h1Counters["2.3.fp2"] += criterion23ResultBigrams.h1;

			auto criterion40ResultLetters = criterion40(letterFrequency(part), (int)part.size(), letterCounters, (int)text.size(), 0.009);
			h1Counters["4.0.fp1"] += criterion40ResultLetters.h1;
			auto criterion40ResultBigrams = criterion40(bigramFrequency(part), (int)part.size(), bigramCounters, (int)text.size(), 0.009);
			h1Counters["4.0.fp2"] += criterion40ResultBigrams.h1;

			if (l == 10) {
				auto criterion50ResultLetters2 = criterion50(part, letterCounters, 2, 1); // нет пустых ящиков, 2 редких буквы есть в последовательности
				h1Counters["5.0-2.fp1"] += criterion50ResultLetters2.h1;
				auto criterion50ResultLetters4 = criterion50(part, letterCounters, 4, 2); // хотя бы 2 пустых есть
				h1Counters["5.0-4.fp1"] += criterion50ResultLetters4.h1;
				auto criterion50ResultLetters6 = criterion50(part, letterCounters, 6, 3); // хотя бы 3 пустых есть
				h1Counters["5.0-6.fp1"] += criterion50ResultLetters6.h1;
				auto criterion50ResultBigrams50 = criterion50(part, bigramCounters, 50, 48); // если из 50, попалась 2-1=1, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2"] += criterion50ResultBigrams50.h1;
				auto criterion50ResultBigrams100 = criterion50(part, bigramCounters, 100, 97); // если из 100, попалось 2, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2"] += criterion50ResultBigrams100.h1;
				auto criterion50ResultBigrams200 = criterion50(part, bigramCounters, 200, 196); // если из 200, попалось 3, то случайный (тут длина маленькая, больше нельзя)
				h1Counters["5.0.fp2"] += criterion50ResultBigrams200.h1;
			}
			if (l == 100) {
				auto criterion50ResultLetters2 = criterion50(part, letterCounters, 2, 1);
				h1Counters["5.0-2.fp1"] += criterion50ResultLetters2.h1;
				auto criterion50ResultLetters4 = criterion50(part, letterCounters, 4, 2);
				h1Counters["5.0-4.fp1"] += criterion50ResultLetters4.h1;
				auto criterion50ResultLetters6 = criterion50(part, letterCounters, 6, 3);
				h1Counters["5.0-6.fp1"] += criterion50ResultLetters6.h1;
				auto criterion50ResultBigrams50 = criterion50(part, bigramCounters, 50, 43);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams50.h1;
				auto criterion50ResultBigrams100 = criterion50(part, bigramCounters, 100, 91);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams100.h1;
				auto criterion50ResultBigrams200 = criterion50(part, bigramCounters, 200, 188);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams200.h1;
			}

			if (l == 1000) {
				auto criterion50ResultLetters2 = criterion50(part, letterCounters, 2, 1); // пустых нет
				h1Counters["5.0-2.fp1"] += criterion50ResultLetters2.h1;
				auto criterion50ResultLetters4 = criterion50(part, letterCounters, 4, 1);
				h1Counters["5.0-4.fp1"] += criterion50ResultLetters4.h1;
				auto criterion50ResultLetters6 = criterion50(part, letterCounters, 6, 2);
				h1Counters["5.0-6.fp1"] += criterion50ResultLetters6.h1;
				auto criterion50ResultBigrams50 = criterion50(part, bigramCounters, 50, 34);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams50.h1;
				auto criterion50ResultBigrams100 = criterion50(part, bigramCounters, 100, 79);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams100.h1;
				auto criterion50ResultBigrams200 = criterion50(part, bigramCounters, 200, 160);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams200.h1;
			}
			if (l == 10000) {
				auto criterion50ResultLetters2 = criterion50(part, letterCounters, 2, 1); // пустых нет
				h1Counters["5.0-2.fp1"] += criterion50ResultLetters2.h1;
				auto criterion50ResultLetters4 = criterion50(part, letterCounters, 4, 1);
				h1Counters["5.0-4.fp1"] += criterion50ResultLetters4.h1;
				auto criterion50ResultLetters6 = criterion50(part, letterCounters, 6, 1);
				h1Counters["5.0-6.fp1"] += criterion50ResultLetters6.h1;
				auto criterion50ResultBigrams50 = criterion50(part, bigramCounters, 50, 20); // хотя бы 30 редких в последовательности есть
				h1Counters["5.0.fp2"] += criterion50ResultBigrams50.h1;
				auto criterion50ResultBigrams100 = criterion50(part, bigramCounters, 100, 67);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams100.h1;
				auto criterion50ResultBigrams200 = criterion50(part, bigramCounters, 200, 149);
				h1Counters["5.0.fp2"] += criterion50ResultBigrams200.h1;
			}

			if (i == 0 && l == 10)
			{
				cout << "\nКритерий 2.0:" << endl;
				if (criterion20ResultLetters.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " <<
						"существует такая частая буква \"" << criterion20ResultLetters.unit << "\", которая отсутствует в " <<
						"последовательности" << endl;
				}
				else if (criterion20ResultBigrams.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " <<
						"существует такая частая биграмма \"" << criterion20ResultBigrams.unit << "\", которая отсутствует в " <<
						"последовательности" << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что не существует такой " <<
						"частой буквы/биграммы, которая отсутствовала бы в последовательности" << endl;
				}

				cout << "\nКритерий 2.1:" << endl;
				if (criterion21ResultLetters.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что мощность <= " <<
						criterion21ResultLetters.kf << endl;
				}
				else if (criterion21ResultBigrams.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что мощность <= " <<
						criterion21ResultBigrams.kf << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что мощность > " <<
						criterion21ResultLetters.kf << endl;
				}

				cout << "\nКритерий 2.2:" << endl;
				if (criterion22ResultLetters.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что существует " <<
						"такая буква \"" << setprecision(3) << fixed << criterion22ResultLetters.unit << "\", для которой " <<
						criterion22ResultLetters.fx << " < " << criterion22ResultLetters.kx << endl;
				}
				else if (criterion22ResultBigrams.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что существует " <<
						"такая биграмма \"" << criterion22ResultBigrams.unit << "\", для которой " <<
						criterion22ResultBigrams.fx << " < " << criterion22ResultBigrams.kx << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что не существует такой " <<
						"буквы / биграммы, для которой f(x) < k(x)" << endl;
				}

				cout << "\nКритерий 2.3:" << endl;
				if (criterion23ResultLetters.h1 || criterion23ResultBigrams.h1)
				{
					cout << "Последовательность \"" << setprecision(3) << fixed << part << "\" случайна, потому что "
						<< criterion23ResultLetters.ff << " < " << criterion23ResultLetters.kf << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что " << criterion23ResultLetters.ff
						<< " >= " << criterion23ResultLetters.kf << endl;
				}

				cout << "\nКритерий 4.0:" << endl;
				if (criterion40ResultLetters.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " << criterion40ResultLetters.k
						<< " > " << criterion40ResultLetters.kl << endl;
				}
				else if (criterion40ResultLetters.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " << criterion40ResultBigrams.k
						<< " > " << criterion40ResultBigrams.kl << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что " << criterion40ResultLetters.k
						<< " <= " << criterion40ResultLetters.kl << endl;
				}

				/*cout << "\nКритерий 5.0:" << endl;
				if (criterion50ResultLetters2.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " << criterion50ResultLetters2.fempt
						<< " < " << criterion50ResultLetters2.kempt << endl;
				}
				else if (criterion50ResultBigrams50.h1)
				{
					cout << "Последовательность \"" << part << "\" случайна, потому что " << criterion50ResultBigrams50.fempt
						<< " < " << criterion50ResultBigrams50.kempt << endl;
				}
				else
				{
					cout << "Последовательность \"" << part << "\" неслучайна, потому что " << criterion50ResultLetters2.fempt
						<< " >= " << criterion50ResultLetters2.kempt << endl;
				}
				cout << endl;*/
			}
		}

		// Подчет FP и FN
		h1Counters["2.0.fp1"] /= (double)cicles[l];
		h1Counters["2.1.fp1"] /= (double)cicles[l];
		h1Counters["2.2.fp1"] /= (double)cicles[l];
		h1Counters["2.3.fp1"] /= (double)cicles[l];
		h1Counters["4.0.fp1"] /= (double)cicles[l];
		h1Counters["5.0.fp1-2"] /= (double)cicles[l];
		h1Counters["5.0.fp1-4"] /= (double)cicles[l];
		h1Counters["5.0.fp1-6"] /= (double)cicles[l];

		h1Counters["2.0.fn1"] = 1.0 - h1Counters["2.0.fp1"];
		h1Counters["2.1.fn1"] = 1.0 - h1Counters["2.1.fp1"];
		h1Counters["2.2.fn1"] = 1.0 - h1Counters["2.2.fp1"];
		h1Counters["2.3.fn1"] = 1.0 - h1Counters["2.3.fp1"];
		h1Counters["4.0.fn1"] = 1.0 - h1Counters["4.0.fp1"];
		h1Counters["5.0.fn1-2"] = 1.0 - h1Counters["5.0.fp1-2"];
		h1Counters["5.0.fn1-4"] = 1.0 - h1Counters["5.0.fp1-4"];
		h1Counters["5.0.fn1-6"] = 1.0 - h1Counters["5.0.fp1-6"];

		h1Counters["2.0.fp1"] = h1Counters["2.0.fp1_o"] / (double)cicles[l];
		h1Counters["2.1.fp1"] = h1Counters["2.1.fp1_o"] / (double)cicles[l];
		h1Counters["2.2.fp1"] = h1Counters["2.2.fp1_o"] / (double)cicles[l];
		h1Counters["2.3.fp1"] = h1Counters["2.3.fp1_o"] / (double)cicles[l];
		h1Counters["4.0.fp1"] = h1Counters["4.0.fp1_o"] / (double)cicles[l];
		h1Counters["5.0.fp1-2"] = h1Counters["5.0.fp1-2_o"] / (double)cicles[l];
		h1Counters["5.0.fp1-4"] = h1Counters["5.0.fp1-4_o"] / (double)cicles[l];
		h1Counters["5.0.fp1-6"] = h1Counters["5.0.fp1-6_o"] / (double)cicles[l];

		h1Counters["2.0.fp2"] /= (double)cicles[l];
		h1Counters["2.1.fp2"] /= (double)cicles[l];
		h1Counters["2.2.fp2"] /= (double)cicles[l];
		h1Counters["2.3.fp2"] /= (double)cicles[l];
		h1Counters["4.0.fp2"] /= (double)cicles[l];
		h1Counters["5.0.fp2-50"] /= (double)cicles[l];
		h1Counters["5.0.fp2-100"] /= (double)cicles[l];
		h1Counters["5.0.fp2-200"] /= (double)cicles[l];

		h1Counters["2.0.fn2"] = 1.0 - h1Counters["2.0.fp2"];
		h1Counters["2.1.fn2"] = 1.0 - h1Counters["2.1.fp2"];
		h1Counters["2.2.fn2"] = 1.0 - h1Counters["2.2.fp2"];
		h1Counters["2.3.fn2"] = 1.0 - h1Counters["2.3.fp2"];
		h1Counters["4.0.fn2"] = 1.0 - h1Counters["4.0.fp2"];
		h1Counters["5.0.fn2-50"] = 1.0 - h1Counters["5.0.fp2-50"];
		h1Counters["5.0.fn2-100"] = 1.0 - h1Counters["5.0.fp2-100"];
		h1Counters["5.0.fn2-200"] = 1.0 - h1Counters["5.0.fp2-200"];

		h1Counters["2.0.fp2"] = h1Counters["2.0.fp2_o"] / (double)cicles[l];
		h1Counters["2.1.fp2"] = h1Counters["2.1.fp2_o"] / (double)cicles[l];
		h1Counters["2.2.fp2"] = h1Counters["2.2.fp2_o"] / (double)cicles[l];
		h1Counters["2.3.fp2"] = h1Counters["2.3.fp2_o"] / (double)cicles[l];
		h1Counters["4.0.fp2"] = h1Counters["4.0.fp2_o"] / (double)cicles[l];
		h1Counters["5.0.fp2-50"] = h1Counters["5.0.fp2-50_o"] / (double)cicles[l];
		h1Counters["5.0.fp2-100"] = h1Counters["5.0.fp2-100_o"] / (double)cicles[l];
		h1Counters["5.0.fp2-200"] = h1Counters["5.0.fp2-200_o"] / (double)cicles[l];

		printCriterionsTable(h1Counters, l, "соотношения");
		printCriterionsTable(h1Counters, l, "соотношения", criteriaPassingResultsFile);
	}

	// Подсчет структурного критерия
	structureCriterionH1DEFLATECounter = 0;
	structureCriterionH1GZipCounter = 0;
	structureCriterionH1HaffmanCounter = 0;
	structureCriterionH1ShannonFanoCounter = 0;

	lenght = 10;
	for (int i = 0; i < 10000; i++)
	{
		generateRandomText(randomText);
		copy(text.begin() + i * lenght, text.begin() + (i + 1) * lenght, damagedText.begin());

		auto randomTextHaffmane = haffman(randomText);
		auto randomTextShannonFano = shannonFano(randomText);
		auto randomTextDEFLATE = deflate(randomText);
		auto randomTextGZip = gzip(randomText);

		auto TextHaffmane = haffman(damagedText);
		auto TextShannonFano = shannonFano(damagedText);
		auto TextDEFLATE = deflate(damagedText);
		auto TextGZip = gzip(damagedText);

		auto resultTopen = structureCriterion(TextHaffmane, randomTextHaffmane);
		structureCriterionH1HaffmanCounterTopen += resultTopen.h1;

		resultTopen = structureCriterion(TextShannonFano, randomTextShannonFano);
		structureCriterionH1ShannonFanoCounterTopen += resultTopen.h1;

		resultTopen = structureCriterion(TextDEFLATE, randomTextDEFLATE);
		structureCriterionH1DEFLATECounterTopen += resultTopen.h1;

		resultTopen = structureCriterion(TextGZip, randomTextGZip);
		structureCriterionH1GZipCounterTopen += resultTopen.h1;


		distribution(damagedText);

		auto damagedTextHaffmane = haffman(damagedText);
		auto damagedTextShannonFano = shannonFano(damagedText);
		auto damagedTextDEFLATE = deflate(damagedText);
		auto damagedTextGZip = gzip(damagedText);

		auto result = structureCriterion(damagedTextHaffmane, randomTextHaffmane);
		structureCriterionH1HaffmanCounter += result.h1;

		result = structureCriterion(damagedTextShannonFano, randomTextShannonFano);
		structureCriterionH1ShannonFanoCounter += result.h1;

		result = structureCriterion(damagedTextDEFLATE, randomTextDEFLATE);
		structureCriterionH1DEFLATECounter += result.h1;

		result = structureCriterion(damagedTextGZip, randomTextGZip);
		structureCriterionH1GZipCounter += result.h1;
	}


	compressions["DEFLATE.FP"] = structureCriterionH1DEFLATECounterTopen / 10000.0;
	compressions["DEFLATE.FN"] = 1.0 - structureCriterionH1DEFLATECounter / 10000.0;

	compressions["GZip.FP"] = structureCriterionH1GZipCounterTopen / 10000.0;
	compressions["GZip.FN"] = 1.0 - structureCriterionH1GZipCounter / 10000.0;

	compressions["Huffman.FP"] = structureCriterionH1HaffmanCounterTopen / 10000.0;
	compressions["Huffman.FN"] = 1.0 - structureCriterionH1HaffmanCounter / 10000.0;

	compressions["ShannonFano.FP"] = structureCriterionH1ShannonFanoCounterTopen / 10000.0;
	compressions["ShannonFano.FN"] = 1.0 - structureCriterionH1ShannonFanoCounter / 10000.0;

	// Вывод структурного критерия
	printCompressionTable(compressions, lenght, "Соотношение");
	printCompressionTable(compressions, lenght, "Соотношение", criteriaPassingResultsFile);

	criteriaPassingResultsFile.close();

	// Генерация 10 текстов по 10000 символов и проверка 4.0 критериемы
	vector<bool> creterion40Values;
	string randomText100000(10000, ' ');
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10000; j++)
			randomText100000[j] = letter(rand());
		ofstream encFile("task5_textEnc" + to_string(i + 1) + ".txt");
		encFile << randomText100000;
		encFile.close();
		vigener(randomText100000);
		ofstream decFile("task5_textDec" + to_string(i + 1) + ".txt");
		decFile << randomText100000;
		decFile.close();
		auto randomTextFrequancy = letterFrequency(randomText);
		creterion40Values.push_back(criterion40(randomTextFrequancy, (int)randomText100000.size(), letterCounters, (int)text.size()).h1);
	}
	printTask5Table(creterion40Values);

	return 0;
}
