//
// Created by swh on 2021/11/24.
//

#ifndef MAC_PREIN_H
#define MAC_PREIN_H

#include <cstring>
#include "util.h"
#include "constant.h"
#include "omp.h"
#include <algorithm>
#include <unordered_set>
#include <thread>

#define _for(i,a,b) for( int i=(a); i<(b); ++i)
#define __for(i,a,b) for( int i=(a); i<=(b); ++i)
#define mfor(i,a,b) for(int i=(a);i>(b);--i)
#define mmfor(i,a,b) for(int i=(a);i>=(b);--i)

struct parallelData{
	bool* bits;
	bool* attExist;
	vector<vector<vector<Combo>>>* data;
	const Pub* pub;
	int begin;
	int end;
	int buckStep;
};

void pReinThreadFunction(bool bits[],bool attExist[],vector<vector<vector<Combo>>> data[],const Pub& pub,int begin,int end,int buckStep);

// Parallel Rein in openmp
class pRein {
	int numSub, numDimension, buckStep,pD;    // parallelDegree
	vector<vector<vector<Combo>>> data[2];    // 0:left parenthesis, 1:right parenthesis

public:
	int numBucket;
	double compareTime = 0.0; // ����ά�����¼�ֵ������Ǹ�cell�������ȷ�Ƚϵ�ʱ��
	double markTime = 0.0;    // ���ʱ��
	double bitTime = 0.0;     // ����bits����õ���������ʱ��
	vector<unordered_set<int>> bucketSub;   // id��ͬ��Ͱ�洢�Ĳ�ͬ���ĸ����ĺ�

	pRein();
	//void insert(Sub sub);
	void insert(IntervalSub sub);
	//void match(const Pub& pub, int& matchSubs, const vector<Sub>& subList);
	void match(const Pub& pub, int& matchSubs);
	void parallelMatch(const Pub& pub, int& matchSubs);
	bool deleteSubscription(IntervalSub sub);
	void calBucketSize(); // ����bucketSize
	int calMemory();     // ����ռ���ڴ��С, ����MB
};

#endif //MAC_PREIN_H
