#include "HEM1.h"

HEM1::HEM1(){
	numSub = 0;
	numDimension = atts;
	buckStep = (valDom - 1) / buks + 1;
	numBucket = (valDom - 1) / buckStep + 1;
    cout <<"ExpID = "<<expID<< ". HEM1SS: bit exponent = "<< be<<", bucketStep = " << buckStep << ", numBucket = " << numBucket << endl;
	// ���Ͱ����仯�����´���ҲҪ����init������
	//bucketSub.resize(numBucket);
	data[0].resize(numDimension, vector<vector<Combo>>(numBucket));
	data[1].resize(numDimension, vector<vector<Combo>>(numBucket));

	if (be == -1)
		numBits = be2;
	else
		numBits = pow(2, be);  // ÿ��ά����lowValue��Ӧ��bits�������
	//if (numBits > 1) {
		fullBits.resize(numDimension);  // ά��������Զ���䣬����ֻ��Ҫresizeһ��
		bitStep = (numBucket + numBits - 1) / numBits;  // ÿ����ôԶ����һ��bits
		// �ȼ�д����bitStep = numBucket % numBits == 0 ? numBucket / numBits : numBucket / numBits + 1;
	//}
	//else bitStep = numBucket >> 1;

	doubleReverse[0] = new bool[numBucket];
	doubleReverse[1] = new bool[numBucket];
	endBucket[0] = new int[numBucket];
	endBucket[1] = new int[numBucket];
	bitsID[0] = new int[numBucket];
	bitsID[1] = new int[numBucket];

	//fix[0].resize(numDimension, vector<int>(numBucket + 1, 0));
	//fix[1].resize(numDimension, vector<int>(numBucket + 1, 0));
}

HEM1::~HEM1() {
	delete[] endBucket[0], endBucket[1], bitsID[0], bitsID[1], doubleReverse[0], doubleReverse[1];
}

void HEM1::insert(IntervalSub sub)
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
void HEM1::initBits() {

	// ����ж�γ�ʼ��
	delete[] doubleReverse[0], doubleReverse[1], endBucket[0], endBucket[1], bitsID[0], bitsID[1];
	doubleReverse[0] = new bool[numBucket];
	doubleReverse[1] = new bool[numBucket];
	endBucket[0] = new int[numBucket];
	endBucket[1] = new int[numBucket];
	bitsID[0] = new int[numBucket];
	bitsID[1] = new int[numBucket];
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
	//	fix[1][i][numBucket] = fix[1][i][numBucket] + data[1][i][numBucket - 1].size();
	//}

	//if (numBits == 1) {                           // ֻ��һ��bitsʱ���У�����fullBits

	//	_for(j, 0, numBucket >> 1) {
	//		// ��ʱlow��һ��һ���õ�Ҳֻ���õ�0��bits����
	//		bitsID[0][j] = 0;                     // ��ʱ��0�Ŵ���0.5~1
	//		endBucket[0][j] = numBucket >> 1;     // ���ʱ������С�����ֵ
	//		doubleReverse[0][j] = false;          // Bug�����ҲҪ��ֵ��û��ʼ����������һ����Сʱ
	//		int bid1 = -1, bid2 = 0;
	//		int bktid1 = 0, bktid2 = bitStep;     // Bug: bitStep��0����500! ��ĥ����Ķ���!???û���
	//		int midid = (bktid1 + bktid2) / 2;

	//		if (j <= midid) {
	//			bitsID[1][j] = bid1;              // Ϊ-1ʱ��ʾȷʵ�ò���bits����
	//			endBucket[1][j] = bktid1;         // ������1ʱ�� j-1 ������ bktid1 ��Ͱ
	//			doubleReverse[1][j] = false;
	//		}
	//		else {
	//			bitsID[1][j] = bid2;
	//			endBucket[1][j] = bktid2;         // ���ط�����0ʱ�� j ������ bktid2 - 1 ��Ͱ
	//			doubleReverse[1][j] = true;
	//		}
	//	}
	//	_for(j, numBucket >> 1, numBucket) {
	//		// ��ʱhigh��һ��һ���õ�Ҳֻ���õ�0��bits����
	//		bitsID[1][j] = 0;
	//		endBucket[1][j] = numBucket >> 1;     // ���ʱ�������������ֵ Bug: bitStep��0����500! ??? ��500��
	//		doubleReverse[1][j] = false;

	//		int bid1 = -1, bid2 = 0;
	//		int bktid1 = numBucket, bktid2 = numBucket - (numBucket >> 1);// 1000-500
	//		int midid = (bktid1 + bktid2) / 2;
	//		if (j < midid) {
	//			bitsID[0][j] = bid2;              // Ϊ-1ʱ��ʾȷʵ�ò���bits����
	//			endBucket[0][j] = bktid2;         // ���ұ��1ʱ�� j+1 ������ bktid1-1 ��Ͱ
	//			doubleReverse[0][j] = true;
	//		}
	//		else {
	//			bitsID[0][j] = bid1;
	//			endBucket[0][j] = bktid1;         // ���ط�����0ʱ�� bktid2 ������ j ��Ͱ
	//			doubleReverse[0][j] = false;
	//		}
	//	}

	//	// ��α���������ӳ���������ˣ��ɲ����Ⱥ�ִ��
	//	_for(i, 0, numDimension) {                // ÿ��ά��
	//		_for(j, 0, numBucket >> 1)            // ÿ����벿�ֵ�Ͱ
	//			_for(k, 0, data[1][i][j].size())  // Ͱ��ÿ������
	//			bits[1][i][0][data[1][i][j][k].subID] = 1;  // Bug: high����low, iά, 0��bits, subID
	//		_for(j, numBucket >> 1, numBucket)    // ÿ���Ұ벿�ֵ�Ͱ
	//			_for(k, 0, data[0][i][j].size())  // Ͱ��ÿ������
	//			bits[0][i][0][data[0][i][j][k].subID] = 1;  // high, iά, 0��bits, subID
	//	}
	//	//cout << "Stop.\n";
	//	return;
	//}

	_for(i, 0, numDimension)
		_for(j, 0, numBucket) {
		int bid1 = j / bitStep - 1, bid2 = bid1 + 1;   // 62/63-1=-1, 63/63-1=0, -1+1=0     945/63-1=14, 15������fullBits
		int bktid1 = (bid1 + 1) * bitStep, bktid2 = min(bktid1 + bitStep, numBucket); // (-1+1)*63=0 <= 62, (0+1)*63=63>62
		int midid = (bktid1 + bktid2) / 2;
		if (j <= midid) {
			bitsID[1][j] = bid1;      // Ϊ-1ʱ��ʾȷʵ�ò���bits����
			endBucket[1][j] = bktid1; // ������1ʱ�� j-1 ������ bktid1 ��Ͱ
			doubleReverse[1][j] = false;
		}
		else {
			// numBits>1, bid2=numBits-1ʱ��ʾ��fullBits
			bitsID[1][j] = bid2;        // bid2��ԭ����id����1��������в����ǵ�ʱ����Ҫ���ݼ�һ���ж�
			endBucket[1][j] = bktid2;   // ���ط�����0ʱ�� j ������ bktid2 - 1 ��Ͱ
			doubleReverse[1][j] = true;
		}

		//bid1 = (numBucket - j - 1) / bitStep - 1, bid2 = bid1 + 1;// 2,3; 1,2; 0,1; -1,0; // Ͱ������bits����������ʱ���ܹ���ͬһ��cell
		//bktid1 = numBucket - (bid1 + 1) * bitStep, bktid2 = max(0, bktid1 - bitStep); // <bktid1, >=bktid2 250,0; 500,250; 750,500; 1000,750;
		bid1 = numBits - 3 - bid1, bid2 = bid1 + 1;
		bktid1 = bktid1 ^ bktid2;
		bktid2 = bktid1 ^ bktid2;
		bktid1 = bktid1 ^ bktid2;
		if (j >= midid) {
			bitsID[0][j] = bid1;
			endBucket[0][j] = bktid1;
			doubleReverse[0][j] = false; // Bug: ������û��ʼ��������Ϊtrue��
		}
		else {
			bitsID[0][j] = bid2;
			endBucket[0][j] = bktid2;
			doubleReverse[0][j] = true;
		}
		}

	int subID, b;   // ��ʼ���������±�
	_for(i, 0, numDimension) {          // ÿ��ά��
		_for(j, 0, numBucket) {         // ÿ��Ͱ
			if (doubleReverse[0][j])
				b = bitsID[0][j];       // ����0������С����Ҫ�����bits�����ID
			else b = bitsID[0][j] + 1;
			_for(k, 0, data[0][i][j].size()) {
				subID = data[0][i][j][k].subID;
				fullBits[i][subID] = 1; // 0��bitsÿ�α�����   
				_for(q, b, numBits - 1) // Bug: bits�����ǴӸ�λ(���ǹ�)����λ������
					bits[0][i][q][subID] = 1;
			}

			if (doubleReverse[1][j])
				b = bitsID[1][j];
			else b = bitsID[1][j] + 1;  // ����0������С����Ҫ�����bits�����ID
			_for(k, 0, data[1][i][j].size()) {     // Ͱ��ÿ������
				subID = data[1][i][j][k].subID;
				_for(q, b, numBits - 1)
					bits[1][i][q][subID] = 1;
			}
		}
	}
	//cout << "Stop.\n";
}

//// ����ʱ�����
//void HEM1::match(const Pub& pub, int& matchSubs)
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
//		if (doubleReverse[0][buck]) {
//			Timer markStart;
//			if (bitsID[0][buck] == numBits - 1) // ֻ��1��bitsetʱ����fullBits�ϣ�ȥ��: && numBits > 1
//				bLocal = fullBits[att];
//			else
//				bLocal = bits[0][att][bitsID[0][buck]];
//			_for(j, endBucket[0][buck], buck + 1)
//				_for(k, 0, data[0][att][j].size())
//				bLocal[data[0][att][j][k].subID] = 0;
//			markTime += (double)markStart.elapsed_nano();
//
//			Timer orStart;
//			b = b | bLocal;
//			orTime += (double)orStart.elapsed_nano();
//		}
//		else {
//			Timer markStart;
//			_for(j, buck + 1, endBucket[0][buck])
//				_for(k, 0, data[0][att][j].size())
//				b[data[0][att][j][k].subID] = 1;
//			markTime += (double)markStart.elapsed_nano();
//			Timer orStart;
//			if (bitsID[0][buck] != -1)
//				b = b | bits[0][att][bitsID[0][buck]];
//			orTime += (double)orStart.elapsed_nano();
//		}
//
//		if (doubleReverse[1][buck]) {
//			Timer markStart;
//			if (bitsID[1][buck] == numBits - 1) // ֻ��1��bitsetʱ����fullBits�ϣ�ȥ��: && numBits > 1
//				bLocal = fullBits[att];
//			else
//				bLocal = bits[1][att][bitsID[1][buck]];
//			_for(j, buck, endBucket[1][buck])
//				_for(k, 0, data[1][att][j].size())
//				bLocal[data[1][att][j][k].subID] = 0;
//			markTime += (double)markStart.elapsed_nano();
//			Timer orStart;
//			b = b | bLocal;
//			orTime += (double)orStart.elapsed_nano();
//		}
//		else {
//			Timer markStart;
//			_for(j, endBucket[1][buck], buck)
//				_for(k, 0, data[1][att][j].size())
//				b[data[1][att][j][k].subID] = 1;
//			markTime += (double)markStart.elapsed_nano();
//			Timer orStart;
//			if (bitsID[1][buck] != -1)
//				b = b | bits[1][att][bitsID[1][buck]]; // Bug: ��att����i
//			orTime += (double)orStart.elapsed_nano();
//		}
//	}
//
////	if (numBits > 1) {
//		Timer orStart;
//		_for(i, 0, numDimension)
//			if (!attExist[i])
//				b = b | fullBits[i];
//		orTime += (double)orStart.elapsed_nano();
////	}
////	else {
////		Timer markStart;
////		_for(i, 0, numDimension)
////			if (!attExist[i])
////				_for(j, 0, bitStep)
////				_for(k, 0, data[0][i][j].size())
////				b[data[0][i][j][k].subID] = 1;
////		markTime += (double)markStart.elapsed_nano();
////
////		Timer orStart;
////		_for(i, 0, numDimension)
////			if (!attExist[i])
////				b = b | bits[0][i][0];
////		orTime += (double)orStart.elapsed_nano();
////	}
//
//	Timer bitStart;
//	_for(i, 0, subs)
//		if (!b[i])
//		{
//			++matchSubs;
//			//cout << "HEM1 matches sub: " << i << endl;
//		}
//	bitTime += (double)bitStart.elapsed_nano();
//}

// ������ʱ�����
 void HEM1::match(const Pub& pub, int& matchSubs)
 {
 	bitset<subs> b, bLocal;
 	vector<bool> attExist(numDimension, false);
 	int value, att, buck;

 	_for(i, 0, pub.size)
 	{
 		value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
 		attExist[att] = true;
 		_for(k, 0, data[0][att][buck].size())
 			if (data[0][att][buck][k].val > value)
 				b[data[0][att][buck][k].subID] = 1;
 		_for(k, 0, data[1][att][buck].size())
 			if (data[1][att][buck][k].val < value)
 				b[data[1][att][buck][k].subID] = 1;

 		if (doubleReverse[0][buck]) {
 			if (bitsID[0][buck] == numBits - 1) // ֻ��1��bitsetʱ����fullBits�ϣ�ȥ��: && numBits > 1
 				bLocal = fullBits[att];
 			else
 				bLocal = bits[0][att][bitsID[0][buck]];

 			_for(j, endBucket[0][buck], buck + 1)
 				_for(k, 0, data[0][att][j].size())
 				bLocal[data[0][att][j][k].subID] = 0;

 			b = b | bLocal;
 		}
 		else {
 			_for(j, buck + 1, endBucket[0][buck])
 				_for(k, 0, data[0][att][j].size())
 				b[data[0][att][j][k].subID] = 1;

 			if (bitsID[0][buck] != -1)
 				b = b | bits[0][att][bitsID[0][buck]];
 		}

 		if (doubleReverse[1][buck]) {
 			if (bitsID[1][buck] == numBits - 1) // ֻ��1��bitsetʱ����fullBits�ϣ�ȥ��: && numBits > 1
 				bLocal = fullBits[att];
 			else
 				bLocal = bits[1][att][bitsID[1][buck]];

 			_for(j, buck, endBucket[1][buck])
 				_for(k, 0, data[1][att][j].size())
 				bLocal[data[1][att][j][k].subID] = 0;

 			b = b | bLocal;
 		}
 		else {
 			_for(j, endBucket[1][buck], buck)
 				_for(k, 0, data[1][att][j].size())
 				b[data[1][att][j][k].subID] = 1;

 			if (bitsID[1][buck] != -1)
 				b = b | bits[1][att][bitsID[1][buck]]; // Bug: ��att����i
 		}
 	}

 	//if (numBits > 1) {
 		_for(i, 0, numDimension)
 			if (!attExist[i])
 				b = b | fullBits[i];
 	/*}
 	else {
 		_for(i, 0, numDimension)
 			if (!attExist[i])
 				_for(j, 0, bitStep)
 				_for(k, 0, data[0][i][j].size())
 				b[data[0][i][j][k].subID] = 1;

 		_for(i, 0, numDimension)
 			if (!attExist[i])
 				b = b | bits[0][i][0];
 	}*/

// 	_for(i, 0, subs)
// 		if (!b[i])
// 		{
// 			++matchSubs;
// 			//cout << "HEM1 matches sub: " << i << endl;
// 		}
	matchSubs = subs - b.count();
 }

//void HEM1::calBucketSize() {
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

int HEM1::calMemory() {
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
	//size += sizeof(int) * numDimension * (numBucket + 1);
	// ����endBucket������bitsID������doubleReverse
	size += (4 * sizeof(int) + 2 * sizeof(bool)) * numDimension * numBucket;
	size = size / 1024 / 1024; // MB
	return (int)size;
}

void HEM1::printRelation() {
	cout << "\n\nHEM1SSMap    LowBucket   ----------------\n";
	_for(i, 0, numBucket) {
		cout << "lBkt" << i << ": bID=" << bitsID[0][i] << ", eBkt=" << endBucket[0][i] << ", dRvs=" << doubleReverse[0][i] << "; ";
		if (i % 5 == 0 && i > 0)cout << "\n";
	}
	cout << "\n\nHEM1SSMap    HighBucket   ----------------\n";
	_for(i, 0, numBucket) {
		cout << "hBkt" << i << ": bID=" << bitsID[1][i] << ", eBkt=" << endBucket[1][i] << ", dRvs=" << doubleReverse[1][i] << "; ";
		if (i % 5 == 0 && i > 0)cout << "\n";
	}
	
	cout << "\n\n";
}
