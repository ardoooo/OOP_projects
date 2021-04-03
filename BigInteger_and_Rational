#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class BigInteger {
public:
	BigInteger();
	BigInteger(int x);
	~BigInteger() {};

	BigInteger& operator+=(const BigInteger& x);
	BigInteger& operator-=(const BigInteger& x);
	BigInteger& operator*=(const BigInteger& x);
	BigInteger& operator/=(const BigInteger& x);
	BigInteger& operator%=(const BigInteger& x);

	BigInteger operator-() const;

	BigInteger& operator++();
	BigInteger operator++(int);
	BigInteger& operator--();
	BigInteger operator--(int);

	string toString() const;
	explicit operator bool();

private:
	vector<short> numbers;
	bool sign;

	void norm();

	BigInteger& increase(const BigInteger&);
	BigInteger& decrease(const BigInteger&);

	friend bool operator==(const BigInteger& a, const BigInteger& b);

	friend bool operator<(const BigInteger& a, const BigInteger& b);
	friend istream& operator>>(std::istream& in, BigInteger& x);
	bool is_equal_no_sign(const BigInteger& x);
};

BigInteger operator+(const BigInteger& a, const BigInteger& b);
BigInteger operator-(const BigInteger& a, const BigInteger& b);
BigInteger operator*(const BigInteger& a, const BigInteger& b);
BigInteger operator/(const BigInteger& a, const BigInteger& b);
BigInteger operator%(const BigInteger& a, const BigInteger& b);

bool operator>(const BigInteger& a, const BigInteger& b);
bool operator!=(const BigInteger& a, const BigInteger& b);
bool operator<=(const BigInteger& a, const BigInteger& b);
bool operator>=(const BigInteger& a, const BigInteger& b);

std::ostream& operator<<(std::ostream& out, const BigInteger& x);
std::istream& operator>>(std::istream& in, BigInteger& x);

void BigInteger::norm() {
	while (numbers.size() > 1 && numbers.back() == 0) {
		numbers.pop_back();
	}
	if (numbers.empty()) numbers.push_back(0);
	if (numbers.size() == 1 && numbers.front() == 0) sign = true;

}
BigInteger::BigInteger() {
	numbers.push_back(1);
	numbers.push_back(0);
	sign = true;
}
BigInteger::BigInteger(int x) {
	sign = x >= 0;
	if (!sign) x *= -1;
	while (x > 0) {
		numbers.push_back(x % 10);
		x /= 10;
	}
	norm();
}
bool BigInteger::is_equal_no_sign(const BigInteger& x) {
	if (numbers.size() != x.numbers.size()) {
		return numbers.size() < x.numbers.size();
	}
	for (size_t i = numbers.size(); i > 0;) {
		--i;
		if (numbers[i] < x.numbers[i]) return true;
		if (numbers[i] > x.numbers[i]) return false;
	}
	return true;
}
BigInteger& BigInteger::increase(const BigInteger& x) {
	size_t min_lenght = numbers.size() < x.numbers.size() ? numbers.size() : x.numbers.size();
	size_t i;
	for (i = 0; i < min_lenght; ++i) {
		numbers[i] += x.numbers[i];
	}
	for (i = min_lenght; i < x.numbers.size(); ++i) {
		numbers.push_back(x.numbers[i]);
	}
	for (i = 0; i < numbers.size(); ++i) {
		if (numbers[i] > 9) {
			numbers[i] -= 10;
			if (i < numbers.size() - 1) {
				++numbers[i + 1];
			}
			else {
				numbers.push_back(1);
			}
		}
	}
	norm();
	return *this;
}
BigInteger& BigInteger::decrease(const BigInteger& x) {
	const BigInteger* a = this;
	const BigInteger* b = &x;

	if (is_equal_no_sign(x)) {
		sign ^= true;
		swap(a, b);
	}

	long long sum = 0;
	for (size_t i = 0; i < a->numbers.size(); ++i) {
		sum += a->numbers[i];
		if (i < b->numbers.size()) {
			sum -= b->numbers[i];
		}
		if (a == this && i >= b->numbers.size()) {
			if (sum == numbers[i])
				break;
		}
		long long next_sum = 0;
		if (sum < 0) {
			next_sum = (sum - 9) / 10;
			sum += 10;
		}
		if (numbers.size() <= i) numbers.push_back(0);
		numbers[i] = static_cast<unsigned int>(sum % 10);
		sum = next_sum;
	}

	norm();
	return *this;
}

BigInteger BigInteger::operator-() const {
	BigInteger neg_x = *this;
	neg_x.sign ^= true;
	neg_x.norm();
	return neg_x;
}
BigInteger& BigInteger::operator+=(const BigInteger& x) {
	return (sign == x.sign) ? increase(x) : decrease(x);
}
BigInteger& BigInteger::operator-=(const BigInteger& x) {
	return (sign == x.sign) ? decrease(x) : increase(x);
}
BigInteger& BigInteger::operator*=(const BigInteger& x) {
	vector<short> result;
	size_t length = 1 + numbers.size() + x.numbers.size();
	sign ^= !x.sign;
	result.resize(length);
	for (size_t i = 0; i < numbers.size(); i++) {
		for (size_t j = 0; j < x.numbers.size(); j++) {
			result[i + j] += numbers[i] * x.numbers[j];
		}
	}
	for (size_t i = 0; i < length - 1; i++) {
		result[i + 1] += result[i] / 10;
		result[i] %= 10;
	}

	numbers = result;
	norm();
	return *this;
}
BigInteger& BigInteger::operator/=(const BigInteger& x) {
	vector<short> result;
	BigInteger tmp = 0;
	for (size_t i = numbers.size(); i > 0;) {
		--i;
		tmp = tmp * 10 + numbers[i];
		short left = 0, right = 11;
		while (right - left > 1) {
			short middle = (left + right) / 2;
			if ((x * middle).is_equal_no_sign(tmp)) {
				left = middle;
			}
			else {
				right = middle;
			}
		}

		result.push_back(left);
		tmp.decrease(left * x);

	}

	for (size_t i = 0; i < result.size() / 2; ++i) {
		std::swap(result[i], result[result.size() - 1 - i]);
	}
	numbers = result;
	sign ^= !x.sign;
	norm();
	return *this;
}
BigInteger& BigInteger::operator%=(const BigInteger& x) {
	return *this -= *this / x * x;
}

BigInteger operator+(const BigInteger& a, const BigInteger& b) {
	BigInteger result = a;
	result += b;
	return result;
}
BigInteger operator-(const  BigInteger& a, const BigInteger& b) {
  BigInteger result = a;
	result -= b;
	return result;
}
BigInteger operator*(const BigInteger& a, const BigInteger& b) {
	BigInteger result = a;
	result *= b;
	return result;
}
BigInteger operator/(const BigInteger& a, const BigInteger& b) {
	BigInteger result = a;
	result /= b;
	return result;
}
BigInteger operator%(const BigInteger& a, const BigInteger& b) {
	BigInteger result = a;
	result %= b;
	return result;
}

BigInteger& BigInteger::operator++() {
	return *this += 1;
}
BigInteger BigInteger::operator++(int) {
	BigInteger copy = *this;
	++* this;
	return copy;
}
BigInteger& BigInteger::operator--() {
	return *this -= 1;
}
BigInteger BigInteger::operator--(int) {
	BigInteger copy = *this;
	--* this;
	return copy;
}

bool operator==(const BigInteger& a, const BigInteger& b) {
	if (a.numbers.size() != b.numbers.size()) return false;
	if (a.sign != b.sign) return false;
	for (size_t i = 0; i < a.numbers.size(); ++i) {
		if (a.numbers[i] != b.numbers[i]) return false;
	}
	return true;
}
bool operator<(const BigInteger& a, const BigInteger& b) {
	if (a.sign < b.sign) return true;
	if (a.sign > b.sign) return false;
	if (a.sign) {
		if (b.numbers.size() < a.numbers.size()) return false;
		if (b.numbers.size() > a.numbers.size()) return true;
	}
	else {
		if (b.numbers.size() > a.numbers.size()) return false;
		if (b.numbers.size() < a.numbers.size()) return true;
	}

	for (size_t i = a.numbers.size(); i > 0; --i) {
		if (a.sign) {
			if (a.numbers[i - 1] > b.numbers[i - 1]) return false;
			if (a.numbers[i - 1] < b.numbers[i - 1]) return true;
		}
		else {
			if (a.numbers[i - 1] < b.numbers[i - 1]) return false;
			if (a.numbers[i - 1] > b.numbers[i - 1]) return true;
		}
	}
	return false;
}
bool operator>(const BigInteger& a, const BigInteger& b) {
	return b < a;
}
bool operator!=(const BigInteger& a, const BigInteger& b) {
	return !(a == b);
}
bool operator<=(const BigInteger& a, const BigInteger& b) {
	return !(a > b);
}
bool operator>=(const BigInteger& a, const BigInteger& b) {
	return !(a < b);
}

BigInteger::operator bool() {
	return *this != BigInteger(0);
}
string BigInteger::toString() const {
	string s;
	if (!sign) s += '-';
	for (size_t i = numbers.size(); i >= 1; --i) {
		s += numbers[i - 1] + '0';
	}
	return s;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& x) {
	out << x.toString();
	return out;
}
std::istream& operator>>(std::istream& in, BigInteger& x) {
	string s;
	in >> s;
	x.sign = true;
	x.numbers.clear();
	if (s.size() > 0 && s[0] == '-') x.sign = false;
	for (size_t i = s.size(); i >= 1; --i) {
		if (s[i - 1] != '-')

			x.numbers.push_back(s[i - 1] - '0');
	}
	x.norm();
	return in;

	/*x = 0;
	x.sign = s.size() > 0 && s[0] != '-';
	for (size_t i = 0; i < s.size() / 2; ++i) {
		swap(s[i], s[s.size() - 1 - i]);
	}
	while (s.size() > 1 && s.back() == '0') {
		s.pop_back();
	}
	x.numbers = vector<short>(0);
	size_t power = 1;
	for (size_t i = 0; i < s.size() && s[i] != '-'; ++i) {
		x.numbers.push_back((static_cast<int>(s[i]) - 48));
	}
	x.norm();
	return in;*/
}


class Rational {
public:
	Rational();
	Rational(int x);
	Rational(const BigInteger& x);

	Rational operator-() const;
	Rational& operator+=(const Rational& x);
	Rational& operator-=(const Rational& x);
	Rational& operator*=(const Rational& x);
	Rational& operator/=(const Rational& x);

	string toString() const;
	string asDecimal(size_t precision) const;
	explicit operator double();

private:
	BigInteger numerator;
	BigInteger denominator;

	void norm();

	friend bool operator==(const Rational& a, const Rational& b);
	friend bool operator<=(const Rational& a, const Rational& b);
};

Rational operator+(const Rational& a, const Rational& b);
Rational operator-(const Rational& a, const Rational& b);
Rational operator*(const Rational& a, const Rational& b);
Rational operator/(const Rational& a, const Rational& b);

bool operator==(const Rational& a, const Rational& b);
bool operator!=(const Rational& a, const Rational& b);
bool operator<=(const Rational& a, const Rational& b);
bool operator>=(const Rational& a, const Rational& b);
bool operator<(const Rational& a, const Rational& b);
bool operator>(const Rational& a, const Rational& b);

std::ostream& operator<<(std::ostream& out, const Rational& x);

Rational::Rational() : numerator(0), denominator(1) {};
Rational::Rational(int x) : numerator(x), denominator(1) {};
Rational::Rational(const BigInteger& x) : numerator(x), denominator(1) {};

void Rational::norm() {
	BigInteger a = numerator;
	BigInteger b = denominator;
	if (a < 0) a *= -1;
	if (b < 0) b *= -1;
	while (b) {
		a %= b;
		std::swap(a, b);
	}
	if (denominator < 0) a *= -1;
	numerator /= a;
	denominator /= a;
}

Rational Rational::operator-() const {
	Rational x = *this;
	x.numerator *= -1;
	x.norm();
	return x;
}

Rational& Rational::operator+=(const Rational& x) {
	numerator = numerator * x.denominator + x.numerator * denominator;
	denominator *= x.denominator;
	norm();
	return *this;
}

Rational& Rational::operator-=(const Rational& x) {
	numerator = numerator * x.denominator - x.numerator * denominator;
	denominator *= x.denominator;
	norm();
	return *this;
}

Rational& Rational::operator*=(const Rational& x) {
	numerator *= x.numerator;
	denominator *= x.denominator;
	norm();
	return *this;
}

Rational& Rational::operator/=(const Rational& x) {
	numerator *= x.denominator;
	denominator *= x.numerator;
	norm();
	return *this;
}

Rational operator+(const Rational& a, const Rational& b) {
	Rational result = a;
	result += b;
	return result;
}

Rational operator-(const Rational& a, const Rational& b) {
	Rational result = a;
	result -= b;
	return result;
}

Rational operator*(const Rational& a, const Rational& b) {
	Rational result = a;
	result *= b;
	return result;
}

Rational operator/(const Rational& a, const Rational& b) {
	Rational result = a;
	result /= b;
	return result;
}

bool operator==(const Rational& a, const Rational& b) {
	return a.numerator == b.numerator && a.denominator == b.denominator;
}

bool operator!=(const Rational& a, const Rational& b) {
	return !(a == b);
}

bool operator<=(const Rational& a, const Rational& b) {
	return a.numerator * b.denominator <= b.numerator * a.denominator;
}

bool operator>=(const Rational& a, const Rational& b) {
	return b <= a;
}

bool operator<(const Rational& a, const Rational& b) {
	return a <= b && a != b;
}

bool operator>(const Rational& a, const Rational& b) {
	return b < a;
}

string Rational::toString() const {
	string result = numerator.toString();
	if (denominator != 1) result += '/' + denominator.toString();
	return result;
}
string Rational::asDecimal(size_t precision = 0) const {
	string temp_help;
	BigInteger power = 1;
	for (size_t i = 0; i < precision; ++i) {
		power *= 10;
	}
	temp_help = (numerator * power / denominator).toString();
	string result;
	for (size_t i = 0; i <= precision || i < temp_help.size(); ++i) {

		if (i < temp_help.size()) {
			if (temp_help[temp_help.size() - 1 - i] != '-')
				result += temp_help[temp_help.size() - 1 - i];
			else result += '0';
		}
		else {
			result += '0';
		}
		if (i + 1 == precision) {
			result += '.';
		}
	}
	if (numerator < 0) {
		result += '-';
	}
	for (size_t i = 0; i < result.size() / 2; ++i) {
		swap(result[i], result[result.size() - i - 1]);
	}
	return result;
}

Rational::operator double() {
	string numbers = asDecimal(16);
	for (size_t i = 0; i < numbers.size() / 2; ++i) {
		swap(numbers[i], numbers[numbers.size() - i - 1]);
	}
	double result = 0;
	size_t power = 1;
	size_t i = 0;
	while (numbers[++i] != '.') {
		result += power * static_cast<short>(numbers[i - 1]) - 48;
		power *= 10;
	}
	power = 1;
	for (size_t j = i; j < 16 + i; ++j) {
		power /= 10;
		result += power * (static_cast<short>(numbers[j]) - 48);
	}
	return result;
}

std::ostream& operator<<(std::ostream& out, const Rational& x) {
	out << x.toString();
	return out;
}
