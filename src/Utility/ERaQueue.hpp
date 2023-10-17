#ifndef INC_ERA_QUEUE_HPP_
#define INC_ERA_QUEUE_HPP_

#include <new>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <ERa/ERaDefine.hpp>

template <class T, int N>
class ERaQueue
{
	const static int eN = N + 1;

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

	T& put(const T& _e) {
		int i = this->w;
		int j = i;
		i = this->inc(i);
		while (i == this->r) {}
		this->e[j] = _e;
		this->w = i;
		return const_cast<T&>(_e);
	}

	int put(const T* p, int n, bool blocking = false) {
		int c = n;
		while (c) {
			int f = 0;
			while ((f = this->space()) == 0) {
				if (!blocking) {
					return n - c;
				}
			}
			if (c < f) {
				f = c;
			}
			int _w = this->w;
			int m = eN - _w;
			if (f > m) {
				f = m;
			}
			memcpy(this->e + _w, p, f);
			this->w = this->inc(_w, f);
			c -= f;
			p += f;
		}
		return n - c;
	}

	int space() {
		int s = this->r - this->w;
		if (s <= 0) {
			s += eN;
		}
		return (s - 1);
	}

	T& get() {
		int _r = this->r;
		while (_r == this->w) {}
		T& _e = this->e[_r];
		this->r = inc(this->r);
		return _e;
	}

	T& peek() {
		int _r = this->r;
		while (_r == this->w) {}
		return this->e[_r];
	}

	int get(T* p, int n, bool blocking = false) {
		int c = n;
		while (c) {
			int f = 0;
			while ((f = this->size()) == 0) {
				if (!blocking) {
					return n - c;
				}
			}
			if (c < f) {
				f = c;
			}
			int _r = this->r;
			int m = eN - _r;
			if (f > m) {
				f = m;
			}
			memcpy(p, this->e + _r, f);
			this->r = this->inc(_r, f);
			c -= f;
			p += f;
		}
		return n - c;
	}

	size_t size() {
		int s = this->w - this->r;
		if (s < 0) {
			s += eN;
		}
		return s;
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

	T& operator = (const T& _e) {
		if (this->writeable()) {
			this->put(_e);
		}
		return const_cast<T&>(_e);
	}

	T& operator += (const T& _e) {
		if (this->writeable()) {
			this->put(_e);
		}
		return const_cast<T&>(_e);
	}

protected:
private:
	int inc(int i, int n = 1) {
		return ((i + n) % eN);
	}

	T e[eN];
	volatile int w;
	volatile int r;
};

template <class T>
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
		iterator(const T& _data)
			: prev(nullptr)
			, next(nullptr)
			, data(_data)
		{}
		iterator(iterator* _prev, const T& _data)
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

		iterator* getNext() const {
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

	void put(const T& value) {
		if (this->first == nullptr) {
			this->first = new iterator(value);
			this->last = this->first;
		}
		else {
			iterator* prev = this->last;
			this->last = new iterator(prev, value);
		}
		if (this->first != nullptr) {
			this->first->prev = this->last;
		}
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

	void remove(int index) {
		iterator* item = this->detach(index);
		if (item != nullptr) {
			delete item;
		}
		item = nullptr;
	}

	void remove(iterator* item) {
		this->detachItem(item);
		if (item != nullptr) {
			delete item;
		}
		item = nullptr;
	}

	void clear() {
		iterator* next = nullptr;
		while (this->first != nullptr) {
			next = this->first->next;
			delete this->first;
			this->first = next;
		}
		this->last = nullptr;
	}

	size_t size() {
		size_t _size {0};
		iterator* next = this->first;
		while (next != nullptr) {
			++_size;
			next = next->next;
		}
		return _size;
	}

	bool readable() const {
		return this->first != nullptr;
	}

	bool isEmpty() const {
		return this->first == nullptr;
	}

	iterator* begin() const {
		return this->first;
	}

	iterator* end() const {
		return nullptr;
	}

	void operator += (const T& value) {
		this->put(value);
	}

	iterator* operator[] (const int index) {
		return this->get(index);
	}

	operator iterator* () {
		return this->get();
	}

private:
	iterator* detach(int index) {
		iterator* item = this->get(index);
		return this->detachItem(item);
	}

	iterator* detachItem(iterator* const item) {
		if ((item == nullptr) ||
			(this->first == nullptr)) {
			return nullptr;
		}

		if ((item != this->first) &&
			(item->prev != nullptr)) {
			item->prev->next = item->next;
		}
		if (item->next != nullptr) {
			item->next->prev = item->prev;
		}
		if (item == this->first) {
			this->first = item->next;
		}
		else if (item->next == nullptr) {
			this->first->prev = item->prev;
		}
		if (item == this->last) {
			this->last = item->prev;
		}

		item->prev = nullptr;
		item->next = nullptr;

		return item;
	}

	iterator* first;
	iterator* last;
};

#endif /* INC_ERA_QUEUE_HPP_ */
