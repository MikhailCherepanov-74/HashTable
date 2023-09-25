#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "HashTable.h"
using namespace std;
void PrintMenu() {
    std::cout << "Меню:\n";
    std::cout << "1. Ввод новой строки для записи в таблицу\n";
    std::cout << "2. Ввод строки для удаления из таблицы\n";
    std::cout << "3. Поиск строки в таблице\n";
    std::cout << "4. Очистить таблицу\n";
    std::cout << "5. Записать строки из input.txt в таблицу\n";
    std::cout << "6. Вывести таблицу на экран\n";
    std::cout << "7. Выход\n";
}

int main() {
    string s = "abc";
    HashTable<string> *hash=new HashTable<string>;

    while (true) {
        PrintMenu();

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::string str;
                std::cout << "Введите строку для записи в таблицу: ";
                std::cin.ignore(); // Очистка буфера
                std::getline(std::cin, str); // Считываем всю строку
                std::istringstream iss(str);
                std::vector<std::string> wordsVector;
                // Разделяем строку и добавляем слова в вектор
                std::string word;
                while (iss >> word) {
                    if(word[word.size()-1]=='.'||
                       word[word.size()-1]==','||
                       word[word.size()-1]=='!'||
                       word[word.size()-1]=='?'||
                       word[word.size()-1]==':'||
                       word[word.size()-1]==';'){
                        word.erase(word.end()-1);
                    }
                    wordsVector.push_back(word);
                }
                for(auto e: wordsVector){
                    hash->Add(e);
                }
                cout<<"Слова из строки успешно добавлены.";
                break;
            }
            case 2: {
                std::string str;
                std::cout << "Введите строку для удаления из таблицу: ";
                std::cin.ignore(); // Очистка буфера
                std::getline(std::cin, str); // Считываем всю строку
                if(hash->Remove(str))
                    cout<<"Слово успешно удалено"<<endl;
                else
                    cout<<"Слово не найдено"<<endl;
                break;
            }
            case 3: {
                std::string str;
                std::cout << "Введите строку для поиска: ";
                std::cin.ignore(); // Очистка буфера
                std::getline(std::cin, str); // Считываем всю строку
                if (hash->Find(str)) {
                    std::cout << "Строка найдена в таблице.\n";
                } else {
                    std::cout << "Строка не найдена в таблице.\n";
                }
                break;
            }
            case 4: {
                hash->Clear();
                std::cout << "Таблица очищено.\n";
                break;
            }
            case 5: {
                std::ifstream inputFile("input.txt");
                if (inputFile.is_open()) {
                    std::string line;
                    while (std::getline(inputFile, line)) {
                        std::istringstream iss(line);
                        std::vector<std::string> wordsVector;
                        std::string word;
                        while (iss >> word) {
                            if(word[word.size()-1]=='.'||
                               word[word.size()-1]==','||
                               word[word.size()-1]=='!'||
                               word[word.size()-1]=='?'||
                               word[word.size()-1]==':'||
                               word[word.size()-1]==';'){
                                word.erase(word.end()-1);
                            }
                            wordsVector.push_back(word);
                        }
                        for (const auto &e : wordsVector) {
                            hash->Add(e);
                        }
                    }
                    inputFile.close();
                    std::cout << "Строки из input.txt добавлены в таблицу.\n";
                } else {
                    std::cout << "Не удалось открыть файл input.txt.\n";
                }
                break;
            }
            case 6: {
                hash->PrintAll();
                break;
            }
            case 7: {
                delete hash;
                std::cout << "Программа завершена.\n";
                return 0;
            }
            default:
                std::cout << "Неверный выбор. Попробуйте снова.\n";
                break;
        }
    }

    return 0;
}

