#ifndef STRING_H_LIBRARY_H
#define STRING_H_LIBRARY_H

#include <iostream>
#include <cstring>

class String {
private:
    char* str = nullptr;
    size_t sz = 0;
    size_t capacity = 0;
public:
    String() = default; /// Конструктор по умолчанию

    String(const char* c) { /// Конструктор от C-Style строки
        sz = strlen(c);
        str = new char[sz];
        capacity = sz;
        memcpy(str, c, sz);
    }
    String(size_t n, char c){ /// n-копий символа c
        str = new char[n];
        sz = n;
        capacity = sz;
        memset(str, c, n);
    }

    String (const String& s) { /// Конструктор от строки
        sz = s.sz;
        str = new char[s.capacity];
        memcpy(str, s.str, s.sz);
        capacity = s.capacity;
    }

    String& operator=(const String& s){ /// Присваивание через копирование памяти из массива в массив
        if (this == &s) {
            return *this;
        } else {
            if (str != nullptr) {
                delete[] str;
            }
            sz = s.sz;
            str = new char[s.capacity];
            memcpy(str, s.str, sz);
            capacity = s.capacity;
            return *this;
        }
    }

    char& operator[](size_t index) { /// [] для неконстантных строк
        return str[index];
    }


    char operator[](size_t index) const { /// [] для константных строк
        return str[index];
    }

    size_t length() const { /// возвращение длины
        return sz;
    }

    bool empty() const { /// true - если массив пуст, false - иначе
        return (sz == 0);
    }

    void clear(){  /// Очистка и удаление массива, длина и размер обнуляются
        if (str != nullptr) {
            delete[] str;
            str = nullptr;
        }
        sz = 0;
        capacity = 0;
    }

    char& front() { /// первый символ неконстантной строки
        return str[0];
    }

    char& back() { /// последний символ неконстантной строки
        return str[sz - 1];
    }

    char front() const { ///первый символ константной строки
        return str[0];
    }

    char back() const { /// последний символ константной строки
        return str[sz - 1];
    }

    void push_back(const char& c) { ///Реализация push_back
        if (sz < capacity) {
            str[sz] = c;
            ++sz;
        } else {                 /// В случае, когда мы доходим до конца массива, увеличиваем его размер в два раза, таким образом,
            if (capacity == 0) { /// для выполнения тяжёлой операции за O(n) нужно сделать n операций за O(1), значит асимптотика O*(1)
                capacity = 1;
            } else {
                capacity *= 2;
            }
            char* temp = new char[capacity];
            memcpy(temp, str, sz);
            temp[sz] = c;
            ++sz;
            if (str != nullptr) {
                delete[] str;
            }
            str = temp;
        }
    }

    void pop_back() {                           /// Для pop_back, в тот момент, когда мы доходим до середины первой половины
        --sz;                                   /// В целях экономии памяти уменьшаем массив в 2 раза. Аналогично push_back работает за O*(1)
        str[sz] = '\0';
        if (capacity / 4 > sz)  {
            char* temp = new char[capacity / 2];
            memcpy(temp, str, sz);
            capacity /= 2;
            if (str != nullptr) {
                delete[] str;
            }
            str = temp;
        }
        if (sz == 0) {
            if (str != nullptr){
                delete[] str;
                str = nullptr;
            }
            capacity = 0;
        }
    }
                                            /// Операторы += от символа, C-Style строки и строки (посимвольное добавление)
    String& operator+=(const char& c) {
        this->push_back(c);
        return *this;
    }

    String& operator+=(const char* c) {
        for(size_t i = 0; i < strlen(c); ++i) {
            this->push_back(c[i]);
        }
        return *this;
    }

    String& operator+=(const String& s){
        String temp = *this;
        for(size_t i = 0; i < s.length(); ++i) {
            temp.push_back(s[i]);
        }
        *this = temp;
        return *this;
    }
                                              /// find, Rfind от символа и строки, идём по массиву и начиная с каждого элемента, пока искомая строка помещается
    size_t find(const String& SubS) const {   /// Проверяем, совпадают ли символы. Если совпали, выходим из цикла и выводим ответ, иначе возвращаем фиктивный индекс. Работает за O(n^2). Rfind - как find, только справа-налево
        size_t index;
        bool flag = true;
        if (sz  >= SubS.sz) {
            for (size_t i = 0; i <= sz - SubS.sz ; ++i) {
                flag = false;
                index = i;
                size_t c = i;
                for (size_t j = 0; j < SubS.sz; ++j) {
                    if (str[c + j] != SubS[j]) {
                        flag = true;
                        break;
                    }
                }
                if (!flag) {
                    break;
                }
            }
        }
        if (!flag) {
            return index;
        } else {
            return sz;
        }
    }

    size_t find(const char& c) const {
        bool flag = false;
        size_t index;
        for (size_t i = 0; i < sz; ++i) {
            if (str[i] == c) {
                flag = true;
                index = i;
                break;
            }
        }
        if (!flag) {
            return sz;
        } else {
            return index;
        }
    }

    size_t rfind(const String& SubS) const {
        size_t index;
        bool flag = true;
        if (sz >= SubS.sz) {
            for (long long i = sz - SubS.sz + 1; i >= 0; --i) {
                flag = false;
                index = i;
                size_t c = i;
                for (size_t j = 0; j < SubS.sz; ++j) {
                    if (str[c + j] != SubS[j]) {
                        flag = true;
                        break;
                    }
                }
                if (!flag) {
                    break;
                }
            }
        }
        if (!flag) {
            return static_cast<size_t>(index);
        } else {
            return sz;
        }
    }

    size_t rfind(const char& c) const{
        bool flag = false;
        size_t index;
        for (size_t i = sz; i != 0; ++i) {
            if (str[i - 1] == c) {
                flag = true;
                index = i - 1;
                break;
            }
        }
        if (!flag) {
            return sz;
        } else {
            return index;
        }
    }

    String substr(const int& start, const int& count) const { /// В новую строку копируем посимвольным добавлением подстроку, работает за O(n)
        String temp;
        for (long long i = start; i < start + count; ++i) {
            temp.push_back(str[i]);
        }
        return temp;
    }

    ~String(){             /// Деструктор с очисткой памяти
        if (str != nullptr) {
            delete[] str;
        }
    };
};

std::ostream& operator<<(std::ostream& out, const String& s) { /// Вывод в поток - посимвольный вывод
    for (size_t i = 0; i < s.length(); ++i) {
        out << s[i];
    }
    return out;
}

std::istream& operator>>(std::istream& in, String& s){ /// Ввод из потока - посимвольное добавление, пока символ - не пробел или служебный
    char c;
    s.clear();
    while (in.get(c) && (!isspace(c)) && (c != '\0')) {
        s += c;
    }
    return in;
}
                                                      /// Операторы сложения для всех комбинаций строк, C-Style строк и символов. Сделаны через += для эффективности.
String operator+(const String& s ,const char& c) {
    String temp = s;
    temp += c;
    return temp;
}

String operator+(const char& c , const String& s) {
    String temp (1, c);
    temp += s;
    return temp;
}

String operator+(const char* c , const String& s) {
    String temp = c;
    temp += s;
    return temp;
}

String operator+(const String& s, const char* c) {
    String temp = s;
    temp += c;
    return temp;
}

String operator+(const String& s ,const String& s1) {
    String temp = s;
    temp += s1;
    return temp;
}

bool operator==(const String& s1, const String& s2) { /// Оператор сравнения - если длины строк совпадают, считаем количество совпавших символов, если != длине - не верно, что равны.
    size_t n1 = s1.length();
    size_t n2 = s2.length();
    if (n1 != n2) {
        return false;
    } else {
        size_t n = 0;
        while ((n < n1) && (s1[n] == s2[n])) {
            ++n;
        }
        return (n == n2);
    }
}


#endif //STRING_H_LIBRARY_H
