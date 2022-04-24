//
// Created by Ilya on 12.12.2020.
//

#ifndef MATRIX_MATRIX_H
#define MATRIX_MATRIX_H

#include <vector>
#include <cstring>
#include <string>
const static int base = 1000000000;

class BigInteger {
private:
    std::vector <int> num;

    void removeZero() {
        while (num.size() > 1 && num.back() == 0) {
            num.pop_back();
        }
    }

    friend bool Compare(const BigInteger &left, const BigInteger &right) {
        bool res = 0;
        if (left.num.size() != right.num.size()) {
            res = left.num.size() < right.num.size() ? (0) : (1);
        } else {
            for (size_t i = 0; i < left.num.size(); i++) {
                if (left.num[i] != right.num[i]) {
                    res = (left.num[i] < right.num[i]) ? (0) : (1);
                }
            }
        }
        return res;
    }

    void plus(unsigned long long add, size_t p) {
        if (add >= base) {
            plus(add % base, p);
            plus(add / base, p + 1);
        } else if (add > 0 && add < base) {
            for (size_t i = num.size(); i < p + 1; ++i) {
                num.push_back(0);
            }
            add += num[p];
            num[p] = add % base;
            plus(add / base, p + 1);
        }
    }

    void minus(long long substract, size_t p) {
        if (substract >= base) {
            minus(substract / base, p + 1);
            minus(substract % base, p);
        } else if (substract < base && substract > 0) {
            if (substract < num[p]) {
                num[p] -= substract;
            } else if (substract > num[p]) {
                minus(1, p + 1);
                num[p] += base - substract;
            } else {
                if (num.size() == p + 1 && num.size() > 1) {
                    num.pop_back();
                    while (num.size() > 1 && num.back() == 0) {
                        num.pop_back();
                        removeZero();
                    }
                } else {
                    num[p] = 0;
                }
            }
        }
    }

public:
    short sign;
    BigInteger() {
        num.push_back(0);
        sign = 1;
    };

    BigInteger(const int& value) {
        sign = (value >= 0) ? (1) : (-1);
        int val_cpy = abs(value);
        if (val_cpy < base){
            num.push_back(val_cpy);
        } else {
            num.push_back(val_cpy % base);
            num.push_back(val_cpy / base);
        }
    }

    void fastDiv() {
        num[0] /= 2;
        for (size_t i = 1; i < num.size(); ++i) {
            if (num[i] % 2 == 1) {
                num[i] /= 2;
                plus(base/2, i - 1);
            } else {
                num[i] /= 2;
            }
        }
        removeZero();
    }

    void fastMult() {
        size_t N = num.size();
        for (int i = N - 1; i >= 0; --i) {
            plus(num[i], i);
        }
    }

    bool isDiv() const {
        return !(num[0] % 2);
    }

    BigInteger& operator=(const BigInteger& b) {
        this->num = b.num;
        this->sign = b.sign;
        return *this;
    }

    BigInteger& operator+=(const BigInteger& b) {
        if (this->sign == b.sign) {
            for (size_t i = 0; i < b.num.size(); ++i) {
                plus(b.num[i], i);
            }
        } else {
            if (Compare(*this, b)) {
                for (size_t i = 0; i < b.num.size(); ++i) {
                    minus(b.num[i], i);
                }
            } else if (*this == -b) {
                this->num.clear();
                this->num.push_back(0);
                this->sign = 1;
            } else {
                BigInteger ans = b;
                for (size_t i = 0; i < num.size(); ++i) {
                    ans.minus(num[i], i);
                }
                *this = ans;
            }
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

    BigInteger& operator*=(const BigInteger& b) {
        BigInteger result=0;
        unsigned long long tmp;
        for (size_t i = 0; i < num.size(); ++i) {
            for (size_t j = 0; j < b.num.size(); ++j) {
                tmp = num[i];
                tmp *= b.num[j];
                result.plus(tmp, i + j);
            }
        }
        this->num = result.num;
        sign *= b.sign;
        if (this->num.size() == 1 && this->num[0] == 0){
            sign = 1;
        }
        return *this;
    }

    BigInteger& operator /=(const BigInteger& b) {
        BigInteger result;
        if (b == 0) {
            *this = result;
            return *this;
        }
        if (Compare(b, *this)) {
            return *this = BigInteger(0);
        }
        BigInteger tmp = *this;
        size_t difference = this->num.size() - b.num.size() + 1;
        BigInteger pos = 0;
        BigInteger copy = b;
        result = 0;
        pos.plus(1, difference);
        copy *= pos;
        for (size_t i = 0; i < difference; ++i) {
            for (size_t j = 0; j < 9; ++j) {
                while (!Compare(copy, *this)) {
                    for (size_t i = 0; i < copy.num.size(); ++i) {
                        minus(copy.num[i], i);
                    }
                    result += pos;
                }
                copy.num[0] /= 10;
                for (size_t k = 1; k < copy.num.size(); ++k) {
                    copy.plus((copy.num[k] % 10) * (base / 10), k - 1);
                    copy.num[k] /= 10;
                }
                copy.removeZero();
                pos.num[0] /= 10;
                for (size_t k = 1; k < pos.num.size(); ++k) {
                    pos.plus((pos.num[k] % 10) * (base / 10), k - 1);
                    pos.num[k] /= 10;
                }
                pos.removeZero();;
            }
        }
        while (!Compare(copy, *this)) {
            for (size_t i = 0; i < copy.num.size(); ++i) {
                minus(copy.num[i], i);
            }
            result += pos;
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
        if (num.size() == 0) {
            return output;
        }
        if (num.size() == 1 && num.back() == 0) {
            output.push_back('0');
            return output;
        }
        int n;
        for (size_t i = 0; i < num.size(); ++i) {
            n = num[i];
            if (i != num.size()-1) {
                for (int j = 0; j < 9; ++j) {
                    output.push_back(n % 10 + 48);
                    n /= 10;
                }
            } else {
                while (n > 0) {
                    output.push_back(n % 10 + 48);
                    n /= 10;
                }
            }
        }
        if (sign == -1) {
            output.push_back('-');
        }
        char c;
        size_t l = output.size();
        for (size_t t = 0; t < (l / 2); ++t) {
            c = output[t];
            output[t] = output[l - t - 1];
            output[l - t - 1] = c;
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

    friend std::istream& operator >> (std::istream &in, BigInteger &b) {
        std::string input;
        in >> input;
        size_t it = 0;
        size_t size = input.size();
        char tmp;
        for (size_t t = 0; t < (size / 2); ++t) {
            tmp = input[t];
            input[t] = input[size - t - 1];
            input[size - t - 1] = tmp;
        }
        if (input[size - 1] == '-') {
            b.sign = -1;
            --size;
        } else {
            b.sign = 1;
        }
        b.num.clear();
        b.num.push_back(0);
        while (it < size) {
            int ord = 1;
            for (size_t i = 0; i < it % 9; i++) {
                ord *= 10;
            }
            b.plus((input[it] - '0') * ord,(it / 9));
            ++it;
        }
        if (b.sign == -1 && b.num.size() == 1 && b.num[0] == 0) {
            b.sign = 1;
        }
        return in;
    }

    ~BigInteger() = default;
};

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

    BigInteger gcd(BigInteger& a, BigInteger& b) { /// Быстрый нод (бинарный)
        if (a == 0 || b == 1) {
            return b;
        }
        if (a == 1 || b == 0 || a == b ) {
            return a;
        }
        if (a.isDiv()) {
            a.fastDiv();
            if (b.isDiv()) {
                b.fastDiv();
                BigInteger ans = gcd(a, b);
                ans.fastMult();
                return  ans;
            } else {
                return gcd(a,b);
            }
        } else {
            if (b.isDiv()) {
                b.fastDiv();
                return gcd(a, b);
            } else {
                if (a > b) {
                    a -= b;
                    a.fastDiv();
                    return gcd(a, b);
                } else {
                    b -= a;
                    b.fastDiv();
                    return gcd(a, b);
                }
            }
        }
    }

    void reduce() {
        BigInteger n = nm;
        BigInteger m = dnm;
        n.sign = 1;
        BigInteger z;
        z = gcd(n, m);
        z.sign = 1;
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
        if ((nm == 1 || nm == -1) && dnm == 1) {
            nm = b.dnm * ((b.nm >= 0)?(1) : (-1));
            dnm = b.nm * ((b.nm >= 0)?(1) : (-1));
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

    std::string asDecimal (size_t precision = 0) const {
        std::string output;
        BigInteger new_nm = nm;
        new_nm = (nm/dnm);
        new_nm.sign = 1;
        if (nm < 0) output += '-';
        output += new_nm.toString();
        if (precision > 0) {
            output += ".";
            new_nm = nm;
            new_nm.sign = 1;
        }
        for (size_t i = 0; i < precision; ++i) {
            new_nm *= 10;
            BigInteger tmp = new_nm/dnm;
            std::string tmp_str = tmp.toString();
            output += tmp_str.back();
        }
        return output;
    }

    explicit operator double() const{
        if (nm == 0) {
            return 0;
        }
        std::string temp;
        temp = asDecimal(18);
        int i,l ,k;
        l = temp.length();
        k = 1;
        double r ,p;
        r = 0;
        p = 1;
        int j = (nm.sign == 1) ? 0 : 1;
        for (i=j; i< l; i++) {
            if (k < 0) {
                p = p * 10;
            }
            if (temp[i] == '.') {
                k = -k;
            } else {
                r = r * 10 + (temp[i] - '0');
            }
        }
        return (r / p * nm.sign);
    }

    friend std::istream &operator>>(std::istream &in, Rational &r) {
        in >> r.nm;
        r.dnm = 1;
        return in;
    }

    ~Rational() = default;
};



template <int N>
class Finite { /// Класс конечного поля
private:
    unsigned long long value;
public:
    Finite() {
        value  = 0;
    }
    Finite(const int& x) {
        if (x >= 0) {
            value = x % N;
        } else {
            value = N + x % N;
        }
    }

    Finite<N> binaryPower(Finite<N> v, unsigned p) { /// Нахождение обратного числа
        Finite<N> result = 1;
        while (p > 0) {
            if (p & 1) {
                result *= v;
            }
            v *= v;
            p >>= 1;
        }
        return result;
    }

    Finite<N> operator=(const Finite<N>& f) {
        value = f.value;
        return *this;
    }

    Finite<N> operator-() const{
        Finite<N> copy = *this;
        copy.value = N - value;
        return copy;
    }

    Finite<N>& operator+= (const Finite<N>& another) {
        value = (value + another.value) % N;
        return *this;
    }

    Finite<N>& operator++(){
        *this += 1;
        return *this;
    }

    Finite<N> operator++(int) {
        Finite<N> copy = *this;
        ++(*this);
        return copy;
    }

    Finite<N>& operator-=(const Finite<N>& another) {
        *this += -another;
        return *this;
    }

    Finite<N>& operator --(){
        *this -= 1;
        return *this;
    }

    Finite<N> operator--(int) {
        Finite<N> copy = *this;
        --(*this);
        return copy;
    }


    Finite<N>& operator*=(const Finite<N>& another) {
        value = (value * another.value) % N;
        return *this;
    }

    Finite<N>& operator/=(const Finite<N>& another){
        *this *= binaryPower(another, N - 2);
        return *this;
    }

    friend Finite<N> operator+(const Finite<N>& left, const Finite<N>& right) {
        Finite<N> _left = left;
        _left += right;
        return _left;
    }

    friend Finite<N> operator*(const Finite<N>& left, const Finite<N>& right) {
        Finite<N> _left = left;
        _left *= right;
        return _left;
    }

    friend Finite<N> operator-(const Finite<N>& left, const Finite<N>& right) {
        Finite<N> _left = left;
        _left -= right;
        return _left;
    }
    friend Finite<N> operator/(const Finite<N>& left, const Finite<N>& right) {
        Finite<N> _left = left;
        _left /= right;
        return _left;
    }

    friend std::ostream &operator<<(std::ostream &out, const Finite<N> &finite_num) {
        out << finite_num.value;
        return out;
    }

    bool operator==(const Finite<N>& another) const {
        return (value == another.value);
    }

    bool operator!=(const Finite<N>& another) const {
        return (value != another.value);
    }


    ~Finite() = default;
};




template <unsigned M, unsigned N, typename Field = Rational>
class Matrix{
protected:
    Matrix& operator =(const Matrix& m){
        matrix = m.matrix;
        return *this;
    }
public:

    void gaussConvert(size_t& n, unsigned M_, unsigned N_){ /// Метод гаусса для создания треугольной мтарицы с нулями внизу
        unsigned _N = std::min(M, N);
        for (unsigned i = 0; i < _N - 1; ++i) {
            if (this->matrix[i][i] == 0) {
                for (unsigned y = i; y < M_; ++y) {
                    if (this->matrix[y][i] != 0) {
                        std::swap(this->matrix[y], this->matrix[i]);
                        n *= -1;
                        break;
                    }
                }
            }
            if (this->matrix[i][i] == 0) {
                n = 0;
                continue;
            }
            for (unsigned j = i + 1; j < M_; ++j) {
                if (this->matrix[j][i] != 0) {
                    Field t = -this->matrix[j][i] / this->matrix[i][i];
                    for (unsigned k = i; k < N_; ++k) {
                        this->matrix[j][k] += this->matrix[i][k] * t;
                    }
                }
            }
        }
    }

    void gaussBackConvert(unsigned M_, unsigned N_) { /// Доведение предыдущего метода для доведения матрицы до единичной
        for (int i = M_ - 1; i >= 0; --i) {
            if (matrix[i][i] == 0) {
                continue;
            }
            for (int j = N_ - 1; j > i; --j) {
                matrix[i][j] /= matrix[i][i];
            }
            matrix[i][i] = 1;
            for (int j = i - 1; j >= 0; --j) {
                if (matrix[j][i] == 0) {
                    continue;
                }
                Field t = -matrix[j][i];
                for (unsigned k = i; k < N_; ++k) {
                    matrix[j][k] += matrix[i][k] * t;
                }
            }
        }
    }


    std::vector< std::vector<Field>> matrix;
    Matrix() {
        matrix.resize(M);
        for (unsigned i = 0; i < M; ++i){
            matrix[i].resize(N);
        }
        if (M == N) {
            for (unsigned i = 0; i < M; ++i) {
                for (unsigned j = 0; j < N; ++j) {
                    matrix[i][j] = (i == j);
                }
            }
        }
    }

    template<typename T>
    Matrix(std::vector <std::vector<T>> another_v) {
        for (unsigned i = 0; i < M; ++i) {
            std::vector<Field> temporary;
            for (unsigned j = 0; j < N; ++j) {
                Field temp (another_v[i][j]);
                temporary.push_back(temp);
            }
            matrix.push_back(temporary);
        }
    }
    template<unsigned M_, unsigned N_>
    bool operator==(const Matrix<M_, N_, Field>& m1) const {
        if (M_ != M || N_ != N) {
            return 0;
        } else {
            return matrix == m1.matrix;
        }
    }
    template<unsigned M_, unsigned N_>
    bool operator!=(const Matrix<M_, N_, Field>& m1) const {
        return !(*this == m1);
    }

    Matrix<M, N, Field>& operator += (const Matrix<M,N, Field>& another) {
        for (unsigned i = 0; i < M; ++i) {
            for (unsigned j = 0; j < N; ++j) {
                matrix[i][j] += another.matrix[i][j];
            }
        }
        return *this;
    }

    Matrix<M, N, Field>& operator -= (const Matrix<M,N, Field>& another) {
        for (unsigned i = 0; i < M; ++i) {
            for (unsigned j = 0; j < N; ++j) {
                matrix[i][j] -= another.matrix[i][j];
            }
        }
        return *this;
    }


    Matrix<M, N, Field>& operator *=(const Field& value) {
        for (unsigned i = 0; i < M; ++i) {
            for (unsigned j = 0; j < N; ++j) {
                matrix[i][j] *= value;
            }
        }
        return *this;
    }

    std::vector<Field>& operator [](const size_t& i) {
        return matrix[i];
    }

    std::vector<Field> operator [](const size_t& i) const {
        return matrix[i];
    }

    std::vector<Field> getRow(const unsigned& i) const {
        return matrix[i];
    }

    std::vector<Field> getColumn(const unsigned& i) const {
        std::vector<Field> temp;
        for (unsigned k = 0; k < M; ++k) {
            temp.push_back(matrix[k][i]);
        }
        return temp;
    }

    Matrix<N, M, Field> transposed() const {
        Matrix<N, M, Field> result;
        for (unsigned i = 0; i < N; ++i) {
            for (unsigned j = 0; j < M; ++j) {
                result[i][j] = matrix[j][i];
            }
        }
        return result;
    }

    unsigned int rank() const {
        Matrix<M,N,Field> copy = *this;
        size_t n = 0;
        unsigned ans = 0;
        copy.gaussConvert(n,M, N);
        bool f = true;
        for (unsigned i = 0; i < M; ++i) {
            for (unsigned j = 0; j <  N; ++j) {
                if (copy[i][j] != 0) {
                    f = false;
                    break;
                }
            }
            if (f) {
                break;
            } else {
                ++ans;
                f = true;
            }
        }
        return ans;
    }

    Field det() const{
        static_assert(N == M);
        size_t n = 1;
        Field ans = 1;
        Matrix<M, N, Field> copy = *this;
        copy.gaussConvert(n, M, N);
        ans *= n;
        if (ans != 0) {
            for (unsigned i = 0; i < N; ++i) {
                ans *= copy[i][i];
            }
        }
        return ans;
    }

    void invert() {  /// Расчет обратной матрицы методом Гаусса-Иордана
        static_assert (N == M);
        Matrix<N, N, Field> copy = *this;
        for (unsigned i = 0; i < M; ++i){
            for (unsigned j = 0; j < N; ++j) {
                matrix[i].push_back(0);
            }
        }
        for (unsigned i = 0; i < N; ++i) {
             matrix[i][i + N] = 1;
        }
        size_t n = 1;
        unsigned Z = 2 * N;
        gaussConvert(n, N, Z);
        if (n != 0) {
            gaussBackConvert(N, Z);
            for (unsigned i = 0; i < N; ++i) {
                for (unsigned j = 0; j < N; ++j) {
                    copy.matrix[i][j] = matrix[i][j + N];
                }
            }
        }
        matrix = copy.matrix;
    }

    Matrix<M, N, Field> inverted() const {
        Matrix<M,N,Field> copy = *this;
        copy.invert();
        return copy;
    }

    Field trace() const {
        static_assert(N == M);
        Field sum = 0;
        for (unsigned i = 0; i < N; ++i) {
            sum += this->matrix[i][i];
        }
        return sum;
    }

};
template <unsigned M, unsigned N, typename Field = Rational>
Matrix<M, N, Field> operator +(const Matrix<M, N, Field>& left, const Matrix<M, N, Field>& right) {
    Matrix<M, N, Field> _left = left;
    _left += right;
    return _left;
}
template <unsigned M, unsigned N, typename Field = Rational>
Matrix<M, N, Field> operator -(const Matrix<M, N, Field>& left, const Matrix<M, N, Field>& right) {
    Matrix<M, N, Field> _left = left;
    _left -= right;
    return _left;
}
template <unsigned M, unsigned N, typename Field = Rational>
Matrix<M, N, Field> operator* (const Matrix<M, N, Field>& matrix_, const Field& value) {
    Matrix <M, N, Field> _matrix = matrix_;
    _matrix *= value;
    return _matrix;
}
template <unsigned M, unsigned N, typename Field = Rational>
Matrix<M, N, Field> operator* (const Field& value, const Matrix<M, N, Field>& matrix_) {
    Matrix <M, N, Field> _matrix = matrix_;
    _matrix *= value;
    return _matrix;
}

template <unsigned M, unsigned N, unsigned K, typename Field = Rational>
Matrix<M, N, Field> operator * (const Matrix<M, K, Field>& m1 , const Matrix<K, N, Field>& m2) {
    Matrix<M, N, Field> result;
    for (unsigned i = 0; i < M; ++i) {
        for (unsigned j = 0; j < N; ++j) {
            Field sum;
            for (unsigned k = 0; k < K; ++k) {
                sum += m1.matrix[i][k] * m2.matrix[k][j];
            }
            result.matrix[i][j] = sum;
            sum = 0;
        }
    }
    return result;
}


template<unsigned N, typename Field = Rational>
class SquareMatrix: public Matrix<N, N, Field> {
public:
    SquareMatrix(Matrix<N, N, Field> m) {
        this->matrix = m.matrix;
    }
    SquareMatrix() : Matrix<N, N, Field>() {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                this->matrix[i][j] =( (i == j) ? (1) : (0));
            }
        }
    }
    template<typename T>
    SquareMatrix(std::vector<std::vector<T>> another_v): Matrix<N, N, Field> (another_v) {}
    template<unsigned M>
    SquareMatrix<N>& operator *=(const Matrix<M, N>& another){
        static_assert(N == M);
        *this = *this * another;
        return *this;
    }

    explicit operator Matrix<N,N,Field>() const {
        Matrix<N, N, Field> copy;
        copy.matrix = this->matrix;
        return copy;
    }

    SquareMatrix<N> transposed() const {
        SquareMatrix<N, Field> result;
        for (unsigned i = 0; i < N; ++i) {
            for (unsigned j = 0; j < N; ++j) {
                result[i][j] = this->matrix[j][i];
            }
        }
        return result;
    }
};





#endif //MATRIX_MATRIX_H
