#ifdef HEAD_H
#define HEAD_H
#endif // HEAD_H

#include<iostream>
#include<bitset>
#include<string>
#include<fstream>
#include<exception>
using namespace std;


void Assert(bool val, string s) {
	if (!val) {
		cout << s << endl;
		exit(-1);
	}
}

template<typename Key, typename E>class KVpair {
private:
	Key k;
	E e;
public:
	KVpair() {}
	KVpair(Key kval, E eval) {
		k = kval;
		e = eval;
	}
	KVpair(const KVpair& o) {
		k = o.k;
		e = o.e;
	}

	void operator = (const KVpair& o) {
		k = o.k;
		e = o.e;
	}

	Key key() const { return k; }
	void setKey(Key newKey) { k = newKey; }
	E value() const { return e; }
	void setValue(E newValue) { e = newValue; }
};

template<typename E>class heap {
private:
	E* Heap;
	int maxSize;
	int n;

	void siftdown(int pos) {
		while (!isLeaf(pos)) {
			int j = leftchild(pos); int rc = rightchild(pos);
			if (rc<n&&Heap[rc]->weight()<Heap[j]->weight())
				j = rc;
			if (Heap[pos]->weight() < Heap[j]->weight())
				return;
			E temp;
			temp = Heap[pos];
			Heap[pos] = Heap[j];
			Heap[j] = temp;
			pos = j;
		}
	}

public:
	heap(E* h, int num, int max) {
		Heap = h;
		maxSize = max; n = num;
		buildHeap();
	}

	int size() const { return n; }

	bool isLeaf(int pos) const { return (pos < n) && (pos >= n / 2); }

	int leftchild(int pos) const { return 2 * pos + 1; }

	int rightchild(int pos) const { return 2 * pos + 2; }

	int parent(int pos) const { return (pos - 1) / 2; }

	void buildHeap() {
		for (int i = n / 2 - 1; i >= 0; i--)
			siftdown(i);
	}

	void insert(const E&it) {
		Assert(n < maxSize, "Heap is full");
		int curr = n++;
		Heap[curr] = it;
		while (curr != 0 && Heap[curr]->weight() < Heap[parent(curr)]->weight()) {
			E temp;
			temp = Heap[curr];
			Heap[curr] = Heap[parent(curr)];
			Heap[parent(curr)] = temp;
			curr = parent(curr);
		}
	}

	E removefirst() {
		Assert(n > 0, "Heap is empty");
		E temp;
		temp = Heap[0];
		Heap[0] = Heap[n - 1];
		Heap[--n] = temp;
		if (n != 0) siftdown(0);
		return Heap[n];
	}

	E remove(int pos) {
		Assert((pos >= 0) && (pos < n), "Bad position");
		if (pos == n - 1) n--;
		else {
			int temp;
			temp = Heap[pos];
			Heap[pos] = Heap[n - 1];
			Heap[--n] = temp;
			while (pos != 0 && Heap[pos]->weight() < Heap[parent(pos)]->weight()) {
				temp = Heap[pos];
				Heap[pos] = Heap[parent(pos)];
				Heap[parent(pos)] = temp;
				pos = parent(pos);
			}
			if (n != 0) siftdown(pos);
		}
		return Heap[n];
	}
};

template<typename E>class HuffNode {
public:
	virtual ~HuffNode() {}

	virtual int weight() = 0;

	virtual bool isLeaf() = 0;
};

template<typename E>class LeafNode :public HuffNode<E> {
private:
	E it;
	int wgt;
public:
	LeafNode(const E&eval, const int freq) {
		it = eval;
		wgt = freq;
	}

	int weight() { return wgt; }
	E val() { return it; }
	bool isLeaf() { return true; }
};

template<typename E>class InitNode :public HuffNode<E> {
private:
	HuffNode<E>* lc;
	HuffNode<E>* rc;
	int wgt;
public:
	InitNode(HuffNode<E>* l, HuffNode<E>* r) {
		if (l == NULL&&r == NULL) {
			wgt = 0;
		}
		else {
			wgt = l->weight() + r->weight();
		}
		lc = l;
		rc= r;
	}

	int weight() { return wgt; }
	bool isLeaf() { return false; }

	HuffNode<E>* left() const { return lc; }

	void setLeft(HuffNode<E>* b) { lc = (HuffNode<E>*)b; }

	HuffNode<E>* right() const { return rc; }

	void setRight(HuffNode<E>* b) { rc = (HuffNode<E>*)b; }
};

template<typename E>class HuffTree {
private:
	HuffNode<E>* Root;
public:
	HuffTree(E& val, int freq) {
		Root = new LeafNode<E>(val, freq);
	}

	HuffTree(HuffTree<E>* l, HuffTree<E>* r) {
		Root = new InitNode<E>(l->root(), r->root());
	}

	~HuffTree() {}

	HuffNode<E>* root() { return Root; }

	int weight() {
		return Root->weight();
	}
};
template<typename E>
HuffTree<E>* buildHuff(HuffTree<E>** TreeArray, int count) {
	heap<HuffTree<E>*>* forest = new heap<HuffTree<E>*>(TreeArray, count, count);

	HuffTree<E>* temp1, *temp2, *temp3 = NULL;
	while (forest->size() != 1) {
		temp1 = forest->removefirst();
		temp2 = forest->removefirst();
		temp3 = new HuffTree<E>(temp1, temp2);
		forest->insert(temp3);
		delete temp1;
		delete temp2;
	}
	return temp3;
}
#pragma once
