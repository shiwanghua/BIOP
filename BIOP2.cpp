#include "BIOP2.h"

BIOP2::BIOP2(){
	numSub = 0;
	numDimension = atts;
	buckStep = (valDom - 1) / buks + 1;
	numBucket = (valDom - 1) / buckStep + 1;
	cout << "ExpID = " << expID << ". BIOP2SD: bit exponent = " << be << ", bucketStep = " << buckStep << ", numBucket = " << numBucket << endl;

	// ���Ͱ����仯�����´���ҲҪ����init������
	//bucketSub.resize(numBucket);
	data[0].resize(numDimension, vector<vector<Combo>>(numBucket));
	data[1].resize(numDimension, vector<vector<Combo>>(numBucket));

	if (be == -1)
		numBits = numBucket;
	else
		numBits = pow(2, be);  // ÿ��ά����lowValue��Ӧ��bits�������
	if (numBits > 1) {
		fullBits.resize(numDimension);  // ά��������Զ���䣬����ֻ��Ҫresizeһ��
		bitStep = (numBucket + numBits - 1) / numBits;  // ÿ����ôԶ����һ��bits
		// �ȼ�д����bitStep = numBucket % numBits == 0 ? numBucket / numBits : numBucket / numBits + 1;
	}
	else bitStep = numBucket >> 1;

	doubleReverse[0] = new bool* [numDimension];
	doubleReverse[1] = new bool* [numDimension];
	endBucket[0] = new int* [numDimension];
	endBucket[1] = new int* [numDimension];
	bitsID[0] = new int* [numDimension];
	bitsID[1] = new int* [numDimension];
	_for(i, 0, numDimension) {
		doubleReverse[0][i] = new bool[numBucket];
		doubleReverse[1][i] = new bool[numBucket];
		endBucket[0][i] = new int[numBucket];
		endBucket[1][i] = new int[numBucket];
		bitsID[0][i] = new int[numBucket];
		bitsID[1][i] = new int[numBucket];
	}

	fix[0].resize(numDimension, vector<int>(numBucket + 1, 0));
	fix[1].resize(numDimension, vector<int>(numBucket + 1, 0));
}

BIOP2::~BIOP2() {
	_for(i, 0, numDimension)
		delete[] doubleReverse[0][i], doubleReverse[1][i], endBucket[0][i], endBucket[1][i], bitsID[0][i], bitsID[1][i];
	delete[] endBucket[0], endBucket[1], bitsID[0], bitsID[1], doubleReverse[0], doubleReverse[1];
}

void BIOP2::insert(IntervalSub sub)
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

// fullBits�����洢�İ汾
void BIOP2::initBits() {

	// ����ж�γ�ʼ��
	_for(i, 0, numDimension)
		delete[] doubleReverse[0][i], doubleReverse[1][i], endBucket[0][i], endBucket[1][i], bitsID[0][i], bitsID[1][i];
	delete[] doubleReverse[0], doubleReverse[1], endBucket[0], endBucket[1], bitsID[0], bitsID[1];
	doubleReverse[0] = new bool* [numDimension];
	doubleReverse[1] = new bool* [numDimension];
	endBucket[0] = new int* [numDimension];
	endBucket[1] = new int* [numDimension];
	bitsID[0] = new int* [numDimension];
	bitsID[1] = new int* [numDimension];
	_for(i, 0, numDimension) {
		doubleReverse[0][i] = new bool[numBucket];
		doubleReverse[1][i] = new bool[numBucket];
		endBucket[0][i] = new int[numBucket];
		endBucket[1][i] = new int[numBucket];
		bitsID[0][i] = new int[numBucket];
		bitsID[1][i] = new int[numBucket];
	}
	bits[0].clear(), bits[1].clear();
	bits[0].resize(numDimension, vector<bitset<subs>>(numBits > 1 ? numBits - 1 : 1));
	bits[1].resize(numDimension, vector<bitset<subs>>(max(numBits - 1, 1)));

	//// ǰ׺�͡���׺������, ����������
	//_for(i, 0, numDimension) {
	//	_for(j, 1, numBucket) {
	//		fix[0][i][numBucket - 1 - j] = fix[0][i][numBucket - j] + data[0][i][numBucket - j].size();
	//		fix[1][i][j] = fix[1][i][j - 1] + data[1][i][j - 1].size();
	//	}
	//	// ��������ĺʹ������һ��Ԫ����
	//	fix[0][i][numBucket] = fix[0][i][0] + data[0][i][0].size();
	//	fix[1][i][numBucket] = fix[1][i][numBucket-1] + data[1][i][numBucket - 1].size();  // Bug: ����-1!!!
	//}

	// ǰ׺������(��������)����׺������(��������)
	_for(i, 0, numDimension) {
		fix[0][i][numBucket - 1] = data[0][i][numBucket - 1].size();
		_for(j, 1, numBucket) {
			fix[0][i][numBucket - 1 - j] = fix[0][i][numBucket - j] + data[0][i][numBucket - j - 1].size();
			fix[1][i][j] = fix[1][i][j - 1] + data[1][i][j - 1].size();
		}
		// ��������ĺʹ������һ��Ԫ����
		fix[0][i][numBucket] = fix[0][i][0];
		fix[1][i][numBucket] = fix[1][i][numBucket - 1] + data[1][i][numBucket - 1].size(); // Bug: ����-1!!!
	}

	if (numBits == 1) {                           // ֻ��һ��bitsʱ���У�����fullBits

		_for(i, 0, numDimension) {
			_for(j, 0, numBucket >> 1) {
				// ��ʱlow��һ��һ���õ�Ҳֻ���õ�0��bits����
				bitsID[0][i][j] = 0;                     // ��ʱ��0�Ŵ���0.5~1
				endBucket[0][i][j] = numBucket >> 1;     // ���ʱ������С�����ֵ
				doubleReverse[0][i][j] = false;          // Bug�����ҲҪ��ֵ��û��ʼ����������һ����Сʱ
				int bid1 = -1, bid2 = 0;
				int bktid1 = 0, bktid2 = bitStep;
				int workload1 = fix[1][i][j], workload2 = fix[1][i][bktid2] - fix[1][i][j]; // ��j��ͰҲ��Ҫ��1���0������бȽ�
				if (workload1 <= workload2) {
					bitsID[1][i][j] = bid1;              // Ϊ-1ʱ��ʾȷʵ�ò���bits����
					endBucket[1][i][j] = bktid1;         // ������1ʱ�� j-1 ������ bktid1 ��Ͱ
					doubleReverse[1][i][j] = false;
				}
				else {
					bitsID[1][i][j] = bid2;
					endBucket[1][i][j] = bktid2;         // ���ط�����0ʱ�� j ������ bktid2 - 1 ��Ͱ
					doubleReverse[1][i][j] = true;
				}
			}
			_for(j, numBucket >> 1, numBucket) {
				// ��ʱhigh��һ��һ���õ�Ҳֻ���õ�0��bits����
				bitsID[1][i][j] = 0;
				endBucket[1][i][j] = bitStep;            // ���ʱ�������������ֵ
				doubleReverse[1][i][j] = false;          

				int bid1 = -1, bid2 = 0;
				int bktid1 = numBucket, bktid2 = numBucket - bitStep;// 1000-500
				int workload1 = fix[0][i][j], workload2 = fix[0][i][bktid2 - 1] - fix[0][i][j];
				if (workload1 <= workload2) {
					bitsID[0][i][j] = bid1;              // Ϊ-1ʱ��ʾȷʵ�ò���bits����
					endBucket[0][i][j] = bktid1;         // ���ұ��1ʱ�� j+1 ������ bktid1-1 ��Ͱ
					doubleReverse[0][i][j] = false;
				}
				else {
					bitsID[0][i][j] = bid2;
					endBucket[0][i][j] = bktid2;         // ���ط�����0ʱ�� bktid2 ������ j ��Ͱ
					doubleReverse[0][i][j] = true;
				}
			}
		}

		// ��α���������ӳ���������ˣ��ɲ����Ⱥ�ִ��
		_for(i, 0, numDimension) {                // ÿ��ά��
			_for(j, 0, numBucket >> 1)            // ÿ����벿�ֵ�Ͱ
				_for(k, 0, data[1][i][j].size())  // Ͱ��ÿ������
				bits[1][i][0][data[1][i][j][k].subID] = 1;  // Bug: high����low, iά, 0��bits, subID
			_for(j, numBucket >> 1, numBucket)    // ÿ���Ұ벿�ֵ�Ͱ
				_for(k, 0, data[0][i][j].size())  // Ͱ��ÿ������
				bits[0][i][0][data[0][i][j][k].subID] = 1;  // low, iά, 0��bits, subID
		}
		//cout << "Stop.\n";
		return;
	}

	_for(i, 0, numDimension)
		_for(j, 0, numBucket) {
		int bid1 = j / bitStep - 1, bid2 = bid1 + 1;   // 62/63-1=-1, 63/63-1=0, -1+1=0     945/63-1=14, 15������fullBits
		int bktid1 = (bid1 + 1) * bitStep, bktid2 = min(bktid1 + bitStep, numBucket); // (-1+1)*63=0 <= 62, (0+1)*63=63>62
		int workload1 = fix[1][i][j] - fix[1][i][bktid1], workload2 = fix[1][i][bktid2] - fix[1][i][j]; // ��j��ͰҲ��Ҫ��1���0������бȽ�
		if (workload1 <= workload2) {
			bitsID[1][i][j] = bid1;      // Ϊ-1ʱ��ʾȷʵ�ò���bits����
			endBucket[1][i][j] = bktid1; // ������1ʱ�� j-1 ������ bktid1 ��Ͱ
			doubleReverse[1][i][j] = false;
		}
		else {
			// numBits>1, bid2=numBits-1ʱ��ʾ��fullBits
			bitsID[1][i][j] = bid2;        // bid2��ԭ����id����1��������в����ǵ�ʱ����Ҫ���ݼ�һ���ж�
			endBucket[1][i][j] = bktid2;   // ���ط�����0ʱ�� j ������ bktid2 - 1 ��Ͱ
			doubleReverse[1][i][j] = true;
		}

		//bid1 = (numBucket - j - 1) / bitStep - 1, bid2 = bid1 + 1;// 2,3; 1,2; 0,1; -1,0; // Ͱ������bits����������ʱ���ܹ���ͬһ��cell
		//bktid1 = numBucket - (bid1 + 1) * bitStep, bktid2 = max(0, bktid1 - bitStep); // <bktid1, >=bktid2 250,0; 500,250; 750,500; 1000,750;
		bid1 = numBits - 3 - bid1, bid2 = bid1 + 1;
		bktid1 = bktid1 ^ bktid2;
		bktid2 = bktid1 ^ bktid2;
		bktid1 = bktid1 ^ bktid2;
		workload1 = fix[0][i][j] - fix[0][i][bktid1 - 1];
		workload2 = fix[0][i][bktid2 > 0 ? bktid2 - 1 : numBucket] - fix[0][i][j];
		if (workload1 <= workload2) {
			bitsID[0][i][j] = bid1;
			endBucket[0][i][j] = bktid1;
			doubleReverse[0][i][j] = false; // Bug: ������û��ʼ��������Ϊtrue��
		}
		else {
			bitsID[0][i][j] = bid2;
			endBucket[0][i][j] = bktid2;
			doubleReverse[0][i][j] = true;
		}
	}

	int subID, b;   // ��ʼ���������±�
	_for(i, 0, numDimension) {          // ÿ��ά��
		_for(j, 0, numBucket) {         // ÿ��Ͱ
			if (doubleReverse[0][i][j])
				b = bitsID[0][i][j];       // ��С����Ҫ�����bits�����ID
			else b = bitsID[0][i][j] + 1;
			_for(k, 0, data[0][i][j].size()) {
				subID = data[0][i][j][k].subID;
				fullBits[i][subID] = 1; // 0��bitsÿ�α�����   
				_for(q, b, numBits - 1) // Bug: bits�����ǴӸ�λ(���ǹ�)����λ������
					bits[0][i][q][subID] = 1;
			}

			if (doubleReverse[1][i][j])
				b = bitsID[1][i][j];
			else b = bitsID[1][i][j] + 1;  // ��С����Ҫ�����bits�����ID
			_for(k, 0, data[1][i][j].size()) {     // Ͱ��ÿ������
				subID = data[1][i][j][k].subID;
				_for(q, b, numBits - 1)
					bits[1][i][q][subID] = 1;
			}
		}
	}
	//cout << "Stop.\n";
}

// ����˳��汾��buck���ʱ���ڱȽ�ʱһ������ 
// ���ʱ����Ȼ������һ�㣬���Ƚ�ʱ�����һ����
//void BIOP2::match(const Pub& pub, int& matchSubs)
//{
//	bitset<subs> b, bLocal;
//	vector<bool> attExist(numDimension, false);
//	int value, att, buck;
//
//	_for(i, 0, pub.size)
//	{
//		Timer compareStart;
//		value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
//		attExist[att] = true;
//		_for(k, 0, data[0][att][buck].size())
//			if (data[0][att][buck][k].val > value)
//				b[data[0][att][buck][k].subID] = 1;
//		_for(k, 0, data[1][att][buck].size())
//			if (data[1][att][buck][k].val < value)
//				b[data[1][att][buck][k].subID] = 1;
//		compareTime += (double)compareStart.elapsed_nano();
//
//		if (doubleReverse[0][att][buck]) {
//			Timer markStart;
//			if (bitsID[0][att][buck] == numBits - 1 && numBits > 1)
//				bLocal = fullBits[att];
//			else
//				bLocal = bits[0][att][bitsID[0][att][buck]];
//			_for(j, endBucket[0][att][buck], buck)  // buck�����Ƚ�ʱ�ٱ��
//				_for(k, 0, data[0][att][j].size())
//				bLocal[data[0][att][j][k].subID] = 0;
//			markTime += (double)markStart.elapsed_nano();
//
//			Timer compareStart;
//			_for(k, 0, data[0][att][buck].size())
//				if (data[0][att][buck][k].val > value) {
//					b[data[0][att][buck][k].subID] = 1;
//					bLocal[data[0][att][buck][k].subID] = 0;
//				}
//			compareTime += (double)compareStart.elapsed_nano();
//
//			Timer orStart;
//			b = b | bLocal;
//			orTime += (double)orStart.elapsed_nano();
//		}
//		else {
//			Timer compareStart;
//			_for(k, 0, data[0][att][buck].size())
//				if (data[0][att][buck][k].val > value)
//					b[data[0][att][buck][k].subID] = 1;
//			compareTime += (double)compareStart.elapsed_nano();
//
//			Timer markStart;
//			_for(j, buck + 1, endBucket[0][att][buck])
//				_for(k, 0, data[0][att][j].size())
//				b[data[0][att][j][k].subID] = 1;
//			markTime += (double)markStart.elapsed_nano();
//
//			Timer orStart;
//			if (bitsID[0][att][buck] != -1)
//				b = b | bits[0][att][bitsID[0][att][buck]];
//			orTime += (double)orStart.elapsed_nano();
//		}
//
//		if (doubleReverse[1][att][buck]) {
//			Timer markStart;
//			if (bitsID[1][att][buck] == numBits - 1 && numBits > 1)
//				bLocal = fullBits[att];
//			else
//				bLocal = bits[1][att][bitsID[1][att][buck]];
//			_for(j, buck+1, endBucket[1][att][buck]) // buck�����Ƚ�ʱ���
//				_for(k, 0, data[1][att][j].size())
//				bLocal[data[1][att][j][k].subID] = 0;
//			markTime += (double)markStart.elapsed_nano();
//
//			Timer compareStart;
//			_for(k, 0, data[1][att][buck].size())
//				if (data[1][att][buck][k].val < value) {
//					b[data[1][att][buck][k].subID] = 1;
//					bLocal[data[1][att][buck][k].subID] = 0;
//				}		
//			compareTime += (double)compareStart.elapsed_nano();
//
//			Timer orStart;
//			b = b | bLocal;
//			orTime += (double)orStart.elapsed_nano();
//		}
//		else {
//			Timer compareStart;
//			_for(k, 0, data[1][att][buck].size())
//				if (data[1][att][buck][k].val < value)
//					b[data[1][att][buck][k].subID] = 1;
//			compareTime += (double)compareStart.elapsed_nano();
//
//			Timer markStart;
//			_for(j, endBucket[1][att][buck], buck)
//				_for(k, 0, data[1][att][j].size())
//				b[data[1][att][j][k].subID] = 1;
//			markTime += (double)markStart.elapsed_nano();
//
//			Timer orStart;
//			if (bitsID[1][att][buck] != -1)
//				b = b | bits[1][att][bitsID[1][att][buck]]; // Bug: ��att����i
//			orTime += (double)orStart.elapsed_nano();
//		}
//	}
//
//	Timer orStart;
//	_for(i, 0, numDimension)
//		if (!attExist[i])
//			b = b | fullBits[i];
//	orTime += (double)orStart.elapsed_nano();
//
//	Timer bitStart;
//	_for(i, 0, subs)
//		if (!b[i])
//		{
//			++matchSubs;
//			//cout << "BIOP2 matches sub: " << i << endl;
//		}
//	bitTime += (double)bitStart.elapsed_nano();
//}

void BIOP2::match(const Pub& pub, int& matchSubs)
{
	bitset<subs> b, bLocal;
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

		if (doubleReverse[0][att][buck]) {
			Timer markStart;
			if (bitsID[0][att][buck] == numBits - 1 && numBits > 1)
				bLocal = fullBits[att];
			else
				bLocal = bits[0][att][bitsID[0][att][buck]];
			_for(j, endBucket[0][att][buck], buck + 1)
				_for(k, 0, data[0][att][j].size())
				bLocal[data[0][att][j][k].subID] = 0;
			markTime += (double)markStart.elapsed_nano();

			Timer orStart;
			b = b | bLocal;
			orTime += (double)orStart.elapsed_nano();
		}
		else {
			Timer markStart;
			_for(j, buck + 1, endBucket[0][att][buck])
				_for(k, 0, data[0][att][j].size())
				b[data[0][att][j][k].subID] = 1;
			markTime += (double)markStart.elapsed_nano();
			Timer orStart;
			if (bitsID[0][att][buck] != -1)
				b = b | bits[0][att][bitsID[0][att][buck]];
			orTime += (double)orStart.elapsed_nano();
		}

		if (doubleReverse[1][att][buck]) {
			Timer markStart;
			if (bitsID[1][att][buck] == numBits - 1 && numBits > 1)
				bLocal = fullBits[att];
			else
				bLocal = bits[1][att][bitsID[1][att][buck]];
			_for(j, buck, endBucket[1][att][buck])
				_for(k, 0, data[1][att][j].size())
				bLocal[data[1][att][j][k].subID] = 0;
			markTime += (double)markStart.elapsed_nano();
			Timer orStart;
			b = b | bLocal;
			orTime += (double)orStart.elapsed_nano();
		}
		else {
			Timer markStart;
			_for(j, endBucket[1][att][buck], buck)
				_for(k, 0, data[1][att][j].size())
				b[data[1][att][j][k].subID] = 1;
			markTime += (double)markStart.elapsed_nano();
			Timer orStart;
			if (bitsID[1][att][buck] != -1)
				b = b | bits[1][att][bitsID[1][att][buck]]; // Bug: ��att����i
			orTime += (double)orStart.elapsed_nano();
		}
	}

	Timer orStart;
	_for(i, 0, numDimension)
		if (!attExist[i])
			b = b | fullBits[i];
	orTime += (double)orStart.elapsed_nano();

	Timer bitStart;
	_for(i, 0, subs)
		if (!b[i])
		{
			++matchSubs;
			//cout << "BIOP2 matches sub: " << i << endl;
		}
	bitTime += (double)bitStart.elapsed_nano();
}

//void BIOP2::calBucketSize() {
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

int BIOP2::calMemory() {
	long long size = 0; // Byte
	_for(i, 0, numDimension) {
		// ��ÿ��ά����bits�������һ������ 2*sizeof(bitset<subs>)*numDimension*numBits
		size += sizeof(bitset<subs>) * (bits[0][i].size() + bits[1][i].size());
		_for(j, 0, numBucket)
			size += sizeof(Combo) * (data[0][i][j].size() + data[1][i][j].size());
	}

	// fullBits
	if (numBits > 1)
		size += sizeof(bitset<subs>) * fullBits.size(); // fullBits.size()��numDimension

	// ����fix
	size += sizeof(int) * numDimension * (numBucket + 1);
	// ����endBucket������bitsID������doubleReverse
	size += (4 * sizeof(int) + 2 * sizeof(bool)) * numDimension * numBucket;
	size = size / 1024 / 1024; // MB
	return (int)size;
}

void BIOP2::printRelation(int dimension_i) {
	cout << "\n\nBIOP2SDMap\n";
	if (dimension_i == -1)
		_for(i, 0, numDimension) {
		cout << "\nDimension " << i << "    LowBucket Predicates: " << fix[0][dimension_i][numBucket] << "   ----------------\n";
		_for(j, 0, numBucket) {
			cout << "lBkt" << j << ": bID=" << bitsID[0][i][j] << ", eBkt=" << endBucket[0][i][j] << ", dRvs=" << doubleReverse[0][i][j] <<"; ";
			if (j % 5 == 0 && j > 0)cout << "\n";
		}
		cout << "\n\nDimension " << i << "    HighBucket Predicates: " << fix[1][dimension_i][numBucket] << "   ----------------\n";
		_for(j, 0, numBucket) {
			cout << "hBkt" << j << ": bID=" << bitsID[1][i][j] << ", eBkt=" << endBucket[1][i][j] << ", dRvs=" << doubleReverse[1][i][j]<<"; ";
			if (j % 5 == 0 && j > 0)cout << "\n";
		}
	}
	else {
		cout << "\nDimension: " << dimension_i << "    LowBucket Predicates: " << fix[0][dimension_i][numBucket] << "   ----------------\n";
		_for(i, 0, numBucket) {
			cout << "lBkt" << i << ": bID=" << bitsID[0][dimension_i][i] << ", eBkt=" << endBucket[0][dimension_i][i] << ", dRvs=" << doubleReverse[0][dimension_i][i]<<"; ";
			if (i % 5 == 0 && i > 0)cout << "\n";
		}
		cout << "\n\nDimension: " << dimension_i << "    HighBucket Predicates: " << fix[1][dimension_i][numBucket] << "   ----------------\n";
		_for(i, 0, numBucket) {
			cout << "hBkt" << i << ": bID=" << bitsID[1][dimension_i][i] << ", eBkt=" << endBucket[1][dimension_i][i] << ", dRvs=" << doubleReverse[1][dimension_i][i] << "; ";
			if (i % 5 == 0 && i > 0)cout << "\n";
		}
	}
	cout << "\n\n";
}
 