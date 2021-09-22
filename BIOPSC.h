#pragma once
//#ifndef _BIOPSC_H
//#define _BIOPSC_H

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

// ���ķ���+����ģʽ Subscriptions Classification
class BIOPSC
{
private:
	int numSub, numDimension, buckStep, levelStep, numBits;
	vector<vector<vector<vector<Combo>>>> data[2];  // low/high, ά��, ���, Ͱ��, Ͱ��ƫ����
	vector<vector<vector<bitset<subs>>>> bits[2];   // low/high, ά��, ���, bitset��, ����id
	vector<vector<vector<int>>> fix[2];             // low/high, ά��, ���, Ͱ��
	vector<bitset<subs>> fullBits;					// ֻ�����ڿ��¼�ά����
	vector<vector<bitset<subs>>> fullBL;			// Bug: ֻ������ÿһ���˫�ط���ƥ��!�洢ÿһ������ж���
	int*** endBucket[2], *** bitsID[2];             // low/high, ά��, ���, Ͱ��
	bool*** doubleReverse[2];                       // low/high, ά��, ���, Ͱ��

public:
	int numBucket;                          // һ������Ͱ��
	int numLevel;                           // ÿ��ά���ϵĲ���
	double compareTime = 0.0;               // ����ά�����¼�ֵ������Ǹ�cell�������ȷ�Ƚϵ�ʱ��
	double markTime = 0.0;                  // ���ʱ��
	double orTime = 0.0;                    // ������ʱ��
	double bitTime = 0.0;                   // ����bits����õ���������ʱ��
	//vector<unordered_set<int>> bucketSub;   // id��ͬ��Ͱ�洢�Ĳ�ͬ���ĸ����ĺ�

	BIOPSC();
	~BIOPSC();

	//void insert(Sub sub);
	void insert(IntervalSub sub);
	//void match(const Pub& pub, int& matchSubs, const vector<Sub>& subList);
	void match(const Pub& pub, int& matchSubs);

	void initBits();      // ��������ʼ��bits����
	//void calBucketSize(); // ����bucketSize
	int calMemory();      // ����ռ���ڴ��С
	void printRelation(int dimension_i, int li); // ��ӡӳ���ϵ
};

//#endif