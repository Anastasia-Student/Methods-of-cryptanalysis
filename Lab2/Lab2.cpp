#include "distortion.h"
#include "file.h"
#include "stat.h"
#include "ShannonFano.h"
#include "criterions.h"
#include "Huffman.h"
#include "GZip.h"
#include "Deflate.h"
#include "LZ77.h"
#include "RLE.h"
//#include "Arithmetic_compression.h"

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
void printLetterFrequency(const map<Letter, Frequancy>& counters, size_t lenght, ostream& stream = cout)
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

// Вывести данные о сжатии
void printCompressedTable(ComressionResult result, string name, ostream& stream = cout)
{
	stream << "\rТекст с файла filtered_text.txt сжат алгоритмом " << name << " с " << result.size
		<< " бит до " << result.compressedSize << " бит (" << setprecision(2) << fixed
		<< result.compressedSize / (float)result.size * 100.0 << "%)\n";
}

struct FPFN
{
	string name;
	double l1fn;
	double l1fp;
	double l2fn;
	double l2fp;
	double l2fnwi;
	double l2fpwi;
};

struct Criterion50Data
{
	string name;
	int L;
	int l;
	int j;

	friend bool operator == (const Criterion50Data& left, const Criterion50Data& right)
	{
		return left.L == right.L && left.l == right.l && left.name == right.name && left.j == right.j;
	}

	friend bool operator < (const Criterion50Data& left, const Criterion50Data& right)
	{
		if (left.name < right.name)
			return true;
		else if (left.L < right.L)
			return true;
		else if (left.l < right.l)
			return true;
		else if (left.j < right.j)
			return true;
		return false;
	}
};

// Вывести таблицу с FP и FN значениями
void printCriterionsTableBody(map<string, FPFN> h1Counters, ostream& stream = cout)
{
	for (auto value : h1Counters)
		stream << "| " << setw(14) << left << value.first << " | " << setprecision(3) << fixed << setw(10) << value.second.l1fp << " | " << setw(10) << value.second.l1fn << " | "
		<< setw(13) << value.second.l2fpwi << " | " << setw(13) << value.second.l2fnwi << " | "
		<< setw(10) << value.second.l2fp << " | " << setw(10) << value.second.l2fn << " |" << endl;
	stream << "------------------------------------------------------------------------------------------------------" << endl;
}

// Вывести заголовок таблицы с FP и FN значениями с r
void printCriterionsTable(map<string, FPFN> h1Counters, int l, int r, string type, ostream& stream = cout)
{
	stringstream header;
	stream << "------------------------------------------------------------------------------------------------------" << endl;
	header << "Искажение c помощью " << type << " (L = " << l << ", r = " << r << ")";
	stream << "| " << setw(98) << left << header.str() << " |" << endl;
	stream << "------------------------------------------------------------------------------------------------------" << endl;
	stream << "| Номер критерия | FP (l = 1) | FN (l = 1) | FP (l = 2, п) | FN (l = 2, п) | FP (l = 2) | FN (l = 2) |" << endl;
	stream << "------------------------------------------------------------------------------------------------------" << endl;
	printCriterionsTableBody(h1Counters, stream);
}

// Вывести заголовок таблицы с FP и FN значениями
void printCriterionsTable(map<string, FPFN> h1Counters, int l, string type, ostream& stream = cout)
{
	stringstream header;
	stream << "------------------------------------------------------------------------------------------------------" << endl;
	header << "Искажение c помощью " << type << " (L = " << l << ")";
	stream << "| " << setw(98) << left << header.str() << " |" << endl;
	stream << "------------------------------------------------------------------------------------------------------" << endl;
	stream << "| Номер критерия | FP (l = 1) | FN (l = 1) | FP (l = 2, п) | FN (l = 2, п) | FP (l = 2) | FN (l = 2) |" << endl;
	stream << "------------------------------------------------------------------------------------------------------" << endl;
	printCriterionsTableBody(h1Counters, stream);
}

struct CompressionData
{
	double fn;
	double fp;
};

// Вывести таблицу со значениями сжатий
void printCompressionTable(map<string, CompressionData> values, int lenght, int n, string type, ostream& stream = cout)
{
	stream << "Результаты применения структурного критерия с разными алгоритмами сжатия(L = " << lenght <<
		", N = " << n << ", вид искажения = " << type << "):" << endl;
	stream << "----------------------------------------" << endl;
	stream << "| Алгоритм сжатия  | FP      | FN      |" << endl;
	stream << "|--------------------------------------|" << endl;
	for (auto value : values)
	{
		stream << "| " << setw(16) << left << value.first << " | " << setw(7) << left << value.second.fp << " | "
			<< setw(7) << left << value.second.fn << " |" << endl;
		stream << "|--------------------------------------|" << endl;
	}
}

// Вывести таблицу из 5 задачи
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

// Генерация случайного текста
void generateRandomText(string& text)
{
	for (size_t j = 0; j < text.length(); j++)
		text[j] = letter(rand());
}

// Шифр Виженера
void vigener(string& text, int r = INT_MAX)
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
	setlocale(LC_ALL, "rus");  // Для вывод кирилицы

	filter("text.txt", "filtered_text.txt");  // Фильтрация текста
	auto text = load("filtered_text.txt");  // Загрузка отфильтрованного текста
	if (text.length() == 0)
	{
		cerr << "Неудалось найти файл \"filtered_text.txt\" либо он пуст!";
		return 1;
	}

	auto letterCounters = letterFrequency(text);  // Частота букв
	auto bigramCounters = bigramFrequency(text);  // Частота биграмм
	auto bigramWithIntersectionsCounters = bigramFrequencyWithIntersections(text);  // Частота биграмм с пересечениями

	ofstream letterFrequencyFile("letter_frequency.txt");  // Запись частоты букв
	printLetterFrequency(letterCounters, text.size(), letterFrequencyFile);
	letterFrequencyFile.close();
	printBigramFrequencyHeader(letterCounters, text.size());

	ofstream bigrammFrequencyFile("frequency_of_bigrams.txt");  // Запись частоты биграмм
	printBigramFrequency(bigramCounters, text.size(), bigrammFrequencyFile);
	bigrammFrequencyFile.close();
	printBigramFrequencyHeader(bigramCounters, text.size());

	ofstream bigrammWithIntersectionsFrequencyFile("frequency_of_bigrams_with_intersections.txt");  // Запись частоты биграмм
	printBigramFrequency(bigramWithIntersectionsCounters, text.size(), bigrammWithIntersectionsFrequencyFile);
	bigrammWithIntersectionsFrequencyFile.close();
	printBigramFrequencyHeader(bigramWithIntersectionsCounters, text.size());

	vector<int> rs = { 1, 5, 10 };  // Значения r
	vector<int> ls = { 10, 100, 1000, 10000 };  // Значения L
	map<int, int> cicles = { {10, 10000}, {100, 10000}, {1000, 10000}, {10000, 1000} };  // Количество циклов для каждой L
	map<Criterion50Data, int> Criterion50Datas =  // Значения для критерия 5.0
	{
		{{"5.0 2 50", 10, 2, 50}, 49}, {{"5.0 4 100", 10, 2, 100}, 98}, {{"5.0 6 200", 10, 2, 200}, 98},

		{{"5.0 2 50", 10, 1, 2}, 2},  {{"5.0 4 100", 10, 1, 4}, 3},   {{"5.0 6 200", 10, 1, 6}, 4},

		{{"5.0 2 50", 100, 2, 50}, 45}, {{"5.0 4 100", 100, 2, 100}, 70}, {{"5.0 6 200", 100, 2, 200}, 90},

		{{"5.0 2 50", 100, 1, 2}, 2},   {{"5.0 4 100", 100, 1, 4}, 2},    {{"5.0 6 200", 100, 1, 6}, 5},

		{{"5.0 2 50", 1000, 2, 50}, 30}, {{"5.0 4 100", 1000, 2, 100}, 50}, {{"5.0 6 200", 1000, 2, 200}, 70},

		{{"5.0 2 50", 1000, 1, 2}, 1},   {{"5.0 4 100", 1000, 1, 4}, 2},    {{"5.0 6 200", 1000, 1, 6}, 3},

		{{"5.0 2 50", 10000, 2, 50}, 20}, {{"5.0 4 100", 10000, 2, 100}, 30}, {{"5.0 6 200", 10000, 2, 200}, 50},

		{{"5.0 2 50", 10000, 1, 2}, 1},   {{"5.0 4 100", 10000, 1, 4}, 2},    {{"5.0 6 200", 10000, 1, 6}, 3}
	};
	ofstream criteriaPassingResultsFile("criteria_passing_results.txt");  // Файл для записи критериев

	// Шифр Виженера

	for (auto l : ls)  // Для каждой L
	{
		string part(l, ' ');  // Строка длиной L
		for (auto r : rs)  // Для каждой r 
		{
			map<string, FPFN> h1Counters;  // Счетчики H1

			for (int i = 0; i < cicles[l] && i * l < (int)text.size() - l; i++)  // Циклы 10000 и 1000
			{
				copy(text.begin() + (long long)i * l, text.begin() + ((long long)i + 1) * l, part.begin());

				auto criterion20ResultLetters = criterion20(part, letterCounters, 2);
				h1Counters["2.0"].l1fp += criterion20ResultLetters.h1;
				auto criterion20ResultBigrams = criterion20(part, bigramCounters, 1);
				h1Counters["2.0"].l2fp += criterion20ResultBigrams.h1;
				auto criterion20ResultBigramsWithIntersection = criterion20(part, bigramWithIntersectionsCounters, 1);
				h1Counters["2.0"].l2fpwi += criterion20ResultBigrams.h1;

				auto criterion21ResultLetters = criterion21(part, letterCounters, 5, 1);
				h1Counters["2.1"].l1fp += criterion21ResultLetters.h1;
				auto criterion21ResultBigrams = criterion21(part, bigramCounters, 5, 1);
				h1Counters["2.1"].l2fp += criterion21ResultBigrams.h1;
				auto criterion21ResultBigramsWithIntersection = criterion21(part, bigramWithIntersectionsCounters, 5, 2);
				h1Counters["2.1"].l2fpwi += criterion21ResultBigrams.h1;

				auto criterion22ResultLetters = criterion22(part, letterCounters, 2);
				h1Counters["2.2"].l1fp += criterion22ResultLetters.h1;
				auto criterion22ResultBigrams = criterion22(part, bigramCounters, 1);
				h1Counters["2.2"].l2fp += criterion22ResultBigrams.h1;
				auto criterion22ResultBigramsWithIntersection = criterion22(part, bigramWithIntersectionsCounters, 2);
				h1Counters["2.2"].l2fpwi += criterion22ResultBigrams.h1;

				auto criterion23ResultLetters = criterion23(part, letterCounters, 2);
				h1Counters["2.3"].l1fp += criterion23ResultLetters.h1;
				auto criterion23ResultBigrams = criterion23(part, bigramCounters, 2);
				h1Counters["2.3"].l2fp += criterion23ResultBigrams.h1;
				auto criterion23ResultBigramsWithIntersection = criterion23(part, bigramWithIntersectionsCounters, 2);
				h1Counters["2.3"].l2fpwi += criterion23ResultBigrams.h1;

				auto criterion40ResultLetters = criterion40(letterFrequency(part), (int)part.size(), letterCounters, (int)text.size(), 0.05);
				h1Counters["4.0"].l1fp += criterion40ResultLetters.h1;
				auto criterion40ResultBigrams = criterion40(bigramFrequency(part), (int)part.size(), bigramCounters, (int)text.size(), 0.05);
				h1Counters["4.0"].l2fp += criterion40ResultBigrams.h1;
				auto criterion40ResultBigramsWithIntersection = criterion40(bigramFrequency(part), (int)part.size(), bigramWithIntersectionsCounters, (int)text.size(), 0.05);
				h1Counters["4.0"].l2fpwi += criterion40ResultBigrams.h1;

				for (auto criterion50Data : Criterion50Datas)
				{
					if (criterion50Data.first.L == l)
					{
						if (criterion50Data.first.l == 1)
						{
							auto criterion50ResultLetters = criterion50(part, letterCounters, criterion50Data.first.j, criterion50Data.second);
							h1Counters[criterion50Data.first.name].l1fp += criterion50ResultLetters.h1;
						}
						else if (criterion50Data.first.l == 2)
						{
							auto criterion50ResultBigrams = criterion50(part, bigramCounters, criterion50Data.first.j, criterion50Data.second);
							h1Counters[criterion50Data.first.name].l2fp += criterion50ResultBigrams.h1;
							auto criterion50ResultBigramsWithIntersitions = criterion50(part, bigramWithIntersectionsCounters, criterion50Data.first.j, criterion50Data.second);
							h1Counters[criterion50Data.first.name].l2fpwi += criterion50ResultBigramsWithIntersitions.h1;
						}
					}
				}

				vigener(part, r);

				// Подсчет критериев
				criterion20ResultLetters = criterion20(part, letterCounters, 2);
				h1Counters["2.0"].l1fn += !criterion20ResultLetters.h1;
				criterion20ResultBigrams = criterion20(part, bigramCounters, 1);
				h1Counters["2.0"].l2fn += !criterion20ResultBigrams.h1;
				criterion20ResultBigramsWithIntersection = criterion20(part, bigramWithIntersectionsCounters, 1);
				h1Counters["2.0"].l2fnwi += !criterion20ResultBigrams.h1;

				criterion21ResultLetters = criterion21(part, letterCounters, 5, 1);
				h1Counters["2.1"].l1fn += !criterion21ResultLetters.h1;
				criterion21ResultBigrams = criterion21(part, bigramCounters, 5, 1);
				h1Counters["2.1"].l2fn += !criterion21ResultBigrams.h1;
				criterion21ResultBigramsWithIntersection = criterion21(part, bigramWithIntersectionsCounters, 5, 2);
				h1Counters["2.1"].l2fnwi += !criterion21ResultBigrams.h1;

				criterion22ResultLetters = criterion22(part, letterCounters, 2);
				h1Counters["2.2"].l1fn += !criterion22ResultLetters.h1;
				criterion22ResultBigrams = criterion22(part, bigramCounters, 1);
				h1Counters["2.2"].l2fn += !criterion22ResultBigrams.h1;
				criterion22ResultBigramsWithIntersection = criterion22(part, bigramWithIntersectionsCounters, 2);
				h1Counters["2.2"].l2fnwi += !criterion22ResultBigrams.h1;

				criterion23ResultLetters = criterion23(part, letterCounters, 2);
				h1Counters["2.3"].l1fn += !criterion23ResultLetters.h1;
				criterion23ResultBigrams = criterion23(part, bigramCounters, 2);
				h1Counters["2.3"].l2fn += !criterion23ResultBigrams.h1;
				criterion23ResultBigramsWithIntersection = criterion23(part, bigramWithIntersectionsCounters, 2);
				h1Counters["2.3"].l2fnwi += !criterion23ResultBigrams.h1;

				criterion40ResultLetters = criterion40(letterFrequency(part), (int)part.size(), letterCounters, (int)text.size(), 0.05);
				h1Counters["4.0"].l1fn += !criterion40ResultLetters.h1;
				criterion40ResultBigrams = criterion40(bigramFrequency(part), (int)part.size(), bigramCounters, (int)text.size(), 0.05);
				h1Counters["4.0"].l2fn += !criterion40ResultBigrams.h1;
				criterion40ResultBigramsWithIntersection = criterion40(bigramFrequency(part), (int)part.size(), bigramWithIntersectionsCounters, (int)text.size(), 0.05);
				h1Counters["4.0"].l2fnwi += !criterion40ResultBigrams.h1;

				for (auto criterion50Data : Criterion50Datas)
				{
					if (criterion50Data.first.L == l && criterion50Data.first.l == 1)
					{
						auto criterion50ResultLetters = criterion50(part, letterCounters, criterion50Data.first.j, criterion50Data.second);
						h1Counters[criterion50Data.first.name].l1fn += !criterion50ResultLetters.h1;
					}
					if (criterion50Data.first.L == l && criterion50Data.first.l == 2)
					{
						auto criterion50ResultBigrams = criterion50(part, bigramCounters, criterion50Data.first.j, criterion50Data.second);
						h1Counters[criterion50Data.first.name].l2fn += !criterion50ResultBigrams.h1;
						criterion50ResultBigrams = criterion50(part, bigramWithIntersectionsCounters, criterion50Data.first.j, criterion50Data.second);
						h1Counters[criterion50Data.first.name].l2fnwi += !criterion50ResultBigrams.h1;
					}
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
					cout << endl;
				}
			}

			// Подчет FP и FN
			for (auto& fnfp : h1Counters)
			{
				fnfp.second.l1fn /= 10000.0;
				fnfp.second.l1fp /= 10000.0;
				fnfp.second.l2fn /= 10000.0;
				fnfp.second.l2fp /= 10000.0;
				fnfp.second.l2fnwi /= 10000.0;
				fnfp.second.l2fpwi /= 10000.0;
			}

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

	// Сжатие алгоритмом LZ77
	auto lz77Result = lz77("filtered_text.txt", "LZ77_res.txt");
	printCompressedTable(lz77Result, "LZ77");

	// Сжатие алгоритмом RLE
	/*auto RLEResult = RLE("filtered_text.txt", "RLE_res.txt");
	printCompressedTable(RLEResult, "RLE");

	// Сжатие алгоритмом RLE_Bit
	auto RLE_Bit_Result = RLE_Bit("filtered_text.txt", "RLE_Bit_res.txt");
	printCompressedTable(RLE_Bit_Result, "RLE_bit");*/

	int lenght = 10;
	while(lenght <= 10000){
	string randomText(lenght, ' ');
	string damagedText(lenght, ' ');
	string partN(lenght, ' ');
	map<string, CompressionData> compressions;
	for (int i = 0; i < cicles[lenght] && (i + 1) * lenght < text.size(); i++)
	{
		generateRandomText(randomText);
		copy(text.begin() + i * lenght, text.begin() + (i + 1) * lenght, damagedText.begin());
		copy(text.begin() + i * lenght, text.begin() + (i + 1) * lenght, partN.begin());

		auto randomTextHaffmane = haffman(randomText);
		auto damagedTextHaffmane = haffman(damagedText);
		auto partNTextHaffmane = haffman(partN);

		auto randomTextShannonFano = shannonFano(randomText);
		auto damagedTextShannonFano = shannonFano(damagedText);
		auto partNTextShannonFano = shannonFano(partN);

		auto randomTextDEFLATE = deflate(randomText);
		auto damagedTextDEFLATE = deflate(damagedText);
		auto partNTextDEFLATE = deflate(partN);

		auto randomTextGZip = gzip(randomText);
		auto damagedTextGZip = gzip(damagedText);
		auto partNTextGZip = gzip(partN);

		auto randomTextLZ77 = lz77(randomText);
		auto damagedTextLZ77 = lz77(damagedText);
		auto partNTextLZ77 = lz77(partN);

		auto randomTextRLE = RLE(randomText);
		auto damagedTextRLE = RLE(damagedText);
		auto partNTextRLE = RLE(partN);

		auto randomTextRLE_Bit = RLE_Bit(randomText);
		auto damagedTextRLE_Bit = RLE_Bit(damagedText);
		auto partNTextRLE_Bit = RLE_Bit(partN);

		auto result = structureCriterion(damagedTextHaffmane, randomTextHaffmane, 1.0);
		compressions["Huffman"].fp += result.h1;
		result = structureCriterion(randomTextHaffmane, partNTextHaffmane, 1.0);
		compressions["Huffman"].fn += !result.h1;

		result = structureCriterion(damagedTextShannonFano, randomTextShannonFano, 1.0);
		compressions["ShannonFano"].fp += result.h1;
		result = structureCriterion(randomTextShannonFano, partNTextShannonFano, 1.0);
		compressions["ShannonFano"].fn += !result.h1;

		result = structureCriterion(damagedTextDEFLATE, randomTextDEFLATE, 1.0);
		compressions["DEFLATE"].fp += result.h1;
		result = structureCriterion(randomTextDEFLATE, partNTextDEFLATE, 1.0);
		compressions["DEFLATE"].fn += !result.h1;

		result = structureCriterion(damagedTextGZip, randomTextGZip, 1.0);
		compressions["GZip"].fp += result.h1;
		result = structureCriterion(randomTextGZip, partNTextGZip, 1.0);
		compressions["GZip"].fn += !result.h1;

		result = structureCriterion(damagedTextLZ77, randomTextLZ77, 1.0);
		compressions["LZ77"].fp += result.h1;
		result = structureCriterion(randomTextLZ77, partNTextLZ77, 1.0);
		compressions["LZ77"].fn += !result.h1;

		result = structureCriterion(damagedTextRLE, randomTextRLE, 1.0);
		compressions["RLE"].fp += result.h1;
		result = structureCriterion(randomTextRLE, partNTextRLE, 1.0);
		compressions["RLE"].fn += !result.h1;

		result = structureCriterion(damagedTextRLE_Bit, randomTextRLE_Bit, 1.0);
		compressions["RLE_Bit"].fp += result.h1;
		result = structureCriterion(randomTextRLE_Bit, partNTextRLE_Bit, 1.0);
		compressions["RLE_Bit"].fn += !result.h1;
	}

	for (auto& value : compressions)
	{
		value.second.fp /= 10000.0;
		value.second.fn /= 10000.0;
	}

	// Вывод структурного критерия
	printCompressionTable(compressions, lenght, cicles[lenght], "Виженер");
	printCompressionTable(compressions, lenght, cicles[lenght], "Виженер", criteriaPassingResultsFile);
	lenght = lenght * 10;
	}

	// Афинная подстановка

	for (auto l : ls)  // Для каждой L
	{
		string part(l, ' ');  // Строка длиной L
		map<string, FPFN> h1Counters;  // Счетчики H1

		for (int i = 0; i < cicles[l] && i * l < (int)text.size() - l; i++)  // Циклы 10000 и 1000
		{
			copy(text.begin() + (long long)i * l, text.begin() + ((long long)i + 1) * l, part.begin());

			auto criterion20ResultLetters = criterion20(part, letterCounters, 2);
			h1Counters["2.0"].l1fp += criterion20ResultLetters.h1;
			auto criterion20ResultBigrams = criterion20(part, bigramCounters, 1);
			h1Counters["2.0"].l2fp += criterion20ResultBigrams.h1;
			auto criterion20ResultBigramsWithIntersection = criterion20(part, bigramWithIntersectionsCounters, 1);
			h1Counters["2.0"].l2fpwi += criterion20ResultBigrams.h1;

			auto criterion21ResultLetters = criterion21(part, letterCounters, 5, 1);
			h1Counters["2.1"].l1fp += criterion21ResultLetters.h1;
			auto criterion21ResultBigrams = criterion21(part, bigramCounters, 5, 1);
			h1Counters["2.1"].l2fp += criterion21ResultBigrams.h1;
			auto criterion21ResultBigramsWithIntersection = criterion21(part, bigramWithIntersectionsCounters, 5, 2);
			h1Counters["2.1"].l2fpwi += criterion21ResultBigrams.h1;

			auto criterion22ResultLetters = criterion22(part, letterCounters, 2);
			h1Counters["2.2"].l1fp += criterion22ResultLetters.h1;
			auto criterion22ResultBigrams = criterion22(part, bigramCounters, 1);
			h1Counters["2.2"].l2fp += criterion22ResultBigrams.h1;
			auto criterion22ResultBigramsWithIntersection = criterion22(part, bigramWithIntersectionsCounters, 2);
			h1Counters["2.2"].l2fpwi += criterion22ResultBigrams.h1;

			auto criterion23ResultLetters = criterion23(part, letterCounters, 2);
			h1Counters["2.3"].l1fp += criterion23ResultLetters.h1;
			auto criterion23ResultBigrams = criterion23(part, bigramCounters, 2);
			h1Counters["2.3"].l2fp += criterion23ResultBigrams.h1;
			auto criterion23ResultBigramsWithIntersection = criterion23(part, bigramWithIntersectionsCounters, 2);
			h1Counters["2.3"].l2fpwi += criterion23ResultBigrams.h1;

			auto criterion40ResultLetters = criterion40(letterFrequency(part), (int)part.size(), letterCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l1fp += criterion40ResultLetters.h1;
			auto criterion40ResultBigrams = criterion40(bigramFrequency(part), (int)part.size(), bigramCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l2fp += criterion40ResultBigrams.h1;
			auto criterion40ResultBigramsWithIntersection = criterion40(bigramFrequency(part), (int)part.size(), bigramWithIntersectionsCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l2fpwi += criterion40ResultBigrams.h1;

			for (auto criterion50Data : Criterion50Datas)
			{
				if (criterion50Data.first.L == l)
				{
					if (criterion50Data.first.l == 1)
					{
						auto criterion50ResultLetters = criterion50(part, letterCounters, criterion50Data.first.j, criterion50Data.second);
						h1Counters[criterion50Data.first.name].l1fp += criterion50ResultLetters.h1;
					}
					else if (criterion50Data.first.l == 2)
					{
						auto criterion50ResultBigrams = criterion50(part, bigramCounters, criterion50Data.first.j, criterion50Data.second);
						h1Counters[criterion50Data.first.name].l2fp += criterion50ResultBigrams.h1;
						auto criterion50ResultBigramsWithIntersitions = criterion50(part, bigramWithIntersectionsCounters, criterion50Data.first.j, criterion50Data.second);
						h1Counters[criterion50Data.first.name].l2fpwi += criterion50ResultBigramsWithIntersitions.h1;
					}
				}
			}

			affineSubstitution(part);

			// Подсчет критериев
			criterion20ResultLetters = criterion20(part, letterCounters, 2);
			h1Counters["2.0"].l1fn += !criterion20ResultLetters.h1;
			criterion20ResultBigrams = criterion20(part, bigramCounters, 1);
			h1Counters["2.0"].l2fn += !criterion20ResultBigrams.h1;
			criterion20ResultBigramsWithIntersection = criterion20(part, bigramWithIntersectionsCounters, 1);
			h1Counters["2.0"].l2fnwi += !criterion20ResultBigrams.h1;

			criterion21ResultLetters = criterion21(part, letterCounters, 5, 1);
			h1Counters["2.1"].l1fn += !criterion21ResultLetters.h1;
			criterion21ResultBigrams = criterion21(part, bigramCounters, 5, 1);
			h1Counters["2.1"].l2fn += !criterion21ResultBigrams.h1;
			criterion21ResultBigramsWithIntersection = criterion21(part, bigramWithIntersectionsCounters, 5, 2);
			h1Counters["2.1"].l2fnwi += !criterion21ResultBigrams.h1;

			criterion22ResultLetters = criterion22(part, letterCounters, 2);
			h1Counters["2.2"].l1fn += !criterion22ResultLetters.h1;
			criterion22ResultBigrams = criterion22(part, bigramCounters, 1);
			h1Counters["2.2"].l2fn += !criterion22ResultBigrams.h1;
			criterion22ResultBigramsWithIntersection = criterion22(part, bigramWithIntersectionsCounters, 2);
			h1Counters["2.2"].l2fnwi += !criterion22ResultBigrams.h1;

			criterion23ResultLetters = criterion23(part, letterCounters, 2);
			h1Counters["2.3"].l1fn += !criterion23ResultLetters.h1;
			criterion23ResultBigrams = criterion23(part, bigramCounters, 2);
			h1Counters["2.3"].l2fn += !criterion23ResultBigrams.h1;
			criterion23ResultBigramsWithIntersection = criterion23(part, bigramWithIntersectionsCounters, 2);
			h1Counters["2.3"].l2fnwi += !criterion23ResultBigrams.h1;

			criterion40ResultLetters = criterion40(letterFrequency(part), (int)part.size(), letterCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l1fn += !criterion40ResultLetters.h1;
			criterion40ResultBigrams = criterion40(bigramFrequency(part), (int)part.size(), bigramCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l2fn += !criterion40ResultBigrams.h1;
			criterion40ResultBigramsWithIntersection = criterion40(bigramFrequency(part), (int)part.size(), bigramWithIntersectionsCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l2fnwi += !criterion40ResultBigrams.h1;

			for (auto criterion50Data : Criterion50Datas)
			{
				if (criterion50Data.first.L == l && criterion50Data.first.l == 1)
				{
					auto criterion50ResultLetters = criterion50(part, letterCounters, criterion50Data.first.j, criterion50Data.second);
					h1Counters[criterion50Data.first.name].l1fn += !criterion50ResultLetters.h1;
				}
				if (criterion50Data.first.L == l && criterion50Data.first.l == 2)
				{
					auto criterion50ResultBigrams = criterion50(part, bigramCounters, criterion50Data.first.j, criterion50Data.second);
					h1Counters[criterion50Data.first.name].l2fn += !criterion50ResultBigrams.h1;
					criterion50ResultBigrams = criterion50(part, bigramWithIntersectionsCounters, criterion50Data.first.j, criterion50Data.second);
					h1Counters[criterion50Data.first.name].l2fnwi += !criterion50ResultBigrams.h1;
				}
			}

			// Вывод критериев первого текста
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
				cout << endl;
			}
		}

		// Подчет FP и FN
		for (auto& fnfp : h1Counters)
		{
			fnfp.second.l1fn /= 10000.0;
			fnfp.second.l1fp /= 10000.0;
			fnfp.second.l2fn /= 10000.0;
			fnfp.second.l2fp /= 10000.0;
			fnfp.second.l2fnwi /= 10000.0;
			fnfp.second.l2fpwi /= 10000.0;
		}

		// Вывод таблиц критериев
		printCriterionsTable(h1Counters, l, "афинной подстановки");
		printCriterionsTable(h1Counters, l, "афинной подстановки", criteriaPassingResultsFile);
	}
	cout << endl;

	lenght = 10;
	while (lenght <= 10000) {
		string randomText(lenght, ' ');
		string damagedText(lenght, ' ');
		string partN(lenght, ' ');
		map<string, CompressionData> compressions;
		for (int i = 0; i < cicles[lenght] && (i + 1) * lenght < text.size(); i++)
		{
			generateRandomText(randomText);
			copy(text.begin() + i * lenght, text.begin() + (i + 1) * lenght, damagedText.begin());
			affineSubstitution(damagedText);
			copy(text.begin() + i * lenght, text.begin() + (i + 1) * lenght, partN.begin());

			auto randomTextHaffmane = haffman(randomText);
			auto damagedTextHaffmane = haffman(damagedText);
			auto partNTextHaffmane = haffman(partN);

			auto randomTextShannonFano = shannonFano(randomText);
			auto damagedTextShannonFano = shannonFano(damagedText);
			auto partNTextShannonFano = shannonFano(partN);

			auto randomTextDEFLATE = deflate(randomText);
			auto damagedTextDEFLATE = deflate(damagedText);
			auto partNTextDEFLATE = deflate(partN);

			auto randomTextGZip = gzip(randomText);
			auto damagedTextGZip = gzip(damagedText);
			auto partNTextGZip = gzip(partN);

			auto randomTextLZ77 = lz77(randomText);
			auto damagedTextLZ77 = lz77(damagedText);
			auto partNTextLZ77 = lz77(partN);

			auto randomTextRLE = RLE(randomText);
			auto damagedTextRLE = RLE(damagedText);
			auto partNTextRLE = RLE(partN);

			auto randomTextRLE_Bit = RLE_Bit(randomText);
			auto damagedTextRLE_Bit = RLE_Bit(damagedText);
			auto partNTextRLE_Bit = RLE_Bit(partN);

			auto result = structureCriterion(damagedTextHaffmane, randomTextHaffmane, 1.0);
			compressions["Huffman"].fp += result.h1;
			result = structureCriterion(randomTextHaffmane, partNTextHaffmane, 1.0);
			compressions["Huffman"].fn += result.h1;

			result = structureCriterion(damagedTextShannonFano, randomTextShannonFano, 1.0);
			compressions["ShannonFano"].fp += result.h1;
			result = structureCriterion(randomTextShannonFano, partNTextShannonFano, 1.0);
			compressions["ShannonFano"].fn += result.h1;

			result = structureCriterion(damagedTextDEFLATE, randomTextDEFLATE, 1.0);
			compressions["DEFLATE"].fp += result.h1;
			result = structureCriterion(randomTextDEFLATE, partNTextDEFLATE, 1.0);
			compressions["DEFLATE"].fn += result.h1;

			result = structureCriterion(damagedTextGZip, randomTextGZip, 1.0);
			compressions["GZip"].fp += result.h1;
			result = structureCriterion(randomTextGZip, partNTextGZip, 1.0);
			compressions["GZip"].fn += result.h1;

			result = structureCriterion(damagedTextLZ77, randomTextLZ77, 1.0);
			compressions["LZ77"].fp += result.h1;
			result = structureCriterion(randomTextLZ77, partNTextLZ77, 1.0);
			compressions["LZ77"].fn += result.h1;

			result = structureCriterion(damagedTextRLE, randomTextRLE, 1.0);
			compressions["RLE"].fp += result.h1;
			result = structureCriterion(randomTextRLE, partNTextRLE, 1.0);
			compressions["RLE"].fn += result.h1;

			result = structureCriterion(damagedTextRLE_Bit, randomTextRLE_Bit, 1.0);
			compressions["RLE_Bit"].fp += result.h1;
			result = structureCriterion(randomTextRLE_Bit, partNTextRLE_Bit, 1.0);
			compressions["RLE_Bit"].fn += result.h1;
		}

		for (auto& value : compressions)
		{
			value.second.fp /= 10000.0;
			value.second.fn /= 10000.0;
		}

		// Вывод структурного критерия
		printCompressionTable(compressions, lenght, cicles[lenght], "Афинная подстановка");
		printCompressionTable(compressions, lenght, cicles[lenght], "Афинная подстановка", criteriaPassingResultsFile);
		lenght = lenght * 10;
	}

	for (auto l : ls)  // Для каждой L
	{
		string part(l, ' ');  // Строка длиной L
		map<string, FPFN> h1Counters;  // Счетчики H1

		for (int i = 0; i < cicles[l] && i * l < (int)text.size() - l; i++)  // Циклы 10000 и 1000
		{
			copy(text.begin() + (long long)i * l, text.begin() + ((long long)i + 1) * l, part.begin());

			auto criterion20ResultLetters = criterion20(part, letterCounters, 2);
			h1Counters["2.0"].l1fp += criterion20ResultLetters.h1;
			auto criterion20ResultBigrams = criterion20(part, bigramCounters, 1);
			h1Counters["2.0"].l2fp += criterion20ResultBigrams.h1;
			auto criterion20ResultBigramsWithIntersection = criterion20(part, bigramWithIntersectionsCounters, 1);
			h1Counters["2.0"].l2fpwi += criterion20ResultBigrams.h1;

			auto criterion21ResultLetters = criterion21(part, letterCounters, 5, 1);
			h1Counters["2.1"].l1fp += criterion21ResultLetters.h1;
			auto criterion21ResultBigrams = criterion21(part, bigramCounters, 5, 1);
			h1Counters["2.1"].l2fp += criterion21ResultBigrams.h1;
			auto criterion21ResultBigramsWithIntersection = criterion21(part, bigramWithIntersectionsCounters, 5, 2);
			h1Counters["2.1"].l2fpwi += criterion21ResultBigrams.h1;

			auto criterion22ResultLetters = criterion22(part, letterCounters, 2);
			h1Counters["2.2"].l1fp += criterion22ResultLetters.h1;
			auto criterion22ResultBigrams = criterion22(part, bigramCounters, 1);
			h1Counters["2.2"].l2fp += criterion22ResultBigrams.h1;
			auto criterion22ResultBigramsWithIntersection = criterion22(part, bigramWithIntersectionsCounters, 2);
			h1Counters["2.2"].l2fpwi += criterion22ResultBigrams.h1;

			auto criterion23ResultLetters = criterion23(part, letterCounters, 2);
			h1Counters["2.3"].l1fp += criterion23ResultLetters.h1;
			auto criterion23ResultBigrams = criterion23(part, bigramCounters, 2);
			h1Counters["2.3"].l2fp += criterion23ResultBigrams.h1;
			auto criterion23ResultBigramsWithIntersection = criterion23(part, bigramWithIntersectionsCounters, 2);
			h1Counters["2.3"].l2fpwi += criterion23ResultBigrams.h1;

			auto criterion40ResultLetters = criterion40(letterFrequency(part), (int)part.size(), letterCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l1fp += criterion40ResultLetters.h1;
			auto criterion40ResultBigrams = criterion40(bigramFrequency(part), (int)part.size(), bigramCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l2fp += criterion40ResultBigrams.h1;
			auto criterion40ResultBigramsWithIntersection = criterion40(bigramFrequency(part), (int)part.size(), bigramWithIntersectionsCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l2fpwi += criterion40ResultBigrams.h1;

			for (auto criterion50Data : Criterion50Datas)
			{
				if (criterion50Data.first.L == l)
				{
					if (criterion50Data.first.l == 1)
					{
						auto criterion50ResultLetters = criterion50(part, letterCounters, criterion50Data.first.j, criterion50Data.second);
						h1Counters[criterion50Data.first.name].l1fp += criterion50ResultLetters.h1;
					}
					else if (criterion50Data.first.l == 2)
					{
						auto criterion50ResultBigrams = criterion50(part, bigramCounters, criterion50Data.first.j, criterion50Data.second);
						h1Counters[criterion50Data.first.name].l2fp += criterion50ResultBigrams.h1;
						auto criterion50ResultBigramsWithIntersitions = criterion50(part, bigramWithIntersectionsCounters, criterion50Data.first.j, criterion50Data.second);
						h1Counters[criterion50Data.first.name].l2fpwi += criterion50ResultBigramsWithIntersitions.h1;
					}
				}
			}

			uniformDistribution(part);

			// Подсчет критериев
			criterion20ResultLetters = criterion20(part, letterCounters, 2);
			h1Counters["2.0"].l1fn += !criterion20ResultLetters.h1;
			criterion20ResultBigrams = criterion20(part, bigramCounters, 1);
			h1Counters["2.0"].l2fn += !criterion20ResultBigrams.h1;
			criterion20ResultBigramsWithIntersection = criterion20(part, bigramWithIntersectionsCounters, 1);
			h1Counters["2.0"].l2fnwi += !criterion20ResultBigrams.h1;

			criterion21ResultLetters = criterion21(part, letterCounters, 5, 1);
			h1Counters["2.1"].l1fn += !criterion21ResultLetters.h1;
			criterion21ResultBigrams = criterion21(part, bigramCounters, 5, 1);
			h1Counters["2.1"].l2fn += !criterion21ResultBigrams.h1;
			criterion21ResultBigramsWithIntersection = criterion21(part, bigramWithIntersectionsCounters, 5, 2);
			h1Counters["2.1"].l2fnwi += !criterion21ResultBigrams.h1;

			criterion22ResultLetters = criterion22(part, letterCounters, 2);
			h1Counters["2.2"].l1fn += !criterion22ResultLetters.h1;
			criterion22ResultBigrams = criterion22(part, bigramCounters, 1);
			h1Counters["2.2"].l2fn += !criterion22ResultBigrams.h1;
			criterion22ResultBigramsWithIntersection = criterion22(part, bigramWithIntersectionsCounters, 2);
			h1Counters["2.2"].l2fnwi += !criterion22ResultBigrams.h1;

			criterion23ResultLetters = criterion23(part, letterCounters, 2);
			h1Counters["2.3"].l1fn += !criterion23ResultLetters.h1;
			criterion23ResultBigrams = criterion23(part, bigramCounters, 2);
			h1Counters["2.3"].l2fn += !criterion23ResultBigrams.h1;
			criterion23ResultBigramsWithIntersection = criterion23(part, bigramWithIntersectionsCounters, 2);
			h1Counters["2.3"].l2fnwi += !criterion23ResultBigrams.h1;

			criterion40ResultLetters = criterion40(letterFrequency(part), (int)part.size(), letterCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l1fn += !criterion40ResultLetters.h1;
			criterion40ResultBigrams = criterion40(bigramFrequency(part), (int)part.size(), bigramCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l2fn += !criterion40ResultBigrams.h1;
			criterion40ResultBigramsWithIntersection = criterion40(bigramFrequency(part), (int)part.size(), bigramWithIntersectionsCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l2fnwi += !criterion40ResultBigrams.h1;

			for (auto criterion50Data : Criterion50Datas)
			{
				if (criterion50Data.first.L == l && criterion50Data.first.l == 1)
				{
					auto criterion50ResultLetters = criterion50(part, letterCounters, criterion50Data.first.j, criterion50Data.second);
					h1Counters[criterion50Data.first.name].l1fn += !criterion50ResultLetters.h1;
				}
				if (criterion50Data.first.L == l && criterion50Data.first.l == 2)
				{
					auto criterion50ResultBigrams = criterion50(part, bigramCounters, criterion50Data.first.j, criterion50Data.second);
					h1Counters[criterion50Data.first.name].l2fn += !criterion50ResultBigrams.h1;
					criterion50ResultBigrams = criterion50(part, bigramWithIntersectionsCounters, criterion50Data.first.j, criterion50Data.second);
					h1Counters[criterion50Data.first.name].l2fnwi += !criterion50ResultBigrams.h1;
				}
			}

			// Вывод критериев первого текста
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
				cout << endl;
			}
		}

		// Подчет FP и FN
		for (auto& fnfp : h1Counters)
		{
			fnfp.second.l1fn /= 10000.0;
			fnfp.second.l1fp /= 10000.0;
			fnfp.second.l2fn /= 10000.0;
			fnfp.second.l2fp /= 10000.0;
			fnfp.second.l2fnwi /= 10000.0;
			fnfp.second.l2fpwi /= 10000.0;
		}

		// Вывод таблиц критериев
		printCriterionsTable(h1Counters, l, "раномерного заполднения");
		printCriterionsTable(h1Counters, l, "раномерного заполднения", criteriaPassingResultsFile);
	}
	cout << endl;

	lenght = 10;
	while (lenght <= 10000) {
		string randomText(lenght, ' ');
		string damagedText(lenght, ' ');
		string partN(lenght, ' ');
		map<string, CompressionData> compressions;
		for (int i = 0; i < cicles[lenght] && (i + 1) * lenght < text.size(); i++)
		{
			generateRandomText(randomText);
			copy(text.begin() + i * lenght, text.begin() + (i + 1) * lenght, damagedText.begin());
			uniformDistribution(damagedText);
			copy(text.begin() + i * lenght, text.begin() + (i + 1) * lenght, partN.begin());

			auto randomTextHaffmane = haffman(randomText);
			auto damagedTextHaffmane = haffman(damagedText);
			auto partNTextHaffmane = haffman(partN);

			auto randomTextShannonFano = shannonFano(randomText);
			auto damagedTextShannonFano = shannonFano(damagedText);
			auto partNTextShannonFano = shannonFano(partN);

			auto randomTextDEFLATE = deflate(randomText);
			auto damagedTextDEFLATE = deflate(damagedText);
			auto partNTextDEFLATE = deflate(partN);

			auto randomTextGZip = gzip(randomText);
			auto damagedTextGZip = gzip(damagedText);
			auto partNTextGZip = gzip(partN);

			auto randomTextLZ77 = lz77(randomText);
			auto damagedTextLZ77 = lz77(damagedText);
			auto partNTextLZ77 = lz77(partN);

			auto randomTextRLE = RLE(randomText);
			auto damagedTextRLE = RLE(damagedText);
			auto partNTextRLE = RLE(partN);

			auto randomTextRLE_Bit = RLE_Bit(randomText);
			auto damagedTextRLE_Bit = RLE_Bit(damagedText);
			auto partNTextRLE_Bit = RLE_Bit(partN);

			auto result = structureCriterion(damagedTextHaffmane, randomTextHaffmane, 1.0);
			compressions["Huffman"].fp += result.h1;
			result = structureCriterion(randomTextHaffmane, partNTextHaffmane, 1.0);
			compressions["Huffman"].fn += !result.h1;

			result = structureCriterion(damagedTextShannonFano, randomTextShannonFano, 1.0);
			compressions["ShannonFano"].fp += result.h1;
			result = structureCriterion(randomTextShannonFano, partNTextShannonFano, 1.0);
			compressions["ShannonFano"].fn += !result.h1;

			result = structureCriterion(damagedTextDEFLATE, randomTextDEFLATE, 1.0);
			compressions["DEFLATE"].fp += result.h1;
			result = structureCriterion(randomTextDEFLATE, partNTextDEFLATE, 1.0);
			compressions["DEFLATE"].fn += !result.h1;

			result = structureCriterion(damagedTextGZip, randomTextGZip, 1.0);
			compressions["GZip"].fp += result.h1;
			result = structureCriterion(randomTextGZip, partNTextGZip, 1.0);
			compressions["GZip"].fn += !result.h1;

			result = structureCriterion(damagedTextLZ77, randomTextLZ77, 1.0);
			compressions["LZ77"].fp += result.h1;
			result = structureCriterion(randomTextLZ77, partNTextLZ77, 1.0);
			compressions["LZ77"].fn += !result.h1;

			result = structureCriterion(damagedTextRLE, randomTextRLE, 1.0);
			compressions["RLE"].fp += result.h1;
			result = structureCriterion(randomTextRLE, partNTextRLE, 1.0);
			compressions["RLE"].fn += !result.h1;

			result = structureCriterion(damagedTextRLE_Bit, randomTextRLE_Bit, 1.0);
			compressions["RLE_Bit"].fp += result.h1;
			result = structureCriterion(randomTextRLE_Bit, partNTextRLE_Bit, 1.0);
			compressions["RLE_Bit"].fn += !result.h1;
		}

		for (auto& value : compressions)
		{
			value.second.fp /= 10000.0;
			value.second.fn /= 10000.0;
		}

		// Вывод структурного критерия
		printCompressionTable(compressions, lenght, cicles[lenght], "Равномерное заполнение");
		printCompressionTable(compressions, lenght, cicles[lenght], "Равномерное заполнение", criteriaPassingResultsFile);
		lenght = lenght * 10;
	}

	for (auto l : ls)  // Для каждой L
	{
		string part(l, ' ');  // Строка длиной L
		map<string, FPFN> h1Counters;  // Счетчики H1

		for (int i = 0; i < cicles[l] && i * l < (int)text.size() - l; i++)  // Циклы 10000 и 1000
		{
			copy(text.begin() + (long long)i * l, text.begin() + ((long long)i + 1) * l, part.begin());

			auto criterion20ResultLetters = criterion20(part, letterCounters, 2);
			h1Counters["2.0"].l1fp += criterion20ResultLetters.h1;
			auto criterion20ResultBigrams = criterion20(part, bigramCounters, 1);
			h1Counters["2.0"].l2fp += criterion20ResultBigrams.h1;
			auto criterion20ResultBigramsWithIntersection = criterion20(part, bigramWithIntersectionsCounters, 1);
			h1Counters["2.0"].l2fpwi += criterion20ResultBigrams.h1;

			auto criterion21ResultLetters = criterion21(part, letterCounters, 5, 1);
			h1Counters["2.1"].l1fp += criterion21ResultLetters.h1;
			auto criterion21ResultBigrams = criterion21(part, bigramCounters, 5, 1);
			h1Counters["2.1"].l2fp += criterion21ResultBigrams.h1;
			auto criterion21ResultBigramsWithIntersection = criterion21(part, bigramWithIntersectionsCounters, 5, 2);
			h1Counters["2.1"].l2fpwi += criterion21ResultBigrams.h1;

			auto criterion22ResultLetters = criterion22(part, letterCounters, 2);
			h1Counters["2.2"].l1fp += criterion22ResultLetters.h1;
			auto criterion22ResultBigrams = criterion22(part, bigramCounters, 1);
			h1Counters["2.2"].l2fp += criterion22ResultBigrams.h1;
			auto criterion22ResultBigramsWithIntersection = criterion22(part, bigramWithIntersectionsCounters, 2);
			h1Counters["2.2"].l2fpwi += criterion22ResultBigrams.h1;

			auto criterion23ResultLetters = criterion23(part, letterCounters, 2);
			h1Counters["2.3"].l1fp += criterion23ResultLetters.h1;
			auto criterion23ResultBigrams = criterion23(part, bigramCounters, 2);
			h1Counters["2.3"].l2fp += criterion23ResultBigrams.h1;
			auto criterion23ResultBigramsWithIntersection = criterion23(part, bigramWithIntersectionsCounters, 2);
			h1Counters["2.3"].l2fpwi += criterion23ResultBigrams.h1;

			auto criterion40ResultLetters = criterion40(letterFrequency(part), (int)part.size(), letterCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l1fp += criterion40ResultLetters.h1;
			auto criterion40ResultBigrams = criterion40(bigramFrequency(part), (int)part.size(), bigramCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l2fp += criterion40ResultBigrams.h1;
			auto criterion40ResultBigramsWithIntersection = criterion40(bigramFrequency(part), (int)part.size(), bigramWithIntersectionsCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l2fpwi += criterion40ResultBigrams.h1;

			for (auto criterion50Data : Criterion50Datas)
			{
				if (criterion50Data.first.L == l)
				{
					if (criterion50Data.first.l == 1)
					{
						auto criterion50ResultLetters = criterion50(part, letterCounters, criterion50Data.first.j, criterion50Data.second);
						h1Counters[criterion50Data.first.name].l1fp += criterion50ResultLetters.h1;
					}
					else if (criterion50Data.first.l == 2)
					{
						auto criterion50ResultBigrams = criterion50(part, bigramCounters, criterion50Data.first.j, criterion50Data.second);
						h1Counters[criterion50Data.first.name].l2fp += criterion50ResultBigrams.h1;
						auto criterion50ResultBigramsWithIntersitions = criterion50(part, bigramWithIntersectionsCounters, criterion50Data.first.j, criterion50Data.second);
						h1Counters[criterion50Data.first.name].l2fpwi += criterion50ResultBigramsWithIntersitions.h1;
					}
				}
			}

			distribution(part);

			// Подсчет критериев
			criterion20ResultLetters = criterion20(part, letterCounters, 2);
			h1Counters["2.0"].l1fn += !criterion20ResultLetters.h1;
			criterion20ResultBigrams = criterion20(part, bigramCounters, 1);
			h1Counters["2.0"].l2fn += !criterion20ResultBigrams.h1;
			criterion20ResultBigramsWithIntersection = criterion20(part, bigramWithIntersectionsCounters, 1);
			h1Counters["2.0"].l2fnwi += !criterion20ResultBigrams.h1;

			criterion21ResultLetters = criterion21(part, letterCounters, 5, 1);
			h1Counters["2.1"].l1fn += !criterion21ResultLetters.h1;
			criterion21ResultBigrams = criterion21(part, bigramCounters, 5, 1);
			h1Counters["2.1"].l2fn += !criterion21ResultBigrams.h1;
			criterion21ResultBigramsWithIntersection = criterion21(part, bigramWithIntersectionsCounters, 5, 2);
			h1Counters["2.1"].l2fnwi += !criterion21ResultBigrams.h1;

			criterion22ResultLetters = criterion22(part, letterCounters, 2);
			h1Counters["2.2"].l1fn += !criterion22ResultLetters.h1;
			criterion22ResultBigrams = criterion22(part, bigramCounters, 1);
			h1Counters["2.2"].l2fn += !criterion22ResultBigrams.h1;
			criterion22ResultBigramsWithIntersection = criterion22(part, bigramWithIntersectionsCounters, 2);
			h1Counters["2.2"].l2fnwi += !criterion22ResultBigrams.h1;

			criterion23ResultLetters = criterion23(part, letterCounters, 2);
			h1Counters["2.3"].l1fn += !criterion23ResultLetters.h1;
			criterion23ResultBigrams = criterion23(part, bigramCounters, 2);
			h1Counters["2.3"].l2fn += !criterion23ResultBigrams.h1;
			criterion23ResultBigramsWithIntersection = criterion23(part, bigramWithIntersectionsCounters, 2);
			h1Counters["2.3"].l2fnwi += !criterion23ResultBigrams.h1;

			criterion40ResultLetters = criterion40(letterFrequency(part), (int)part.size(), letterCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l1fn += !criterion40ResultLetters.h1;
			criterion40ResultBigrams = criterion40(bigramFrequency(part), (int)part.size(), bigramCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l2fn += !criterion40ResultBigrams.h1;
			criterion40ResultBigramsWithIntersection = criterion40(bigramFrequency(part), (int)part.size(), bigramWithIntersectionsCounters, (int)text.size(), 0.05);
			h1Counters["4.0"].l2fnwi += !criterion40ResultBigrams.h1;

			for (auto criterion50Data : Criterion50Datas)
			{
				if (criterion50Data.first.L == l && criterion50Data.first.l == 1)
				{
					auto criterion50ResultLetters = criterion50(part, letterCounters, criterion50Data.first.j, criterion50Data.second);
					h1Counters[criterion50Data.first.name].l1fn += !criterion50ResultLetters.h1;
				}
				if (criterion50Data.first.L == l && criterion50Data.first.l == 2)
				{
					auto criterion50ResultBigrams = criterion50(part, bigramCounters, criterion50Data.first.j, criterion50Data.second);
					h1Counters[criterion50Data.first.name].l2fn += !criterion50ResultBigrams.h1;
					criterion50ResultBigrams = criterion50(part, bigramWithIntersectionsCounters, criterion50Data.first.j, criterion50Data.second);
					h1Counters[criterion50Data.first.name].l2fnwi += !criterion50ResultBigrams.h1;
				}
			}

			// Вывод критериев первого текста
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
				cout << endl;
			}
		}

		// Подчет FP и FN
		for (auto& fnfp : h1Counters)
		{
			fnfp.second.l1fn /= 10000.0;
			fnfp.second.l1fp /= 10000.0;
			fnfp.second.l2fn /= 10000.0;
			fnfp.second.l2fp /= 10000.0;
			fnfp.second.l2fnwi /= 10000.0;
			fnfp.second.l2fpwi /= 10000.0;
		}

		// Вывод таблиц критериев
		printCriterionsTable(h1Counters, l, "соотношения");
		printCriterionsTable(h1Counters, l, "соотношения", criteriaPassingResultsFile);
	}
	cout << endl;

	lenght = 10;
	while (lenght <= 10000) {
		string randomText(lenght, ' ');
		string damagedText(lenght, ' ');
		string partN(lenght, ' ');
		map<string, CompressionData> compressions;
		for (int i = 0; i < cicles[lenght] && (i + 1) * lenght < text.size(); i++)
		{
			generateRandomText(randomText);
			copy(text.begin() + i * lenght, text.begin() + (i + 1) * lenght, damagedText.begin());
			distribution(damagedText);
			copy(text.begin() + i * lenght, text.begin() + (i + 1) * lenght, partN.begin());

			auto randomTextHaffmane = haffman(randomText);
			auto damagedTextHaffmane = haffman(damagedText);
			auto partNTextHaffmane = haffman(partN);

			auto randomTextShannonFano = shannonFano(randomText);
			auto damagedTextShannonFano = shannonFano(damagedText);
			auto partNTextShannonFano = shannonFano(partN);

			auto randomTextDEFLATE = deflate(randomText);
			auto damagedTextDEFLATE = deflate(damagedText);
			auto partNTextDEFLATE = deflate(partN);

			auto randomTextGZip = gzip(randomText);
			auto damagedTextGZip = gzip(damagedText);
			auto partNTextGZip = gzip(partN);

			auto randomTextLZ77 = lz77(randomText);
			auto damagedTextLZ77 = lz77(damagedText);
			auto partNTextLZ77 = lz77(partN);

			auto randomTextRLE = RLE(randomText);
			auto damagedTextRLE = RLE(damagedText);
			auto partNTextRLE = RLE(partN);

			auto randomTextRLE_Bit = RLE_Bit(randomText);
			auto damagedTextRLE_Bit = RLE_Bit(damagedText);
			auto partNTextRLE_Bit = RLE_Bit(partN);

			auto result = structureCriterion(damagedTextHaffmane, randomTextHaffmane, 1.0);
			compressions["Huffman"].fp += result.h1;
			result = structureCriterion(randomTextHaffmane, partNTextHaffmane, 1.0);
			compressions["Huffman"].fn += !result.h1;

			result = structureCriterion(damagedTextShannonFano, randomTextShannonFano, 1.0);
			compressions["ShannonFano"].fp += result.h1;
			result = structureCriterion(randomTextShannonFano, partNTextShannonFano, 1.0);
			compressions["ShannonFano"].fn += !result.h1;

			result = structureCriterion(damagedTextDEFLATE, randomTextDEFLATE, 1.0);
			compressions["DEFLATE"].fp += result.h1;
			result = structureCriterion(randomTextDEFLATE, partNTextDEFLATE, 1.0);
			compressions["DEFLATE"].fn += !result.h1;

			result = structureCriterion(damagedTextGZip, randomTextGZip, 1.0);
			compressions["GZip"].fp += result.h1;
			result = structureCriterion(randomTextGZip, partNTextGZip, 1.0);
			compressions["GZip"].fn += !result.h1;

			result = structureCriterion(damagedTextLZ77, randomTextLZ77, 1.0);
			compressions["LZ77"].fp += result.h1;
			result = structureCriterion(randomTextLZ77, partNTextLZ77, 1.0);
			compressions["LZ77"].fn += !result.h1;

			result = structureCriterion(damagedTextRLE, randomTextRLE, 1.0);
			compressions["RLE"].fp += result.h1;
			result = structureCriterion(randomTextRLE, partNTextRLE, 1.0);
			compressions["RLE"].fn += !result.h1;

			result = structureCriterion(damagedTextRLE_Bit, randomTextRLE_Bit, 1.0);
			compressions["RLE_Bit"].fp += result.h1;
			result = structureCriterion(randomTextRLE_Bit, partNTextRLE_Bit, 1.0);
			compressions["RLE_Bit"].fn += !result.h1;
		}

		for (auto& value : compressions)
		{
			value.second.fp /= 10000.0;
			value.second.fn /= 10000.0;
		}

		// Вывод структурного критерия
		printCompressionTable(compressions, lenght, cicles[lenght], "На основе соотношения");
		printCompressionTable(compressions, lenght, cicles[lenght], "На основе соотношения", criteriaPassingResultsFile);
		lenght = lenght * 10;
	}

	criteriaPassingResultsFile.close();

	// Генерация 10 текстов по 10000 символов и проверка 4.0 критериемы
	vector<bool> creterion40Values;
	string randomText(10, ' ');
	generateRandomText(randomText);
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

	cin.get();
	cin.get();
	return 0;
}
