#pragma once
#include <iostream>
#include <cstring>


class String {
public:
	String() = default;

	String(const char* s) {
		sz = strlen(s);
		upt_capacity();
		str = new char[capacity];
		memcpy(str, s, sz);
	}

	String(size_t sz, char c) {
		this->sz = sz;
		upt_capacity();
		str = new char[capacity];
		memset(str, c, sz);
	}

	String(const char s) {
		sz = 1;
		upt_capacity();
		str = new char[capacity];
		*str = s;
	}

	String(const String& s) {
		sz = s.sz;
		upt_capacity();
		str = new char[capacity];
		memcpy(str, s.str, sz);
	}

	String& operator=(String s) {
		swap(s);
		return *this;
	}

	~String() {
		delete[] str;
	}

	size_t length() const {
		return sz;
	}

	bool empty() const {
		return sz == 0;
	}

	const char& front() const {
		return str[0];
	}

	const char& back() const {
		return str[sz - 1];
	}

	char& front() {
		return str[0];
	}

	char& back() {
		return str[sz - 1];
	}

	void push_back(char p) {
		if (sz == 0) {
			sz = 1;
			str = new char[capacity];
			str[0] = p;
		}
		else {
			str[sz] = p;
			++sz;
			if (sz == capacity) {
				capacity *= 2;
				char* temporary = new char[capacity];
				memcpy(temporary, str, sz);
				char* old = str;
				str = temporary;
				delete[] old;
			}
		}
	}
	void pop_back() {
		--sz;
		if (sz <= capacity / 4) {
			capacity /= 2;
			char* temporary = new char[capacity];
			memcpy(temporary, str, sz);
			char* old = str;
			str = temporary;
			delete[] old;
		}
	}
	void clear() {
		sz = 0;
		capacity = 2;
		delete[] str;
		str = new char[capacity];
	}
	String& operator+=(const String& s) {
		for (size_t i = 0; i < s.sz; ++i) {
			this->push_back(s[i]);
		}
		return *this;
	}
	friend bool operator==(const String& a, const String& b) {
		return strcmp(a.str, b.str) == 0;
	}
	char& operator[](size_t index) {
		return str[index];
	}

	const char& operator[](size_t index) const {
		return str[index];
	}
	String substr(size_t start, size_t count) const {
		String substr;
		for (size_t i = start; i < start + count; ++i) {
			substr.push_back(str[i]);
		}
		return substr;
	}

	size_t find(const String& sub) const {
		for (size_t i = 0; i < sz - sub.length() + 1; ++i) {
			if (memcmp(str + i, sub.str, sub.length()) == 0) {
				return i;
			}
		}
		return sz;
	}
	size_t rfind(const String& sub) const {
		for (int i = sz - sub.length(); i >= 0; --i) {
			if (memcmp(str + i, sub.str, sub.length()) == 0) {
				return i;
			}
		}
		return sz;
	}

private:
	size_t sz = 0;
	size_t capacity = 2;
	char* str = nullptr;
	void upt_capacity() {
		size_t tmp = 4;
		while (tmp <= sz) {
			tmp *= 2;
		}
		capacity = tmp * 2;
	}
	void swap(String& s) {
		std::swap(sz, s.sz);
		std::swap(capacity, s.capacity);
		std::swap(str, s.str);
	}
};

std::ostream& operator<<(std::ostream& out, const String& s) {
	for (size_t i = 0; i < s.length(); ++i) {
		out << s[i];
	}
	return out;
}

std::istream& operator>>(std::istream& in, String& str) {
	char c;
	str.clear();
	do {
		c = in.get();
	} while (isspace(c));
	while (!in.eof() && !isspace(c)) {
		str.push_back(c);
		c = in.get();
	}
	return in;
}

String operator+(const String& a, const String& b) {
	String copy = a;
	copy += b;
	return copy;
}
