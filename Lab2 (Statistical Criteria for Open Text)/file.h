#pragma once

#include "alphabet.h"
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

string load(string inputPath)  // Загрузка файла
{
	ifstream input(inputPath);  // Файл с текстом
	if (!input.fail())  // Если открыт корректно
	{
		stringstream stream;  // Поток символов
		stream << input.rdbuf();  // Чтение из файла
		return stream.str();  // Вернуть строку
	}
	return "";
}

bool save(string outputPath, const string& content)  // Сохранение файла
{
	ofstream output(outputPath);  // Файл для сохранения
	if (!output.fail())  // Если открыт корректно
	{
		output << content;  // Запись в файл
		output.close();  // Закрытие файла
		return true;
	}
	return false;
}
