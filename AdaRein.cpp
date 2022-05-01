#include "AdaRein.h"

AdaRein::AdaRein(int type) : numSub(0) {
	buckStep = (valDom - 1) / buks + 1;
	numBucket = (valDom - 1) / buckStep + 1;

	string TYPE;
	switch (type) {
		case AdaRein_ORI:
			TYPE = "AdaRein_ORI";
			attsCounts.resize(atts);
			data[0].resize(atts, vector<vector<Combo >>(numBucket));
			data[1].resize(atts, vector<vector<Combo >>(numBucket));
			skipped.resize(atts, false);
			break;
		case AdaRein_SSS:
			TYPE = "AdaRein_SSS";
			attsCounts.resize(atts);
			endBucket[0].resize(atts, buks - 1);
			endBucket[1].resize(atts, 0);
			data[0].resize(atts, vector<vector<Combo >>(numBucket));
			data[1].resize(atts, vector<vector<Combo >>(numBucket));
			skipped.resize(atts, false);
			break;
		case AdaRein_SSS_B:
			TYPE = "AdaRein_SSS_B";
			attsCounts.resize(atts);
			beginBucket[0].resize(atts, 0);
			beginBucket[1].resize(atts, buks - 1);
			data[0].resize(atts, vector<vector<Combo >>(numBucket));
			data[1].resize(atts, vector<vector<Combo >>(numBucket));
			skipped.resize(atts, false);
			break;
		case AdaRein_SSS_C:
			TYPE = "AdaRein_SSS_C";
			attsCounts.resize(atts);
			beginBucket[0].resize(atts, 0);
			beginBucket[1].resize(atts, buks - 1);
			endBucket[0].resize(atts, buks - 1);
			endBucket[1].resize(atts, 0);
			data[0].resize(atts, vector<vector<Combo >>(numBucket));
			data[1].resize(atts, vector<vector<Combo >>(numBucket));
			skipped.resize(atts, false);
			break;
		case AdaRein_SSS_C_PPH:
			TYPE = "AdaRein_SSS_C_PPH";
			buckStep = (valDom - 1) / (buks / 2) + 1;
			//dividerValue = divider * valDom;
			attsCounts.resize(atts);
			beginBucket[0].resize(atts, 0);
			beginBucket[1].resize(atts, buks - 1);
			endBucket[0].resize(atts, buks - 1);
			endBucket[1].resize(atts, 0);
			/*_for(i, 0, adarein_level) {
				dataL[i][0].resize(atts, vector<vector<Combo >>(numBucket*));
				dataL[i][1].resize(atts, vector<vector<Combo >>(numBucket*()));
			}*/
			skipped.resize(atts, false);
			break;
		case AdaRein_SSS_C_W:
			TYPE = "AdaRein_SSS_C_W";
			levelBuks = buks / adarein_level;
			buckStep = (valDom - 1) / levelBuks + 1;
			levelBuks = (valDom - 1) / buckStep + 1;
			widthStep = valDom / adarein_level;
			attsCounts.resize(atts);
			skippedW.resize(atts, vector<bool>(adarein_level, false));
			dataW.resize(atts, vector<vector<vector<vector<Combo>>>>(
				adarein_level, vector<vector<vector<Combo>>>(
					2, vector<vector<Combo>>(levelBuks))));
			beBucketW.resize(atts, vector<pair<pair<int, int>, pair<int, int>>>(adarein_level));
			_for(i, 0, atts) {
				_for(j, 0, adarein_level) {
					beBucketW[i][j].first.first = 0; // low begin
					beBucketW[i][j].first.second = levelBuks - 1; // low end
					beBucketW[i][j].second.first = levelBuks - 1; // high begin
					beBucketW[i][j].second.second = 0; // high end
				}
			}
			break;
		case AdaRein_SDS:
			TYPE = "AdaRein_SDS";
			break;
		case AdaRein_DSS:
			TYPE = "AdaRein_DSS";
			break;
		case AdaRein_DDS:
			TYPE = "AdaRein_DDS";
			break;
		case AdaRein_IBU:
			TYPE = "AdaRein_IBU";
			break;
		default:
			break;
	}
	cout << "ExpID = " << expID << ". " + TYPE + ": falsePositiveRate_local = " << falsePositiveRate
		 << ", bucketStep = "
		 << buckStep << ", numBucket = " << numBucket;
	if (type == AdaRein_SSS_C_PPH || type == AdaRein_SSS_C_W)cout << ", adarein_level" << adarein_level;
	cout << endl;
}

void AdaRein::insert(IntervalSub sub) {
	for (int i = 0; i < sub.size; i++) {
		IntervalCnt cnt = sub.constraints[i];
		Combo c;
		c.val = cnt.lowValue;
		c.subID = sub.id;
		data[0][cnt.att][c.val / buckStep].push_back(c);
		c.val = cnt.highValue;
		data[1][cnt.att][c.val / buckStep].push_back(c);
	}
	numSub++;
}

bool AdaRein::deleteSubscription(IntervalSub sub) {
	int find = 0;
	IntervalCnt cnt;
	int bucketID, id = sub.id;

	_for(i, 0, sub.size) {
		cnt = sub.constraints[i];
		bucketID = cnt.lowValue / buckStep;
		vector<Combo>::iterator it;
		for (it = data[0][cnt.att][bucketID].begin(); it != data[0][cnt.att][bucketID].end(); it++)
			if (it->subID == id) {
				data[0][cnt.att][bucketID].erase(it); // it =
				find++;
				break;
			}

		bucketID = cnt.highValue / buckStep;
		for (it = data[1][cnt.att][bucketID].begin(); it != data[1][cnt.att][bucketID].end(); it++)
			if (it->subID == id) {
				data[1][cnt.att][bucketID].erase(it); // it =
				find++;
				break;
			}
	}
	if (find == 2 * sub.size)
		numSub--;
	return find == 2 * sub.size;
}

void AdaRein::exact_match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList) {
	vector<bool> bits(subs, false);
	vector<bool> attExist(atts, false);

	for (int i = 0; i < pub.size; i++) {
		int value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
		attExist[att] = true;
		for (int k = 0; k < data[0][att][buck].size(); k++)
			if (data[0][att][buck][k].val > value)
				bits[data[0][att][buck][k].subID] = true;
		for (int j = buck + 1; j < numBucket; j++)
			for (int k = 0; k < data[0][att][j].size(); k++)
				bits[data[0][att][j][k].subID] = true;

		for (int k = 0; k < data[1][att][buck].size(); k++)
			if (data[1][att][buck][k].val < value)
				bits[data[1][att][buck][k].subID] = true;
		for (int j = buck - 1; j >= 0; j--)
			for (int k = 0; k < data[1][att][j].size(); k++)
				bits[data[1][att][j][k].subID] = true;
	}

	for (int i = 0; i < atts; i++)
		if (!attExist[i])
			for (int j = 0; j < numBucket; j++)
				for (int k = 0; k < data[0][i][j].size(); k++)
					bits[data[0][i][j][k].subID] = true;

	for (int i = 0; i < subList.size(); i++)
		if (!bits[i])
			++matchSubs;
}

void AdaRein::original_selection(double falsePositive, const vector<IntervalSub> &subList) {
	for (int i = 0; i < atts; i++) {
		attsCounts[i].att = i;
		attsCounts[i].count = 0;
	}

	int numPredicate = 0, numSkipPredicate = 0;
	double avgSubSize = 0, avgWidth = 0;

	for (int i = 0; i < subList.size(); i++) {
		numPredicate += subList[i].size;
		for (int j = 0; j < subList[i].size; j++) {
			++attsCounts[subList[i].constraints[j].att].count;
			avgWidth += subList[i].constraints[j].highValue - subList[i].constraints[j].lowValue;
		}
	}
	avgSubSize = numPredicate / subList.size();
	avgWidth /= numPredicate;

	// faster version, need to be verified.--û����avgWidth��
	/*_for(skipIndex, 0, atts) {
		_for(j, 0, bucks)
			attsCounts[skipIndex].count += data[0][skipIndex][j].size();
		numPredicate += attsCounts[skipIndex].count;
	}*/

	sort(attsCounts.begin(), attsCounts.end());

	int maxSkipPredicate =
		numPredicate - (avgSubSize + log(falsePositive + 1) / log(avgWidth / valDom)) *
					   subs; // ������Թ��˵�ν����, currentSum�����ֵ
#ifdef DEBUG
	cout << "maxSkipPredicate= " << maxSkipPredicate << "\n";
	int numSkipAttr = 0;
#endif // DEBUG

	for (int i = 0; i < atts; i++) {
		numSkipPredicate += attsCounts[i].count;
		// ��Ч�汾:
		if ((double) (numPredicate - numSkipPredicate) / (double) subs >
			avgSubSize + log(falsePositive + 1) / log(avgWidth / valDom)) {
			//		if ((double)(numPredicate - numSkipPredicate) / (double)subList.size() > subList[0].constraints.size() + log(falsePositive + 1) / log((subList[0].constraints[0].highValue - subList[0].constraints[0].lowValue) / (double)valDom)) {
			skipped[attsCounts[i].att] = true;
#ifdef DEBUG
			numSkipAttr++;
			//cout << "Skip Attribute " << attsCounts[skipIndex].att<<"\n"; // could output in finding order.
#endif
		} else {
			numSkipPredicate -= attsCounts[i].count; // back
			break;
		}
	}

#ifdef DEBUG
	cout << "avgSubSize= " << avgSubSize << ", " << "avgWidth= " << avgWidth << ", numPredicate= " << numPredicate
		 << ", maxSkipPredicate= " << maxSkipPredicate << ", numSkipPredicate= " << numSkipPredicate << ".\n";
	cout << "Total skipped attribute: " << numSkipAttr << " among " << atts << " attributes.\n";
	cout << "Skip attribute:";
	_for(i, 0, atts) if (skipped[i])
			cout << " " << i;
	cout << "\n";
#endif
}

void AdaRein::approx_match_ori(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList) {
	vector<bool> bits(subs, false);
	vector<bool> attExist(atts, false);
	for (int i = 0; i < pub.size; i++) {
		int att = pub.pairs[i].att;
		attExist[att] = true;
		if (skipped[att])
			continue;
		int value = pub.pairs[i].value, buck = value / buckStep;
		for (int k = 0; k < data[0][att][buck].size(); k++)
			if (data[0][att][buck][k].val > value)
				bits[data[0][att][buck][k].subID] = true;
		for (int j = buck + 1; j < numBucket; j++)
			for (int k = 0; k < data[0][att][j].size(); k++)
				bits[data[0][att][j][k].subID] = true;

		for (int k = 0; k < data[1][att][buck].size(); k++)
			if (data[1][att][buck][k].val < value)
				bits[data[1][att][buck][k].subID] = true;
		for (int j = buck - 1; j >= 0; j--)
			for (int k = 0; k < data[1][att][j].size(); k++)
				bits[data[1][att][j][k].subID] = true;
	}

	for (int i = 0; i < atts; i++)
		if (!attExist[i] && !skipped[i])
			for (int j = 0; j < numBucket; j++)
				for (int k = 0; k < data[0][i][j].size(); k++)
					bits[data[0][i][j][k].subID] = true;

	for (int i = 0; i < subList.size(); i++)
		if (!bits[i])
			++matchSubs;
}

void AdaRein::static_succession_selection(double falsePositive, const vector<IntervalSub> &subList) {
	for (int i = 0; i < atts; i++) {
		attsCounts[i].att = i;
		attsCounts[i].count = 0;
	}

	int numPredicate = 0, numSkipPredicate = 0; // ν������, �ѹ��˵�ν������
	double avgSubSize = 0, avgWidth = 0; // ƽ��ÿ�������ж��ٸ�ν��, ν�ʵ�ƽ�����
	for (auto &&iSub: subList) {
		//numPredicate += iSub.constraints.size(); // ����
		for (auto &&iCnt: iSub.constraints) {
			//++attsCounts[iCnt.att].count; // ����
			avgWidth += iCnt.highValue - iCnt.lowValue;
		}
	}
	// ��Ҫ�Ȳ��붩�Ĳ�����ô��
	_for(i, 0, atts) {
		_for(j, 0, buks) attsCounts[i].count += data[0][i][j].size();
		numPredicate += attsCounts[i].count;
	}
	avgSubSize = (double) numPredicate / subList.size();
	avgWidth /= numPredicate;

	sort(attsCounts.begin(), attsCounts.end());

	// minPredicate �����Դ���
	//double minK = log(pow(avgWidth / valDom, avgSubSize) + falsePositive) / log(avgWidth / valDom);
	//inline auto valid = [&](double) {return (double)(numPredicate - numSkipPredicate) > minK * subs; };

	double falsePositiveRate_global = pow(width, avgSubSize) * subs / (1 - falsePositive) * falsePositive / subs;

#ifdef DEBUG
	maxSkipPredicate = numPredicate - (avgSubSize - log(1 - falsePositive) / log(avgWidth / valDom)) * subs; // k2
	cout << "k2= " << maxSkipPredicate << "\n";
	maxSkipPredicate =
		numPredicate - log(pow(avgWidth / valDom, avgSubSize) + falsePositiveRate_global) / log(avgWidth / valDom) *
					   subs; // ������Թ��˵�ν����, currentSum�����ֵ
	cout << "k3_global= " << maxSkipPredicate << "\n";
	int numSkipAttr = 0;
	int numSkipBkt = 0;
#endif

	maxSkipPredicate =
		numPredicate - log(pow(avgWidth / valDom, avgSubSize) / (1 - falsePositive)) / log(avgWidth / valDom) * subs;
	//	maxSkipPredicate/=2;
	cout << "k3_local= " << maxSkipPredicate << "\n";

	int skipIndex = 0;

	for (skipIndex = 0; skipIndex < atts; skipIndex++) {
		if (numSkipPredicate + attsCounts[skipIndex].count < maxSkipPredicate) {
			numSkipPredicate = numSkipPredicate + attsCounts[skipIndex].count;
			skipped[attsCounts[skipIndex].att] = true;
#ifdef DEBUG
			numSkipAttr++;
			//cout << "Skip Attribute " << attsCounts[skipIndex].att<<"\n"; // could output in finding order.
#endif // DEBUG
		} else {
			break;
		}
	}

	// <low0/high1, AttributeId, bucketId, sizeOfBucket>
	auto cmp = [&](const auto &a, const auto &b) {
		return get<3>(a) > get<3>(b); // aͰС, ����false, �Ͱ�a��Ϊ��, ��ʵ��С����
	};
	priority_queue<tuple<int, int, int, int>, vector<tuple<int, int, int, int>>, decltype(cmp)> minHeap(cmp);
	while (skipIndex < atts) {
		minHeap.push(
			make_tuple(0, attsCounts[skipIndex].att, buks - 1, data[0][attsCounts[skipIndex].att][buks - 1].size()));
		minHeap.push(make_tuple(1, attsCounts[skipIndex].att, 0, data[1][attsCounts[skipIndex].att][0].size()));
		skipIndex++;
	}
	while (true) {
		auto item = minHeap.top();
		minHeap.pop();
		if (numSkipPredicate + get<3>(item) / 2 > maxSkipPredicate) {
			break;
		}
		numSkipPredicate = numSkipPredicate + get<3>(item) / 2;
#ifdef DEBUG
		numSkipBkt++;
#endif
		if (get<0>(item) == 0) { // low
			get<2>(item) -= 1; // ���˵����Ͱ
			endBucket[0][get<1>(item)] = get<2>(item); // ���˵����Ͱ
			if (get<2>(item) >= 1) { // ����ʣ���Ͱ���Ա�����, Ϊ0ʱֹͣ, ��Ϊ0��Ͱ��LVE�϶����ǱȽ�Ͱ�������Ǳ��Ͱ
				get<3>(item) = data[0][get<1>(item)][get<2>(item)].size(); // ����Ͱ��С
				minHeap.push(item);
			}
		} else { // high
			get<2>(item) += 1; // ���˵����Ͱ
			endBucket[1][get<1>(item)] = get<2>(item); // ���˵����Ͱ
			if (get<2>(item) < buks - 1) { // ����ʣ���Ͱ���Ա�����
				get<3>(item) = data[1][get<1>(item)][get<2>(item)].size();
				minHeap.push(item);
			}
		}
	}
#ifdef DEBUG
	cout << "In theory, rightMatchNum= " << pow(width, avgSubSize) * subs << ", wrongMatchNum= "
		 << pow(width, avgSubSize) * subs / (1 - falsePositive) * falsePositive << ", falsePositiveRate_local= "
		 << falsePositive
		 << ", falsePositiveRate_global= " << falsePositiveRate_global << ".\n";
	cout << "avgSubSize= " << avgSubSize << ", " << "avgWidth= " << avgWidth << ", numPredicate= " << numPredicate
		 << ", maxSkipPredicate= " << maxSkipPredicate << ", numSkipPredicate= " << numSkipPredicate << ".\n";
	cout << "Total skipped attribute: " << numSkipAttr << " among " << atts << " attributes.\n";
	cout << "Total skipped bucket: " << numSkipAttr << "*2*" << buks << " + " << numSkipBkt << " = "
		 << numSkipAttr * 2 * buks + numSkipBkt << " among " << atts * 2 * buks << " buckets.\n";
	cout << "Skip attribute:";
	_for(i, 0, atts) if (skipped[i])
			cout << " " << i;
	cout << "\n";
#endif
}

void AdaRein::approx_match_sss(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList) {
	bitset<subs> bits;
	vector<bool> attExist(atts, false);
	for (auto &&iPair: pub.pairs) {
		int att = iPair.att;
		attExist[att] = true;
		if (skipped[att])
			continue;
		int value = iPair.value, buck = value / buckStep;

		for (int k = 0; k < data[0][att][buck].size(); k++)
			if (data[0][att][buck][k].val > value)
				bits[data[0][att][buck][k].subID] = true;
		for (int j = buck + 1; j <= endBucket[0][att]; j++) // ��HEMϵ�е���Ʋ�ͬ, ����ȡ������
			for (int k = 0; k < data[0][att][j].size(); k++)
				bits[data[0][att][j][k].subID] = true;

		for (int k = 0; k < data[1][att][buck].size(); k++)
			if (data[1][att][buck][k].val < value)
				bits[data[1][att][buck][k].subID] = true;
		for (int j = buck - 1; j >= endBucket[1][att]; j--)
			for (int k = 0; k < data[1][att][j].size(); k++)
				bits[data[1][att][j][k].subID] = true;
	}

	// �����滻Ϊ1��λ����
	for (int i = 0; i < atts; i++)
		if (!attExist[i] && !skipped[i])
			for (int j = buks - 1; j >= endBucket[1][i]; j--)
				for (int k = 0; k < data[1][i][j].size(); k++)
					bits[data[1][i][j][k].subID] = true;

	matchSubs = subs - bits.count();
}

void AdaRein::static_succession_selection_backward(double falsePositive, const vector<IntervalSub> &subList) {
	for (int i = 0; i < atts; i++) {
		attsCounts[i].att = i;
		attsCounts[i].count = 0;
	}

	int numPredicate = 0, numSkipPredicate = 0; // ν������, �ѹ��˵�ν������
	double avgSubSize = 0, avgWidth = 0; // ƽ��ÿ�������ж��ٸ�ν��, ν�ʵ�ƽ�����
	for (auto &&iSub: subList) {
		//numPredicate += iSub.constraints.size(); // ����
		for (auto &&iCnt: iSub.constraints) {
			//++attsCounts[iCnt.att].count; // ����
			avgWidth += iCnt.highValue - iCnt.lowValue;
		}
	}
	// ��Ҫ�Ȳ��붩�Ĳ�����ô��
	_for(i, 0, atts) {
		_for(j, 0, buks) attsCounts[i].count += data[0][i][j].size();
		numPredicate += attsCounts[i].count;
	}
	avgSubSize = (double) numPredicate / subList.size();
	avgWidth /= numPredicate;

	sort(attsCounts.begin(), attsCounts.end());

	// minPredicate �����Դ���
	//double minK = log(pow(avgWidth / valDom, avgSubSize) + falsePositive) / log(avgWidth / valDom);
	//inline auto valid = [&](double) {return (double)(numPredicate - numSkipPredicate) > minK * subs; };

	double falsePositiveRate_global = pow(width, avgSubSize) * subs / (1 - falsePositive) * falsePositive / subs;

	int maxSkipPredicate = numPredicate - (avgSubSize - log(1 - falsePositive) / log(avgWidth / valDom)) * subs; // k2

#ifdef DEBUG
	cout << "k2= " << maxSkipPredicate << "\n";
	maxSkipPredicate =
		numPredicate - log(pow(avgWidth / valDom, avgSubSize) + falsePositiveRate_global) / log(avgWidth / valDom) *
					   subs; // ������Թ��˵�ν����, currentSum�����ֵ
	cout << "k3_global= " << maxSkipPredicate << "\n";
	int numSkipAttr = 0;
	int numSkipBkt = 0;
#endif

	maxSkipPredicate =
		numPredicate - log(pow(avgWidth / valDom, avgSubSize) / (1 - falsePositive)) / log(avgWidth / valDom) * subs;
	//	maxSkipPredicate *= 6;
	cout << "k3_local= " << maxSkipPredicate << "\n";

	int skipIndex = 0;


	for (skipIndex = 0; skipIndex < atts; skipIndex++) {
		if (numSkipPredicate + attsCounts[skipIndex].count < maxSkipPredicate) {
			numSkipPredicate = numSkipPredicate + attsCounts[skipIndex].count;
			skipped[attsCounts[skipIndex].att] = true;
#ifdef DEBUG
			numSkipAttr++;
			//cout << "Skip Attribute " << attsCounts[skipIndex].att<<"\n"; // could output in finding order.
#endif // DEBUG
		} else {
			break;
		}
	}

	// <low0/high1, AttributeId, bucketId, sizeOfBucket>
	auto cmp = [&](const auto &a, const auto &b) {
		return get<3>(a) > get<3>(b); // aͰС, ����false, �Ͱ�a��Ϊ��, ��ʵ��С����
	};
	priority_queue<tuple<int, int, int, int>, vector<tuple<int, int, int, int>>, decltype(cmp)> minHeap(cmp);
	while (skipIndex < atts) {
		minHeap.push(
			make_tuple(0, attsCounts[skipIndex].att, 0, data[0][attsCounts[skipIndex].att][0].size()));
		minHeap.push(
			make_tuple(1, attsCounts[skipIndex].att, buks - 1, data[1][attsCounts[skipIndex].att][buks - 1].size()));
		skipIndex++;
	}

	while (true) {
		auto item = minHeap.top();
		minHeap.pop();
		if (numSkipPredicate + get<3>(item) / 2 > maxSkipPredicate) {
			break;
		}
		numSkipPredicate = numSkipPredicate + get<3>(item) / 2;
#ifdef DEBUG
		numSkipBkt++;
#endif
		if (get<0>(item) == 0) { // low
			get<2>(item) += 1; // ���˵����Ͱ
			beginBucket[0][get<1>(item)] = get<2>(item); // ���˵����Ͱ
			if (get<2>(item) < buks - 1) { // ����ʣ���Ͱ���Ա�����
				get<3>(item) = data[0][get<1>(item)][get<2>(item)].size();
				minHeap.push(item);
			}
		} else { // high
			get<2>(item) -= 1; // ���˵����Ͱ
			beginBucket[1][get<1>(item)] = get<2>(item); // ���˵����Ͱ
			if (get<2>(item) > 0) { // ����ʣ���Ͱ���Ա�����, Ϊ0ʱֹͣ, ��Ϊ0��Ͱ��LVE�϶����ǱȽ�Ͱ�������Ǳ��Ͱ
				get<3>(item) = data[1][get<1>(item)][get<2>(item)].size(); // ����Ͱ��С
				minHeap.push(item);
			}
		}
	}
#ifdef DEBUG
	cout << "In theory, rightMatchNum= " << pow(width, avgSubSize) * subs << ", wrongMatchNum= "
		 << pow(width, avgSubSize) * subs / (1 - falsePositive) * falsePositive << ", falsePositiveRate_local= "
		 << falsePositive
		 << ", falsePositiveRate_global= " << falsePositiveRate_global << ".\n";
	cout << "avgSubSize= " << avgSubSize << ", " << "avgWidth= " << avgWidth << ", numPredicate= " << numPredicate
		 << ", maxSkipPredicate= " << maxSkipPredicate << ", numSkipPredicate= " << numSkipPredicate << ".\n";
	cout << "Total skipped attribute: " << numSkipAttr << " among " << atts << " attributes.\n";
	cout << "Total skipped bucket: " << numSkipAttr << "*2*" << buks << " + " << numSkipBkt << " = "
		 << numSkipAttr * 2 * buks + numSkipBkt << " among " << atts * 2 * buks << " buckets.\n";
	cout << "Skip attribute:";
	_for(i, 0, atts) if (skipped[i])
			cout << " " << i;
	cout << "\n";
#endif
}

void AdaRein::approx_match_sss_b(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList) {
	bitset<subs> bits;
	vector<bool> attExist(atts, false);
	for (auto &&iPair: pub.pairs) {
		int att = iPair.att;
		attExist[att] = true;
		if (skipped[att])
			continue;
		int value = iPair.value, buck = value / buckStep;

		for (int k = 0; k < data[0][att][buck].size(); k++)
			if (data[0][att][buck][k].val > value)
				bits[data[0][att][buck][k].subID] = true;
		for (int j = max(buck + 1, beginBucket[0][att]); j < buks; j++) // ��HEMϵ�е���Ʋ�ͬ, ����ȡ������
			for (int k = 0; k < data[0][att][j].size(); k++)
				bits[data[0][att][j][k].subID] = true;

		for (int k = 0; k < data[1][att][buck].size(); k++)
			if (data[1][att][buck][k].val < value)
				bits[data[1][att][buck][k].subID] = true;
		for (int j = min(buck - 1, beginBucket[1][att]); j >= 0; j--)
			for (int k = 0; k < data[1][att][j].size(); k++)
				bits[data[1][att][j][k].subID] = true;
	}

	// �����滻Ϊ1��λ����
	for (int i = 0; i < atts; i++)
		if (!attExist[i] && !skipped[i])
			for (int j = beginBucket[1][i]; j >= 0; j--)
				for (auto &&k: data[1][i][j])
					bits[k.subID] = true;

	matchSubs = subs - bits.count();
}

void AdaRein::static_succession_selection_crossed(double falsePositive, const vector<IntervalSub> &subList) {
	for (int i = 0; i < atts; i++) {
		attsCounts[i].att = i;
		attsCounts[i].count = 0;
	}

	int numPredicate = 0, numSkipPredicate = 0; // ν������, �ѹ��˵�ν������
	double avgSubSize = 0, avgWidth = 0; // ƽ��ÿ�������ж��ٸ�ν��, ν�ʵ�ƽ�����
	for (auto &&iSub: subList) {
		//numPredicate += iSub.constraints.size(); // ����
		for (auto &&iCnt: iSub.constraints) {
			//++attsCounts[iCnt.att].count; // ����
			avgWidth += iCnt.highValue - iCnt.lowValue;
		}
	}
	// ��Ҫ�Ȳ��붩�Ĳ�����ô��
	_for(i, 0, atts) {
		_for(j, 0, buks) attsCounts[i].count += data[0][i][j].size();
		numPredicate += attsCounts[i].count;
	}
	avgSubSize = (double) numPredicate / subList.size();
	avgWidth /= numPredicate;

	sort(attsCounts.begin(), attsCounts.end());

	// minPredicate �����Դ���
	//double minK = log(pow(avgWidth / valDom, avgSubSize) + falsePositive) / log(avgWidth / valDom);
	//inline auto valid = [&](double) {return (double)(numPredicate - numSkipPredicate) > minK * subs; };

	double falsePositiveRate_global = pow(width, avgSubSize) * subs / (1 - falsePositive) * falsePositive / subs;

	int maxSkipPredicate = numPredicate - (avgSubSize - log(1 - falsePositive) / log(avgWidth / valDom)) * subs; // k2

#ifdef DEBUG
	cout << "k2= " << maxSkipPredicate << "\n";
	maxSkipPredicate =
		numPredicate - log(pow(avgWidth / valDom, avgSubSize) + falsePositiveRate_global) / log(avgWidth / valDom) *
					   subs; // ������Թ��˵�ν����, currentSum�����ֵ
	cout << "k3_global= " << maxSkipPredicate << "\n";
	int numSkipAttr = 0;
	int numSkipBkt = 0;
#endif

	maxSkipPredicate =
		numPredicate - log(pow(avgWidth / valDom, avgSubSize) / (1 - falsePositive)) / log(avgWidth / valDom) * subs;
	//	maxSkipPredicate *= 6;
	cout << "k3_local= " << maxSkipPredicate << "\n";

	int skipIndex = 0;
	for (skipIndex = 0; skipIndex < atts; skipIndex++) {
		if (numSkipPredicate + attsCounts[skipIndex].count < maxSkipPredicate) {
			numSkipPredicate = numSkipPredicate + attsCounts[skipIndex].count;
			skipped[attsCounts[skipIndex].att] = true;
#ifdef DEBUG
			numSkipAttr++;
			//cout << "Skip Attribute " << attsCounts[skipIndex].att<<"\n"; // could output in finding order.
#endif // DEBUG
		} else {
			break;
		}
	}

	// ���˿�Ͱ
	for (int i = 0; i < atts; i++) {
		while (beginBucket[0][i] < endBucket[0][i] && data[0][i][beginBucket[0][i]].size() == 0) beginBucket[0][i]++;
		while (beginBucket[0][i] < endBucket[0][i] && data[0][i][beginBucket[0][i]].size() == 0) endBucket[0][i]--;
		while (beginBucket[1][i] > endBucket[1][i] && data[1][i][beginBucket[1][i]].size() == 0) beginBucket[1][i]--;
		while (beginBucket[1][i] > endBucket[1][i] && data[1][i][endBucket[1][i]].size() == 0) endBucket[1][i]++;
#ifdef DEBUG
		numSkipBkt += beginBucket[0][i] + buks - 1 - endBucket[0][i] + endBucket[1][i] + buks - 1 - beginBucket[1][i];
#endif
	}

	// <low0/high1, AttributeId, bucketId, sizeOfBucket>
	auto cmp = [&](const auto &a, const auto &b) {
		return get<3>(a) > get<3>(b); // aͰС, ����false, �Ͱ�a��Ϊ��, ��ʵ��С����
	};
	priority_queue<tuple<int, int, int, int>, vector<tuple<int, int, int, int>>, decltype(cmp)> minHeap(cmp);
	vector<bool> end[2];    // low/high -> att -> 0: ����С�Ŷˣ�1�����˴�Ŷ�
	end[0].resize(atts, true);  // ��ֵ�˴Ӵ�Ŷ�(������)��ʼ����
	end[1].resize(atts, false); // ��ֵ�˴�С��Ͱ��(������)��ʼ����
	while (skipIndex < atts) {
		int &att = attsCounts[skipIndex].att;
		if (endBucket[0][att] > beginBucket[0][att])
			minHeap.push(make_tuple(0, att, endBucket[0][att],
									data[0][att][endBucket[0][att]].size()));
		if (endBucket[1][att] < beginBucket[0][att])
			minHeap.push(make_tuple(1, att, endBucket[1][att],
									data[1][att][endBucket[1][att]].size()));
		skipIndex++;
	}

	while (true) {
		auto item = minHeap.top();
		minHeap.pop();
		if (numSkipPredicate + get<3>(item) / 2 > maxSkipPredicate) {
			break;
		}
		numSkipPredicate = numSkipPredicate + get<3>(item) / 2;
#ifdef DEBUG
		numSkipBkt++;
#endif
		if (get<0>(item) == 0) { // low
			if (end[0][get<1>(item)]) { // ���Ͱ
				get<2>(item) -= 1;
				endBucket[0][get<1>(item)] = get<2>(item); // ���˵����Ͱ
				if (get<2>(item) >= beginBucket[0][get<1>(item)]) {
					get<2>(item) = beginBucket[0][get<1>(item)]; // ת��
					get<3>(item) = data[0][get<1>(item)][get<2>(item)].size();
					minHeap.push(item);
				}
			} else { // С��Ͱ
				get<2>(item) += 1; // ���˵����Ͱ
				beginBucket[0][get<1>(item)] = get<2>(item); // ���˵����Ͱ
				if (get<2>(item) <= endBucket[0][get<1>(item)]) { // ����ʣ���Ͱ���Ա�����
					get<2>(item) = endBucket[0][get<1>(item)]; // ת��
					get<3>(item) = data[0][get<1>(item)][get<2>(item)].size();
					minHeap.push(item);
				}
			}
			end[0][get<1>(item)] = !end[0][get<1>(item)];
		} else { // high
			if (end[1][get<1>(item)]) { // ���Ͱ(beginBucket)
				get<2>(item) -= 1; // ���˵����Ͱ
				beginBucket[1][get<1>(item)] = get<2>(item); // ���˵�������Ͱ
				if (get<2>(item) >= endBucket[1][get<1>(item)]) { // ����ʣ���Ͱ���Ա�����
					get<2>(item) = endBucket[1][get<1>(item)]; // ת��
					get<3>(item) = data[1][get<1>(item)][get<2>(item)].size(); // ����Ͱ��С
					minHeap.push(item);
				}
			} else { // С��Ͱ(endBucket)
				get<2>(item) += 1; // ���˵����Ͱ
				endBucket[1][get<1>(item)] = get<2>(item); // ���˵����Ͱ
				if (get<2>(item) <= beginBucket[1][get<1>(item)]) { // ����ʣ���Ͱ���Ա�����
					get<2>(item) = beginBucket[1][get<1>(item)]; // ת��
					get<3>(item) = data[1][get<1>(item)][get<2>(item)].size();
					minHeap.push(item);
				}
			}
			end[1][get<1>(item)] = !end[1][get<1>(item)];
		}
	}

	// �����ɢ�Ķ�Ͱ������������ȫ��������, �ͱ�ǵ�sekippedW��
	_for(i, 0, atts) {
		if (beginBucket[0][i] >= endBucket[0][i] && beginBucket[1][i] <= endBucket[1][i]) {
			printf(
				"Error: if %d is a full-skipped attribute, it would already be skipped before discretization selection.",
				i);
			skipped[i] = true;
#ifdef DEBUG
			numSkipAttr++; // ������ϵĻ�, numSkipBkt�Ͳ�׼ȷ��
#endif // DEBUG
		}
	}

#ifdef DEBUG
	cout << "In theory, rightMatchNum= " << pow(width, avgSubSize) * subs << ", wrongMatchNum= "
		 << pow(width, avgSubSize) * subs / (1 - falsePositive) * falsePositive << ", falsePositiveRate_local= "
		 << falsePositive
		 << ", falsePositiveRate_global= " << falsePositiveRate_global << ".\n";
	cout << "avgSubSize= " << avgSubSize << ", " << "avgWidth= " << avgWidth << ", numPredicate= " << numPredicate
		 << ", maxSkipPredicate= " << maxSkipPredicate << ", numSkipPredicate= " << numSkipPredicate << ".\n";
	cout << "Total skipped attribute: " << numSkipAttr << " among " << atts << " attributes.\n";
	cout << "Total skipped bucket: " << numSkipAttr << "*2*" << buks << " + " << numSkipBkt << " = "
		 << numSkipAttr * 2 * buks + numSkipBkt << " among " << atts * 2 * buks << " buckets.\n";
	cout << "Skip attribute:";
	_for(i, 0, atts) if (skipped[i])
			cout << " " << i;
	cout << "\n";
#endif
}

void AdaRein::approx_match_sss_c(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList) {
	bitset<subs> bits;
	vector<bool> attExist(atts, false);
	for (auto &&iPair: pub.pairs) {
		int att = iPair.att;
		attExist[att] = true;
		if (skipped[att])
			continue;
		int value = iPair.value, buck = value / buckStep;

		for (int k = 0; k < data[0][att][buck].size(); k++)
			if (data[0][att][buck][k].val > value)
				bits[data[0][att][buck][k].subID] = true;
		for (int j = max(buck + 1, beginBucket[0][att]); j <= endBucket[0][att]; j++)
			for (auto &&k: data[0][att][j])
				bits[k.subID] = true;

		for (int k = 0; k < data[1][att][buck].size(); k++)
			if (data[1][att][buck][k].val < value)
				bits[data[1][att][buck][k].subID] = true;
		for (int j = min(buck - 1, beginBucket[1][att]); j >= endBucket[1][att]; j--)
			for (auto &&k: data[1][att][j])
				bits[k.subID] = true;
	}

	// �����滻Ϊ1��λ����
	for (int i = 0; i < atts; i++)
		if (!attExist[i] && !skipped[i])
			for (int j = beginBucket[1][i]; j >= max(0, endBucket[1][i]); j--)
				for (auto &&k: data[1][i][j])
					bits[k.subID] = true;

	matchSubs = subs - bits.count();
}

//void AdaRein::insert_sss_c_pph(IntervalSub sub) {
//	Combo c;
//	int levelId = -1, divider = 0.2;
//	for (const auto& cnt:sub.constraints) {
//		c.subID = sub.id;
//		c.val = cnt.lowValue;
//		if (cnt.highValue - cnt.lowValue < dividerValue) {
//			dataL[0][0][cnt.att][c.val / buckStep].push_back(c);
//			c.val = cnt.highValue;
//			dataL[0][1][cnt.att][c.val / buckStep].push_back(c);
//		}
//		else {
//			dataL[1][0][cnt.att][c.val / buckStep].push_back(c);
//			c.val = cnt.highValue;
//			dataL[1][1][cnt.att][c.val / buckStep].push_back(c);
//		}
//	}
//	numSub++;
//}

void AdaRein::insert_sss_c_w(IntervalSub sub) {
	Combo c;
	int levelId = -1;
	for (const auto &cnt: sub.constraints) {
		c.subID = sub.id;
		c.val = cnt.lowValue;
		levelId = (cnt.highValue - cnt.lowValue) / widthStep;
		dataW[cnt.att][levelId][0][c.val / levelBuckStep].push_back(c);
		c.val = cnt.highValue;
		dataW[cnt.att][levelId][1][c.val / levelBuckStep].push_back(c);
	}
	numSub++;
}

void AdaRein::static_succession_selection_crossed_width(double falsePositive, const vector<IntervalSub> &subList) {

	int numPredicate = 0, numSkipPredicate = 0; // ν������, �ѹ��˵�ν������
	double avgSubSize = 0, avgWidth = 0; // ƽ��ÿ�������ж��ٸ�ν��, ν�ʵ�ƽ�����
	for (auto &&iSub: subList) {
		numPredicate += iSub.constraints.size();
		for (auto &&iCnt: iSub.constraints) {
			//++attsCounts[iCnt.att].count;
			avgWidth += iCnt.highValue - iCnt.lowValue;
		}
	}
	avgSubSize = (double) numPredicate / subList.size();
	avgWidth /= numPredicate;

	// minPredicate �����Դ���
	//double minK = log(pow(avgWidth / valDom, avgSubSize) + falsePositive) / log(avgWidth / valDom);
	//inline auto valid = [&](double) {return (double)(numPredicate - numSkipPredicate) > minK * subs; };

	double falsePositiveRate_global = pow(width, avgSubSize) * subs / (1 - falsePositive) * falsePositive / subs;

	int maxSkipPredicate = numPredicate - (avgSubSize - log(1 - falsePositive) / log(avgWidth / valDom)) * subs; // k2

#ifdef DEBUG
	cout << "k2= " << maxSkipPredicate << "\n";
	maxSkipPredicate =
		numPredicate - log(pow(avgWidth / valDom, avgSubSize) + falsePositiveRate_global) / log(avgWidth / valDom) *
					   subs; // ������Թ��˵�ν����, currentSum�����ֵ
	cout << "k3_global= " << maxSkipPredicate << "\n";
	int numSkipAttr = 0;
	int numSkipBkt = 0;
#endif

	maxSkipPredicate =
		numPredicate - log(pow(avgWidth / valDom, avgSubSize) / (1 - falsePositive)) / log(avgWidth / valDom) * subs;
	//	maxSkipPredicate *= 6;
	cout << "k3_local= " << maxSkipPredicate << "\n";

	int skipWidthIndex = adarein_level - 1; // �������ʲ㿪ʼ����
	while (skipWidthIndex >= 0) {

		// ͳ������ϵĸ������ϵ�ν������
		for (int i = 0; i < atts; i++) {
			attsCounts[i].att = i;
			attsCounts[i].count = 0;
		}
		for (int i = 0; i < atts; i++) {
			for (int j = 0; j < levelBuks; j++)
				attsCounts[i].count += dataW[i][skipWidthIndex][0][j].size();
		}
		sort(attsCounts.begin(), attsCounts.end());

		// ��̬�������Ҹò��ϵ�ȫ��������
		int skipIndex = 0;
		for (skipIndex = 0; skipIndex < atts; skipIndex++) {
			if (numSkipPredicate + attsCounts[skipIndex].count < maxSkipPredicate) {
				numSkipPredicate = numSkipPredicate + attsCounts[skipIndex].count;
				skippedW[skipWidthIndex][attsCounts[skipIndex].att] = true;
#ifdef DEBUG
				numSkipAttr++; // ���˲�ͬ����ϵ�ͬһ���Ի�ƶ��
				//cout << "Skip Attribute " << attsCounts[skipIndex].att<<"on widthIndex "<<skipWidthIndex<<"\n"; // could output in finding order.
#endif // DEBUG
			} else {
				break;
			}
		}

		// ����ÿһ���ϵĿ�Ͱ
		for (int i = skipIndex; i < atts; i++) {
			auto &beBW = beBucketW[i][skipWidthIndex];
			const auto &dataw = dataW[i][skipWidthIndex];
			while (beBW.first.first < beBW.first.second && dataw[0][beBW.first.first].size() == 0)beBW.first.first++;
			while (beBW.first.first < beBW.first.second && dataw[0][beBW.first.second].size() == 0)
				beBW.first.second--;
			while (beBW.second.first > beBW.second.second && dataw[1][beBW.second.first].size() == 0)
				beBW.second.first--;
			while (beBW.second.first > beBW.second.second && dataw[1][beBW.second.second].size() == 0)
				beBW.second.second++;
#ifdef DEBUG
			numSkipBkt += beBW.first.first + levelBuks - 1 - beBW.first.second + beBW.second.second + levelBuks - 1 -
						  beBW.second.first;
#endif
		}

//		printf("skipWidthIndex=%d, skipIndex=%d\n", skipWidthIndex, skipIndex);

		// ��������Ͱ����
		// <low0/high1, AttributeId, bucketId, sizeOfBucket>
		auto cmp = [&](const auto &a, const auto &b) {
			return get<3>(a) > get<3>(b); // aͰС, ����false, �Ͱ�a��Ϊ��, ��ʵ��С����
		};
		priority_queue<tuple<int, int, int, int>, vector<tuple<int, int, int, int>>, decltype(cmp)> minHeap(cmp);
		vector<bool> end[2];    // ��¼�������С��Ͱ�˻��Ǵ��Ͱ��: low/high -> att -> 0: ����С��Ͱ�ˣ�1�����˴��Ͱ��
		end[0].resize(atts, true);  // ��ֵ�˴Ӵ��Ͱ��(������)��ʼ����
		end[1].resize(atts, false); // ��ֵ�˴�С��Ͱ��(������)��ʼ����
		while (skipIndex < atts) {
			const int &att = attsCounts[skipIndex].att;
			auto &beBW = beBucketW[att][skipWidthIndex];
			const auto &dataw = dataW[att][skipWidthIndex];
			if (beBW.first.first < beBW.first.second) // ��ֵ�˻��й��˿ռ�
				minHeap.push(
					make_tuple(0, att, beBW.first.second,
							   dataw[0][beBW.first.second].size()));
			if (beBW.second.first > beBW.second.second) // ��ֵ�˻��й��˿ռ�
				minHeap.push(
					make_tuple(1, att, beBW.second.second,
							   dataw[1][beBW.second.second].size()));
			skipIndex++;
		}

		// ����ÿ�Ȳ��ϵĲ��ֹ�������, ���˶���Ķ�Ͱ
		while (!minHeap.empty()) {
			auto item = minHeap.top();
			minHeap.pop();
			if (numSkipPredicate + get<3>(item) / 2 > maxSkipPredicate) {
				break;
			}
			numSkipPredicate = numSkipPredicate + get<3>(item) / 2;
#ifdef DEBUG
			numSkipBkt++;
#endif
			const int &att = get<1>(item);
			int &bktId = get<2>(item);
			auto &beBW = beBucketW[att][skipWidthIndex];
			const auto &dataw = dataW[att][skipWidthIndex];
			if (get<0>(item) == 0) { // low
				if (end[0][att]) { // ���Ͱ�� endBucket
					bktId -= 1;
					beBW.first.second = bktId; // ���˵����Ͱ
					if (bktId > beBW.first.first) {
						bktId = beBW.first.first; // ת��
						get<3>(item) = dataw[0][bktId].size();
						minHeap.push(item);
					}
				} else { // С��Ͱ�� beginBucket
					bktId += 1;
					beBW.first.first = bktId; // ���˵����Ͱ
					if (bktId < beBW.first.second) { // ����ʣ���Ͱ���Ա�����
						bktId = beBW.first.second; // ת��
						get<3>(item) = dataw[0][bktId].size();
						minHeap.push(item);
					}
				}
				end[0][get<1>(item)] = !end[0][get<1>(item)];
			} else { // high
				if (end[1][att]) { // ���Ͱ beginBucket
					bktId -= 1;
					beBW.second.first = bktId; // ���˵�������Ͱ
					if (bktId > beBW.second.second) { // ����ʣ���Ͱ���Ա�����
						bktId = beBW.second.second; // ת��
						get<3>(item) = dataw[1][bktId].size(); // ����Ͱ��С
						minHeap.push(item);
					}
				} else { // С��Ͱ endBucket
					bktId += 1; // ���˵����Ͱ
					beBW.second.second = bktId; // ���˵����С��Ͱ
					if (bktId < beBW.second.first) { // ����ʣ���Ͱ���Ա�����
						bktId = beBW.second.first; // ת��
						get<3>(item) = dataw[1][bktId].size();
						minHeap.push(item);
					}
				}
				end[1][get<1>(item)] = !end[1][get<1>(item)];
			} // high
		} // ��
		skipWidthIndex--;
	} // ��Ȳ�

	// �����ɢ�Ķ�Ͱ������������ȫ��������, �ͱ�ǵ�sekippedW��
	_for(i, 0, atts) {
		_for(j, 0, adarein_level) {
			if (beBucketW[i][j].first.first >= beBucketW[i][j].first.second &&
				beBucketW[i][j].second.first <= beBucketW[i][j].second.second) {
				printf(
					"Error: if %d is a full-skipped attribute on width level %d, it would already be skipped before discretization selection.\n",
					i, j);
				skippedW[i][j] = true;
#ifdef DEBUG
				numSkipAttr++; // ������ϵĻ�, numSkipBkt�Ͳ�׼ȷ��
#endif // DEBUG
			}
		}
	}

#ifdef DEBUG
	cout << "In theory, rightMatchNum= " << pow(width, avgSubSize) * subs << ", wrongMatchNum= "
		 << pow(width, avgSubSize) * subs / (1 - falsePositive) * falsePositive << ", falsePositiveRate_local= "
		 << falsePositive
		 << ", falsePositiveRate_global= " << falsePositiveRate_global << ".\n";
	cout << "avgSubSize= " << avgSubSize << ", " << "avgWidth= " << avgWidth << ", numPredicate= " << numPredicate
		 << ", maxSkipPredicate= " << maxSkipPredicate << ", numSkipPredicate= " << numSkipPredicate << ".\n";
	cout << "Total skipped attribute on all widths: " << numSkipAttr << " among " << adarein_level << " widths of "
		 << atts << " attributes.\n";
	cout << "Total skipped bucket: " << numSkipAttr << "*2*" << levelBuks << " + " << numSkipBkt << " = "
		 << numSkipAttr * 2 * levelBuks + numSkipBkt << " among " << atts * adarein_level * 2 * levelBuks
		 << " buckets.\n";
	cout << "Skip attribute:";
	_for(i, 0, atts) {
		_for(j, 0, adarein_level) {
			if (skippedW[i][j]) {
				cout << " a" << i << "w" << j;
			}
		}
		cout << ";  ";
	}
	cout << "\n";
#endif
}

void AdaRein::approx_match_sss_c_w(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList) {
	bitset<subs> bits;
	vector<bool> attExist(atts, false);
	for (auto &&iPair: pub.pairs) {
		int att = iPair.att;
		int value = iPair.value, buck = value / levelBuckStep;
		attExist[att] = true;

		_for(wi, 0, adarein_level) {
			if (skippedW[att][wi])
				continue;

			for (auto cmb: dataW[att][wi][0][buck])
				if (cmb.val > value)
					bits[cmb.subID] = true;
			for (int j = max(buck + 1, beBucketW[att][wi].first.first);
				 j <= beBucketW[att][wi].first.second; j++) // ��HEMϵ�е���Ʋ�ͬ, ����ȡ������
				for (auto &&k: dataW[att][wi][0][j])
					bits[k.subID] = true;

			for (auto cmb: dataW[att][wi][1][buck])
				if (cmb.val < value)
					bits[cmb.subID] = true;
			for (int j = min(buck - 1, beBucketW[att][wi].second.first);
				 j >= beBucketW[att][wi].second.second; j--)
				for (auto &&k: dataW[att][wi][1][j])
					bits[k.subID] = true;
		}
	}

	// �����滻Ϊ1��λ����
	for (int ai = 0; ai < atts; ai++)
		if (!attExist[ai]) {
			for (int wi = 0; wi < adarein_level; wi++) {
				if (skippedW[ai][wi]) continue;
				for (int j = beBucketW[ai][wi].second.first; j >= beBucketW[ai][wi].second.second; j--)
					for (auto &&k: dataW[ai][wi][1][j])
						bits[k.subID] = true;
			}
		}

	matchSubs = subs - bits.count();
}

int AdaRein::calMemory() {
	long long size = 0; // Byte
	_for(i, 0, atts) _for(j, 0, numBucket) size += sizeof(Combo) * (data[0][i][j].size() + data[1][i][j].size());
	size += sizeof(bool) * atts + sizeof(attAndCount) * atts;
	//cout << "attAndCount size = " << sizeof(attAndCount) << endl; // 8
	size = size / 1024 / 1024; // MB
	return (int) size;
}

int AdaRein::calMemory_sss_c_w() {
	long long size = 0; // Byte
	_for(ai, 0, atts)
		_for(wi, 0, adarein_level)
			_for(bi, 0, levelBuks)
				size += sizeof(Combo) * (dataW[ai][wi][0][bi].size() +dataW[ai][wi][1][bi].size());
	size += sizeof(bool) * atts + sizeof(attAndCount) * atts;
	//cout << "attAndCount size = " << sizeof(attAndCount) << endl; // 8
	size = size / 1024 / 1024; // MB
	return (int) size;
}
