// 此文件用于测试skiplist, 定义控制台应用程序的入口点。
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

	// 测试插入
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

	// 测试查找
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

	//测试删除  
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
	// 设置NIL节点
	NewNodeWithLevel(0, NIL_);
	NIL_->key = 0x7fffffff;

	// 设置链表List
	list_ = (List)malloc(sizeof(listStructure));
	list_->level = 0;

	// 设置头节点
	NewNodeWithLevel(MAX_LEVEL, list_->header);
	for (int i = 0; i < MAX_LEVEL; i++) {
		list_->header->forward[i] = NIL_;
	}

	// 设置链表元素数目
	size_ = 0;
}

void SkipList::NewNodeWithLevel(const int& level, Node& node) {
	// 新节点空间大小
	int total_size = sizeof(nodeSrtucture) + level * sizeof(Node);
	// 申请空间
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

	// 寻找key所要插入的位置
	// 保存大约key的位置
	for (i = list_->level; i >= 0; --i) {
		while (x->forward[i]->key < key) {
			x = x->forward[i];
		}
		update[i] = x;
	}

	x = x->forward[0];

	// 如果key已经存在
	if (x->key = key) {
		x->value = value;
		return false;
	}
	else {
		// 随机生成新节点的层数
		int level = Randomlevel();
		// 如果随机生成的节点层数level大于当前链表的最大层数
		// 为了节省空间，采用比当前最大层数+1的策略
		if (level > list_->level) {
			level = ++list_->level;
			update[level] = list_->header;
		}

		// 申请新节点
		Node newNode;
		NewNodeWithLevel(level, newNode);
		newNode->key = key;
		newNode->value = value;

		// 调整forward指针
		for (int i = level; i >= 0; --i) {
			x = update[i];
			newNode->forward[i] = x->forward[i];
			x->forward[i] = newNode;
		}

		// 更新链表元素数目
		++size_;

		return true;
	}
}

bool SkipList::Delete(const KeyType& key, ValueType& value) {
	Node update[MAX_LEVEL];
	int i;
	Node x = list_->header;

	// 寻找要删除的节点
	for (i = list_->level; i >= 0; --i) {
		while (x->forward[i]->key < key) {
			x = x->forward[i];
		}
		update[i] = x;
	}

	x = x->forward[0];
	// 节点不存在
	if (x->key != key) {
		return false;
	}
	else {
		x->value = value;
		// 调整指针
		for (i = 0; i <= list_->level; ++i) {
			if (update[i]->forward[i] != x)
				break;
			update[i]->forward[i] = x->forward[i];
		}

		// 删除节点
		free(x);
		// 更新level的值，有可能会变化，造成空间的浪费
		while (list_->level > 0
			&& list_->header->forward[list_->level] == NIL_) {
			--list_->level;
		}

		// 更新链表元素数目
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

