#ifndef ADAREIN_H
#define ADAREIN_H
#include<vector>
#include <cstring>
#include <algorithm>
#include <queue>
#include "generator.h"
#include "chrono_time.h"
#include "util.h"
#include "data_structure.h"
#include "constant.h"

//const int MAX_SUBS = 2000001;
//const int MAX_ATTS = 3000;
//const int MAX_BUCKS = 2000;

class AdaRein {
    int buckStep, numSub;
    //vector<Combo> data[MAX_ATTS][2][MAX_BUCKS];    // 0:left parenthesis, 1:right parenthesis
//    bool bits[MAX_SUBS];
//    bool skipped[MAX_ATTS];
//    attAndCount attsCounts[MAX_ATTS];
	vector<vector<vector<Combo>>> data[2];
	vector<bool> skipped;
	vector<attAndCount> attsCounts; // ����ԭʼ�汾
    vector<int> endBucket[2]; // i����������Ӧ���������յ�Ͱ low(0)�ϱ�ʾ������С�����Ͱ, high(1)�ϱ�ʾ���������ڵ������Ͱ
    vector<int> beginBucket[2]; // SSS_b: i�������Ͽ�ʼ������Ͱ low(0)�ϱ�ʾ�����Ͱ��ʼ�����Ͱ����

public:
    int numBucket;
	int maxSkipPredicate;
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

    int calMemory();      // ����ռ���ڴ��С, ����MB
};

#endif //ADAREIN_H
