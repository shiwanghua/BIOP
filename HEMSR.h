//#pragma once
#ifndef HEMSR_H
#define HEMSR_H

#include <cstring>
#include "util.h"
#include "constant.h"
#include <algorithm>
#include <unordered_set>
#include <bitset>
//#define NDEBUG  // ����assert����, ���ٿ���
#include <assert.h>

#define _for(i,a,b) for( int i=(a); i<(b); ++i)
#define __for(i,a,b) for( int i=(a); i<=(b); ++i)
#define mfor(i,a,b) for(int i=(a);i>(b);--i)
#define mmfor(i,a,b) for(int i=(a);i>=(b);--i)

// ���ڴ���ģʽ��״̬ѹ���Ż� State Reduction
class HEMSR
{
private:
	int numSub, numDimension;
	int numGroup, numState;                 // �ж��ٸ���, һ�������ж��ٸ�״̬
	int buckStep, bitStep;                  // ��ǰ���bits�����඼��bitStep������Ķ��������һ��bits����
	vector<vector<vector<Combo>>> data[2];  // 0:left parenthesis, 1:right parenthesis
	vector<bitset<subs>> bits[2];           // ÿ��ά��������bitset
	vector<vector<bitset<subs>>> bitsSR;    // �ڼ��������ϵĵڼ���״̬
	//vector<bitset<subs>> fullBits;        // ȫ���ǵ�bits�����棬��ΪֻҪ��һ��
	int* endBucket[2], * bitsID[2];         // �������bucket���¼��ڱ��ʱ��ֹ����һ��bucket���õ���bits������±�
	vector<int> fix[2];                     // 0��low�ϵĺ�벿�ֵĺͣ�1��high�ϵ�ǰ�벿�ֺͣ��������ڼ���������
public:
	int numBucket;
	double compareTime = 0.0;               // ����ά�����¼�ֵ������Ǹ�cell�������ȷ�Ƚϵ�ʱ��
	double markTime = 0.0;                  // ���ʱ��
	double orTime = 0.0;                    // ������ʱ��
	double bitTime = 0.0;                   // ����bits����õ���������ʱ��
	//vector<unordered_set<int>> bucketSub;   // id��ͬ��Ͱ�洢�Ĳ�ͬ���ĸ����ĺ�

	HEMSR();
	~HEMSR();

	//void insert(Sub sub);
	void insert(IntervalSub sub);
	//void insert_online(IntervalSub sub);
	//void match(const Pub& pub, int& matchSubs, const vector<Sub>& subList);
	void match(const Pub& pub, int& matchSubs);

	void initBits();      // ��������ʼ��bits����
	//void calBucketSize(); // ����bucketSize
	int calMemory();      // ����ռ���ڴ��С
	void printRelation(); // ��ӡӳ���ϵ
};

#endif