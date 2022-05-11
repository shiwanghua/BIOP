#pragma once

#include <cstring>
#include "Util.h"
#include "constant.h"
#include <algorithm>
#include <unordered_set>
#include <bitset>
#include "ThreadPool.h"

// Virtual/Real Attribute Group Mode
class HEM5_AS {
private:
	int numSub, numDimension, buckStep, numBits, numAttrGroup, attrGroupSize;
	vector<vector<vector<Combo>>> data[2];  // 0:left parenthesis, 1:right parenthesis
	vector<vector<bitset<subs>>> bits[2];   // ��Ҫ��ǰ֪�����ĸ���...
	vector<vector<int>> fix[2];             // 0��low�ϵĺ�׺�ͣ�1��high�ϵ�ǰ׺�ͣ��������ڼ���������
	vector<bitset<subs>> fullBits;          // ȫ���ǵ�bits�����棬��ΪֻҪ��һ��
	vector<bitset<subs>> attrGroupBits;     // ÿ��ʵor���������Ӧһ��bitset
	int **endBucket[2], **bitsID[2];       // �������bucket���¼��ڱ��ʱ��ֹ����һ��bucket���õ���bits������±�
	bool **doubleReverse[2];                // Ϊtrueʱ�ǰ�1���0
	ThreadPool threadPool;
public:
	int numBucket;
	double compareTime = 0.0;               // ����ά�����¼�ֵ������Ǹ�cell�������ȷ�Ƚϵ�ʱ��
	double markTime = 0.0;                  // ���ʱ��
	double orTime = 0.0;                    // ������ʱ��
	double bitTime = 0.0;                   // ����bits����õ���������ʱ��
	double mergeTime = 0.0;
	//vector<unordered_set<int>> bucketSub;   // id��ͬ��Ͱ�洢�Ĳ�ͬ���ĸ����ĺ�

	HEM5_AS(int);

	~HEM5_AS();

	void insert_VAS(IntervalSub sub); // û��bitsetʱ�Ĳ����㷨
	void insert_RAS(IntervalSub sub); // û��bitsetʱ�Ĳ����㷨

	void initBits();      // ��������ʼ��bits����

	void insert_online_VAS(IntervalSub sub); // �����ö��ļ�������߲��붩���㷨, ����������汾
	void insert_online_RAS(IntervalSub sub); // �����ö��ļ�������߲��붩���㷨, ʵ��������汾

	bool deleteSubscription_VAS(IntervalSub sub);

	bool deleteSubscription_RAS(IntervalSub sub);

	void match_VAS(const Pub &pub, int &matchSubs);

	void match_RAS(const Pub &pub, int &matchSubs);

	void match_RAS_avxOR_parallel(const Pub &pub, int &matchSubs);

	//void calBucketSize(); // ����bucketSize
	int calMemory();      // ����ռ���ڴ��С
	void printRelation(int dimension_i); // ��ӡӳ���ϵ
	vector<int> calMarkNumForBuckets(); // �����¼��䵽ÿ�����Ե�ͬһ��Ͱ��ʱ��Ҫ��ǺͱȽϵ�ν�ʸ���
};
