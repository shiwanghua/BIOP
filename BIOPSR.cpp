#include "BIOPSR.h"

BIOPSR::BIOPSR() {
	numSub = 0;
	numDimension = atts;
	numGroup = (numDimension - 1) / gs + 1;
	numState = pow(2, gs); // ÿ��ά����������״̬
	buckStep = (valDom - 1) / buks + 1;
	numBucket = (valDom - 1) / buckStep + 1;
	bitStep = numBucket >> 1;

	cout << "ExpID = " << expID << ". BIOPSRPS: bucketStep = " << buckStep << ", numBucket = " << numBucket << endl;

	if (be != 0) be = 0;
	//assert(be == 0);

	//bucketSub.resize(numBucket);
	data[0].resize(numDimension, vector<vector<Combo>>(numBucket));
	data[1].resize(numDimension, vector<vector<Combo>>(numBucket));

	endBucket[0] = new int[numBucket];
	endBucket[1] = new int[numBucket];
	bitsID[0] = new int[numBucket];
	bitsID[1] = new int[numBucket];
}

BIOPSR::~BIOPSR() {
	delete[] endBucket[0], endBucket[1], bitsID[0], bitsID[1];
}

void BIOPSR::insert(IntervalSub sub)
{
	for (int i = 0; i < sub.size; i++)
	{
		IntervalCnt cnt = sub.constraints[i];
		Combo c;
		// int bucketID = cnt.lowValue / buckStep; // Bug: ���ﱻ����
		c.val = cnt.lowValue;
		c.subID = sub.id;
		data[0][cnt.att][cnt.lowValue / buckStep].push_back(c);
		c.val = cnt.highValue;
		data[1][cnt.att][cnt.highValue / buckStep].push_back(c);
	}
	numSub++;
}

void BIOPSR::initBits() {

	// ����ж�γ�ʼ��
	delete[] endBucket[0], endBucket[1], bitsID[0], bitsID[1];
	endBucket[0] = new int[numBucket];
	endBucket[1] = new int[numBucket];
	bitsID[0] = new int[numBucket];
	bitsID[1] = new int[numBucket];
	bits[0].clear(), bits[1].clear();
	bits[0].resize(numDimension);
	bits[1].resize(numDimension);
	bitsSR.clear();
	bitsSR.resize(numGroup, vector<bitset<subs>>(numState)); // ���һ�����û��ô��״̬

	_for(i, 0, numBucket >> 1) {
		bitsID[0][i] = 0;                     // ��ʱ��0�Ŵ���0.5~1, ����0~1
		bitsID[1][i] = -1;                    // ��ʱ�ò���bits����, -1��ʾ�Ƿ�
		endBucket[0][i] = bitStep;            // ���ʱ������С�����ֵ
		endBucket[1][i] = 0;                  // ���ʱ���������ڵ������ֵ
	}
	_for(i, numBucket >> 1, numBucket) {
		bitsID[0][i] = -1;
		bitsID[1][i] = 0;
		endBucket[0][i] = numBucket;
		endBucket[1][i] = bitStep;
	}

	_for(i, 0, numDimension) {                // ÿ��ά��
		_for(j, 0, bitStep)                   // ÿ����벿�ֵ�Ͱ
			_for(k, 0, data[1][i][j].size())  // Ͱ��ÿ������
			bits[1][i][data[1][i][j][k].subID] = 1;  // high, iά, subID
		_for(j, bitStep, numBucket)           // ÿ���Ұ벿�ֵ�Ͱ
			_for(k, 0, data[0][i][j].size())  // Ͱ��ÿ������
			bits[0][i][data[0][i][j][k].subID] = 1;  // high, iά, subID
	}

	_for(g, 0, numGroup) {
		int begin = g * gs, end = min(numDimension, (g + 1) * gs); // Ϊ����Ӧ���һ���鲻��gs��ά�ȵ����
		int group_size = end - begin;
		int state_num = pow(2, group_size);                        // һ�㶼����numState
		_for(s, 0, state_num) {
			int si = 1; // s�����λ��ʾbeginά��, ���λ��ʾend-1ά��
			_for(i, 0, group_size) {
				if (si & s) // s�ĵ�iλΪ1��ʾ��high�ϵ�bitset
					bitsSR[g][s] = bitsSR[g][s] | bits[1][begin + i];
				else bitsSR[g][s] = bitsSR[g][s] | bits[0][begin + i];
				si = si << 1;
			}
		}
	}

	//cout << "Stop.\n";
	return;
}

void BIOPSR::match(const Pub& pub, int& matchSubs)
{
	bitset<subs> globalBitset;
	vector<int> attExist(numDimension, -1);
	int value, att, buck;
	_for(i, 0, pub.size)
	{
		Timer compareStart;
		value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
		attExist[att] = bitsID[1][buck] + 1; // ��low�ϵľ���0, ��high�ϵľ���1
		_for(k, 0, data[0][att][buck].size())
			if (data[0][att][buck][k].val > value)
				globalBitset[data[0][att][buck][k].subID] = 1;
		_for(k, 0, data[1][att][buck].size())
			if (data[1][att][buck][k].val < value)
				globalBitset[data[1][att][buck][k].subID] = 1;
		compareTime += (double)compareStart.elapsed_nano();

		Timer markStart;
		_for(j, buck + 1, endBucket[0][buck])
			_for(k, 0, data[0][att][j].size())
			globalBitset[data[0][att][j][k].subID] = 1;
		mmfor(j, buck - 1, endBucket[1][buck])
			_for(k, 0, data[1][att][j].size())
			globalBitset[data[1][att][j][k].subID] = 1;
		markTime += (double)markStart.elapsed_nano();

		//Timer orStart;
		//if (bitsID[0][buck] != -1) 
		//	globalBitset = globalBitset | bits[0][att];
		//else //if (bitsID[1][buck] != -1)  // ÿ��ά����ֻ������״̬
		//	globalBitset = globalBitset | bits[1][att]; // Bug: ��att����i
		//orTime += (double)orStart.elapsed_nano();
	}

	Timer markStart2;
	_for(i, 0, numDimension)
		if (attExist[i] == -1) {
			attExist[i] = 0;
			_for(j, 0, bitStep) // ���ά�ȵ�״̬Ϊ0, ��ʹ��low�ϵ�bitset
				_for(k, 0, data[0][i][j].size())
				globalBitset[data[0][i][j][k].subID] = 1;
		}else{
			
		}
	markTime += (double)markStart2.elapsed_nano();

	Timer orStart;
	int i = 0, j, state, end; // i ����ÿ��ά��, j �������ڲ���ÿһλ, ÿ�������һ��Ԫ�ص���һ��λ����end, 
	_for(g, 0, numGroup) {
		state = 0, j = 1;
		end = min(numDimension, i + gs);
		while (i < end) {
			if (attExist[i])
				state |= j;
			j = j << 1;
			i++;
		}
		globalBitset = globalBitset | bitsSR[g][state];
	}
	orTime += (double)orStart.elapsed_nano();

	Timer bitStart;
	_for(i, 0, subs)
		if (!globalBitset[i])
		{
			++matchSubs;
			//cout << "BIOPSR matches sub: : " << i << endl;
		}
	bitTime += (double)bitStart.elapsed_nano();
}

//void BIOPSR::calBucketSize() {
//	bucketSub.clear();
//	bucketSub.resize(numBucket);
//	_for(i, 0, numDimension)
//		_for(j, 0, numBucket)
//		{
//			_for(k, 0, data[0][i][j].size())
//				bucketSub[j].insert(data[0][i][j][k].subID);
//			_for(k, 0, data[1][i][j].size())
//				bucketSub[j].insert(data[1][i][j][k].subID);
//		}
//}

int BIOPSR::calMemory() {
	long long size = 0; // Byte
	size += 2 * numDimension * sizeof(bitset<subs>); // bits
	size += numGroup*numState* sizeof(bitset<subs>); // bitsSR
	size -= (numState - pow(2, numDimension % numGroup)) * sizeof(bitset<subs>); // ��ȥbitsSR���һ�������ڴ��С
	_for(i, 0, numDimension) {
		_for(j, 0, numBucket)
			size += sizeof(Combo) * (data[0][i][j].size() + data[1][i][j].size());
	}

	// ����endBucket������bitsID
	size += 4 * numBucket * sizeof(int);
	size = size / 1024 / 1024; // MB
	return (int)size;
}

void BIOPSR::printRelation() {
	cout << "\n\nBIOP-SR-PS Map LowBucket\n";
	_for(i, 0, numBucket) {
		cout << "LBkt" << i << ": bID=" << bitsID[0][i] << ", eBkt=" << endBucket[0][i] << "; ";
		if (i % 5 == 0 && i > 0)cout << "\n";
	}
	cout << "\n\nBIOP-SR-PS Map HighBucket\n";
	_for(i, 0, numBucket) {
		cout << "HBkt" << i << ": bID=" << bitsID[1][i] << ", eBkt=" << endBucket[1][i] << "; ";
		if (i % 5 == 0 && i > 0)cout << "\n";
	}
	cout << "\n\n";
}
