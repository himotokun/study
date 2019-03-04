#ifndef __ARRAY_HPP
#define __ARRAY_HPP

#include <stdlib.h>
#include <string.h>
#include "errors.h"

template<class T>
class Array{
protected:
	T *arr;
	int size;
	Array(const Array&);
public:
	Array(int new_size = 0);
	//Array(T* c_arr, int new_size);
	~Array();
	T& operator[](int idx);
	const T& operator[](int idx) const;
	void Resize(int new_size);
	int Size() const;
	int Find(const T& key) const;
	void Insert(const T& key, int idx);
	void Delete(int idx);
	void PushBack(const T& item);
	void PopBack();
	//void PushFront(const T& item);
	//void PopFront();
	T* CArray();
	Array<T>& operator=(const Array<T>& A);
	Array<T> operator+(const Array<T>& A) const;
	Array<T>& operator+=(const Array<T>& A);
};

/* Implementation */

#define _ template<class T>

_ Array<T>::Array(int new_size)
{
	arr = 0;
	size = 0;
	Resize(new_size);
	/*if(!new_size){
		arr = 0;
		size = 0;
	}else{
		arr = (T*)malloc(new_size*sizeof(T));
		size = new_size;
	}*/
}

/*_ Array<T>::Array(T* c_arr, int new_size)
{
	arr = c_arr;
	size = new_size;
}*/

_ Array<T>::~Array()
{
	free(arr);
}

_ T& Array<T>::operator[](int idx)
{
	if(idx >= size || -idx > size)
		throw OutOfRangeException();
	return arr[ idx >= 0 ? idx : size - idx ];
}

_ const T& Array<T>::operator[](int idx) const
{
	if(idx >= size || -idx > size)
		throw OutOfRangeException();
	return arr[ idx >= 0 ? idx : size - idx ];
}

_ void Array<T>::Resize(int new_size)
{
	if(new_size < 0)
		throw;
	//int old_size = size;
	arr = (T*)realloc(arr, (size = new_size)*sizeof(T));
	/*T init_obj;
	for(int i = old_size; i < size; i++)
		memcpy(arr+i, &init_obj, sizeof(T));*/
}

_ int Array<T>::Size() const
{
	return size;
}

_ int Array<T>::Find(const T& key) const
{
	for(int i = 0; i < size; i++){
		if(arr[i] == key)
			return i;
	}
	return -1;
}

_ void Array<T>::Insert(const T& key, int idx)
{
	if(idx < 0 || idx > size)
		throw OutOfRangeException();
	Resize(size+1);
	memmove(arr+sizeof(T)*(idx+1), arr+sizeof(T)*idx,
			sizeof(T)*(size-idx));
	arr[idx] = key;
}

_ void Array<T>::Delete(int idx)
{
	if(idx != size-1)
		memmove(arr+sizeof(T)*(idx), arr+sizeof(T)*(idx+1),
				sizeof(T)*(size-idx));
	//for(int i = idx; i < size-1; i++)
	//	arr[i] = arr[i+1];
	Resize(size-1);
}

_ void Array<T>::PushBack(const T& item)
{
	Resize(size+1);
	arr[size-1] = item;
}

_ void Array<T>::PopBack()
{
	Resize(size-1);
}

#if 0
_ void Array<T>::PushFront(const T& item)
{
	Resize(size+1);
	memmove
	for(int i = 0; i < size-1; i++)
		arr[i] = arr[i+1];
	arr[0] = item;
}

_ void Array<T>::PopFront()
{
	for(int i = 0; i < size-1; i++)
		arr[i] = arr[i+1];
	Resize(size-1);
}
#endif

_ T* Array<T>::CArray()
{
	return arr;
}

_ Array<T>& Array<T>::operator=(const Array<T>& A)
{
	Resize(A.size);
	for(int i = 0; i < size; i++)
		arr[i] = A.arr[i];
	return *this;
}

_ Array<T> Array<T>::operator+(const Array<T>& A) const
{
	Array<T> Res = *this;
	Res += A;
	return Res;
}

_ Array<T>& Array<T>::operator+=(const Array<T>& A)
{
	int old_size = size;
	Resize(size + A.size);
	for(int i = old_size; i < size; i++)
		arr[i] = A.arr[i - old_size];
	return *this;
}

#undef _

#endif
