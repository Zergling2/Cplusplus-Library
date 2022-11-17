#pragma once

namespace SJNET
{
	namespace LIB
	{
		template<typename T>
		class list
		{

		private:
			struct Node;
		public:
			class iterator;

		public:
			list();
			~list();
			void push_front(T data);
			void push_back(T data);
			void pop_front();
			void pop_front_with_scalar_delete();
			void pop_front_with_vector_delete();
			void pop_back();
			void pop_back_with_scalar_delete();
			void pop_back_with_vector_delete();
			iterator begin();
			iterator end();
			void clear();
			size_t size() const;
			bool isEmpty() const;
			iterator erase(const iterator& iter);
			iterator erase_with_scalar_delete(const iterator& iter);
			iterator erase_with_vector_delete(const iterator& iter);
			iterator erase(const iterator& begin, const iterator& end);
			iterator erase_with_scalar_delete(const iterator& begin, const iterator& end);
			iterator erase_with_vector_delete(const iterator& begin, const iterator& end);
			void remove(const T& data);
			void remove_with_scalar_delete(const T& data);
			void remove_with_vector_delete(const T& data);
		private:
			Node head;	// Dummy node
			Node tail; // Dummy node
			size_t numOfNode;
		private:
			struct Node
			{
			public:
				Node();		// constructor for dummy node
				Node(T data);
				T data;
				Node* prev;
				Node* next;
			};

		public:
			// iterator class declare
			class iterator
			{
				friend iterator list<T>::erase(const iterator& iter);
				friend iterator list<T>::erase_with_scalar_delete(const iterator& iter);
				friend iterator list<T>::erase_with_vector_delete(const iterator& iter);
				friend iterator list<T>::erase(const iterator& begin, const iterator& end);
				friend iterator list<T>::erase_with_scalar_delete(const iterator& begin, const iterator& end);
				friend iterator list<T>::erase_with_vector_delete(const iterator& begin, const iterator& end);
			public:
				iterator(Node* node = nullptr);
				iterator(const iterator& iter);
				iterator operator+(int);
				iterator operator-(int);
				iterator operator++(int);
				iterator& operator++();
				iterator operator--(int);
				iterator& operator--();
				T& operator*();
				bool operator==(const iterator& other);
				bool operator!=(const iterator& other);
			private:
				Node* pNode;
			};
			// iterator class declare end

		};

		template<typename T>
		inline list<T>::Node::Node() : data(*(new (&data)T())), prev(nullptr), next(nullptr)
		{
		}

		template<typename T>
		inline list<T>::Node::Node(T data) : data(data), prev(nullptr), next(nullptr)
		{
		}

		template<typename T>
		inline list<T>::iterator::iterator(Node* node)
		{
			this->pNode = node;
		}

		template<typename T>
		inline list<T>::iterator::iterator(const iterator& iter)
		{
			this->pNode = iter.pNode;
		}

		template<typename T>
		inline typename list<T>::iterator list<T>::iterator::operator+(int num)
		{
			list<T>::iterator iter(*this);
			for (int i = 0; i < num; i++)
				iter++;

			return iter;
		}

		template<typename T>
		inline typename list<T>::iterator list<T>::iterator::operator-(int num)
		{
			list<T>::iterator iter(*this);
			for (int i = 0; i < num; i++)
				iter--;

			return iter;
		}

		template<typename T>
		inline typename list<T>::iterator list<T>::iterator::operator++(int)
		{
			list<T>::iterator temp = *this;
			this->pNode = this->pNode->next;
			return temp;
		}

		template<typename T>
		inline typename list<T>::iterator& list<T>::iterator::operator++()
		{
			this->pNode = this->pNode->next;
			return *this;
		}

		template<typename T>
		inline typename list<T>::iterator list<T>::iterator::operator--(int)
		{
			list<T>::iterator temp = *this;
			this->pNode = this->pNode->prev;
			return temp;
		}

		template<typename T>
		inline typename list<T>::iterator& list<T>::iterator::operator--()
		{
			this->pNode = this->pNode->prev;
			return *this;
		}

		template<typename T>
		inline typename T& list<T>::iterator::operator*()
		{
			return this->pNode->data;
		}

		template<typename T>
		inline typename bool list<T>::iterator::operator==(const iterator& other)
		{
			return this->pNode == other.pNode;
		}

		template<typename T>
		inline typename bool list<T>::iterator::operator!=(const iterator& other)
		{
			return this->pNode != other.pNode;
		}

		template<typename T>
		list<T>::list()
		{
			this->numOfNode = 0;
			this->head.next = &this->tail;
			this->tail.next = nullptr;
			this->tail.prev = &this->head;
			this->head.prev = nullptr;
		}

		template<typename T>
		list<T>::~list()
		{
			Node* cursor = this->head.next;
			while (cursor != &this->tail)
			{
				Node* temp = cursor->next;
				delete cursor;
				cursor = temp;
			}

			this->numOfNode = 0;
		}

		template<typename T>
		inline void list<T>::push_front(const T data)
		{
			Node* newNode = new Node(data);

			Node* rTemp = this->head.next;
			rTemp->prev = newNode;
			newNode->next = rTemp;
			newNode->prev = &this->head;
			this->head.next = newNode;

			this->numOfNode++;
		}

		template<typename T>
		inline void list<T>::push_back(const T data)
		{
			Node* newNode = new Node(data);

			Node* lTemp = this->tail.prev;
			lTemp->next = newNode;
			newNode->prev = lTemp;
			newNode->next = &this->tail;
			this->tail.prev = newNode;

			this->numOfNode++;
		}

		template<typename T>
		inline void list<T>::pop_front()
		{
			Node* deleteNode = this->head.next;
			if (deleteNode == &this->tail)
				return;

			deleteNode->next->prev = deleteNode->prev;
			deleteNode->prev->next = deleteNode->next;
			delete deleteNode;

			this->numOfNode--;
		}

		template<typename T>
		inline void list<T>::pop_front_with_scalar_delete()
		{
			Node* deleteNode = this->head.next;
			T deleteData = deleteNode->data;
			if (deleteNode == &this->tail)
				return;

			deleteNode->next->prev = deleteNode->prev;
			deleteNode->prev->next = deleteNode->next;
			delete deleteData;
			delete deleteNode;

			this->numOfNode--;
		}

		template<typename T>
		inline void list<T>::pop_front_with_vector_delete()
		{
			Node* deleteNode = this->head.next;
			T deleteData = deleteNode->data;
			if (deleteNode == &this->tail)
				return;

			deleteNode->next->prev = deleteNode->prev;
			deleteNode->prev->next = deleteNode->next;
			delete[] deleteData;
			delete deleteNode;

			this->numOfNode--;
		}

		template<typename T>
		inline void list<T>::pop_back()
		{
			Node* deleteNode = this->tail.prev;
			if (deleteNode == &this->head)
				return;

			deleteNode->prev->next = deleteNode->next;
			deleteNode->next->prev = deleteNode->prev;
			delete deleteNode;

			this->numOfNode--;
		}

		template<typename T>
		inline void list<T>::pop_back_with_scalar_delete()
		{
			Node* deleteNode = this->tail.prev;
			T data = deleteNode->data;
			if (deleteNode == &this->head)
				return;

			deleteNode->prev->next = deleteNode->next;
			deleteNode->next->prev = deleteNode->prev;
			delete data;
			delete deleteNode;

			this->numOfNode--;
		}

		template<typename T>
		inline void list<T>::pop_back_with_vector_delete()
		{
			Node* deleteNode = this->tail.prev;
			T data = deleteNode->data;
			if (deleteNode == &this->head)
				return;

			deleteNode->prev->next = deleteNode->next;
			deleteNode->next->prev = deleteNode->prev;
			delete[] data;
			delete deleteNode;

			this->numOfNode--;
		}

		template<typename T>
		inline typename list<T>::iterator list<T>::begin()
		{
			list<T>::iterator iter(this->head.next);
			return iter;
		}

		template<typename T>
		inline typename list<T>::iterator list<T>::end()
		{
			list<T>::iterator iter(&this->tail);
			return iter;
		}

		template<typename T>
		inline void list<T>::clear()
		{
			Node* cursor = this->head.next;
			while (cursor != &this->tail)
			{
				Node* temp = cursor->next;
				delete cursor;
				cursor = temp;
			}

			this->numOfNode = 0;
		}

		template<typename T>
		inline size_t list<T>::size() const
		{
			return this->numOfNode;
		}

		template<typename T>
		inline bool list<T>::isEmpty() const
		{
			return this->numOfNode == 0;
		}

		template<typename T>
		inline typename list<T>::iterator list<T>::erase(const iterator& iter)
		{
			Node* lTemp = iter.pNode->prev;
			Node* rTemp = iter.pNode->next;

			delete iter.pNode;

			lTemp->next = rTemp;
			rTemp->prev = lTemp;

			this->numOfNode--;

			return list<T>::iterator(rTemp);
		}

		template<typename T>
		inline typename list<T>::iterator list<T>::erase_with_scalar_delete(const iterator& iter)
		{
			Node* lTemp = iter.pNode->prev;
			Node* rTemp = iter.pNode->next;

			delete iter.pNode->data;
			delete iter.pNode;

			lTemp->next = rTemp;
			rTemp->prev = lTemp;

			this->numOfNode--;

			return list<T>::iterator(rTemp);
		}

		template<typename T>
		inline typename list<T>::iterator list<T>::erase_with_vector_delete(const iterator& iter)
		{
			Node* lTemp = iter.pNode->prev;
			Node* rTemp = iter.pNode->next;

			delete[] iter.pNode->data;
			delete iter.pNode;

			lTemp->next = rTemp;
			rTemp->prev = lTemp;

			this->numOfNode--;

			return list<T>::iterator(rTemp);
		}

		template<typename T>
		inline typename list<T>::iterator list<T>::erase(const iterator& begin, const iterator& end)
		{
			Node* lTemp = begin.pNode->prev;
			Node* rTemp = end.pNode;

			lTemp->next = rTemp;
			rTemp->prev = lTemp;

			list<T>::iterator deleteIter(begin);

			while (deleteIter.pNode != rTemp)
			{
				Node* deleteNode = deleteIter.pNode;
				deleteIter++;
				delete deleteNode;

				this->numOfNode--;
			}

			return list<T>::iterator(rTemp);
		}

		template<typename T>
		inline typename list<T>::iterator list<T>::erase_with_scalar_delete(const iterator& begin, const iterator& end)
		{
			Node* lTemp = begin.pNode->prev;
			Node* rTemp = end.pNode;

			lTemp->next = rTemp;
			rTemp->prev = lTemp;

			list<T>::iterator deleteIter(begin);

			while (deleteIter.pNode != rTemp)
			{
				Node* deleteNode = deleteIter.pNode;
				T deleteData = deleteIter.pNode->data;
				deleteIter++;
				delete deleteData;
				delete deleteNode;

				this->numOfNode--;
			}

			return list<T>::iterator(rTemp);
		}

		template<typename T>
		inline typename list<T>::iterator list<T>::erase_with_vector_delete(const iterator& begin, const iterator& end)
		{
			Node* lTemp = begin.pNode->prev;
			Node* rTemp = end.pNode;

			lTemp->next = rTemp;
			rTemp->prev = lTemp;

			list<T>::iterator deleteIter(begin);

			while (deleteIter.pNode != rTemp)
			{
				Node* deleteNode = deleteIter.pNode;
				T deleteData = deleteIter.pNode->data;
				deleteIter++;
				delete[] deleteData;
				delete deleteNode;

				this->numOfNode--;
			}

			return list<T>::iterator(rTemp);
		}

		template<typename T>
		inline void list<T>::remove(const T& data)
		{
			list<T>::iterator iter = this->begin();

			while (iter != this->end())
			{
				if (*iter == data)
					iter = this->erase(iter);
				else
					iter++;
			}
		}

		template<typename T>
		inline void list<T>::remove_with_scalar_delete(const T& data)
		{
			list<T>::iterator iter = this->begin();

			while (iter != this->end())
			{
				if (*iter == data)
					iter = this->erase_with_scalar_delete(iter);
				else
					iter++;
			}
		}

		template<typename T>
		inline void list<T>::remove_with_vector_delete(const T& data)
		{
			list<T>::iterator iter = this->begin();

			while (iter != this->end())
			{
				if (*iter == data)
					iter = this->erase_with_vector_delete(iter);
				else
					iter++;
			}
		}

		// Resource management object for scalar dynamic object list
		template<typename T>
		class safe_delete_scalar_dynamic_object_list
		{
		public:
			safe_delete_scalar_dynamic_object_list(const list<T>* pList);
			~safe_delete_scalar_dynamic_object_list();
		private:
			list<T>* pList;
		};

		template<typename T>
		inline safe_delete_scalar_dynamic_object_list<T>::safe_delete_scalar_dynamic_object_list(const list<T>* pList)
		{
			this->pList = pList;
		}

		template<typename T>
		inline safe_delete_scalar_dynamic_object_list<T>::~safe_delete_scalar_dynamic_object_list()
		{
			for (typename list<T>::iterator iter = pList->begin(); iter != pList->end(); iter++)
			{
				delete (*iter);
			}
		}

		// Resource management object for vector dynamic object list
		template<typename T>
		class safe_delete_vector_dynamic_object_list
		{
		public:
			safe_delete_vector_dynamic_object_list(const list<T>* pList);
			~safe_delete_vector_dynamic_object_list();
		private:
			list<T>* pList;
		};

		template<typename T>
		inline safe_delete_vector_dynamic_object_list<T>::safe_delete_vector_dynamic_object_list(const list<T>* pList)
		{
			this->pList = pList;
		}

		template<typename T>
		inline safe_delete_vector_dynamic_object_list<T>::~safe_delete_vector_dynamic_object_list()
		{
			for (typename list<T>::iterator iter = pList->begin(); iter != pList->end(); iter++)
			{
				delete[](*iter);
			}
		}
	}
}
