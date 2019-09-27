#ifndef CIRCULAR_ITERATOR_HPP
#define CIRCULAR_ITERATOR_HPP
#include <iterator>
#include <iostream>

template <typename T>
class CircularIterator {
	public:

		using value_type = typename T::value_type;
		class iterator: public std::iterator<
						std::random_access_iterator_tag, // iterator_category
						typename T::value_type, // value_type
						size_t, // difference_type
						const typename T::value_type*, // pointer
						typename T::value_type  // reference
					>
		{
			private:
				T* obj;
				size_t num;
				size_t size;
				size_t head;
				
			public:
				using difference_type = typename std::iterator<std::random_access_iterator_tag, value_type, size_t, const value_type*, value_type&>::difference_type;
				using reference = typename std::iterator<std::random_access_iterator_tag, value_type, size_t, const value_type*, value_type&>::reference;
						
				explicit iterator(T* _obj, size_t _num, size_t _head) : obj(_obj), num(_num), size(_obj->size()), head(_head) {}
				
				iterator& operator++() {
					num++;
					if(num >= size) {
						num = 0;
					}
					return *this;
				}
				
				iterator operator++(int) {
					iterator retval = *this; 
					++(*this);
					return retval;
				}
				
				iterator& operator--() {
					if(num == 0) {
						num = size-1;
					} else {
						num--;
					}
					return *this;
				}
				
				iterator operator--(int) {
					iterator old = *this;
					if(num == 0) {
						num = size-1;
					} else {
						num--;
					}
					return old;
				}
				
				
				
				bool operator==(iterator other) const {
					return num == other.num;
				}
				
				bool operator<(iterator other) const {
					// if(!((num < other.num && other.num <= head) || (num > head && other.num <= head)))
					// std::cout << "num: " << num << " other.num: " << other.num << " size: " << size << "\n";
					// return (num < other.num && other.num <= head) || (num > head && other.num <= head);
					// 1. --B--HT---A-- +
					// 2. T--A--B--H--- +
					// 3. --A--B---HT-- +
					// 4. ---HT--A--B-- +
					return (num < other.num && (head < num || other.num <= head) ) || (num > head && other.num <= head);
				}

				bool operator!=(iterator other) const {
					return num != other.num;
				}
				
				iterator operator+(size_t n) const {
					iterator it = *this;
					it+=n;
					return it;
				}
				
				iterator operator-(size_t n) const {
					iterator it = *this;
					it-=n;
					return it;
				}

				difference_type operator-(iterator it) const {
					if(num < it.num) {
						return size - it.num + num;
					} else if(num > it.num) {
						return num - it.num;
					} else {
						return 0;
					}
				}
				
				iterator end() {
					return iterator(obj, head, head);
				}
				
				iterator& operator+=(size_t n) {
					size_t dist = end() - *this;
					num+=std::min(dist,n);
					if(num >= size) {
						num = num % size;
					}
					return *this;
				}
				
				iterator& operator-=(size_t n) {
					size_t dist = end() - *this;
					size_t sub = std::min(size-dist,n);
					if(sub > num) {
						num = size - (sub - num);
					} else {
						num -= sub;
					}
					return *this;
				}

				reference operator*() const {
					return obj->operator[](num);
				}
				
				value_type* operator->() const {
					return &obj->operator[](num);
				}
				
				size_t GetIndex() {
					return num;
				}
				
				size_t GetHead() {
					return head;
				}
		};

		iterator begin() {
			return iterator(m_obj, m_tail, m_head);
		}
		iterator end() {
			return iterator(m_obj, m_head, m_head);
		}
		
		void SetHead(size_t head) {
			m_head = head;
		}
		
		void SetTail(size_t tail) {
			m_tail = tail;
		}
		
		
		template<typename Iterator>
		void insert(Iterator it1, Iterator it2) {
			auto it = begin();
			auto itend = end();
			int dist = it - itend;
			int c=0;
			for(; it1 != it2; it1++, itend++, c++) {
				*itend = *it1;
			}
			
			m_head = itend.GetIndex();
			if(c >= dist) {
				m_tail = (m_head+1) % m_size;
			}
		}
		

		CircularIterator(T& obj, int tail, int head) {
			m_obj = &obj;
			m_size = obj.size();
			m_tail = tail;
			m_head = head;
		}
		
	private:
		T* m_obj;
		size_t m_size;
		size_t m_head;
		size_t m_tail;
};

template<typename C>
CircularIterator<C> make_circular_iterator(C &container, int tail, int head) {
	return CircularIterator<C>(container, tail, head);
}

#undef dbg

#endif
