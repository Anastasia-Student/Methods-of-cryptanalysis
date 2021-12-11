#pragma once
#include "alphabet.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <locale>
#include <codecvt>
#include <map>

using namespace std;

void filter(string inputPath, string outputPath)  // Фильтрация текста от лишних символов
{
	wifstream input(inputPath);  // Входной файл
	locale utf8(locale::empty(), new codecvt_utf8<wchar_t>());  // Для декодирования UTF8
	auto& correct = getAlphabetСorrection();  // Получить map для коррекции текста
	input.imbue(utf8);  // Установка кодировки
	if (!input.fail())  // Если файл открыт корректно
	{
		ofstream output(outputPath);  // Файл для сохранения
		output.imbue(utf8);  // Установка кодировки
		if (!output.fail())  // Если файл открыт корректно
		{
			wchar_t readBuffer[1024];  // Буфер для чтения
			char writeBuffer[1024];  // Буфер для записи
			while (!input.eof())  // Пока не конец файла
			{
				input.read(readBuffer, 1024);  // Прочитать 1024 символа
				int count = (int)input.gcount();  // Количество прочитанных файлов
				int j = 0;  // Индекс записываемого символа
				for (int i = 0; i < count; i++)  // Пройтись по символам
					if (correct.find(readBuffer[i]) != correct.end())  // Если символ есть в map для коррекции
						writeBuffer[j++] = ascii(correct[readBuffer[i]]);  // Скорректировать
				output.write(writeBuffer, j);  // Записать в файл
				output.flush();  // Завершить запись
			}
			output.close();  // Закрыть файл для записи
		}
		input.close();  // Закрыть файл для чтения
	}
}
