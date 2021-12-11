#pragma once

#include <map>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

typedef char Letter;  // Буква (char)
typedef vector<Letter> Alphabet;  // Алфавит (состоит из букв)
typedef wchar_t UTF8Letter;  // UTF8 Буква (wchar_t)
typedef vector<UTF8Letter> UTF8Alphabet;  // Алфавит (состоит из UTF8 букв)

struct Bigram  // Биграмма
{
	Letter letters[2];  // Две буквы

	Bigram(Letter first, Letter second)  // Конструктор из двух букв
	{
		letters[0] = first;
		letters[1] = second;
	};

	Bigram()  // Конструктор по умолчанию
	{
		letters[0] = 0;
		letters[1] = 0;
	};

	Letter& operator[](const int& index)  // Перегрузка оператора []
	{
		return letters[index];  // Возврат элемента из массива букв
	}

	bool operator==(const Bigram& other) const  // Оператор сравнения (равно)
	{
		return letters[0] == other.letters[0] &&  // Сравнение букв
			letters[1] == other.letters[1];
	}

	bool operator < (const Bigram& other) const  // Оператор сравнения (меньше)
	{
		if (letters[0] < other.letters[0])  // Сравнение первой буквы
			return true;
		else if (letters[0] > other.letters[0])  // Сравнение первой буквы
			return false;
		else if (letters[1] < other.letters[1])  // Сравнение второй буквы
			return true;
		return false;
	}

	operator string()  // Преобразование в строку
	{
		return string("") + letters[0] + letters[1];
	}

	friend ostream& operator << (ostream& stream, const Bigram& bigram)  // Оператор вывода в поток
	{
		stream << bigram.letters[0] << bigram.letters[1];  // Вывод букв в поток
		return stream;
	}
};

Alphabet alphabet = { 'а', 'б', 'в', 'г', 'д', 'е', 'є', 'ж', 'з', 'и', 'i', 'ї', 'й', 'к', 'л',  // Алфавит
		'м', 'н', 'о', 'п', 'р', 'с', 'т', 'у', 'ф', 'х', 'ц', 'ч', 'ш', 'щ', 'ь', 'ю', 'я' };

UTF8Alphabet utf8Alphabet = { L'а', L'б', L'в', L'г', L'д', L'е', L'є', L'ж', L'з', L'и', L'і',  // UTF8 Алфавит
	L'ї', L'й', L'к', L'л', L'м', L'н', L'о', L'п', L'р', L'с', L'т', L'у', L'ф', L'х', L'ц', L'ч',
	L'ш', L'щ', L'ь', L'ю', L'я' };

map<UTF8Letter, UTF8Letter> alphabetСorrection = { {L'а', L'а'}, { L'А', L'а' }, { L'б', L'б' },  // Для преобразования текста к 32 буквенному алфавиту
	{ L'Б', L'б' }, { L'в', L'в' }, { L'В', L'в' }, { L'г', L'г' }, { L'Г', L'г' }, { L'ґ', L'г' },
	{ L'Ґ', L'г' }, { L'д', L'д' }, { L'Д', L'д' }, { L'е', L'е' }, { L'Е', L'е' }, { L'є', L'є' },
	{ L'Є', L'є' }, { L'ж', L'ж' }, { L'Ж', L'ж' }, { L'з', L'з' }, { L'З', L'з' }, { L'и', L'и' },
	{ L'И', L'и' }, { L'і', L'і' }, { L'І', L'і' }, { L'ї', L'ї' }, { L'Ї', L'ї' }, { L'й', L'й' },
	{ L'Й', L'й' }, { L'к', L'к' }, { L'К', L'к' }, { L'л', L'л' }, { L'Л', L'л' }, { L'м', L'м' },
	{ L'М', L'м' }, { L'н', L'н' }, { L'Н', L'н' }, { L'о', L'о' }, { L'О', L'о' }, { L'п', L'п' },
	{ L'П', L'п' }, { L'р', L'р' }, { L'Р', L'р' }, { L'с', L'с' }, { L'С', L'с' }, { L'т', L'т' },
	{ L'Т', L'т' }, { L'у', L'у' }, { L'У', L'у' }, { L'ф', L'ф' }, { L'Ф', L'ф' }, { L'х', L'х' },
	{ L'Х', L'х' }, { L'ц', L'ц' }, { L'Ц', L'ц' }, { L'ч', L'ч' }, { L'Ч', L'ч' }, { L'ш', L'ш' },
	{ L'Ш', L'ш' }, { L'щ', L'щ' }, { L'Щ', L'щ' }, { L'ь', L'ь' }, { L'Ь', L'ь' }, { L'ю', L'ю' },
	{ L'Ю', L'ю' }, { L'я', L'я' }, { L'Я', L'я' } };

map<UTF8Letter, Letter> utf8ToASCII = { { L'а', 'а' }, { L'б', 'б' }, { L'в', 'в' }, { L'г', 'г' },  // Для преобразования utf8 в ascii
	{ L'д', 'д' }, { L'е', 'е' }, { L'є', 'є' }, { L'ж', 'ж' }, { L'з', 'з' }, { L'и', 'и' },
	{ L'і', 'i' }, { L'ї', 'ї' }, { L'й', 'й' }, { L'к', 'к' }, { L'л', 'л' }, { L'м', 'м' },
	{ L'н', 'н' }, { L'о', 'о' }, { L'п', 'п' }, { L'р', 'р' }, { L'с', 'с' }, { L'т', 'т' },
	{ L'у', 'у' }, { L'ф', 'ф' }, { L'х', 'х' }, { L'ц', 'ц' }, { L'ч', 'ч' }, { L'ш', 'ш' },
	{ L'щ', 'щ' }, { L'ь', 'ь' }, { L'ю', 'ю' }, { L'я', 'я' } };

Alphabet& getAlphabet()  // Получить алфавит
{
	return alphabet;
}

UTF8Alphabet& getUTF8Alphabet()  // Получить UTF8 алфавит
{
	return utf8Alphabet;
}

map<UTF8Letter, UTF8Letter>& getAlphabetСorrection()  // Получить map для преобразования текста
{
	return alphabetСorrection;
}

Letter ascii(UTF8Letter letter)  // Преобразовать UTF8 символ в ascii
{
	if (utf8ToASCII.find(letter) != utf8ToASCII.end())  // Проверка наличия в map
		return utf8ToASCII[letter];  // Вернуть ascii символ
	return ' ';
}

string ascii(wstring utf8String)  // Преобразовать UTF8 строку в ascii
{
	stringstream stream;
	for (auto& letter : utf8String)  // Пройтись по строке
		stream << ascii(letter);  // Преобразовать символ
	return stream.str();  // Вернуть строку
}

int code(Letter letter)  // Получить код символа (0 - 31)
{
	auto iterator = find(alphabet.begin(), alphabet.end(), letter);  // Поиск позиции символа в алфавите
	if (iterator != alphabet.end())  // Если символ есть в алфавите
		return (int)(iterator - alphabet.begin());  // Вернуть код
	throw out_of_range("Алфавит не содержит буквы!");
}

Letter letter(int code)  // Получить символ по коду (0 - 31)
{
	if (code < 0)  // Некорректный код
		throw out_of_range("Не существует символа соответствующего коду!");
	return alphabet[code % alphabet.size()];  // Возврат символа по модулю m
}
