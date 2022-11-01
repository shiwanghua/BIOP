#include "HEM.h"

HEM::HEM() {
	numSub = 0;
	numDimension = atts;
	buckStep = (valDom - 1) / buks + 1;
	numBucket = (valDom - 1) / buckStep + 1;
	cout << "ExpID = " << expID << ". HEMPS: bit exponent = " << be << ", bucketStep = " << buckStep << ", numBucket = "
		<< numBucket << endl;

	//bucketSub.resize(numBucket);
	data[0].resize(numDimension, vector<vector<Combo>>(numBucket));
	data[1].resize(numDimension, vector<vector<Combo>>(numBucket));

	if (be == -1)
		numBits = be2;
	else
		numBits = pow(2, be);  // ÿ��ά����lowValue��Ӧ��bits�������
//	if (numBits > 1) {
	fullBits.resize(numDimension);  // ά��������Զ���䣬����ֻ��Ҫresizeһ��
	bitStep = (numBucket + numBits - 1) / numBits;  // ÿ����ôԶ����һ��bits
	// �ȼ�д����bitStep = numBucket % numBits == 0 ? numBucket / numBits : numBucket / numBits + 1;
//	}
//	else bitStep = numBucket >> 1;

	endBucket[0] = new int[numBucket];
	endBucket[1] = new int[numBucket];
	bitsID[0] = new int[numBucket];
	bitsID[1] = new int[numBucket];

	//fix[0].resize(numDimension, vector<int>(numBucket));
	//fix[1].resize(numDimension, vector<int>(numBucket));
}

HEM::~HEM() {
	delete[] endBucket[0], endBucket[1], bitsID[0], bitsID[1];
	//delete[] endBucket, bitsID; // ����new������
}

void HEM::insert(IntervalSub sub) {
	for (int i = 0; i < sub.size; i++) {
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

// fullBits�����洢�İ汾
void HEM::initBits() {

	// ����ж�γ�ʼ��
	delete[] endBucket[0], endBucket[1], bitsID[0], bitsID[1];
	endBucket[0] = new int[numBucket];
	endBucket[1] = new int[numBucket];
	bitsID[0] = new int[numBucket];
	bitsID[1] = new int[numBucket];
	bits[0].clear(), bits[1].clear();
	bits[0].resize(numDimension, vector<bitset<subs>>(numBits - 1));
	bits[1].resize(numDimension, vector<bitset<subs>>(numBits - 1));

	// ����old initBits(fullBits��0�������д������εİ汾��
	//bits[0].resize(numDimension, vector<bitset<subs>>(numBits));
	//bits[1].resize(numDimension, vector<bitset<subs>>(numBits));

	// ǰ׺�͡���׺������
	/*_for(i, 0, numDimension) {
		fix[0][i][numBucket - 1] = data[0][i][numBucket - 1].size();
		fix[1][i][0] = data[1][i][0].size();
		_for(j, 1, numBucket) {
			fix[0][i][numBucket - 1 - j] = fix[0][i][numBucket - j] + data[0][i][numBucket - 1 - j].size();
			fix[1][i][j] = fix[1][i][j - 1] + data[1][i][j].size();
		}
	}*/

	//	if (numBits == 1) {                           // ֻ��һ��bitsʱ���У�only ��fullBits
	//
	////		_for(i, 0, numBucket >> 1) {
	////			bitsID[0][i] = 0;                     // ��ʱ��0�Ŵ���0.5~1, ����0~1
	////			bitsID[1][i] = -1;                    // ��ʱ�ò���bits����, -1��ʾ�Ƿ�
	////			endBucket[0][i] = numBucket >> 1;     // ���ʱ������С�����ֵ
	////			endBucket[1][i] = 0;                  // ���ʱ���������ڵ������ֵ
	////		}
	////		_for(i, numBucket >> 1, numBucket) {
	////			bitsID[0][i] = -1;
	////			bitsID[1][i] = 0;
	////			endBucket[0][i] = numBucket;
	////			endBucket[1][i] = numBucket >> 1;
	////		}
	////		_for(i, 0, numDimension) {                // ÿ��ά��
	////			_for(j, 0, numBucket >> 1)            // ÿ����벿�ֵ�Ͱ
	////				_for(k, 0, data[1][i][j].size())  // Ͱ��ÿ������
	////				bits[1][i][0][data[1][i][j][k].subID] = 1;  // Bug: high����low, iά, 0��bits, subID
	////			_for(j, numBucket >> 1, numBucket)    // ÿ���Ұ벿�ֵ�Ͱ
	////				_for(k, 0, data[0][i][j].size())  // Ͱ��ÿ������
	////				bits[0][i][0][data[0][i][j][k].subID] = 1;  // high, iά, 0��bits, subID
	////		}
	//
	//		_for(i, 0, numBucket) {
	//			bitsID[0][i] = bitsID[1][i] = -1;
	//			endBucket[0][i] = numBucket;
	//			endBucket[1][i] = 0;
	//		}
	//		_for(i, 0, numDimension) {              // ÿ��ά��
	//			_for(j, 0, numBucket)           
	//				for (auto &&cb: data[1][i][j])  // Ͱ��ÿ������
	//					fullBits[i][cb.subID] = 1;  // Bug: high����low, iά, 0��bits, subID
	//		}
	//
	//		//cout << "Stop.\n";
	//		return;
	//	}

	_for(i, 0, numBucket) {
		//bitsID[0][i] = (numBucket - i - 1) / bitStep - 1; // (1000-499-1)/500=1, (1000-749-1)/250=1, (1000-936-1)/63=1, (1000-873-1)/63=2, (1000-54-1)/63=15 // ���ӳ���ϵ���Լ���numBitsΪ1�������numBitsΪ1ʱbitStepΪ500
		bitsID[1][i] = i / bitStep - 1; // 750/250=3, 63/63=1
		bitsID[0][i] = numBits - 3 - bitsID[1][i]; // ��֤����ͬһ��cell

		//endBucket[0][i] = numBucket - (bitsID[0][i] + 1) * bitStep; // 1000-1*63=937, 1000-2*63=874, 1000-15*63=55
		endBucket[1][i] = (bitsID[1][i] + 1) * bitStep;
		endBucket[0][i] = min(numBucket, endBucket[1][i] + bitStep);
	}

	int subID, b;   // ��ʼ���������±�
	_for(i, 0, numDimension) {      // ÿ��ά��
		_for(j, 0, numBucket) {     // ÿ��Ͱ
			b = bitsID[0][j] + 1;
			_for(k, 0, data[0][i][j].size()) {
				subID = data[0][i][j][k].subID;
				fullBits[i][subID] = 1;            // 0��bitsÿ�α�����
				//bits[0][i][0][subID] = 1;        
				_for(q, b, numBits - 1)              // Bug: bits�����ǴӸ�λ(���ǹ�)����λ������
					bits[0][i][q][subID] = 1;
			}

			b = bitsID[1][j] + 1; // ����0������С����Ҫ�����bits�����ID
			_for(k, 0, data[1][i][j].size()) {     // Ͱ��ÿ������
				subID = data[1][i][j][k].subID;
				//bits[1][i][0][subID] = 1;        // 0��bitsÿ�α�����
				_for(q, b, numBits - 1)
					bits[1][i][q][subID] = 1;
			}
		}
	}
	//cout << "Stop.\n";
}

// old initBits(fullBits��0�������д������εİ汾��
//void HEM::initBits() {
//
//	// ����ж�γ�ʼ��
//	delete[] endBucket[0], endBucket[1], bitsID[0], bitsID[1];
//	endBucket[0] = new int[numBucket];
//	endBucket[1] = new int[numBucket];
//	bitsID[0] = new int[numBucket];
//	bitsID[1] = new int[numBucket];
//	bits[0].clear(), bits[1].clear();
//	bits[0].resize(numDimension, vector<bitset<subs>>(numBits));
//	bits[1].resize(numDimension, vector<bitset<subs>>(numBits));
//
//	/*_for(i, 0, numDimension) {
//		fix[0][i][numBucket - 1] = data[0][i][numBucket - 1].size();
//		fix[1][i][0] = data[1][i][0].size();
//		_for(j, 1, numBucket) {
//			fix[0][i][numBucket - 1 - j] = fix[0][i][numBucket - j] + data[0][i][numBucket - 1 - j].size();
//			fix[1][i][j] = fix[1][i][j - 1] + data[1][i][j].size();
//		}
//	}*/
//
//	if (numBits == 1) {                           // ֻ��һ��bitsʱ���У���Ϊ����Ĭ��0��bits����0~1��ȫ��bits
//
//		_for(i, 0, numBucket >> 1) {
//			bitsID[0][i] = 0;                     // ��ʱ��0�Ŵ���0.5~1, ����0~1
//			bitsID[1][i] = -1;                    // ��ʱ�ò���bits����, -1��ʾ�Ƿ�
//			endBucket[0][i] = numBucket >> 1;     // ���ʱ������С�����ֵ
//			endBucket[1][i] = 0;                  // ���ʱ���������ڵ������ֵ
//		}
//		_for(i, numBucket >> 1, numBucket) {
//			bitsID[0][i] = -1;
//			bitsID[1][i] = 0;
//			endBucket[0][i] = numBucket;
//			endBucket[1][i] = numBucket >> 1;
//		}
//
//		_for(i, 0, numDimension) {                // ÿ��ά��
//			_for(j, 0, numBucket >> 1)            // ÿ����벿�ֵ�Ͱ
//				_for(k, 0, data[1][i][j].size())  // Ͱ��ÿ������
//				bits[1][i][0][data[1][i][j][k].subID] = 1;  // Bug: high����low, iά, 0��bits, subID
//			_for(j, numBucket >> 1, numBucket)    // ÿ���Ұ벿�ֵ�Ͱ
//				_for(k, 0, data[0][i][j].size())  // Ͱ��ÿ������
//				bits[0][i][0][data[0][i][j][k].subID] = 1;  // high, iά, 0��bits, subID
//		}
//
//		//cout << "Stop.\n";
//		return;
//	}
//
//	int subID;
//    // ֻ����ӦͰ����bits���������ı������������0��1��2��3,1000Ͱʱ
//	//int b;    // ��Ҫ���bits����ĸ���
//	//_for(i, 0, numDimension) {  // ÿ��ά��
//	//	_for(j, 0, numBucket) { // ÿ��Ͱ
//	//		b = (numBucket - j - 1) * numBits / numBucket; // high ����Ҫ��ǵ�bits����ĸ���(û������0��bits����) Bug: numBucket/numBits�����Ƿ�����������bitStep���� (1000-1-499)/250
//	//		_for(k, 0, data[1][i][j].size()) {        // Ͱ��ÿ������
//	//			subID = data[1][i][j][k].subID;
//	//			bits[1][i][0][subID] = 1;             // 0��bitsÿ�α�����
//	//			mmfor(q, numBits - 1, numBits - b)     // Bug: Ͱ�ű��󣬵�λ��bits�Ͳ��ñ����
//	//				bits[1][i][q][subID] = 1;
//	//		}
//	//		b = numBits - b - 1;                      // low ����Ҫ��ǵ�bits����ĸ���(û������0��bits����)
//	//		_for(k, 0, data[0][i][j].size()) {
//	//			subID = data[0][i][j][k].subID;
//	//			bits[0][i][0][subID] = 1;             // 0��bitsÿ�α�����
//	//			mmfor(q, numBits - 1, numBits - b)     // Bug: bits�����ǴӸ�λ(���ǹ�)����λ������
//	//				bits[0][i][q][subID] = 1;
//	//		}
//	//	}
//	//}
//	//_for(i, 0, numBucket) {
//	//	bitsID[0][i] = (numBucket - i - 1) * numBits / numBucket; // (1000-749-1)*4/1000=1
//	//	bitsID[1][i] = i * numBits / numBucket;                   // 750*4/1000=3
//	//	endBucket[0][i] = numBucket - bitsID[0][i] * bitStep; 
//	//	endBucket[1][i] = bitsID[1][i] * bitStep;
//	//	if (bitsID[0][i] == 0) // ������0��bits������ȫ���ǵ�bits�ı��
//	//		bitsID[0][i] = -1; // -1��ʾ��ʱ�ò���bits����
//	//	if (bitsID[1][i] == 0) // ������ifҪ�ŵ������Ϊ��endBucket���õ�
//	//		bitsID[1][i] = -1;
//	//}
//	int b;   // ��ʼ���������±�
//	_for(i, 0, numBucket) {
//		bitsID[0][i] = (numBucket - i - 1) / bitStep; // (1000-499-1)/500=1, (1000-749-1)/250=1, (1000-936-1)/63=1, (1000-873-1)/63=2, (1000-54-1)/63=15
//		bitsID[1][i] = i / bitStep; // 750/250=3, 63/63=1
//		endBucket[0][i] = numBucket - bitsID[0][i] * bitStep; // 1000-1*63=937, 1000-2*63=874, 1000-15*63=55
//		endBucket[1][i] = bitsID[1][i] * bitStep;
//	}
//	_for(i, 0, numDimension) {      // ÿ��ά��
//		_for(j, 0, numBucket) {     // ÿ��Ͱ
//			b = bitsID[0][j] + 1;
//			_for(k, 0, data[0][i][j].size()) {
//				subID = data[0][i][j][k].subID;
//				fullBits[i][subID] = 1;
//				bits[0][i][0][subID] = 1;        // 0��bitsÿ�α�����
//				_for(q, b, numBits)              // Bug: bits�����ǴӸ�λ(���ǹ�)����λ������
//					bits[0][i][q][subID] = 1;
//			}
//
//			b = bitsID[1][j] + 1;   // ����0������С����Ҫ�����bits�����ID
//			_for(k, 0, data[1][i][j].size()) {   // Ͱ��ÿ������
//				subID = data[1][i][j][k].subID;
//				bits[1][i][0][subID] = 1;        // 0��bitsÿ�α�����
//				_for(q, b, numBits)
//					bits[1][i][q][subID] = 1;
//			}
//		}
//	}
//	_for(i, 0, numBucket) {
//		if (bitsID[0][i] == 0)      // ������0��bits������ȫ���ǵ�bits�ı��
//			bitsID[0][i] = -1;      // -1��ʾ��ʱ�ò���bits����
//		if (bitsID[1][i] == 0)      // ������ifҪ�ŵ������Ϊ��endBucket���õ�bitsID
//			bitsID[1][i] = -1;
//	}
//	//cout << "Stop.\n";
//
//}

// ������ʱ�����
void HEM::match(const Pub &pub, int &matchSubs) {
	bitset<subs> b;
	vector<bool> attExist(numDimension, false);
	int value, att, buck;
	_for(i, 0, pub.size) {
		value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
		attExist[att] = true;
		_for(k, 0, data[0][att][buck].size()) if (data[0][att][buck][k].val > value)
				b[data[0][att][buck][k].subID] = 1;
		_for(k, 0, data[1][att][buck].size()) if (data[1][att][buck][k].val < value)
				b[data[1][att][buck][k].subID] = 1;

		_for(j, buck + 1, endBucket[0][buck]) _for(k, 0, data[0][att][j].size()) b[data[0][att][j][k].subID] = 1;
		mmfor(j, buck - 1, endBucket[1][buck]) _for(k, 0, data[1][att][j].size()) b[data[1][att][j][k].subID] = 1;

		if (bitsID[0][buck] != -1)
			b = b | bits[0][att][bitsID[0][buck]];
		if (bitsID[1][buck] != -1)
			b = b | bits[1][att][bitsID[1][buck]]; // Bug: ��att����i
	}

//	if (numBits > 1) {
		_for(i, 0, numDimension) if (!attExist[i])
				b = b | fullBits[i];
//	} else {
//		_for(i, 0, numDimension) if (!attExist[i])
//				_for(j, 0, bitStep) _for(k, 0, data[0][i][j].size()) b[data[0][i][j][k].subID] = 1;
//
//		_for(i, 0, numDimension) if (!attExist[i])
//				b = b | bits[0][i][0];
//	}

	//_for(i, 0, subs)
	//	if (!b[i])
	//	{
	//		++matchSubs;
	//		//cout << "HEM matches sub: : " << i << endl;
	//	}
	matchSubs = subs - b.count();
}

// ����ʱ�����
void HEM::match_debug(const Pub& pub, int& matchSubs)
{
	bitset<subs> b;
	vector<bool> attExist(numDimension, false);
	int value, att, buck;
	_for(i, 0, pub.size)
	{
		Timer compareStart;
		value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
		attExist[att] = true;
		_for(k, 0, data[0][att][buck].size())
			if (data[0][att][buck][k].val > value)
				b[data[0][att][buck][k].subID] = 1;
		_for(k, 0, data[1][att][buck].size())
			if (data[1][att][buck][k].val < value)
				b[data[1][att][buck][k].subID] = 1;
		compareTime += (double)compareStart.elapsed_nano();

		Timer markStart;
		_for(j, buck + 1, endBucket[0][buck])
			_for(k, 0, data[0][att][j].size())
			b[data[0][att][j][k].subID] = 1;
		mmfor(j, buck - 1, endBucket[1][buck])
			_for(k, 0, data[1][att][j].size())
			b[data[1][att][j][k].subID] = 1;
		markTime += (double)markStart.elapsed_nano();

		Timer orStart;
		if (bitsID[0][buck] != -1)
			b = b | bits[0][att][bitsID[0][buck]];
		if (bitsID[1][buck] != -1)
			b = b | bits[1][att][bitsID[1][buck]]; // Bug: ��att����i
		orTime += (double)orStart.elapsed_nano();
	}

	//	if (numBits > 1) {
	Timer orStart;
	_for(i, 0, numDimension)
		if (!attExist[i])
			b = b | fullBits[i];
	orTime += (double)orStart.elapsed_nano();
	//	}
	//	else {
	//		Timer markStart;
	//		_for(i, 0, numDimension)
	//			if (!attExist[i])
	//				_for(j, 0, bitStep)
	//				_for(k, 0, data[0][i][j].size())
	//				b[data[0][i][j][k].subID] = 1;
	//		markTime += (double)markStart.elapsed_nano();
	//
	//		Timer orStart;
	//		_for(i, 0, numDimension)
	//			if (!attExist[i])
	//				b = b | bits[0][i][0];
	//		orTime += (double)orStart.elapsed_nano();
	//	}

	Timer bitStart;
		_for(i, 0, subs)
			if (!b[i])
			{
				++matchSubs;
				//cout << "HEM matches sub: : " << i << endl;
			}
//	matchSubs = subs - b.count();
	bitTime += (double)bitStart.elapsed_nano();
}

//void HEM::calBucketSize() {
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

int HEM::calMemory() {
	long long size = 0; // Byte
	_for(i, 0, numDimension) {
		// ��ÿ��ά����bits�������һ������ 2*numDimension*numBits*sizeof(bitset<subs>)
		//if (numBits > 1)
		size += sizeof(bitset<subs>) * (bits[0][i].size() + bits[1][i].size());
		_for(j, 0, numBucket) size += sizeof(Combo) * (data[0][i][j].size() + data[1][i][j].size());
	}

	// fullBits
	size += sizeof(bitset<subs>) * fullBits.size(); // fullBits.size()��numDimension

	// ����endBucket������bitsID
	size += 4 * numBucket * sizeof(int);
	size = size / 1024 / 1024; // MB
	return (int)size;
}

void HEM::printRelation() {
	cout << "\n\nHEMPSMap LowBucket\n";
	_for(i, 0, numBucket) {
		cout << "LBkt" << i << ": bID=" << bitsID[0][i] << ", eBkt=" << endBucket[0][i] << "; ";
		if (i % 5 == 0 && i > 0)cout << "\n";
	}
	cout << "\n\nHEMPSMap HighBucket\n";
	_for(i, 0, numBucket) {
		cout << "HBkt" << i << ": bID=" << bitsID[1][i] << ", eBkt=" << endBucket[1][i] << "; ";
		if (i % 5 == 0 && i > 0)cout << "\n";
	}
	cout << "\n\n";
}
