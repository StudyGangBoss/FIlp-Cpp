#include <iostream>
#include <map>
#include <vector>

using namespace std;

class CStringComparator {
public:
	bool operator()(const char* A, const char* B) const {
		while (true) {
			if (A[0] == B[0]) {
				if (!A[0])
					return false;

				A++;
				B++;
			}
			else {
				return A[0] < B[0];
			}
		}
	}
};

const int sz = 20;

class NewAllocator {
#pragma pack(push, 1)
	struct Block {
		unsigned char level = 0;
		bool free = 1;

		union {
			struct {
				Block* next, * prev;
			};
			char* data;
		};
	};

	Block* block;
#pragma pack(pop)

	struct List {
		Block* first = nullptr;

		void add(Block* newblock) {
			if (first == nullptr) {
				first = newblock;
				first->prev = first;
				first->next = first;
			}
			Block* prev = first->prev;
			prev->next = newblock;
			first->prev = newblock;
			newblock->prev = prev;
			newblock->next = first;
		}

		void del(Block* cur) {
			Block* prev = cur->prev; 
			Block* next = cur->next;
			if (cur->prev == cur)
				first = nullptr;
			else {
				prev->next = next;
				next->prev = prev;
				if (cur == first)
					first = prev;
			}
		}
	};

	enum {
		levels = sizeof(void*) * 8
	};

	List BlockTree[levels];

public:
	void* allocate(size_t sz) {
		sz += sizeof(unsigned char) + sizeof(bool);

		for (int i = 0; i < levels; ++i) {
			int lvlsize = (1 << i);
			if (lvlsize >= sz && BlockTree[i].first) {
				Block* cur = BlockTree[i].first;
				BlockTree[i].del(cur);
				int j = i - 1;
				lvlsize = lvlsize / 2;
				while (sz <= lvlsize) {
					Block* newBlock = (Block*)((char*)cur + lvlsize);
					cur->level -= 1;
					newBlock->free = 1;
					newBlock->level = cur->level;
					BlockTree[j].add(newBlock);
					j--;
					lvlsize = lvlsize / 2;
				}
				cur->free = 0;
				return &(cur->data);
			}
		}

		throw bad_alloc();
	}

	void deallocate(void* dlc) {
		Block* curBlock = (Block*)(((char*)dlc) - (sizeof(bool) + sizeof(unsigned char)));

		printf("deallocate memory on ptr %lld\n", (long long)dlc);
		bool exit = 0;
		while (!exit) {
			size_t blockSize = (1 << curBlock->level),
				shift = (char*)curBlock - (char*)block,
				n = shift / blockSize;
			exit = 1;
			if (n % 2 == 0) {
				Block* twin = (Block*)((char*)curBlock + blockSize);
				if (twin->level == curBlock->level && twin->free) {
					BlockTree[twin->level].del(twin);
					curBlock->level++;
					exit = 0;
				}
			}
			else {
				Block* twin = (Block*)((char*)curBlock - blockSize);
				if (twin->level == curBlock->level && twin->free) {
					BlockTree[twin->level].del(twin);
					curBlock = twin;
					curBlock->level++;
					exit = 0;
				}
			}
		}
		curBlock->free = true;
		BlockTree[curBlock->level].add(curBlock);
	}

	NewAllocator() {
		block = (Block*)malloc(1 << sz);
		block->free = 1;
		block->level = sz;
		BlockTree[sz].add(block);
	}
};


NewAllocator alloc;

template <class T>
class CMyAllocator
{
public:
	typedef T value_type;

	CMyAllocator()
	{

	}

	template <class U>
	CMyAllocator(const CMyAllocator<U>& V)
	{

	}

	T* allocate(size_t Count)
	{
		return (T*)alloc.allocate(Count * sizeof(T));
	}

	void deallocate(T* V, size_t Count)
	{
		alloc.deallocate(V);
	}
};

void RunTextTest(char* s) {
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

int main() {
	HANDLE H = CreateFileW(L"F:\\Programming\\Filp\\Readme.txt",
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (H == INVALID_HANDLE_VALUE)
		return cout << "invalid handle value", 0;
	LARGE_INTEGER L;
	if (!GetFileSizeEx(H, &L))
		return cout << "size is empry", 0;
	cout << L.QuadPart << '\n';
	char* T = new char[L.QuadPart + 1];
	DWORD NumberOfBytesRead;
	if (!ReadFile(H, T, L.QuadPart, &NumberOfBytesRead, NULL))
		return cout << "cant read file", 0;
	cout << NumberOfBytesRead << '\n';
	ULONGLONG Start, End;
	Start = GetCurrentTimeMs();
	RunTextTest(T);
	End = GetCurrentTimeMs();
	cout << "Twin time: " << End - Start << '\n';
	getchar();
}