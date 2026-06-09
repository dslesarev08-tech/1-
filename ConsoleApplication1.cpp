// Все строки и символы только в UTF8 кодировке!!!
// Требуется файл со словарем nouns.txt
// По одному слову в строке, маленькими буквами

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string> 
#include <vector>
#include <unordered_map>
#include <chrono>

using namespace std;


static const size_t utf8_skip_data[256] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

// разбор слова с учетом возможного размера символа от 1 до 6 байт и формирование ассоциативного массива числа вхождений каждой буквы
unordered_map<string, int> strtomap_utf8(const char* src, size_t maxncpy)
{
    unordered_map<string, int> dst;
    size_t utf8_size;

    if (maxncpy > 0) {
        while (*src != '\0' && (utf8_size = utf8_skip_data[*((unsigned char*)src)]) < maxncpy) {
            maxncpy -= utf8_size;
            string symb = "";
            switch (utf8_size) {
            case 6: symb += *src++;
            case 5: symb += *src++;
            case 4: symb += *src++;
            case 3: symb += *src++;
            case 2: symb += *src++;
            case 1: symb += *src++;
            }
            dst[symb]++;
        }
    }
    return dst;
}


int main()
{

    cout << "User-preferred locale setting is "
        << locale("").name().c_str() << '\n';

    setlocale(LC_ALL, ".UTF-8");
//    system("chcp 65001");

    string s1 = "Hello, World! Привет, мир! 你好, 世界! 😎\n";
    std::cout << s1;
    cout << "H имеет размер " << string("H").size() << endl;
    cout << "П имеет размер " << string("П").size() << endl;
    cout << "你 имеет размер " << string("你").size() << endl;
    cout << "😎 имеет размер " << string("😎").size() << endl;


    vector<string> lines;

    string word;
//    cout << "Введите слово (в нижнем регистре)! ";
//    cin >> word;
    word = "обороноспособность";

    cout << "Слово " << word << ", размер слова: " << word.size() << endl;

    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    // Разбор и сравнение числа вхождений каждой буквы слова из словаря с заданным словом word

    auto word_map = strtomap_utf8(word.c_str(), word.size() + 1);


    ifstream in("nouns.txt"); // окрываем файл для чтения

    if (!in.is_open()) {
        cout << "failed to open " << "nouns.txt" << '\n';
        return -1;
    }

    // Считываем в вектор строк, но только те слова, которые не длиннее заданного word
    while (in)
    {
        string line;
        in >> line;
//        cout << line << ": " << line.size() << ' ';
        if (line.size() <= word.size())
        {
            bool found = 1;
            auto line_map = strtomap_utf8(line.c_str(), line.size() + 1);
            for (auto p = line_map.begin(); p != line_map.end(); p++)
                if ((*p).second > word_map[(*p).first]) {
                    found = 0;
                    break;
                }
            if (found)
                lines.push_back(line);

        }

    }
    // Сортируем массив строк по убыванию длины
    sort(lines.begin(), lines.end(), [](string& s1, string& s2) {return s1.length() > s2.length(); });
    for (auto i = lines.begin(); i != lines.end(); i++)
        cout << *i << endl;

    chrono::steady_clock::time_point end = chrono::steady_clock::now();

    cout << "Diff(s) = " << 0.000001 * chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << endl;

//    system("pause");
    return 0;
}