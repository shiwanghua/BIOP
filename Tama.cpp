#include "tama.h"

void Tama::initiate(int p, int l, int r, int lvl)
{
	if (lvl == level)  // level ��1��ʼ
		return;
	mid[p] = median(l, r);
	if (l != r)
	{
		lchild[p] = ++nodeCounter;// ���ڵ���0, ��һ��������1
		initiate(nodeCounter, l, mid[p], lvl + 1);
		rchild[p] = ++nodeCounter;
		initiate(nodeCounter, mid[p] + 1, r, lvl + 1);
	}
}

int Tama::median(int l, int r)
{
	return (l + r) >> 1;
}

void Tama::insert(IntervalSub sub)
{
	for (int i = 0; i < sub.size; i++)
		insert(0, sub.constraints[i].att, sub.id, 0, valDom - 1, sub.constraints[i].lowValue, sub.constraints[i].highValue, 1);
}

void Tama::insert(int p, int att, int subID, int l, int r, int low, int high, int lvl)
{
	if ((low <= l && high >= r) || lvl == level)
	{
		data[att][p].push_back(subID);
		return;
	}
	if (high <= mid[p])
		insert(lchild[p], att, subID, l, mid[p], low, high, lvl + 1);
	else if (low > mid[p])
		insert(rchild[p], att, subID, mid[p] + 1, r, low, high, lvl + 1);
	else
	{
		insert(lchild[p], att, subID, l, mid[p], low, high, lvl + 1);
		insert(rchild[p], att, subID, mid[p] + 1, r, low, high, lvl + 1);
	}
}

bool Tama::deleteSubscription(IntervalSub sub) {
	bool find = 1;
	for (int i = 0; i < sub.size; i++)
		if (!deleteSubscription(0, sub.constraints[i].att, sub.id, 0, valDom - 1, sub.constraints[i].lowValue, sub.constraints[i].highValue, 1))
			find = 0;
	return find;
}

bool Tama::deleteSubscription(int p, int att, int subID, int l, int r, int low, int high, int lvl) {
	if ((low <= l && high >= r) || lvl == level)
	{
		vector<int>::iterator it;
		for (it = data[att][p].begin(); it != data[att][p].end(); it++)
			if (*it == subID) {
				data[att][p].erase(it); // it = 
				return 1;
			}
		return 0;
	}
	if (high <= mid[p])
		return deleteSubscription(lchild[p], att, subID, l, mid[p], low, high, lvl + 1);
	else if (low > mid[p])
		return deleteSubscription(rchild[p], att, subID, mid[p] + 1, r, low, high, lvl + 1);
	else
	{
		bool find = deleteSubscription(lchild[p], att, subID, l, mid[p], low, high, lvl + 1);
		if (!deleteSubscription(rchild[p], att, subID, mid[p] + 1, r, low, high, lvl + 1))
			find = 0;  // ���Ҷ������ҵ�
		return find;
	}
}
void Tama::match_accurate(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList)
{
	for (int i = 0; i < subList.size(); i++)
		counter[i] = subList[i].size;
	for (int i = 0; i < pub.size; i++)
		match_accurate(0, pub.pairs[i].att, 0, valDom - 1, pub.pairs[i].value, 1, subList);
	for (int i = 0; i < subList.size(); i++)
		if (counter[i] == 0) {
			++matchSubs;
			//cout << "tama matches sub: " << i << endl;
		}
}

void Tama::match_accurate(int p, int att, int l, int r, const int value, int lvl, const vector<IntervalSub>& subList)
{
	if (level == lvl) {
		int id;
		for (auto& id:data[att][p])
		{
			for(auto& predicate:subList[id].constraints)
				if (att == predicate.att) {
					if(predicate.lowValue<=value&&value<=predicate.highValue)
						--counter[id];
					break;
				}
		}
		return;
	}
	for (int i = 0; i < data[att][p].size(); i++)
		--counter[data[att][p][i]];
	if (l == r) // ����l�п��ܵ���r�𣿵�ȡֵ��Χ�Ƚ�С, �����ܸ�ʱ�����; ���¼�ֵ�պõ��ڱ߽�ʱҲ�����!
		return;
	else if (value <= mid[p])
		match_accurate(lchild[p], att, l, mid[p], value, lvl + 1, subList);
	else
		match_accurate(rchild[p], att, mid[p] + 1, r, value, lvl + 1, subList);
}

void Tama::match_vague(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList)
{
	for (int i = 0; i < subList.size(); i++)
		counter[i] = subList[i].size;
	for (int i = 0; i < pub.size; i++)
		match_vague(0, pub.pairs[i].att, 0, valDom - 1, pub.pairs[i].value, 1);
	for (int i = 0; i < subList.size(); i++)
		if (counter[i] == 0) {
			++matchSubs;
			//cout << "tama matches sub: " << i << endl;
		}
}

void Tama::match_vague(int p, int att, int l, int r, const int value, int lvl)
{
	for (int i = 0; i < data[att][p].size(); i++)
		--counter[data[att][p][i]];
	if (level == lvl||l == r) // ����l�п��ܵ���r�𣿵�ȡֵ��Χ�Ƚ�С, �����ܸ�ʱ�����; ���¼�ֵ�պõ��ڱ߽�ʱҲ�����!
		return;
	else if (value <= mid[p])
		match_vague(lchild[p], att, l, mid[p], value, lvl + 1);
	else
		match_vague(rchild[p], att, mid[p] + 1, r, value, lvl + 1);
}

int Tama::calMemory() {
	long long  size = 0; // Byte
	size += 3 * sizeof(int) * (1 << level);
	_for(i, 0, atts)
		_for(j, 0, 1 << level)
		size += sizeof(int) * data[i][j].size();
	size = size / 1024 / 1024; // MB
	return (int)size;
}