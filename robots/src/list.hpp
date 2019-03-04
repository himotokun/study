#ifndef __LIST_HPP
#define __LIST_HPP

class EmptyStackException{
};

template<class T>
struct node{
	T item;
	node<T> *next;
	node(const T& new_item, node<T> *new_next = 0);
	//void destroy();
	//~node();
private:
	node(const node<T>&);
};

template<class T>
class List{
	node<T> *head;
	//List(const List<T>&);
public:
	List(node<T> *first = 0);
	void Push(const T& new_item);
	void PushStack(const T& new_item);
	node<T>* PopStack();
	node<T>* Begin();
	const node<T>* Begin() const;
	node<T>* Find(const T& key, node<T> *from = 0);
	void Clear();
	int Empty();
	//const node<T>* Find(const T& key) const;
};

#define _ template<class T>

_ node<T>::node(const T& new_item, node<T> *new_next):
	item(new_item), next(new_next)
{}

#if 0
_ node<T>::~node()//destroy()
{
	node<T> *iter = this->next;
	while(iter){
		node<T> *tmp = iter->next;
		delete iter;
		iter = tmp;
	}
	//delete this;
}
#endif

_ List<T>::List(node<T> *first)
{
	head = first;
}

_ void List<T>::Push(const T& new_item)
{
	node<T> *new_str = new node<T>(new_item);
	if(!head){
		head = new_str;
		return;
	}
	node<T> *iter = head;
	while(iter->next)
		iter = iter->next;
	iter->next = new_str;
}

_ node<T>* List<T>::Begin()
{
	return head;
}

_ const node<T>* List<T>::Begin() const
{
	return head;
}

_ node<T>* List<T>::Find(const T& key, node<T> *from)
{
	if(!from)
		from = head;
	for(node<T> *iter = from; iter; iter = iter->next)
		if(iter->item == key)
			return iter;
	return 0;
}

_ void List<T>::PushStack(const T& new_item)
{
	head = new node<T>(new_item, head);
}

_ node<T>* List<T>::PopStack()
{
	if(!head)
		throw EmptyStackException();
	node<T> *tmp = head;
	head = head->next;
	return tmp;
}

_ void List<T>::Clear()
{
	node<T> *tmp = head;
	while(head){
		tmp = head;
		head = head->next;
		delete tmp;
	}
}

_ int List<T>::Empty()
{
	return head == 0;
}

#undef _

#endif
