// MyMap.h
#include "support.h"

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.

template<typename KeyType, typename ValueType>
class MyMap
{
public:
	MyMap();
	~MyMap();
	void clear();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	  // for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	  // for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

	  // C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;

private:
	struct Node
	{
		KeyType key;
		ValueType value;
		Node* left;
		Node* right;
	};

	Node* createNode(const KeyType& key, const ValueType& value)
	{
		Node *n = new Node;
		n->key = key;
		n->value = value;
		n->left = nullptr;
		n->right = nullptr;
		m_size++;
		return n;
	}

	Node* findNode(Node* p, const KeyType& key) const
	{
		if (p == nullptr)
			return nullptr;
		if (p->key == key)
			return p;
		if (key < p->key)
			return findNode(p->left, key);
		if (key > p->key)
			return findNode(p->right, key);
		return nullptr;
	}

	void FreeTree(Node* p)
	{
		if (p == nullptr)
			return;

		FreeTree(p->left);
		FreeTree(p->right);

		delete p;
	}

	void addNode(Node* p, const KeyType& key, const ValueType& value)
	{
		if (p->key == key)
		{
			p->value = value;
			return;
		}
		
		if (key < p->key)
		{
			if (p->left != nullptr)
			{
				addNode(p->left, key, value);
				return;
			}
			Node *n = createNode(key, value);
			p->left = n;
			return;
		}

		if (key > p->key)
		{
			if (p->right != nullptr)
			{
				addNode(p->right, key, value);
				return;
			}
			Node *n = createNode(key, value);
			p->right = n;
			return;
		}

	}

	Node* m_head;
	int m_size;
};

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::MyMap()
{
	m_head = nullptr;
	m_size = 0;
	return;
}

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::~MyMap()
{
	clear();
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clear()
{
	FreeTree(m_head);
	m_head = nullptr;
}

template<typename KeyType, typename ValueType>
int MyMap<KeyType, ValueType>::size() const
{
	return m_size;
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
	if (m_head == nullptr)
	{
		m_head = createNode(key, value);
		return;
	}

	addNode(m_head, key, value);
}

template<typename KeyType, typename ValueType>
const ValueType* MyMap<KeyType, ValueType>::find(const KeyType& key) const
{
	if (m_head == nullptr)
		return nullptr;

	Node* p = findNode(m_head, key);
	if (p == nullptr)
	{
		return nullptr;
	}
	return &(p->value);

}
