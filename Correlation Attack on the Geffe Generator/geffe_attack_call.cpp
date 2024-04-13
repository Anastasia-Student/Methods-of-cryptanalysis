#include "geffe.h"
#include <fstream>

using namespace std;

const char* INPUT_PATH = "./input.txt";  // Входной файл
const char* OUTPUT_PATH = "./output.txt";  // Результирующий файл
const char* OUTPUT_PATH2 = "./output2.txt";  // Результирующий файл

template <typename Stream>
void output(Stream &stream, const Result &result)  // Вывод результатов на экран
{
	stream << "....................................................................." << endl;
	stream << "Количество кандидатов на начальное заполнение L1 = " << result.l1Candidates.size() << endl;
	for (auto candidate : result.l1Candidates)
	{
		stream << "\nКандидат на начальное заполнение L1: " << bin(candidate.data, 30) << endl;
		stream << ((candidate.r < result.c1) ? "Подходит" : "Не подходит") << ", т.к. R=" << candidate.r << (candidate.r < result.c1 ? " < " : " >= ") << result.c1 << endl;
	}
	stream << "....................................................................." << endl;
	stream << "Количество кандидатов на начальное заполнение L2 = " << result.l2Candidates.size() << endl;
	for (auto candidate : result.l2Candidates)
	{
		stream << "\nКандидат на начальное заполнение L2: " << bin(candidate.data, 31) << endl;
		stream << ((candidate.r < result.c2) ? "Подходит" : "Не подходит") << ", т.к. R=" << candidate.r << (candidate.r < result.c2 ? " < " : " >= ") << result.c2 << endl;
	}
	stream << "....................................................................." << endl;
	stream << "Количество кандидатов на начальное заполнение L3 = " << result.l3Candidates.size() << endl;
	/*for (auto candidate : result.l3Candidates)
		stream << "\nКандидат на начальное заполнение L3: " << bin(candidate.data, 32) << endl;*/
	stream << "....................................................................." << endl;
	stream << "Результат проверки: " << (result.complete ? "Все хорошо!" : "Неудача!") << endl;
	if (result.complete)
	{
		stream << "....................................................................." << endl;
		stream << "Начальное заполнение L1: " << bin(result.l1, 30) << endl;
		stream << "Начальное заполнение L2: " << bin(result.l2, 31) << endl;
		stream << "Начальное заполнение L3: " << bin(result.l3, 32) << endl;
	}
}

int main()
{
	setlocale(0, "rus");  // Вывод кирилицы
	ofstream file(OUTPUT_PATH);  // Файл вывода
	ofstream file2(OUTPUT_PATH2);  // Файл вывода

	auto data = load(INPUT_PATH);  // Загрузка входного файла
	auto result = attack(data, file2);  // Попытка атаки

	output(file, result);  // Вывод результатов в файл
	output(cout, result);  // Вывод результатов в консоль

	file.close();  // Закрытие файла
	getchar();  // Ожидание ввода

	return 0;
}
