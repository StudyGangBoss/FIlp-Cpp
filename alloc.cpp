// TextMap.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <stdio.h>
#include <tchar.h>
//end
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <string>
using namespace std;

template<class T>
struct LinkedList {
	T* a;
	LinkedList<T>* prev;

	LinkedList() {
		prev = nullptr;
	}

	~LinkedList() {
		delete a;
		if (prev != nullptr)
			delete prev;
	}

	LinkedList(const LinkedList& x) = delete;
	LinkedList& operator=(const LinkedList& x) {
		this->prev = x->prev;
		this->a = x->a;
		return *this;
	}

	LinkedList(T* value) {
		a = value;
		prev = nullptr;
	}

	LinkedList<T>* push_back(T* value) {
		LinkedList<T>* nv = new LinkedList<T>(value);
		nv->prev = this;
		return nv;
	}
};

class CStringComparator {
public:
	bool operator()(const char* A, const char* B) const {
		while (true) {
			if (A[0] == B[0]) {
				//A = B
				if (!A[0])
					return false;
				A++, B++;
			}
			else
				return A[0] < B[0];
		}
	}
};

const size_t block_size = 1024 * 1024;

class CMyBuffer {
private:
	struct block {
		char* buffer = nullptr;
		size_t ptr;
		size_t size;
		block(size_t sz = block_size) {
			buffer = new char[sz];
			ptr = 0;
			size = sz;
		}

		~block() {
			if (buffer != nullptr)
				delete[] buffer;
		}
	};
	LinkedList<block>* current;

public:
	CMyBuffer(size_t size = block_size) {
		current = new LinkedList<block>(new block());
	}

	~CMyBuffer() {
		delete current;
	}

	void* alloc(size_t n, size_t type_size) {
		if (current->a->ptr + type_size * n > current->a->size)
			current = current->push_back(new block());
		void* ptr = current->a->buffer + current->a->ptr;
		current->a->ptr += type_size * n;
		return ptr;
	}
};

template <class T>
class CMyAllocator
{
public:
	typedef T value_type;

	CMyBuffer* cur_buffer = nullptr;

	CMyAllocator() {
		if (cur_buffer == nullptr)
			cur_buffer = new CMyBuffer();
	}

	template <class U>
	CMyAllocator(const CMyAllocator<U>& V)
	{
		cur_buffer = V.cur_buffer;
	}

	T* allocate(size_t Count)
	{
		printf("Allocate %d\n", (int)(Count * sizeof(T)));

		return (T*)cur_buffer->alloc(Count, sizeof(T));
	}

	void deallocate(T* V, size_t Count)
	{
	}
};

void CustomTextMapTest(char* s, bool output = 0) {
	map<char*, size_t, CStringComparator, CMyAllocator< char*>> Map;

	int index = 0;
	while (s[index] != '\0') {
		if (!(s[index] >= 'a' && s[index] <= 'z' || s[index] >= 'A' && s[index] <= 'Z')) {
			s[index] = '\0';
			if (index > 1)
				Map[s]++;
			s += index + 1;
			index = 0;
			continue;
		}
		index++;
	}

	if (output) {
		map<int, set<char*>> freq;
		for (auto i : Map) {
			transform(i.first, i.first + strlen(i.first), i.first, ::tolower);
			i.first[0] = toupper(i.first[0]);
			freq[i.second].insert(i.first);
		}
		for (auto i : freq) {
			cout << i.first << ":";
			for (auto j : i.second)
				cout << ' ' << j;
			cout << "\n\n";
		}
	}
}

void StandardTextMapTest(string s) {
	map<string, size_t> Map2;
	string Word;
	s += ' ';
	for (auto i : s) {
		if (i >= 'A' && i <= 'Z' || i >= 'a' && i <= 'z')
			Word += i;
		else if (!Word.empty())
			Map2[Word]++, Word.clear();
	}
}

#include <windows.h>

ULONGLONG GetCurrentTimeMs()
{
	SYSTEMTIME S = { 0 };
	FILETIME F = { 0 };
	GetSystemTime(&S);
	SystemTimeToFileTime(&S, &F);
	LARGE_INTEGER Int;
	Int.HighPart = F.dwHighDateTime;
	Int.LowPart = F.dwLowDateTime;
	return Int.QuadPart / 10000;
}

void Testing(char* Text) {
	ULONGLONG Start, End;

	char* TextCopy = (char*)malloc(strlen(Text) + 1);
	strcpy(TextCopy, Text);
	string TextString = Text;

	Start = GetCurrentTimeMs();
	CustomTextMapTest(TextCopy);
	End = GetCurrentTimeMs();
	cout << "Custom realization time: " << End - Start << '\n';
	delete[] TextCopy;

	Start = GetCurrentTimeMs();
	StandardTextMapTest(TextString);
	End = GetCurrentTimeMs();
	cout << "Default time: " << End - Start << '\n';
}

int main() {
	
	HANDLE H = CreateFileW(L"F:\\Programming\\Filp\\Readme.txt",
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (H == INVALID_HANDLE_VALUE)
		return cout << "uff", 0;

	LARGE_INTEGER L;
	if (!GetFileSizeEx(H, &L))
		return cout << "size is empty", 0;
	cout << "Current file size: " << L.QuadPart << '\n';

	char* Text = new char[L.QuadPart + 1];
	DWORD NumberOfBytesRead;
	if (!ReadFile(H, Text, L.QuadPart, &NumberOfBytesRead, NULL))
		return cout << "uff", 0;
	cout << NumberOfBytesRead << " bytes readed\n";

	Testing(Text);
	cout << "Enter to show word frequency\n";
	getchar();

	CustomTextMapTest(Text, 1);
}