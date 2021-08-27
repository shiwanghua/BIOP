#ifndef _REIN_H
#define _REIN_H
#include <cstring>
#include "util.h"
#include <algorithm>
#include <unordered_set>

#define _for(i,a,b) for( int i=(a); i<(b); ++i)
#define __for(i,a,b) for( int i=(a); i<=(b); ++i)
#define mfor(i,a,b) for(int i=(a);i>(b);--i)
#define mmfor(i,a,b) for(int i=(a);i>=(b);--i)

const int MAX_SUBS = 2000001;
const int MAX_ATTS = 101;
const int MAX_BUCKS = 500;

// 01�ڵ�һά
class Rein {
	int numSub, numDimension, buckStep;
	vector<vector<vector<Combo>>> data[2];    // 0:left parenthesis, 1:right parenthesis

public:
	int numBucket;
	double compareTime = 0.0; // ����ά�����¼�ֵ������Ǹ�cell�������ȷ�Ƚϵ�ʱ��
	double markTime = 0.0;    // ���ʱ��
	double bitTime = 0.0;     // ����bits����õ���������ʱ��
	vector<unordered_set<int>> bucketSub;   // id��ͬ��Ͱ�洢�Ĳ�ͬ���ĸ����ĺ�

	Rein(int valDom, int numSubscription, int numDim, int numBuck) :numSub(numSubscription), numDimension(numDim) {
		buckStep = (valDom - 1) / numBuck + 1;
		numBucket = (valDom - 1) / buckStep + 1;
		cout << "Rein: bucketStep = " << buckStep << ", numBucket = " << numBucket << endl;
		bucketSub.resize(numBucket);
		data[0].resize(numDimension, vector<vector<Combo>>(numBucket));
		data[1].resize(numDimension, vector<vector<Combo>>(numBucket));
	}
	//void insert(Sub sub);
	void insert(IntervalSub sub);
	//void match(const Pub& pub, int& matchSubs, const vector<Sub>& subList);
	void match(const Pub& pub, int& matchSubs);
	void calBucketSize(); // ����bucketSize
	int calMemory();     // ����ռ���ڴ��С
};

// 01�ڵڶ�ά��ȫ��̬
//class Rein {
//	int numSub, numDimension, buckStep, numBucket; // ά�ȣ�Ͱ���ȣ�Ͱ��
//	vector<vector<vector<vector<Combo>>>> data;    // 0:left parenthesis, 1:right parenthesis
//
//public:
//	Rein(int valDom, int numSubscription, int numDim, int numBuck) :numSub(numSubscription), numDimension(numDim) {
//		buckStep = (valDom - 1) / numBuck + 1;
//		numBucket = (valDom - 1) / buckStep + 1;
//		cout << "Rein: bucketStep = " << buckStep << ", numBucket = " << numBucket << endl;
//		data.resize(numDimension, vector<vector<vector<Combo>>>(2, vector<vector<Combo>>(numBucket)));
//	}
//	//void insert(Sub sub);
//	void insert(IntervalSub sub);
//	//void match(const Pub& pub, int& matchSubs, const vector<Sub>& subList);
//	void match(const Pub& pub, int& matchSubs);
//};

// 01�ڵ�һά��д��
//class Rein {
//	int numSub, numDimension, buckStep, numBucket; // ά�ȣ�Ͱ���ȣ�Ͱ��
//	vector<Combo> data[MAX_ATTS][2][MAX_BUCKS];
//
//public:
//	Rein(int valDom, int numSubscription, int numDim, int numBuck) :numSub(numSubscription), numDimension(numDim) {
//		buckStep = (valDom - 1) / numBuck + 1;
//		numBucket = (valDom - 1) / buckStep + 1;
//		cout << "Rein: bucketStep = " << buckStep << ", numBucket = " << numBucket << endl;
//		//data.resize(numDimension, vector<vector<vector<Combo>>>(2, vector<vector<Combo>>(numBucket)));
//	}
//	//void insert(Sub sub);
//	void insert(IntervalSub sub);
//	//void match(const Pub& pub, int& matchSubs, const vector<Sub>& subList);
//	void match(const Pub& pub, int& matchSubs);
//};

#endif
