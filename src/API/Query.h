#pragma once
#include <boost\thread\mutex.hpp>

template<typename T>
class Query
{
public:
	Query() {}
	~Query()
	{
		while (first)
		{
			node* tmp = first;
			first = first->next;
			delete tmp;
		}
	}
	bool ready() 
	{
		boost::mutex::scoped_lock lock{ mutex } ;
		return first;
	}
	//todo: make refference
	void push_back(const T& data)
	{
		boost::mutex::scoped_lock lock{ mutex } ;
		node* new_node = new node;
		//mutex
		if (first)
		{
			new_node->next = first;
			first->prev = new_node;

			first = new_node;
		}
		else {

			first = new_node;
			last = new_node;
		}
		new_node->data = data;
	}
	T pop_front()
	{
		if (last)
		{
			boost::mutex::scoped_lock lock{ mutex } ;

			node* tmp = last;

			last = last->prev;
			// 
			if (last)
				last->next = nullptr;
			// 0 left
			else
				first = last;
			
			T tmpData = tmp->data;
			//save data
			delete tmp;

			return tmpData;
		}
		throw 0;
	}
	const T& front()
	{
		boost::mutex::scoped_lock lock{ mutex };
		if (last)
		{
			return last->data;
		}
		throw;
	}
private:
	boost::mutex mutex;

	struct node
	{
		node* next = nullptr;
		T data;
		node* prev = nullptr;
	};
	node* first = nullptr;
	node* last = nullptr;
};

