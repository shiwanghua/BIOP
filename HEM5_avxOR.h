//#pragma once
#ifndef _HEM5_256OR_H
#define _HEM5_256OR_H

#include <cstring>
#include "Util.h"
#include "constant.h"
#include <algorithm>
#include <unordered_set>
#include <bitset>

#define _for(i,a,b) for( int i=(a); i<(b); ++i)
#define __for(i,a,b) for( int i=(a); i<=(b); ++i)
#define mfor(i,a,b) for(int i=(a);i>(b);--i)
#define mmfor(i,a,b) for(int i=(a);i>=(b);--i)

// ����ģʽ ��avx2 ��256λ�Ļ�����
// ���ں� HEM5 �Ա�
class HEM5_avxOR
{
private:
	int numSub, numDimension, buckStep, numBits;
	vector<vector<vector<Combo>>> data[2];  // 0:left parenthesis, 1:right parenthesis
	vector<vector<bitset<subs>>> bits[2];   // ��Ҫ��ǰ֪�����ĸ���...
	vector<vector<int>> fix[2];             // 0��low�ϵĺ�׺�ͣ�1��high�ϵ�ǰ׺�ͣ��������ڼ���������
	vector<bitset<subs>> fullBits;          // ȫ���ǵ�bits�����棬��ΪֻҪ��һ��
	int** endBucket[2], ** bitsID[2];       // �������bucket���¼��ڱ��ʱ��ֹ����һ��bucket���õ���bits������±�
	bool** doubleReverse[2];                // Ϊtrueʱ�ǰ�1���0

public:
	int numBucket;
	double compareTime = 0.0;               // ����ά�����¼�ֵ������Ǹ�cell�������ȷ�Ƚϵ�ʱ��
	double markTime = 0.0;                  // ���ʱ��
	double orTime = 0.0;                    // ������ʱ��
	double bitTime = 0.0;                   // ����bits����õ���������ʱ��
	//vector<unordered_set<int>> bucketSub;   // id��ͬ��Ͱ�洢�Ĳ�ͬ���ĸ����ĺ�

	HEM5_avxOR();
	~HEM5_avxOR();

	//void insert(Sub sub);
	void insert(IntervalSub sub); // û��bitsetʱ�Ĳ����㷨
	void insert_online(IntervalSub sub); // �����ö��ļ�������߲��붩���㷨
	bool deleteSubscription(IntervalSub sub);
	//void match(const Pub& pub, int& matchSubs, const vector<Sub>& subList);
	void match(const Pub& pub, int& matchSubs);
	void match_debug(const Pub& pub, int& matchSubs);

	void initBits();      // ��������ʼ��bits����
	//void calBucketSize(); // ����bucketSize
	int calMemory();      // ����ռ���ڴ��С
	void printRelation(int dimension_i); // ��ӡӳ���ϵ
	vector<int> calMarkNumForBuckets(); // �����¼��䵽ÿ�����Ե�ͬһ��Ͱ��ʱ��Ҫ��ǺͱȽϵ�ν�ʸ���
};

#endif