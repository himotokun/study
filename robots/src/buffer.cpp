#include <string.h>
#include <stdio.h>
#include "buffer.h"

Buffer::Buffer(int init_size)
{
	data = new char[size = init_size];
	memset(data, 0, size);
	ptr = 0;
}

Buffer::~Buffer()
{
	delete[] data;
}

Buffer& Buffer::Push(char c)
{
	if(ptr == size)
		throw BufferOverflowException();
	data[ptr++] = c;
	return *this;
}

char* Buffer::Make()
{
	char *res = strdup(data);
	//char *res = strndup(data, ptr);
	Clear();
	return res;
}

Buffer& Buffer::Clear()
{
	ptr = 0;
	memset(data, 0, size);
	return *this;
}

int Buffer::Empty()
{
	return ptr == 0;
}

char Buffer::Pop()
{
	char res = data[--ptr];
	data[ptr] = 0;
	return res;
}

Buffer::operator const char*()
{
	return data;
}
