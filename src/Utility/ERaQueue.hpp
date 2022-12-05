#ifndef INC_ERA_QUEUE_HPP_
#define INC_ERA_QUEUE_HPP_

#include <new>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ERa/ERaDefine.hpp>

template<class T, int N>
class ERaQueue
{
public:
	ERaQueue()
		: w(0)
		, r(0)
	{}
	~ERaQueue()
	{}

	void clear() {
		this->w = 0;
		this->r = 0;
	}

	T put(const T& _e) {
		int i = this->w;
		int j = i;
		i = this->inc(i);
		while (i == this->r) {}
		this->e[j] = _e;
		this->w = i;
		return _e;
	}

	T& get() {
		int _r = this->r;
		while (_r == this->w) {}
		T& _e = this->e[_r];
		this->r = inc(this->r);
		return _e;
	}

	bool writeable() {
		int i = this->w;
		i = this->inc(i);
		return (i != this->r);
	}

	bool readable() {
		return (this->r != this->w);
	}

	bool isEmpty() {
		return (this->r == this->w);
	}

    operator T&() {
        return this->get();
    }

	T operator=(const T& _e) {
		return (this->writeable() ? this->put(_e) : T());
	}

	T operator+=(const T& _e) {
		return (this->writeable() ? this->put(_e) : T());
	}

protected:
private:
	int inc(int i, int n = 1) {
		return ((i + n) % N);
	}

	int space() {
		int s = this->r - this->w;
		if (s <= 0) {
			s += N;
		}
		return (s - 1);
	}

	T e[N];
	volatile int w;
	volatile int r;
};

template<class T>
class ERaList
{
public:
	class iterator
	{
	public:
		iterator()
			: prev(nullptr)
			, next(nullptr)
			, data(0)
		{}
		iterator(const T _data)
			: prev(nullptr)
			, next(nullptr)
			, data(_data)
		{}
		iterator(iterator* _prev, const T _data)
			: prev(_prev)
			, next(nullptr)
			, data(_data)
		{
			if (_prev != nullptr) {
				_prev->next = this;
			}
		}
		~iterator()
		{}

		iterator* getNext() {
			return this->next;
		}

		void setNext(iterator* _next) {
			this->next = _next;
		}

		T& get() {
			return this->data;
		}

		operator T& () {
			return this->get();
		}

		iterator* prev;
		iterator* next;

	private:
		T data;
	};

	ERaList()
		: first(nullptr)
		, last(nullptr)
	{}

	void put(const T value) {
		if (this->first == nullptr) {
			void* ptr = ERA_MALLOC(sizeof(iterator));
			this->first = new(ptr) iterator(value);
			this->last = this->first;
		}
		else {
			iterator* prev = this->last;
			void* ptr = ERA_MALLOC(sizeof(iterator));
			this->last = new(ptr) iterator(prev, value);
		}
		this->first->prev = this->last;
	}

	iterator* get(int index) {
		iterator* next = this->first;
		while (next != nullptr) {
			if (!index--) {
				return next;
			}
			next = next->next;
		}
		return nullptr;
	}

	iterator* get() {
		if (this->first == nullptr) {
			return nullptr;
		}
		iterator* item = this->first;
		this->first = this->first->next;
		if (this->first != nullptr) {
			this->first->prev = this->last;
		}
		item->next = nullptr;
		item->prev = nullptr;
		return item;
	}

	void clear() {
		iterator* next = nullptr;
		while (this->first != nullptr) {
			next = this->first->next;
			free(this->first);
			this->first = next;
		}
		this->last = nullptr;
	}

	bool readable() {
		return this->first != nullptr;
	}

	bool isEmpty() {
		return this->first == nullptr;
	}

	iterator* begin() {
		return this->first;
	}

	iterator* end() {
		return this->last;
	}

	void operator += (const T value) {
		this->put(value);
	}

	iterator* operator[] (const int index) {
		return this->get(index);
	}

	operator iterator* () {
		return this->get();
	}

private:
	iterator* first;
	iterator* last;
};

#endif /* INC_ERA_QUEUE_HPP_ */
