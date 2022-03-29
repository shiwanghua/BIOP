//#pragma once
#ifndef _REIN_BITS1_H
#define _REIN_BITS1_H

#include <cstring>
#include "util.h"
#include "constant.h"
#include <algorithm>
#include <unordered_set>
#include <bitset>

#define _for(i,a,b) for( int i=(a); i<(b); ++i)
#define __for(i,a,b) for( int i=(a); i<=(b); ++i)
#define mfor(i,a,b) for(int i=(a);i>(b);--i)
#define mmfor(i,a,b) for(int i=(a);i>=(b);--i)

// �ھ�̬HEM�Ļ����ϼ��Ͼ�̬���ط���(��1Ϊ0)
class HEM1
{
private:
	int numSub, numDimension, buckStep, numBits, bitStep; // ��ǰ���bits�����඼��bitStep������Ķ��������һ��bits����
	vector<vector<vector<Combo>>> data[2];  // 0:left parenthesis, 1:right parenthesis
	vector<vector<bitset<subs>>> bits[2];   // ��Ҫ��ǰ֪�����ĸ���...
	vector<bitset<subs>> fullBits;          // ȫ���ǵ�bits�����棬��ΪֻҪ��һ��
	//vector<vector<int>> fix[2];             // 0��low�ϵĺ�׺�ͣ�1��high�ϵ�ǰ׺�ͣ��������ڼ���������
	int* endBucket[2], * bitsID[2];       // �������bucket���¼��ڱ��ʱ��ֹ����һ��bucket���õ���bits������±�
	bool* doubleReverse[2];                // Ϊtrueʱ�ǰ�1���0
public:
	int numBucket;
	double compareTime = 0.0;               // ����ά�����¼�ֵ������Ǹ�cell�������ȷ�Ƚϵ�ʱ��
	double markTime = 0.0;                  // ���ʱ��
	double orTime = 0.0;                    // ������ʱ��
	double bitTime = 0.0;                   // ����bits����õ���������ʱ��
	//vector<unordered_set<int>> bucketSub; // id��ͬ��Ͱ�洢�Ĳ�ͬ���ĸ����ĺ�

	HEM1();
	~HEM1();

	//void insert(Sub sub);
	void insert(IntervalSub sub);
	//void match(const Pub& pub, int& matchSubs, const vector<Sub>& subList);
	void match(const Pub& pub, int& matchSubs);
	void match_debug(const Pub& pub, int& matchSubs);

	void initBits();      // ��������ʼ��bits����
	//void calBucketSize(); // ����bucketSize
	int calMemory();      // ����ռ���ڴ��С
	void printRelation(); // ��ӡӳ���ϵ
};

#endif