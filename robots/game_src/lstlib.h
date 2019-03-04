#ifndef LSTLIB_H
#define LSTLIB_H

#include <cstdlib>

template<class T>
struct ListStr{
	T Item;
	ListStr<T> *Next;
	ListStr(const T& New, ListStr<T> *Node = NULL);
private:
	ListStr(const ListStr<T>&);
};

template<class T>
class List{
	List(const List<T>&);
protected:
	ListStr<T> *Nodes;
	int LastIdx;
	ListStr<T> *LastPtr;
public:
	List(ListStr<T>* Node = NULL);
	List(const T& New);
	virtual ~List();
	ListStr<T>* operator>>=(const T& New); // Add to end
	ListStr<T>* operator<<=(const T& New); // Add to begin
	int Size();
	T& operator[](int idx);
	ListStr<T>* operator()(int idx);
	ListStr<T>* operator[](const T& Key);
	ListStr<T>* operator[](const T *Key);
	//ListStr<T>* operator[](void *Key);
	int operator!();
	operator ListStr<T>*();
	List<T>& operator,(const T& New);
	int operator==(const List<T>& Lst);

};

#include "lstlib.cpp"

#endif
