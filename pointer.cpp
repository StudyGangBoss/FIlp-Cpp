#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <algorithm>
#include <cstring>

class CFile {
    size_t* counter = nullptr;

public:
    FILE* file = nullptr;

    CFile(const CFile& another) {
        CopyFile(another);
    }

   bool operator== (const CFile& another) {
        return this->file == another.Get();
    }
       void operator= (const CFile& another) {
        DeleteFile();
        CopyFile(another);
    }

    operator bool() const {
        return file != nullptr;
    }

    CFile(const char* fname) {
        file = fopen(fname, "r+t");
        counter = new size_t;
        *counter = 1;
    }

    ~CFile() {
        DeleteFile();
    }

    FILE* Get() const {
        return file;
    }

    void DeleteFile() {
        *counter -= 1;
        if (*counter == 0) {
            delete counter;
            if (file) {
                fclose(file);
                file = nullptr;
            }
        }
    }

    bool Seek(int pos, int origin = SEEK_SET) {
        if (!file)
            return 0;
        fseek(file, pos, origin);
        return 1;
    }

    void CopyFile(const CFile& another) {
        file = another.file;
        counter = another.counter;
        *counter += 1;
    }

    bool ReadString(char* string, size_t num,size_t arraySize) {
        num = std::min(arraySize - 1, num);
        if (!file)
            return 0;
        int readed = fread(string, 1, num, file);
        string[readed] = 0;
        return 1;
    }

    int Links() {
        return *counter;
    }

    bool Write(char* string, int pos = -1) {
        if (!file)
            return 0;
        if (pos == -1)
            Seek(0, 2);
        else
            Seek(pos, 0);
        fwrite(string, 1, strlen(string), file);
        return 1;
    }
};

int main() {
    CFile File1("F:\\Programming\\Filp\\Readme.txt");
    CFile File2(File1);
    const size_t maxArraySize = 1000;
    char mystring[maxArraySize];
    if (File1) {
        File2.ReadString(mystring, 25,maxArraySize);
        puts(mystring);
        File2.ReadString(mystring, 25,maxArraySize);
        puts("\n");
        puts(mystring);
        File1.Write(mystring);
    }
    else
        printf("cant find file");
}