#ifndef __BUFFER_H
#define __BUFFER_H

class BufferOverflowException{
};

class Buffer{
	char *data;
	int ptr, size;
	Buffer(const Buffer&);
public:
	Buffer(int init_size = 256);
	~Buffer();
	Buffer& Push(char c);
	char Pop();
	char* Make();
	Buffer& Clear();
	int Empty();
	operator const char*();
};

#endif
