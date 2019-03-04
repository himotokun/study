//#include <cstdlib>

//#include "lstlib.h"

#define _ template<class T>

/* ListStr */

_ ListStr<T>::ListStr(const T& New, ListStr<T> *Node)
	{ Item = New; Next = Node; }

/* List itself */

//_ List<T>::List(const List<T>&) {}

_ List<T>::List(ListStr<T>* Node) { Nodes = Node; }

#include <cstdio>
#if 1
_ List<T>::~List()
{
	while(Nodes){
		ListStr<T> *Temp = Nodes->Next;
		delete Nodes;
		Nodes = Temp;
	}
}
#endif

_ ListStr<T>* List<T>::operator>>=(const T& New)
{
	ListStr<T> *Iter = Nodes;
	ListStr<T> *NewNode = new ListStr<T>(New);
	if(!Nodes){
		Nodes = NewNode;
		return Nodes;
	}
	while(Iter->Next)
		Iter = Iter->Next;
	Iter->Next = NewNode;
	return Nodes;
}

_ ListStr<T>* List<T>::operator<<=(const T& New)
{
	return Nodes = new ListStr<T>(New, Nodes);
}

_ int List<T>::Size()
{
	int i = 0;
	ListStr<T> *Iter = Nodes;
	for(;Iter; Iter = Iter->Next)
		i++;
	return i;
}

_ ListStr<T>* List<T>::operator()(int idx)
{
	ListStr<T> *Iter = Nodes;
	while(idx-- && Iter)
		Iter = Iter->Next;
	if(idx!=-1)
		throw;
	return Iter;
}

_ T& List<T>::operator[](int idx)
{
	return (*this)(idx)->Item;
}

_ ListStr<T>* List<T>::operator[](const T& Key)
{
	ListStr<T> *Iter = Nodes;
	while(Iter->Item != Key && Iter)
		Iter = Iter->Next;
	return Iter;
}

_ ListStr<T>* List<T>::operator[](const T *Key)
{
	ListStr<T> *Iter = Nodes;
	while(Iter != Key && Iter)
		Iter = Iter->Next;
	return Iter;
}

/*_ ListStr<T>* List<T>::operator[](void *Key)
{
	ListStr<T> *Iter = Nodes;
	while(Iter != reinterpret_cast<T*>(Key) && Iter)
		Iter = Iter->Next;
	return Iter;
}*/

_ int List<T>::operator!()
	{ return Nodes == NULL; }

_ List<T>::operator ListStr<T>*()
	{ return Nodes; }

_ List<T>::List(const T& New)
	{ Nodes = new ListStr<T>(New); }

_ List<T>& List<T>::operator,(const T& New)
{ 
	(*this) >>= New;
	return *this;
}

_ List<T>& operator,(const T& New1, const T& New2)
	{ return List<T>(New1), New2; }

_ int List<T>::operator==(const List<T>& Lst)
{
	ListStr<T> *I1 = Nodes, *I2 = Lst->Nodes;
	for(; I1 || I2; I1 = I1->Next, I2 = I2->Next)
		if(I1->Item != I2->Item)
			return 0;
	return 1;
}

#undef _
