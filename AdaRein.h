#ifndef ADAREIN_H
#define ADAREIN_H
#include<vector>
#include <cstring>
#include <algorithm>
#include <queue>
#include "generator.h"
#include "chrono_time.h"
#include "Util.h"
#include "data_structure.h"
#include "constant.h"

//const int MAX_SUBS = 2000001;
//const int MAX_ATTS = 3000;
//const int MAX_BUCKS = 2000;

class AdaRein {
	int buckStep, numSub;
	//int dividerValue; // SSS-C-PPH ���ֽ��

	//vector<Combo> data[MAX_ATTS][2][MAX_BUCKS];    // 0:left parenthesis, 1:right parenthesis
//    bool bits[MAX_SUBS];
//    bool skipped[MAX_ATTS];
//    attAndCount attsCounts[MAX_ATTS];

	vector<attAndCount> attsCounts;
	vector<bool> skipped;
	vector<vector<vector<Combo>>> data[2];
	vector<int> endBucket[2]; // i����������Ӧ���������յ�Ͱ low(0)�ϱ�ʾ������С�����Ͱ, high(1)�ϱ�ʾ���������ڵ������Ͱ
	vector<int> beginBucket[2]; // SSS_b: i�������Ͽ�ʼ������Ͱ low(0)�ϱ�ʾ�����Ͱ��ʼ�����Ͱ����

	int levelBuks, levelBuckStep, widthStep; // ÿһ���Ͱ��, ÿһ���ÿ��Ͱ�����������, ���ڲ�֮��ν������ȵĲ�
	vector<vector<bool>> skippedW; // attr->width
	//vector<vector<attAndCount>> attsCountsW;
	vector<vector<vector<vector<vector<Combo>>>>> dataW; // SSS-C-PPH SSS-C-W: attr->level->low/high->bucketId->offset
	vector<vector<pair<pair<int, int>, pair<int, int>>>> beBucketW; // attr->level-><low,high>-><begin,end>

	vector<int> attsPredicate; // ÿ���������ж��ٸ�ν��
public:
	int numBucket;
	int maxSkipPredicate;
	long numSkipPredicateInTotal; // Used in DSS-B and DSS-B-W
	long numSkipBuckInTotal; // ���˵���ɢͰ��������ȫ���������ϵ�Ͱ��
	long numSkipAttsInTotal;
	AdaRein(int);

	void insert(IntervalSub sub);
	bool deleteSubscription(IntervalSub sub);
	void exact_match(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList);

	void original_selection(double falsePositive, const vector<IntervalSub>& subList);
	void approx_match_ori(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList);

	void static_succession_selection(double falsePositive, const vector<IntervalSub>& subList);
	void approx_match_sss(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList);

	void static_succession_selection_backward(double falsePositive, const vector<IntervalSub>& subList);
	void approx_match_sss_b(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList);

	void static_succession_selection_crossed(double falsePositive, const vector<IntervalSub>& subList);
	void approx_match_sss_c(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList);

	/* void insert_sss_c_pph(IntervalSub sub);
	 bool deleteSubscription_sss_c_pph(IntervalSub sub);
	 void static_succession_selection_crossed_pph(double falsePositive, const vector<IntervalSub>& subList);
	 void approx_match_sss_c_pph(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList);*/

	void insert_sss_c_w(IntervalSub sub);
	void static_succession_selection_crossed_width(double falsePositive, const vector<IntervalSub>& subList);
	void approx_match_sss_c_w(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList);

	void insert_dss_b(IntervalSub sub);
	void dynamic_succession_selection_backward(double falsePositive, const vector<IntervalSub>& subList);
	void approx_match_dss_b(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList);

	void approx_match_dss_b_w(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList);

	int calMemory();      // ����ռ���ڴ��С, ����MB
	//int calMemory_sss_c_pph();
	int calMemory_sss_c_w();
};

#endif //ADAREIN_H
