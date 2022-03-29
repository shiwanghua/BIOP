#include "HEMSC.h"

HEMSC::HEMSC() {
	numSub = 0;
	numDimension = atts;
	levelStep = (valDom - 1) / lvls + 1;
	numLevel = (valDom - 1) / levelStep + 1;
	buckStep = (valDom - 1) / buks + 1;
	//buckStep = buckStep * numLevel; // û��
	numBucket = (valDom - 1) / buckStep + 1;

	cout << "ExpID = " << expID << ". HEMSC: numLevel = " << numLevel << ", levelStep = " << levelStep << ", numBucket = " << numBucket << ", bucketStep = " << buckStep << ", bit exponent = " << be << endl;

	//bucketSub.resize(numBucket);
	data[0].resize(numDimension, vector<vector<vector<Combo>>>(numLevel, vector<vector<Combo>>(numBucket)));
	data[1].resize(numDimension, vector<vector<vector<Combo>>>(numLevel, vector<vector<Combo>>(numBucket)));

	if (be == -1)
		numBits = be2;
	else
		numBits = pow(2, be);  // ÿ��ά����lowValue��Ӧ��bits�������
	//if (numBits > 1) {
	fullBits.resize(numDimension);  // ά��������Զ���䣬����ֻ��Ҫresizeһ��
	fullBL.resize(numDimension, vector<bitset<subs>>(numLevel));
	//}

	doubleReverse[0] = new bool** [numDimension];
	doubleReverse[1] = new bool** [numDimension];
	endBucket[0] = new int** [numDimension];
	endBucket[1] = new int** [numDimension];
	bitsID[0] = new int** [numDimension];
	bitsID[1] = new int** [numDimension];
	_for(lh, 0, 2) // 0 or 1 low/high
		_for(i, 0, numDimension) {
		doubleReverse[lh][i] = new bool* [numLevel];
		endBucket[lh][i] = new int* [numLevel];
		bitsID[lh][i] = new int* [numLevel];
		_for(j, 0, numLevel) {
			doubleReverse[lh][i][j] = new bool[numBucket];
			endBucket[lh][i][j] = new int[numBucket];
			bitsID[lh][i][j] = new int[numBucket];
		}
	}

	fix[0].resize(numDimension, vector<vector<int>>(numLevel, vector<int>(numBucket + 1, 0)));
	fix[1].resize(numDimension, vector<vector<int>>(numLevel, vector<int>(numBucket + 1, 0)));
}

HEMSC::~HEMSC() {
	_for(lh, 0, 2) { // 0 or 1 low/high
		_for(i, 0, numDimension) {
			_for(j, 0, numLevel)
				delete[] doubleReverse[lh][i][j], endBucket[lh][i][j], bitsID[lh][i][j];
			delete[] doubleReverse[lh][i], endBucket[lh][i], bitsID[lh][i];
		}
		delete[] endBucket[lh], bitsID[lh], doubleReverse[lh];
	}
}

void HEMSC::insert(IntervalSub sub)
{
	for (int i = 0; i < sub.size; i++)
	{
		IntervalCnt cnt = sub.constraints[i];
		Combo c;
		//int li = (cnt.highValue - cnt.lowValue) / levelStep;
		int li = cnt.highValue / levelStep;
		//int li = cnt.lowValue / levelStep;

		/*int li;
		if (cnt.highValue <= valDom / 2)
			li = 0;
		else if (cnt.lowValue >= valDom / 2)
			li = 2;
		else li = 1;*/
		c.val = cnt.lowValue;
		c.subID = sub.id;
		data[0][cnt.att][li][cnt.lowValue / buckStep].push_back(c);
		c.val = cnt.highValue;
		data[1][cnt.att][li][cnt.highValue / buckStep].push_back(c);
	}
	numSub++;
}

// fullBits�����洢�İ汾
void HEMSC::initBits() {

	// ����ж�γ�ʼ��
	_for(lh, 0, 2) { // 0 or 1 low/high
		_for(i, 0, numDimension) {
			_for(j, 0, numLevel)
				delete[] doubleReverse[lh][i][j], endBucket[lh][i][j], bitsID[lh][i][j];
			delete[] doubleReverse[lh][i], endBucket[lh][i], bitsID[lh][i];
		}
		delete[] endBucket[lh], bitsID[lh], doubleReverse[lh];
	}
	doubleReverse[0] = new bool** [numDimension];
	doubleReverse[1] = new bool** [numDimension];
	endBucket[0] = new int** [numDimension];
	endBucket[1] = new int** [numDimension];
	bitsID[0] = new int** [numDimension];
	bitsID[1] = new int** [numDimension];
	_for(lh, 0, 2) // 0 or 1 low/high
		_for(i, 0, numDimension) {
		doubleReverse[lh][i] = new bool* [numLevel];
		endBucket[lh][i] = new int* [numLevel];
		bitsID[lh][i] = new int* [numLevel];
		_for(j, 0, numLevel) {
			doubleReverse[lh][i][j] = new bool[numBucket];
			endBucket[lh][i][j] = new int[numBucket];
			bitsID[lh][i][j] = new int[numBucket];
		}
	}
	bits[0].clear(), bits[1].clear();
	bits[0].resize(numDimension, vector<vector<bitset<subs>>>(numLevel, vector<bitset<subs>>(numBits - 1)));
	bits[1].resize(numDimension, vector<vector<bitset<subs>>>(numLevel, vector<bitset<subs>>(numBits - 1)));

	// ǰ׺�͡���׺������, ����������
	//_for(i, 0, numDimension) {
	//	_for(j, 0, numLevel) {
	//		_for(k, 1, numBucket) {
	//			fix[0][i][j][numBucket - 1 - k] = fix[0][i][j][numBucket - k] + data[0][i][j][numBucket - k].size();
	//			fix[1][i][j][k] = fix[1][i][j][k - 1] + data[1][i][j][k - 1].size();
	//		}
	//		// ��������ĺʹ������һ��Ԫ����
	//		fix[0][i][j][numBucket] = fix[0][i][j][0] + data[0][i][j][0].size();
	//		fix[1][i][j][numBucket] = fix[1][i][j][numBucket - 1] + data[1][i][j][numBucket - 1].size();
	//	}
	//}

	// ǰ׺������(����������)����׺������(��������)
	_for(i, 0, numDimension) {
		_for(j, 0, numLevel) {
			fix[0][i][j][numBucket - 1] = data[0][i][j][numBucket - 1].size();
			_for(k, 1, numBucket) {
				fix[0][i][j][numBucket - 1 - k] = fix[0][i][j][numBucket - k] + data[0][i][j][numBucket - k - 1].size();
				fix[1][i][j][k] = fix[1][i][j][k - 1] + data[1][i][j][k - 1].size();
			}

			//fix[0][i][j][numBucket] = fix[0][i][j][0];  // ��׺�����鲻�ô��
			fix[1][i][j][numBucket] = fix[1][i][j][numBucket - 1] + data[1][i][j][numBucket - 1].size();
		}
	}

	//if (numBits == 1) { // ֻ��һ��bitsʱ���У�����fullBits
	//	_for(i, 0, numDimension) {
	//		_for(j, 0, numLevel) {
	//			int halfWorkLoad = fix[1][i][j][numBucket] >> 1; // subWorkLoadStep  fix[0][i][j][0]
	//			int quarterWorkLoad = halfWorkLoad >> 1;
	//			// ��һ����/ǰ׺�Ͱ���һ�붩�ĵ�ͰID��bit������Զ���ø��ǵ�lowHalfPoint��highHalfPoint-1
	//			int lowHalfPoint = -1, lowQuarterPoint = -1, highHalfPoint = -1, highQuarterPoint = -1;
	//			_for(k, 0, numBucket) {
	//				if (lowQuarterPoint == -1 && fix[0][i][j][numBucket - 1 - k] >= quarterWorkLoad)
	//					lowQuarterPoint = numBucket - 1 - k;
	//				else if (lowHalfPoint == -1 && fix[0][i][j][numBucket - 1 - k] >= halfWorkLoad)
	//					lowHalfPoint = numBucket - 1 - k;

	//				if (highQuarterPoint == -1 && fix[1][i][j][k] >= quarterWorkLoad)
	//					highQuarterPoint = k;
	//				else if (highHalfPoint == -1 && fix[1][i][j][k] >= halfWorkLoad)
	//					highHalfPoint = k;
	//			}

	//			_for(k, 0, numBucket) {
	//				if (k < lowHalfPoint) {                    // ��������bitset
	//					bitsID[0][i][j][k] = 0;
	//					endBucket[0][i][j][k] = lowHalfPoint;  // ������С�� lowCriticalPoint
	//					doubleReverse[0][i][j][k] = false;
	//				}
	//				else if (k < lowQuarterPoint) {
	//					bitsID[0][i][j][k] = 0;
	//					endBucket[0][i][j][k] = lowHalfPoint;  // �� k ���ط������������ lowCriticalPoint(������)
	//					doubleReverse[0][i][j][k] = true;
	//					_for(q, 0, data[0][i][j][k].size())    // Ͱ��ÿ������
	//						bits[0][i][j][0][data[0][i][j][k][q].subID] = 1;
	//				}
	//				else {
	//					bitsID[0][i][j][k] = -1;
	//					endBucket[0][i][j][k] = numBucket;
	//					doubleReverse[0][i][j][k] = false;
	//					_for(q, 0, data[0][i][j][k].size())    // Ͱ��ÿ������
	//						bits[0][i][j][0][data[0][i][j][k][q].subID] = 1;
	//				}
	//				if (k < highQuarterPoint) {                // ��������bitset
	//					bitsID[1][i][j][k] = -1;
	//					endBucket[1][i][j][k] = 0;             // ����������0
	//					doubleReverse[1][i][j][k] = false;
	//					_for(q, 0, data[1][i][j][k].size())    // Ͱ��ÿ������
	//						bits[1][i][j][0][data[1][i][j][k][q].subID] = 1;
	//				}
	//				else if (k < highHalfPoint) {
	//					bitsID[1][i][j][k] = 0;
	//					endBucket[1][i][j][k] = highHalfPoint;  // �� j ���ط�����������ڵ��� highCriticalPoint
	//					doubleReverse[1][i][j][k] = true;
	//					_for(q, 0, data[1][i][j][k].size())     // Ͱ��ÿ������
	//						bits[1][i][j][0][data[1][i][j][k][q].subID] = 1;
	//				}
	//				else {
	//					bitsID[1][i][j][k] = 0;
	//					endBucket[1][i][j][k] = highHalfPoint;  // �� j-1 ���������ڵ��� highHalfPoint, ����ǰ����һ��
	//					doubleReverse[1][i][j][k] = false;
	//				}
	//			}
	//		}  // level
	//	}      // dimension
	//	//cout << "HEMSCDD Stop.\n";
	//	return;
	//}

	// ��ǰӦ��ӳ�䵽��bitId, Ͱid, ��һ���ٽ縺�ص�
	int lowBid, highBid, lowBktId, highBktId, lowSubWorkLoad, highSubWorkLoad;
	int subWorkLoadStep; // ÿ��ά���ϵ�subWorkLoadStep����ͬ, ��ͬһ��ά���ϵ�low/high subWorkLoadStep��һ����
	_for(i, 0, numDimension) {
		_for(j, 0, numLevel) {

			if (fix[1][i][j][numBucket] == 0) {
				_for(k, 0, numBucket) {
					bitsID[0][i][j][k] = -1;
					endBucket[0][i][j][k] = k;
					doubleReverse[0][i][j][k] = false;
					bitsID[1][i][j][k] = -1;
					endBucket[1][i][j][k] = k;       // ���������ڵ���endBucket[1][i][j]
					doubleReverse[1][i][j][k] = false;
				}
				continue;
			}

			subWorkLoadStep = (fix[1][i][j][numBucket] + numBits - 1) / numBits; // fix[0][i][j][0]

		// ������low/high���Ƕ�̬��, ���������ܹ���ͬһ��partition/cell,
		// ������low���Ǵ���߿�ʼ��һ��subWorkLoadStep����, ����һ��      
		// ���ߴ��ұ��� ʣ�ฺ���� ��ʼ�ۼ�subWorkLoadStep, �������endBucket!
		// 0��lowͰһ�������õ��� (numBits - 2) Ϊ�±��bitset
		// ���һ��Ͱһ���ò���bitset
		// ����: numBits=15, fix[0][i][numBucket]=1000000, subWorkLoadStep=66667 (low�ϵĺ�14����1, high�ϵ�ǰ14����1)
		// fix[0][i][numBucket] / subWorkLoadStep=14, lowSubWorkLoad=66662
			lowBid = -1;
			lowBktId = numBucket;
			lowSubWorkLoad = fix[0][i][j][0] - (fix[0][i][j][0] - 1) / subWorkLoadStep * subWorkLoadStep;
			highBid = -1;
			highBktId = 0;
			highSubWorkLoad = subWorkLoadStep;

			// lowContain[i]=����(��һ������)lowSubWorkLoad+(i-1)*subWorkLoadStep������������Ͱ��(i>0ʱ)
			vector<int> lowContain(numBits + 1, numBucket);
			// highContain[i]=����i*subWorkLoadStep������������Ͱ��+1����Ϊǰ׺��û��������
			vector<int> highContain(numBits + 1, 0);
			int li = 1, hi = 1; // lowContain��highContain���±�
			_for(k, 0, numBucket) {
				if (fix[1][i][j][k] >= highSubWorkLoad) {
					highContain[hi++] = k;
					highSubWorkLoad += subWorkLoadStep;
				}
				if (fix[0][i][j][numBucket - k - 1] >= lowSubWorkLoad) {
					lowContain[li++] = numBucket - k - 1;
					lowSubWorkLoad += subWorkLoadStep;
				}
			}
			//lowContain[li] = 0;
			if (hi == numBits)  // Bug: ��󼸸�ͰΪ��ʱhi����forѭ�������ӵ�numBits+1
				highContain[hi] = numBucket;

			li = hi = 1; // ˫�ط������ʱ����Ӧ����һ�˵�Ͱ����contain�����е��±�, ��ʵ li=lowBid+2, hi=highBid+2
			lowSubWorkLoad = fix[0][i][j][0] - (fix[0][i][j][0] - 1) / subWorkLoadStep * subWorkLoadStep;
			highSubWorkLoad = subWorkLoadStep;
			_for(k, 0, numBucket) {
				// ��ʱ���ڵ���highSubWorkLoad��, ������bits, ��Ϊbits���ǵ�j-1Ͱ
				if (fix[1][i][j][k] >= highSubWorkLoad) {    // ��һ�����ڵ����ٽ���Ͱ(k-1��, ǰ׺�Ͳ���������)��Ϊbitset���ǵ��յ�Ͱ
					highSubWorkLoad += subWorkLoadStep;
					hi++;
					highBid++;
					highBktId = k;
				}
				//if (i == 5 && j == 0 && k == 827) {
				//	cout << "error\n";
				//}
				// Bug: ��ǰ����, ��󼸸�ͰΪ��, ��ʱhighBid=numBits-1, Խ����, ֱ����fullBL
				if (fix[1][i][j][k] == fix[1][i][j][numBucket]) {
					bitsID[1][i][j][k] = numBits - 1;
					endBucket[1][i][j][k] = j + 1; // ����ǵ�һ�ν���, j��Ͱ�ǿ�, ��Ҫ���ط�����, �����ǿ�Ͱ, ���Լ����������
					doubleReverse[1][i][j][k] = true;
				}
				else if (fix[1][i][j][k] - fix[1][i][j][highBktId] <= fix[1][i][j][highContain[hi]] - fix[1][i][j][k]) {     // Bug: û�м�highBktId
					bitsID[1][i][j][k] = highBid;
					endBucket[1][i][j][k] = highBktId;       // ���������ڵ���endBucket[1][i][j]
					doubleReverse[1][i][j][k] = false;
				}
				else {
					bitsID[1][i][j][k] = hi - 1;             // highBid+1
					endBucket[1][i][j][k] = highContain[hi]; // ��j���ұ�����С��endBucket[1][i][j]
					doubleReverse[1][i][j][k] = true;
				}

				// ��׺�������ʱ��������(�������������, ��������k��lowBktId��lowContain[li]���ټ�һ����lowContain[li]�п���Ϊ0)
				// fix[0][i][j][numBucket]��Ҫ��0, ʹfix[0][i][j][lowBktId]�տ�ʼΪ0
				// Bug: ��ǰ����, ���С�ļ���ͰΪ��, ��������, ֱ���ö��ط���
				if (fix[0][i][j][numBucket - k - 1] == fix[0][i][j][0]) {
					bitsID[0][i][j][numBucket - k - 1] = numBits - 1;
					endBucket[0][i][j][numBucket - k - 1] = numBucket - k - 1;
					doubleReverse[0][i][j][numBucket - k - 1] = true;
				}
				else if (fix[0][i][j][numBucket - k] - fix[0][i][j][lowBktId] <= fix[0][i][j][lowContain[li]] - fix[0][i][j][numBucket - k]) {
					bitsID[0][i][j][numBucket - k - 1] = lowBid;
					endBucket[0][i][j][numBucket - k - 1] = lowBktId;
					doubleReverse[0][i][j][numBucket - k - 1] = false;
				}
				else {
					bitsID[0][i][j][numBucket - k - 1] = li - 1;  // lowBktId+1
					endBucket[0][i][j][numBucket - k - 1] = lowContain[li];
					doubleReverse[0][i][j][numBucket - k - 1] = true;
				}

				// ��ʱ��Ȼ���ڵ���lowSubWorkLoad��, ���Բ�������bits, ��ΪbitsҪ���ǵ�j��Ͱ
				if (fix[0][i][j][numBucket - k - 1] >= lowSubWorkLoad) {
					lowSubWorkLoad += subWorkLoadStep;
					li++;
					lowBid++;
					lowBktId = numBucket - k - 1;
				}
			}
		}   // level
	}       // dimension

	int subID, b;                       // ��ʼ���������±�
	_for(i, 0, numDimension) {          // ÿ��ά��
		_for(j, 0, numLevel) {          // ÿ��
			_for(k, 0, numBucket) {     // ÿ��Ͱ
				if (doubleReverse[0][i][j][k])
					b = bitsID[0][i][j][k];       // ��С����Ҫ�����bits�����ID
				else b = bitsID[0][i][j][k] + 1;
				_for(q, 0, data[0][i][j][k].size()) {
					subID = data[0][i][j][k][q].subID;
					fullBits[i][subID] = 1;    // 0��bitsÿ�α�����  
					fullBL[i][j][subID] = 1;
					_for(p, b, numBits - 1) // Bug: bits�����ǴӸ�λ(���ǹ�)����λ������
						bits[0][i][j][p][subID] = 1;
				}

				if (doubleReverse[1][i][j][k])
					b = bitsID[1][i][j][k];
				else b = bitsID[1][i][j][k] + 1;  // ��С����Ҫ�����bits�����ID
				_for(q, 0, data[1][i][j][k].size()) {     // Ͱ��ÿ������
					subID = data[1][i][j][k][q].subID;
					_for(p, b, numBits - 1)
						bits[1][i][j][p][subID] = 1;
				}
			}
		}
	}
	//cout << "HEMSCDD Stop.\n";
}

// ������ʱ�����
void HEMSC::match(const Pub& pub, int& matchSubs)
{
	bitset<subs> b, bLocal;
	vector<bool> attExist(numDimension, false);
	int value, att, buck;

	_for(i, 0, pub.size)
	{
		// ����ÿ���Ͱ��һ��...
		value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
		attExist[att] = true;
		_for(j, 0, numLevel) {
			_for(q, 0, data[0][att][j][buck].size())
				if (data[0][att][j][buck][q].val > value)
					b[data[0][att][j][buck][q].subID] = 1;
			_for(q, 0, data[1][att][j][buck].size())
				if (data[1][att][j][buck][q].val < value)
					b[data[1][att][j][buck][q].subID] = 1;

			if (doubleReverse[0][att][j][buck]) {
				if (bitsID[0][att][j][buck] == numBits - 1) // ֻ��1��bitsetʱ����fullBits�ϣ�ȥ����: && numBits > 1
					bLocal = fullBL[att][j];
				else
					bLocal = bits[0][att][j][bitsID[0][att][j][buck]];
				_for(k, endBucket[0][att][j][buck], buck + 1)
					_for(q, 0, data[0][att][j][k].size())
					bLocal[data[0][att][j][k][q].subID] = 0;

				b = b | bLocal;
			}
			else {
				_for(k, buck + 1, endBucket[0][att][j][buck])
					_for(q, 0, data[0][att][j][k].size())
					b[data[0][att][j][k][q].subID] = 1;

				if (bitsID[0][att][j][buck] != -1)
					b = b | bits[0][att][j][bitsID[0][att][j][buck]];
			}

			if (doubleReverse[1][att][j][buck]) {
				if (bitsID[1][att][j][buck] == numBits - 1) // ֻ��1��bitsetʱ����fullBits�ϣ�ȥ����: && numBits > 1
					bLocal = fullBL[att][j];
				else
					bLocal = bits[1][att][j][bitsID[1][att][j][buck]];
				_for(k, buck, endBucket[1][att][j][buck])
					_for(q, 0, data[1][att][j][k].size())
					bLocal[data[1][att][j][k][q].subID] = 0;
				b = b | bLocal;
			}
			else {
				_for(k, endBucket[1][att][j][buck], buck)
					_for(q, 0, data[1][att][j][k].size())
					b[data[1][att][j][k][q].subID] = 1;
				if (bitsID[1][att][j][buck] != -1)
					b = b | bits[1][att][j][bitsID[1][att][j][buck]]; // Bug: ��att����i
			}
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
				_for(j, 0, numLevel)
				_for(k, 0, endBucket[0][i][j][0])
				_for(q, 0, data[0][i][j].size())
				b[data[0][i][j][k][q].subID] = 1;

		_for(i, 0, numDimension)
			if (!attExist[i])
				_for(j, 0, numLevel)
				b = b | bits[0][i][j][0];
	}*/

	//	_for(i, 0, subs)
	//		if (!b[i])
	//		{
	//			++matchSubs;
	//			//cout << "HEMSC matches sub: " << i << endl;
	//		}
	matchSubs = subs - b.count();
}

// ����ʱ�����
void HEMSC::match_debug(const Pub& pub, int& matchSubs)
{
	bitset<subs> b, bLocal;
	vector<bool> attExist(numDimension, false);
	int value, att, buck;

	_for(i, 0, pub.size)
	{
		// ����ÿ���Ͱ��һ��...
		value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
		attExist[att] = true;
		_for(j, 0, numLevel) {
			Timer compareStart;
			_for(q, 0, data[0][att][j][buck].size())
				if (data[0][att][j][buck][q].val > value)
					b[data[0][att][j][buck][q].subID] = 1;
			_for(q, 0, data[1][att][j][buck].size())
				if (data[1][att][j][buck][q].val < value)
					b[data[1][att][j][buck][q].subID] = 1;
			compareTime += (double)compareStart.elapsed_nano();

			if (doubleReverse[0][att][j][buck]) {
				Timer markStart;
				if (bitsID[0][att][j][buck] == numBits - 1) // ֻ��1��bitsetʱ����fullBits�ϣ�ȥ����: && numBits > 1
					bLocal = fullBL[att][j];
				else
					bLocal = bits[0][att][j][bitsID[0][att][j][buck]];
				_for(k, endBucket[0][att][j][buck], buck + 1)
					_for(q, 0, data[0][att][j][k].size())
					bLocal[data[0][att][j][k][q].subID] = 0;
				markTime += (double)markStart.elapsed_nano();

				Timer orStart;
				b = b | bLocal;
				orTime += (double)orStart.elapsed_nano();
			}
			else {
				Timer markStart;
				_for(k, buck + 1, endBucket[0][att][j][buck])
					_for(q, 0, data[0][att][j][k].size())
					b[data[0][att][j][k][q].subID] = 1;
				markTime += (double)markStart.elapsed_nano();

				Timer orStart;
				if (bitsID[0][att][j][buck] != -1)
					b = b | bits[0][att][j][bitsID[0][att][j][buck]];
				orTime += (double)orStart.elapsed_nano();
			}

			if (doubleReverse[1][att][j][buck]) {
				Timer markStart;
				if (bitsID[1][att][j][buck] == numBits - 1) // ֻ��1��bitsetʱ����fullBits�ϣ�ȥ����: && numBits > 1
					bLocal = fullBL[att][j];
				else
					bLocal = bits[1][att][j][bitsID[1][att][j][buck]];
				_for(k, buck, endBucket[1][att][j][buck])
					_for(q, 0, data[1][att][j][k].size())
					bLocal[data[1][att][j][k][q].subID] = 0;
				markTime += (double)markStart.elapsed_nano();
				Timer orStart;
				b = b | bLocal;
				orTime += (double)orStart.elapsed_nano();
			}
			else {
				Timer markStart;
				_for(k, endBucket[1][att][j][buck], buck)
					_for(q, 0, data[1][att][j][k].size())
					b[data[1][att][j][k][q].subID] = 1;
				markTime += (double)markStart.elapsed_nano();
				Timer orStart;
				if (bitsID[1][att][j][buck] != -1)
					b = b | bits[1][att][j][bitsID[1][att][j][buck]]; // Bug: ��att����i
				orTime += (double)orStart.elapsed_nano();
			}
		}
	}

	//if (numBits > 1) {
	Timer orStart;
	_for(i, 0, numDimension)
		if (!attExist[i])
			b = b | fullBits[i];
	orTime += (double)orStart.elapsed_nano();
	/*}
	else {
		Timer markStart;
		_for(i, 0, numDimension)
			if (!attExist[i])
				_for(j, 0, numLevel)
				_for(k, 0, endBucket[0][i][j][0])
				_for(q, 0, data[0][i][j].size())
				b[data[0][i][j][k][q].subID] = 1;
		markTime += (double)markStart.elapsed_nano();

		Timer orStart;
		_for(i, 0, numDimension)
			if (!attExist[i])
				_for(j, 0, numLevel)
					b = b | bits[0][i][j][0];
		orTime += (double)orStart.elapsed_nano();
	}*/

	Timer bitStart;
	//_for(i, 0, subs)
	//	if (!b[i])
	//	{
	//		++matchSubs;
	//		//cout << "HEMSC matches sub: " << i << endl;
	//	}
	matchSubs = subs - b.count();
	bitTime += (double)bitStart.elapsed_nano();
}

//void HEMSC::calBucketSize() {
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

int HEMSC::calMemory() {
	long long size = 0; // Byte
	_for(i, 0, numDimension) {
		_for(j, 0, numLevel) {
			// ��ÿ��ά����bits�������һ������ 2*sizeof(bitset<subs>)*numDimension*numBits*numLevel
			size += sizeof(bitset<subs>) * (bits[0][i][j].size() + bits[1][i][j].size());
			_for(k, 0, numBucket)
				size += sizeof(Combo) * (data[0][i][j][k].size() + data[1][i][j][k].size());
		}
	}

	// fullBits
	//if (numBits > 1) {
	size += sizeof(bitset<subs>) * fullBits.size(); // fullBits.size()��numDimension
	size += sizeof(bitset<subs>) * numDimension * numLevel; // fullBL
//}


// ����fix
	size += sizeof(int) * numDimension * numLevel * (numBucket + 1);
	// ����endBucket������bitsID������doubleReverse
	size += (4 * sizeof(int) + 2 * sizeof(bool)) * numDimension * numLevel * numBucket;
	size = size / 1024 / 1024; // MB
	return (int)size;
}

void HEMSC::printRelation(int dimension_i, int li) { // ά�ȺźͲ��
	cout << "\n\nHEMSCDDMap\n";
	if (dimension_i == -1)
		_for(i, 0, numDimension) {
		_for(j, 0, numLevel) {
			cout << "\nDimension " << i << "  Level " << j << "    LowBucket Predicates: " << fix[0][i][j][0] << "   ----------------\n";
			_for(k, 0, numBucket) {
				cout << "lBkt" << k << ": bID=" << bitsID[0][i][j][k] << ", eBkt=" << endBucket[0][i][j][k] << ", dRvs=" << doubleReverse[0][i][j][k] << "; ";
				if (k % 5 == 0 && k > 0)cout << "\n";
			}
			cout << "\n\nDimension " << i << "  Level " << j << "    HighBucket Predicates: " << fix[1][i][j][numBucket] << "   ----------------\n";
			_for(k, 0, numBucket) {
				cout << "hBkt" << k << ": bID=" << bitsID[1][i][j][k] << ", eBkt=" << endBucket[1][i][j][k] << ", dRvs=" << doubleReverse[1][i][j][k] << "; ";
				if (k % 5 == 0 && k > 0)cout << "\n";
			}
		}
	}
	else {
		cout << "\nDimension: " << dimension_i << "  Level " << li << "    LowBucket Predicates: " << fix[0][dimension_i][li][0] << "   ----------------\n";
		_for(k, 0, numBucket) {
			cout << "lBkt" << k << ": bID=" << bitsID[0][dimension_i][li][k] << ", eBkt=" << endBucket[0][dimension_i][li][k] << ", dRvs=" << doubleReverse[0][dimension_i][li][k] << "; ";
			if (k % 5 == 0 && k > 0)cout << "\n";
		}
		cout << "\n\nDimension: " << dimension_i << "  Level " << li << "    HighBucket Predicates: " << fix[1][dimension_i][li][numBucket] << "   ----------------\n";
		_for(k, 0, numBucket) {
			cout << "hBkt" << k << ": bID=" << bitsID[1][dimension_i][li][k] << ", eBkt=" << endBucket[1][dimension_i][li][k] << ", dRvs=" << doubleReverse[1][dimension_i][li][k] << "; ";
			if (k % 5 == 0 && k > 0)cout << "\n";
		}
	}
	cout << "\n\n";
}
