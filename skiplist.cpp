// ���ļ����ڲ���skiplist, �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "skiplist.h"
#include "time.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	cout << "testing is strating......" << endl;

	SkipList list;

	// ���Բ���
	for (int i = 0; i < 100; ++i) {
		list.Insert(i, i);
		//cout << list.GetCurrentLevel() << enld;
	}
	cout << "The number of elements in SkipList is :"
		<< list.size() << endl;

	if (list.size != 100) {
		cout << "Insert failure." << endl;
	}
	else {
		cout << "Insert success." << endl;
	}

	// ���Բ���
	bool is_search_success = true;
	for (int i = 0; i < 100; ++i) {
		int value;
		if (!(list.Search(i, value) && (value == i + 10))) {
			is_search_success = false;
			break;
		}
	}
	if (is_search_success) {
		cout << "Search success." << endl;
	}
	else {
		cout << "Search failure." << endl;
	}

	//����ɾ��  
	bool is_delete_success = true;
	for (int i = 0; i < 100; ++i) {
		int value;
		if (!(list.Delete(i, value) && (value == i + 10))) {
			is_delete_success = false;
			break;
		}
	}
	if (is_delete_success) {
		cout << "Delete success." << endl;
	}
	else {
		cout << "Delete failure." << endl;
	}

	cout << "test is finished ...." << endl;

	return 0;
}

void DebugOutput(const string& information) {
#ifdef Debug
	cout << information << endl;
#endif
}

void SkipList::NewList() {
	// ����NIL�ڵ�
	NewNodeWithLevel(0, NIL_);
	NIL_->key = 0x7fffffff;

	// ��������List
	list_ = (List)malloc(sizeof(listStructure));
	list_->level = 0;

	// ����ͷ�ڵ�
	NewNodeWithLevel(MAX_LEVEL, list_->header);
	for (int i = 0; i < MAX_LEVEL; i++) {
		list_->header->forward[i] = NIL_;
	}

	// ��������Ԫ����Ŀ
	size_ = 0;
}

void SkipList::NewNodeWithLevel(const int& level, Node& node) {
	// �½ڵ�ռ��С
	int total_size = sizeof(nodeSrtucture) + level * sizeof(Node);
	// ����ռ�
	node = (Node)malloc(total_size);
	assert(node != NULL);
}

void SkipList::FreeList() {
	Node p = list_->header;
	Node q;
	while (p != NIL_) {
		q = p->forward[0];
		free(p);
		p = q;
	}
	free(p);
	free(list_);
}

bool SkipList::Search(const KeyType& key, ValueType& value) {
	Node x = list_->header;
	int i;
	for (i = list_->level; i >= 0; --i) {
		while (x->forward[i]->key < key) {
			x = x->forward[i];
		}
	}

	x = x->forward[0];
	if (x->key == key) {
		value = x->value;
		return true;
	}
	else {
		return false;
	}
}

bool SkipList::Insert(const KeyType& key, ValueType& value) {
	Node update[MAX_LEVEL];
	int i;
	Node x = list_->header;

	// Ѱ��key��Ҫ�����λ��
	// �����Լkey��λ��
	for (i = list_->level; i >= 0; --i) {
		while (x->forward[i]->key < key) {
			x = x->forward[i];
		}
		update[i] = x;
	}

	x = x->forward[0];

	// ���key�Ѿ�����
	if (x->key = key) {
		x->value = value;
		return false;
	}
	else {
		// ��������½ڵ�Ĳ���
		int level = Randomlevel();
		// ���������ɵĽڵ����level���ڵ�ǰ�����������
		// Ϊ�˽�ʡ�ռ䣬���ñȵ�ǰ������+1�Ĳ���
		if (level > list_->level) {
			level = ++list_->level;
			update[level] = list_->header;
		}

		// �����½ڵ�
		Node newNode;
		NewNodeWithLevel(level, newNode);
		newNode->key = key;
		newNode->value = value;

		// ����forwardָ��
		for (int i = level; i >= 0; --i) {
			x = update[i];
			newNode->forward[i] = x->forward[i];
			x->forward[i] = newNode;
		}

		// ��������Ԫ����Ŀ
		++size_;

		return true;
	}
}

bool SkipList::Delete(const KeyType& key, ValueType& value) {
	Node update[MAX_LEVEL];
	int i;
	Node x = list_->header;

	// Ѱ��Ҫɾ���Ľڵ�
	for (i = list_->level; i >= 0; --i) {
		while (x->forward[i]->key < key) {
			x = x->forward[i];
		}
		update[i] = x;
	}

	x = x->forward[0];
	// �ڵ㲻����
	if (x->key != key) {
		return false;
	}
	else {
		x->value = value;
		// ����ָ��
		for (i = 0; i <= list_->level; ++i) {
			if (update[i]->forward[i] != x)
				break;
			update[i]->forward[i] = x->forward[i];
		}

		// ɾ���ڵ�
		free(x);
		// ����level��ֵ���п��ܻ�仯����ɿռ���˷�
		while (list_->level > 0
			&& list_->header->forward[list_->level] == NIL_) {
			--list_->level;
		}

		// ��������Ԫ����Ŀ
		--size_;

		return true;
	}
}

int SkipList::Randomlevel() {
	int level = static_cast<int>(rnd_.Uniform(MAX_LEVEL));
	if (level == 0) {
		level = 1;
	}
	// cout << level << endl;
	return list_->level;
}

int SkipList::GetCurrentLevel() {
	return list_->level;
}

