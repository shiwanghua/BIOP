#pragma once

#include <cstring>
#include "util.h"
#include "constant.h"
#include <algorithm>
#include <unordered_set>
#include <bitset>

// Virtual/Real Attribute Group Mode
class HEM5_AG
{
private:
	int numSub, numDimension, buckStep, numBits, numAttrGroup, attrGroupSize;
	vector<vector<vector<Combo>>> data[2];  // 0:left parenthesis, 1:right parenthesis
	vector<vector<bitset<subs>>> bits[2];   // ��Ҫ��ǰ֪�����ĸ���...
	vector<vector<int>> fix[2];             // 0��low�ϵĺ�׺�ͣ�1��high�ϵ�ǰ׺�ͣ��������ڼ���������
	vector<bitset<subs>> fullBits;          // ȫ���ǵ�bits�����棬��ΪֻҪ��һ��
	vector<bitset<subs>> attrGroupBits;     // ÿ��ʵor���������Ӧһ��bitset
	int** endBucket[2], ** bitsID[2];       // �������bucket���¼��ڱ��ʱ��ֹ����һ��bucket���õ���bits������±�
	bool** doubleReverse[2];                // Ϊtrueʱ�ǰ�1���0

public:
	int numBucket;
	double compareTime = 0.0;               // ����ά�����¼�ֵ������Ǹ�cell�������ȷ�Ƚϵ�ʱ��
	double markTime = 0.0;                  // ���ʱ��
	double orTime = 0.0;                    // ������ʱ��
	double bitTime = 0.0;                   // ����bits����õ���������ʱ��
	//vector<unordered_set<int>> bucketSub;   // id��ͬ��Ͱ�洢�Ĳ�ͬ���ĸ����ĺ�

	HEM5_AG(int);
	~HEM5_AG();

	void insert_VAG(IntervalSub sub); // û��bitsetʱ�Ĳ����㷨
	void insert_RAG(IntervalSub sub); // û��bitsetʱ�Ĳ����㷨

	void initBits();      // ��������ʼ��bits����

	void insert_online_VAG(IntervalSub sub); // �����ö��ļ�������߲��붩���㷨, ����������汾
	void insert_online_RAG(IntervalSub sub); // �����ö��ļ�������߲��붩���㷨, ʵ��������汾

	bool deleteSubscription_VAG(IntervalSub sub);
	bool deleteSubscription_RAG(IntervalSub sub);

	void match_VAG(const Pub& pub, int& matchSubs);
	void match_RAG(const Pub& pub, int& matchSubs);

	//void calBucketSize(); // ����bucketSize
	int calMemory();      // ����ռ���ڴ��С
	void printRelation(int dimension_i); // ��ӡӳ���ϵ
	vector<int> calMarkNumForBuckets(); // �����¼��䵽ÿ�����Ե�ͬһ��Ͱ��ʱ��Ҫ��ǺͱȽϵ�ν�ʸ���
};
