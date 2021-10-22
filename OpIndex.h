#ifndef _OPINDEX_H
#define _OPINDEX_H
#include <ctime>
#include "data_structure.h"
#include "generator.h"
#include "printer.h"
#include "chrono_time.h"
#include <string>
#include <cstring>
#include"constant.h"

//const int MAX_ATTS = 101;
//const int MAX_SUBS = 2000001;
//const int SEGMENTS = 32;
//const int MAX_SIGNATURE = 61;

//class OpIndex {
//	// ��ʵ��Щ�����Ա�ɶ�̬��, ���������ֵ�Ͳ���Ҫд��Ϊ������
//	vector<ConElement> data[MAX_ATTS][3][SEGMENTS][MAX_SIGNATURE];
//	bool sig[MAX_ATTS][3][SEGMENTS][MAX_SIGNATURE];
//	int counter[MAX_SUBS];
//	int fre[MAX_ATTS];
//
//	void initCounter(const vector<Sub>& subList);
//
//	void initCounter(const vector<IntervalSub>& subList);
//
//	int getMinFre(Sub x);
//
//	int getMinFre(IntervalSub x);
//
//	int signatureHash1(int att, int val);       // for == operation
//	int signatureHash2(int att);                // for <= and >= operation
//public:
//	int numSub;
//	bool isPivot[MAX_ATTS];
//
//	OpIndex() :numSub(0)
//	{
//		memset(isPivot, 0, sizeof(isPivot));
//		memset(sig, 0, sizeof(sig));
//	}
//
//	void calcFrequency(const vector<Sub>& subList);
//
//	void calcFrequency(const vector<IntervalSub>& subList);
//
//	void insert(Sub x);
//
//	void insert(IntervalSub x);
//	bool deleteSubscription(IntervalSub sub);
//
//	void match(Pub x, int& matchSubs, const vector<Sub>& subList);
//
//	void match(Pub x, int& matchSubs, const vector<IntervalSub>& subList);
//
//	int calMemory();      // ����ռ���ڴ��С, ����MB
//};

class OpIndex2 {
	vector<vector<vector<IntervalCombo>>> data;
	vector<int> fre;
	vector<int> pivotCount; // record how many subs in this pivot attribute, use for deletion decision

	int getMinFre(Sub x);
	int getMinFre(IntervalSub x);

public:
	int numSub;
	vector<bool> isPivot;
	
	OpIndex2();
	~OpIndex2();

	void calcFrequency(const vector<Sub>& subList);
	void calcFrequency(const vector<IntervalSub>& subList);

	// void insert(Sub x);
	void insert(IntervalSub x);
	bool deleteSubscription(IntervalSub sub);

	// void match(Pub x, int& matchSubs, const vector<Sub>& subList);
	void match(Pub x, int& matchSubs, const vector<IntervalSub>& subList);

	int calMemory();      // ����ռ���ڴ��С, ����MB
};

#endif
