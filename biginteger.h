//#ifndef MAIN_CPP_BIGINTEGER_H
//#define MAIN_CPP_BIGINTEGER_H
#ifndef BIGINTEGER_H_LIBRARY_H
#define BIGINTEGER_H_LIBRARY_H


#include <cstring>
#include <iostream>
#include <vector>
#include <string>



class BigInteger {
private:
    std::vector <short> num;
    short sign;

    void removeZero() {
        while (num.size() > 1 && num.back() == 0) {
            num.pop_back();
        }
    }

    static void reverseVec(std::vector <short>& a) {
        int j = a.size();
        int k = ((j - 1) / 2);
        for (int i = 0; i <= k; ++i) {
            std::swap(a[i], a[j - 1 - i]);
        }
    }

    void plus() {
        for (size_t i = 0; i < num.size() - 1; ++i){
            if (num[i] > 9) {
                short t = num[i];
                num[i] %= 10;
                num[i + 1] += (t / 10);
            } else if (num[i] < 0) {
                num[i] = 10 + num[i];
                num[i + 1] -= 1;
            }
        }
        if (num[num.size() - 1] > 9) {
            while (num[num.size() - 1] > 9) {
                short t = num[num.size() - 1] / 10;
                num[num.size() - 1] %= 10;
                num.push_back(t);
            }
        } else if (num[num.size() - 1] == 0) {
            while ((num[num.size() - 1] == 0) && (num.size() > 1)) {
                num.pop_back();
            }
        }
    }


public:

    BigInteger() = default;

    BigInteger(const int& value) {
        sign = (value >= 0) ? (1) : (-1);
        int val_cpy = abs(value);
        while (val_cpy != 0) {
            num.push_back(val_cpy % 10);
            val_cpy /= 10;
        }
        if (value == 0) {
            num.push_back(0);
        }
    }

    BigInteger(const std::string& s){
        std::vector<short> temp;
        int r = 0;
        sign = 1;
        if (s[0] == '-') {
            sign = -1;
            ++r;
        }
        for (int i = s.size() - 1; i >= r; --i) {
            temp.push_back(s[i] - 48);
        }
    }

    BigInteger& operator=(const BigInteger& b) {
        this->num = b.num;
        this->sign = b.sign;
        return *this;
    }

    BigInteger& operator+=(const BigInteger& b) { /// Посимвольное сложение, функция plus() для пересчета чисел, если они окажутся больше 10 или < 0, если длина b больше, то делаем b += this
        int k = num.size() - b.num.size();
        if (k >= 0) {
            for (size_t i = 0; i < b.num.size(); ++i) {
                num[i] += (b.sign * sign * b.num[i]);
            }
            plus();
            if (num[num.size() - 1] < 0) {
                num[num.size() - 1] = abs(num[num.size() - 1]);
                sign *= -1;
            }
        } else {
            BigInteger copy = b;
            *this = copy += *this;
            this->sign = b.sign;
        }
        if (this->num.size() == 1 && this->num[0] == 0){
            sign = 1;
        }
        return *this;
    }

    BigInteger operator-() const{
        BigInteger copy = *this;
        if (!((copy.num.size() == 1) && (copy.num[0] == 0))) {
            copy.sign *= (-1);
        }
        return copy;
    }

    BigInteger& operator-=(const BigInteger& b) {
        return *this += -b;
    }

    BigInteger& operator*=(const BigInteger& b) { /// Умножение, Делаю массив i + j (удобнее, чтобы не морочиться с увеличением длины когда нужно + смещение по разряду), посимвольное премножение с переносом целочисл. части деления на 10
        BigInteger result;
        BigInteger temp = *this;
        temp.sign = 1;
        std::vector<short> v(num.size() + b.num.size());
        result.num = v;
        for (int i = temp.num.size() - 1; i >=0 ; --i) {
            int carry = 0;
            for (size_t j = 0; j < b.num.size() || carry; j++) {
                short current = result.num[i + j] + carry + temp.num[i] * (j < b.num.size() ? b.num[j] : 0);
                carry = current / 10;
                result.num[i + j] = current % 10;
            }
        }
        result.plus();
        this->num = result.num;
        sign *= b.sign;
        if (this->num.size() == 1 && this->num[0] == 0){
            sign = 1;
        }
        return *this;
    }

    BigInteger& operator /=(const BigInteger& b) { /// Деление, собираю по кусочку число, как только могу вычесть - вычитаю, количество поместившихся чисел приписываю к результату
        BigInteger result;
        if (b == 0) {
            *this = result;
            return *this;
        }
        BigInteger temp;
        temp.num.clear();
        temp.sign = 1;
        result.sign = 1;
        BigInteger del = b;
        del.sign = 1;
        for (int i = num.size() - 1; i >= 0; --i) {
            temp.num.emplace(temp.num.begin(), num[i]);
            temp.removeZero();
            short k = 0;
            BigInteger temp2 = del;
            while (temp >= temp2 && k < 10) {
                ++k;
                temp2 = (k + 1) * del;
            }
            temp -= k * del;
            result.num.emplace(result.num.cbegin(), k);
            result.removeZero();
        }
        num = result.num;
        this->sign *= b.sign;
        if (this->num.size() == 1 && this->num[0] == 0){
            sign = 1;
        }
        return *this;
    }

    BigInteger& operator %=(const BigInteger& b) {
        *this -= (b * (*this / b));
        if (this->num.size() == 1 && this->num[0] == 0){
            sign = 1;
        }
        return *this;
    }

    BigInteger& operator++() {
        *this += 1;
        return *this;
    }

    BigInteger operator++(int) {
        BigInteger copy = *this;
        ++*this;
        return copy;
    }

    BigInteger& operator--() {
        *this -= 1;
        return *this;
    }

    BigInteger operator--(int) {
        BigInteger copy = *this;
        --*this;
        return copy;
    }

    friend BigInteger operator*(const BigInteger& a, const BigInteger& b){
        BigInteger a_ = a;
        a_ *= b;
        return a_;
    }

    friend BigInteger operator/(const BigInteger& a, const BigInteger& b) {
        BigInteger a_ = a;
        a_ /= b;
        return a_;
    }

    friend BigInteger operator+(const BigInteger& a, const BigInteger& b) {
        BigInteger a_ = a;
        a_ += b;
        return a_;
    }

    explicit operator int() const{
        return atoi(this->toString().c_str());
    }

    explicit operator bool() const{
        return !(*this == 0);
    }

    std::string toString() const {
        std::string output;
        output.clear();
        if (sign < 0) {
            output.push_back('-');
        }
        for (int i = num.size() - 1; i >= 0; --i) {
            output.push_back(num[i] + 48);
        }
        return output;
    }

    friend bool operator==(const BigInteger& a, const BigInteger& b) {
        return (b.num == a.num) && (b.sign == a.sign);
    }

    friend bool operator<(const BigInteger& a, const BigInteger& b) {
        BigInteger a_ = a;
        a_ -= b;
        return (a_.sign < 0);
    }

    friend bool operator<=(const BigInteger& a, const BigInteger& b) {
        return (a == b) || (a < b);
    }

    friend bool operator>(const BigInteger& a, const BigInteger& b) {
        return !(a <= b);
    }

    friend bool operator>=(const BigInteger& a, const BigInteger& b) {
        return !(a < b);
    }

    friend bool operator!=(const BigInteger& a, const BigInteger& b) {
        return !(a == b);
    }



    friend std::istream& operator>>(std::istream& in, BigInteger& b) {
        b.num.clear();
        char c;
        b.sign = 1;
        while (in.get(c) && (!isspace(c)) && (c != '\0')) {
            if (c == '-') {
                b.sign = -1;
            } else {
                b.num.push_back(c - 48);
            }
        }
        reverseVec(b.num);
        return in;
    }

    friend BigInteger operator "" _bi(const char* bi_str);

    ~BigInteger() = default;
};

BigInteger operator "" _bi(const char* bi_str){
    BigInteger res;
    size_t l_str = strlen(bi_str);
    for (size_t i = 0; i < l_str; ++i) {
        if (bi_str[i] == '-') {
            res.sign = -1;
        } else {
            res.num.push_back(static_cast<int>(bi_str[i]) - 48);
        }

    }
    int j = res.num.size();
    int k = ((j - 1) / 2);
    for (int i = 0; i <= k; ++i) {
        std::swap(res.num[i], res.num[j - 1 - i]);
    }
    return res;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& b) {
    out << b.toString();
    return out;
}

BigInteger operator-(const BigInteger& a, const BigInteger& b) {
    BigInteger a_ = a;
    a_ -= b;
    return a_;
}

BigInteger operator%(const BigInteger& a, const BigInteger& b) {
    BigInteger a_ = a;
    a_ %= b;
    return a_;
}


class Rational {
private:
    BigInteger nm;
    BigInteger dnm=1;

    void reduce() { /// НОД
        BigInteger n = nm < 0 ? -nm : nm;
        BigInteger m = dnm;
        while (n > 0){
            BigInteger tmp = m%n;
            m = n;
            n = tmp;
        }
        BigInteger z = m;
        if (z < 0) z = -z;
        nm /= z;
        dnm /= z;
    }

public:
    Rational() = default;

    Rational (const int& value) {
        nm  = value;
        dnm = 1;
    }

    Rational(const BigInteger& value) {
        nm  = value;
        dnm = 1;
    }


    Rational& operator=(const Rational& r) {
        nm = r.nm;
        dnm = r.dnm;
        return *this;
    }

    Rational operator-() {
        Rational copy = *this;
        copy.nm = -nm;
        copy.dnm = dnm;
        return copy;
    }



    Rational& operator+=(const Rational& b) {
        Rational res;
        nm = nm*b.dnm + dnm*b.nm;
        dnm *= b.dnm;
        reduce();
        return *this;
    }

    Rational& operator-=(const Rational& b) {
        Rational res;
        nm = nm * b.dnm - dnm * b.nm;
        dnm *= b.dnm;
        reduce();
        return *this;
    }

    Rational& operator*=(const Rational& b) {
        Rational res;
        nm *= b.nm;
        dnm *= b.dnm;
        reduce();
        return *this;
    }

    Rational& operator/=(const Rational& b) {
        Rational res;
        if (b==0){
            *this = res;
            return *this;
        }
        Rational Copy = *this;
        nm *= b.dnm * ((b.nm >= 0)?(1) : (-1));
        if (Copy != 0)
            dnm = b.nm * Copy.dnm * ((b.nm >= 0)?(1) : (-1));
        else
            dnm = 1;
        reduce();
        return *this;
    }

    friend const Rational operator +(const Rational& a, const Rational& b) {
        Rational a_ = a;
        a_ += b;
        return a_;
    }

    friend const Rational operator -(const Rational& a, const Rational& b) {
        Rational a_ = a;
        a_ -= b;
        return a_;
    }

    friend const Rational operator *(const Rational& a, const Rational& b) {
        Rational a_ = a;
        a_ *= b;
        return a_;
    }

    friend const Rational operator /(const Rational& a, const Rational& b) {
        Rational a_ = a;
        a_ /= b;
        return a_;
    }

    friend bool operator==(const Rational& a, const Rational& b){
        return (a.dnm == b.dnm) && (a.nm == b.nm);
    }

    friend bool operator<(const Rational& a, const Rational& b) {
        return a.nm*b.dnm < b.nm*a.dnm;
    }

    friend bool operator<=(const Rational& a, const Rational& b) {
        return (a == b) || (a < b);
    }

    friend bool operator>(const Rational& a, const Rational& b) {
        return !(a <= b);
    }

    friend bool operator>=(const Rational& a, const Rational& b) {
        return (a == b) || (a > b);
    }

    friend bool operator!=(const Rational& a, const Rational& b) {
        return !(a == b);
    }

    std::string toString() const{
        std::string output;
        Rational out = *this;
        out.reduce();
        output += out.nm.toString();
        if (out.dnm != 1)
            output += "/" + out.dnm.toString();
        return output;
    }

    std::string asDecimal (size_t precision = 0) const{
        std::string output;
        BigInteger new_nm = nm;
        new_nm = (nm/dnm)*((nm >= 0)?(1):(-1));
        if (nm < 0) output += '-';
        output += new_nm.toString();
        if (precision > 0) output += ".";
        new_nm = nm * ((nm >= 0)?(1):(-1));
        for (size_t i = 0; i < precision; ++i) {
            new_nm *= 10;
            BigInteger tmp = new_nm/dnm;
            std::string tmp_str = tmp.toString();
            output += tmp_str.back();
        }
        return output;
    }

    explicit operator double() const{
        BigInteger _n = nm;
        BigInteger _m = dnm;
        int _nm = (int)_n;
        int _dnm = (int)_m;
        double ans = (double)_nm/_dnm;
        return ans;
    }
    ~Rational() = default;
};


#endif //BIGINTEGER_H_LIBRARY_H


