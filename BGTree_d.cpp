//
// Created by swh on 2022/1/12.
//

#include "BGTree_d.h"

BGTree_d::BGTree_d() {
	numSub = 0;
	numNode = 0;
	height = 0;
	maxNodeSize = MAXNodeSIZE;
	memset(subPredicate, 0, sizeof(subPredicate));
	roots.resize(atts);
	_for(i, 0, atts) {
		roots[i] = new bluenode_d(0, valDom - 1, ++numNode, 1, nullptr, nullptr, nullptr, nullptr, nullptr);
	}
	nB.resize(atts);
	nnB.resize(atts);

	cout << "ExpID = " << expID << ". BGTree_d: maxNodeSize = " << maxNodeSize << "\n";
}

BGTree_d::~BGTree_d() {
	_for(i, 0, atts) {
		releaseBlueNode(roots[i]);
	}
}

void BGTree_d::releaseBlueNode(bluenode_d *&r) {
	if (r->lowGreenChild) releaseGreenNode(r->lowGreenChild);
	if (r->highGreenChild) releaseGreenNode(r->highGreenChild);
	if (r->leftBlueChild) releaseBlueNode(r->leftBlueChild);
	if (r->rightBlueChild) releaseBlueNode(r->rightBlueChild);
	if(r->bst) delete r->bst;
	delete r;
}

void BGTree_d::releaseGreenNode(lgreennode_d *&r) {
	if (r->leftChild) releaseGreenNode(r->leftChild);
	if (r->rightChild) releaseGreenNode(r->rightChild);
	if(r->bst) delete r->bst;
	delete r;
}

void BGTree_d::releaseGreenNode(hgreennode_d *&r) {
	if (r->leftChild) releaseGreenNode(r->leftChild);
	if (r->rightChild) releaseGreenNode(r->rightChild);
	if(r->bst) delete r->bst;
	delete r;
}

void BGTree_d::insert(IntervalSub sub, const vector<IntervalSub> &subList) {
	vector<bool> attrExist(atts, false);
	subPredicate[sub.id] = sub.size;
	for (auto &&c: sub.constraints) {
		insertIntoBlueNode(roots[c.att], sub.id, c.lowValue,
						   c.highValue, c.att, subList);
		attrExist[c.att] = true;
		nnB[c.att][sub.id] = 1;
	}
	_for(i, 0, atts) if (!attrExist[i])
			nB[i][sub.id] = 1;
	numSub++;
}

void BGTree_d::insertIntoBlueNode(bluenode_d *&r, const int &subID, const int &l, const int &h, const int &attrId,
								  const vector<IntervalSub> &subList) {
	r->subids.push_back(subID);
	if (r->subids.size() > boundary) {
		if (r->bst == nullptr) {
			r->bst = new bitset<subs>;
			for (auto &&id: r->subids)
				(*r->bst)[id] = 1;
		}
		(*r->bst)[subID] = 1;
	}
	if (r->mid == h) r->lMidv.push_back(subID);
	else if (r->mid + 1 == l) r->rMidv.push_back(subID);

	if (r->leftBlueChild != nullptr) { // has child
		if (h <= r->mid) {
			insertIntoBlueNode(r->leftBlueChild, subID, l, h, attrId, subList);
		} else if (l > r->mid) {
			insertIntoBlueNode(r->rightBlueChild, subID, l, h, attrId, subList);
		} else {
			insertIntoGreenNode(r->lowGreenChild, subID, l, attrId, subList);
			insertIntoGreenNode(r->highGreenChild, subID, h, attrId, subList);
		}
	} else if (r->subids.size() > maxNodeSize) { // no child and too big node
		r->leftBlueChild = new bluenode_d(r->l, r->mid, ++numNode, r->levelid + 1, nullptr, nullptr, nullptr, nullptr,
										  nullptr);
		r->rightBlueChild = new bluenode_d(r->mid + 1, r->h, ++numNode, r->levelid + 1, nullptr, nullptr, nullptr,
										   nullptr, nullptr);
		r->lowGreenChild = new lgreennode_d(r->l, r->mid, ++numNode, r->levelid + 1, nullptr, nullptr, nullptr);
		r->highGreenChild = new hgreennode_d(r->mid + 1, r->h, ++numNode, r->levelid + 1, nullptr, nullptr, nullptr);
		for (auto &&id: r->subids) {
			for (auto &&c: subList[id].constraints) {
				if (c.att == attrId) {
					if (c.highValue <= r->mid) {
						r->leftBlueChild->subids.push_back(id);
						// r->leftBlueChild->realBstPtr[id] = 1; // boundary>>maxNodeSize, so �ڵ������ʱ�ӽڵ㲻������Ҫbitset�洢
						if (r->leftBlueChild->mid == c.highValue) r->leftBlueChild->lMidv.push_back(id);
						else if (r->leftBlueChild->mid + 1 == c.lowValue) r->leftBlueChild->rMidv.push_back(id);
					} else if (c.lowValue > r->mid) {
						r->rightBlueChild->subids.push_back(id);
						// r->rightBlueChild->realBstPtr[id] = 1;
						if (r->rightBlueChild->mid == c.highValue) r->rightBlueChild->lMidv.push_back(id);
						else if (r->rightBlueChild->mid + 1 == c.lowValue) r->rightBlueChild->rMidv.push_back(id);
					} else { // l<=mid<mid+1<=h
						r->lowGreenChild->subids.push_back(id);
						// r->lowGreenChild->realBstPtr[id] = 1;
						if (r->lowGreenChild->mid + 1 == c.lowValue)
							r->lowGreenChild->midv.push_back(id);
						r->highGreenChild->subids.push_back(id);
						// r->highGreenChild->realBstPtr[id] = 1;
						if (r->highGreenChild->mid - 1 == c.highValue)
							r->highGreenChild->midv.push_back(id);
					}
					break;
				}
			}
		}
	}
}

void BGTree_d::insertIntoGreenNode(lgreennode_d *&r, const int &subID, const int &l, const int &attrId,
								   const vector<IntervalSub> &subList) {
	r->subids.push_back(subID);
	if (r->subids.size() > boundary) {
		if (r->bst == nullptr) {
			r->bst = new bitset<subs>;
			for (auto &&id: r->subids)
				(*r->bst)[id] = 1;
		}
		(*r->bst)[subID] = 1;
	}
	if (r->mid + 1 == l)
		r->midv.push_back(subID);

	if (r->leftChild != nullptr) {  // �����ӱ������ӽڵ�
		if (l <= r->mid)
			insertIntoGreenNode(r->leftChild, subID, l, attrId, subList);
		else insertIntoGreenNode(r->rightChild, subID, l, attrId, subList);
	} else if (r->subids.size() > maxNodeSize) {
		r->leftChild = new lgreennode_d(r->l, r->mid, ++numNode, r->levelid + 1, nullptr, nullptr, nullptr);
		r->rightChild = new lgreennode_d(r->mid + 1, r->h, ++numNode, r->levelid + 1, nullptr, nullptr, nullptr);
		for (auto &&id: r->subids) {
			for (auto &&c: subList[id].constraints) {
				if (c.att == attrId) {
					if (c.lowValue <= r->mid) {
						r->leftChild->subids.push_back(id);
						// r->leftChild->realBstPtr[id] = 1;  // boundary>>maxNodeSize, so �ڵ������ʱ�ӽڵ㲻������Ҫbitset�洢
						if (r->leftChild->mid + 1 == c.lowValue)
							r->leftChild->midv.push_back(id);
					} else {
						r->rightChild->subids.push_back(id);
						// r->rightChild->realBstPtr[id] = 1;
						if (r->rightChild->mid + 1 == c.lowValue)
							r->rightChild->midv.push_back(id);
					}
					break;
				}
			}
		}
	}
}

void BGTree_d::insertIntoGreenNode(hgreennode_d *&r, const int &subID, const int &h, const int &attrId,
								   const vector<IntervalSub> &subList) {
	r->subids.push_back(subID);
	if (r->subids.size() > boundary) {
		if (r->bst == nullptr) {
			r->bst = new bitset<subs>;
			for (auto &&id: r->subids)
				(*r->bst)[id] = 1;
		}
		(*r->bst)[subID] = 1;
	}
	if (r->mid - 1 == h)
		r->midv.push_back(subID);

	if (r->leftChild != nullptr) {  // �����ӱ������ӽڵ�
		if (h < r->mid)
			insertIntoGreenNode(r->leftChild, subID, h, attrId, subList);
		else insertIntoGreenNode(r->rightChild, subID, h, attrId, subList);
	} else if (r->subids.size() > maxNodeSize) {
		r->leftChild = new hgreennode_d(r->l, r->mid - 1, ++numNode, r->levelid + 1, nullptr, nullptr, nullptr);
		r->rightChild = new hgreennode_d(r->mid, r->h, ++numNode, r->levelid + 1, nullptr, nullptr, nullptr);
		for (auto &&id: r->subids) {
			for (auto &&c: subList[id].constraints) {
				if (c.att == attrId) {
					if (c.highValue < r->mid) {
						r->leftChild->subids.push_back(id);
						// r->leftChild->realBstPtr[id] = 1;
						if (r->leftChild->mid - 1 == c.highValue)
							r->leftChild->midv.push_back(id);
					} else {
						r->rightChild->subids.push_back(id);
						// r->rightChild->realBstPtr[id] = 1;
						if (r->rightChild->mid - 1 == c.highValue)
							r->rightChild->midv.push_back(id);
					}
					break;
				}
			}
		}
	}
}

bool BGTree_d::deleteSubscription(IntervalSub sub) {
	bool find = true;
	for (auto &&c: sub.constraints) {
		find &= deleteFromBlueNode(roots[c.att], sub.id, c.lowValue,
								   c.highValue);
	}
	subPredicate[sub.id] = 0;
	numSub--;
	return find;
}

bool BGTree_d::deleteFromBlueNode(bluenode_d *&r, const int &subID, const int &l, const int &h) {
	bool find = false;
	if (r->bst != nullptr) {
		(*(r->bst))[subID] = 0;
		if (r->subids.size() < boundary)
			delete r->bst;
	}
	for (vector<int>::const_iterator it = r->subids.cbegin(); it != r->subids.cend(); it++) {
		if (*it == subID) {
			r->subids.erase(it);
			find = true;
			break;
		}
	}
	if (r->mid == h) {
		find = false;
		for (vector<int>::const_iterator it = r->lMidv.cbegin(); it != r->lMidv.cend(); it++) {
			if (*it == subID) {
				r->lMidv.erase(it);
				find = true;
				break;
			}
		}
	} else if (r->mid + 1 == l) {
		find = false;
		for (vector<int>::const_iterator it = r->rMidv.cbegin(); it != r->rMidv.cend(); it++) {
			if (*it == subID) {
				r->rMidv.erase(it);
				find = true;
				break;
			}
		}
	}

	if (r->leftBlueChild != nullptr) { // has child
		if (h <= r->mid) {
			find &= deleteFromBlueNode(r->leftBlueChild, subID, l, h);
		} else if (l > r->mid) {
			find &= deleteFromBlueNode(r->rightBlueChild, subID, l, h);
		} else {
			find &= deleteFromGreenNode(r->lowGreenChild, subID, l);
			find &= deleteFromGreenNode(r->highGreenChild, subID, h);
		}

		if (r->subids.size() < maxNodeSize) { // has child and too small node
			delete r->leftBlueChild, r->rightBlueChild, r->lowGreenChild, r->highGreenChild;
		}
	}
	return find;
}

bool BGTree_d::deleteFromGreenNode(lgreennode_d *&r, const int &subID, const int &l) {
	bool find = false;
	if (r->bst != nullptr) {
		(*(r->bst))[subID] = 0;
		if (r->subids.size() < boundary)
			delete r->bst;
	}
	for (vector<int>::const_iterator it = r->subids.cbegin(); it != r->subids.cend(); it++) {
		if (*it == subID) {
			r->subids.erase(it);
			find = true;
			break;
		}
	}
	if (r->mid + 1 == l) {
		find = false;
		for (vector<int>::const_iterator it = r->midv.cbegin(); it != r->midv.cend(); it++) {
			if (*it == subID) {
				r->midv.erase(it);
				find = true;
				break;
			}
		}
	}
	if (r->leftChild != nullptr) { // has child
		if (l <= r->mid) {
			find &= deleteFromGreenNode(r->leftChild, subID, l);
		} else {
			find &= deleteFromGreenNode(r->rightChild, subID, l);
		}
		if (r->subids.size() < maxNodeSize) { // has child and too small node
			delete r->leftChild, r->rightChild, r->bst;
		}
	}
	return find;
}

bool BGTree_d::deleteFromGreenNode(hgreennode_d *&r, const int &subID, const int &h) {
	bool find = false;
	if (r->bst != nullptr) {
		(*(r->bst))[subID] = 0;
		if (r->subids.size() < boundary)
			delete r->bst;
	}
	for (vector<int>::const_iterator it = r->subids.cbegin(); it != r->subids.cend(); it++) {
		if (*it == subID) {
			r->subids.erase(it);
			find = true;
			break;
		}
	}
	if (r->mid - 1 == h) {
		find = false;
		for (vector<int>::const_iterator it = r->midv.cbegin(); it != r->midv.cend(); it++) {
			if (*it == subID) {
				r->midv.erase(it);
				find = true;
				break;
			}
		}
	}
	if (r->leftChild != nullptr) { // has child
		if (h < r->mid) {
			find &= deleteFromGreenNode(r->leftChild, subID, h);
		} else {
			find &= deleteFromGreenNode(r->rightChild, subID, h);
		}

		if (r->subids.size() < maxNodeSize) { // has child and too small node
			delete r->leftChild, r->rightChild;
		}
	}
	return find;
}

void BGTree_d::forward_match_native(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList) {
	memcpy(counter, subPredicate, sizeof(subPredicate));
	for (auto &&pi: pub.pairs)
		forward_match_blueNode(roots[pi.att], pi.att, pi.value, subList);
	for (int i = 0; i < subs; i++)
		if (counter[i] == 0) {
#ifdef DEBUG
			numEffectivePredicate += subPredicate[i];
#endif
			++matchSubs;
			//cout << "BG-Tree matches sub: " << i << endl;
		}
}

void
BGTree_d::forward_match_blueNode(bluenode_d *&r, const int &att, const int &value, const vector<IntervalSub> &subList) {
	if (r->leftBlueChild == nullptr) { // 1. Ҷ�ӽڵ㱩������
#ifdef DEBUG
		numProcessTwoCmpNode++;
		numProcessTwoCmpPredicate += r->subids.size();
#endif
		for (auto &&i: r->subids) {
			for (auto &&pi: subList[i].constraints)
				if (att == pi.att) {
					if (pi.lowValue <= value && value <= pi.highValue)
						counter[i]--;
					break;
				}
		}
	} else if (value < r->mid) { // 2. С�����е�, ���������ӽڵ�
		forward_match_blueNode(r->leftBlueChild, att, value, subList);
		forward_match_lgreenNode(r->lowGreenChild, att, value, subList);
	} else if (value == r->mid) { // 3. �������е�, ֱ�ӵõ�ƥ����
#ifdef DEBUG
		hit++;
		numProcessExactNode++;
		numProcessExactPredicate += r->lMidv.size() + r->lowGreenChild->subids.size();
#endif
		for (auto &&id: r->lMidv) {
			counter[id]--;
		}
		for (auto &&id: r->lowGreenChild->subids) {
			counter[id]--;
		}
	} else if (value == r->mid + 1) { // 4. �������е�, ֱ�ӵõ�ƥ����
#ifdef DEBUG
		hit++;
		numProcessExactNode++;
		numProcessExactPredicate += r->rMidv.size() + r->highGreenChild->subids.size();
#endif
		for (auto &&id: r->rMidv) {
			counter[id]--;
		}
		for (auto &&id: r->highGreenChild->subids) {
			counter[id]--;
		}
	} else { // 5. value > r->mid + 1 �������нڵ�, ���������ӽڵ�
		forward_match_blueNode(r->rightBlueChild, att, value, subList);
		forward_match_hgreenNode(r->highGreenChild, att, value, subList);
	}
}

void BGTree_d::forward_match_lgreenNode(lgreennode_d *&r, const int &att, const int &value,
										const vector<IntervalSub> &subList) {
	// �����п��ܳ��ֵķ�ǰ��
	if (r->leftChild == nullptr) { // 1. Ҷ�ӽڵ㱩������
#ifdef DEBUG
		numProcessOneCmpNode++;
		numProcessOneCmpPredicate += r->subids.size();
#endif
		for (auto &&id: r->subids) {
			for (auto &&pi: subList[id].constraints)
				if (att == pi.att) {
					if (pi.lowValue <= value)
						counter[id]--;
					break;
				}
		}
	} else if (value < r->mid) { // 2. С�����е�, �������ӽڵ�
		forward_match_lgreenNode(r->leftChild, att, value, subList);
	} else if (value > r->mid + 1) { // 5. �������е�, ���ӽڵ���ȫƥ��, �������ӽڵ�
#ifdef DEBUG
		numProcessExactNode++;
		numProcessExactPredicate += r->leftChild->subids.size();
#endif
		for (auto &&id: r->leftChild->subids) {
			counter[id]--;
		}
		forward_match_lgreenNode(r->rightChild, att, value, subList);
	} else if (value == r->mid) { // 3. �������е�, ���ӽڵ�ȫ��ƥ��
#ifdef DEBUG
		numProcessExactNode++;
		numProcessExactPredicate += r->leftChild->subids.size();
#endif
		for (auto &&id: r->leftChild->subids) {
			counter[id]--;
		}
	} else { // 4. if (value == r->mid + 1), ���� r->mid+1, ֱ�ӵõ�ƥ����
#ifdef DEBUG
		numProcessExactNode++;
		numProcessExactPredicate += r->midv.size() + r->leftChild->subids.size();
#endif
		for (auto &&id: r->midv) {
			counter[id]--;
		}
		for (auto &&id: r->leftChild->subids) {
			counter[id]--;
		}
	}
}

void BGTree_d::forward_match_hgreenNode(hgreennode_d *&r, const int &att, const int &value,
										const vector<IntervalSub> &subList) {
	// �����п��ܳ��ֵķ�ǰ��
	if (r->leftChild == nullptr) { // 1. Ҷ�ӽڵ㱩������
#ifdef DEBUG
		numProcessOneCmpNode++;
		numProcessOneCmpPredicate += r->subids.size();
#endif
		for (auto &&id: r->subids) {
			for (auto &&pi: subList[id].constraints)
				if (att == pi.att) {
					if (value <= pi.highValue)
						counter[id]--;
					break;
				}
		}
	} else if (value > r->mid) { // 2. �������е�, �������ӽڵ�
		forward_match_hgreenNode(r->rightChild, att, value, subList);
	} else if (value < r->mid - 1) { // 5. С�����е�, ���ӽڵ���ȫƥ��, �������ӽڵ�
#ifdef DEBUG
		numProcessExactNode++;
		numProcessExactPredicate += r->rightChild->subids.size();
#endif
		for (auto &&id: r->rightChild->subids) {
			counter[id]--;
		}
		forward_match_hgreenNode(r->leftChild, att, value, subList);
	} else if (value == r->mid) { // 3. �������е�, ���ӽڵ�ȫ��ƥ��
#ifdef DEBUG
		numProcessExactNode++;
		numProcessExactPredicate += r->rightChild->subids.size();
#endif
		for (auto &&id: r->rightChild->subids) {
			counter[id]--;
		}
	} else { // 4. if (value == r->mid - 1), �������е�, ֱ�ӵõ�ƥ����
#ifdef DEBUG
		numProcessExactNode++;
		numProcessExactPredicate += r->midv.size() + r->rightChild->subids.size();
#endif
		for (auto &&id: r->midv) {
			counter[id]--;
		}
		for (auto &&id: r->rightChild->subids) {
			counter[id]--;
		}
	}
}

void BGTree_d::backward_match_native(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList) {
	bitset<subs> gB; // global bitset
	vector<bool> attExist(atts, false);
	for (auto &&pi: pub.pairs) {
		attExist[pi.att] = true;
		backward_match_blueNode_native(roots[pi.att], pi.att, pi.value, subList, gB);
	}
	_for(i, 0, atts) if (!attExist[i])
			gB = gB | nnB[i];
	matchSubs = numSub - gB.count();
#ifdef DEBUG
	for (int i = 0; i < subs; i++)
		if (gB[i] == 0)
			numEffectivePredicate += subPredicate[i];
#endif
}

void BGTree_d::backward_match_blueNode_native(bluenode_d *&r, const int &att, const int &value,
											  const vector<IntervalSub> &subList, bitset<subs> &gB) {
	if (r->leftBlueChild == nullptr) {
#ifdef DEBUG
		numProcessTwoCmpNode++;
		numProcessTwoCmpPredicate += r->subids.size();
#endif
		for (auto &&i: r->subids) {
			for (auto &&pi: subList[i].constraints)
				if (att == pi.att) {
					if (value < pi.lowValue || pi.highValue < value)
						gB[i] = 1;
					break;
				}
		}
	} else if (value <= r->mid) {
		if (r->rightBlueChild->bst == nullptr) {
#ifdef DEBUG
			numProcessExactNode++;
			numProcessExactPredicate += r->rightBlueChild->subids.size();
#endif
			for (auto &&id: r->rightBlueChild->subids)
				gB[id] = 1;
		} else {
#ifdef DEBUG
			numProcessExactNode++;
			numProcessExactPredicate += r->rightBlueChild->subids.size();
			numOROperation++;
#endif
			gB = gB | *r->rightBlueChild->bst;
		}
		if (value == r->mid && r->leftBlueChild->bst != nullptr) {
#ifdef DEBUG
			numProcessExactNode++;
			numProcessExactPredicate += r->leftBlueChild->subids.size() - r->lMidv.size();
			numOROperation++;
			hit++;
#endif
			bitset<subs> bst_c = *r->leftBlueChild->bst;
			for (auto &&id: r->lMidv)
				bst_c[id] = 0;
			gB = gB | bst_c;
		} else {
			backward_match_blueNode_native(r->leftBlueChild, att, value, subList, gB);
			backward_match_lgreenNode_native(r->lowGreenChild, att, value, subList, gB);
		}
	} else {
		if (r->leftBlueChild->bst == nullptr) {
#ifdef DEBUG
			numProcessExactNode++;
			numProcessExactPredicate += r->leftBlueChild->subids.size();
#endif
			for (auto &&id: r->leftBlueChild->subids)
				gB[id] = 1;
		} else {
#ifdef DEBUG
			numProcessExactNode++;
			numProcessExactPredicate += r->leftBlueChild->subids.size();
			numOROperation++;
#endif
			gB = gB | *r->leftBlueChild->bst;
		}
		if (value == r->mid + 1 && r->rightBlueChild->bst != nullptr) {
#ifdef DEBUG
			numProcessExactNode++;
			numProcessExactPredicate += r->rightBlueChild->subids.size() - r->rMidv.size();
			numOROperation++;
			hit++;
#endif
			bitset<subs> bst_c = *r->rightBlueChild->bst;
			for (auto &&id: r->rMidv)
				bst_c[id] = 0;
			gB = gB | bst_c;
		} else {
			backward_match_blueNode_native(r->rightBlueChild, att, value, subList, gB);
			backward_match_hgreenNode_native(r->highGreenChild, att, value, subList, gB);
		}
	}
}

void
BGTree_d::backward_match_lgreenNode_native(lgreennode_d *&l, const int &att, const int &value,
										   const vector<IntervalSub> &subList, bitset<subs> &gB) {
	// �����п��ܳ��ֵķ�ǰ��
	if (l->leftChild == nullptr) { // 1. Ҷ�ӽڵ㱩������
#ifdef DEBUG
		numProcessOneCmpNode++;
		numProcessOneCmpPredicate += l->subids.size();
#endif
		for (auto &&id: l->subids) {
			for (auto &&pi: subList[id].constraints)
				if (att == pi.att) {
					if (pi.lowValue > value)
						gB[id] = 1;
					break;
				}
		}
	} else if (value <= l->mid) {
		if (l->rightChild->bst == nullptr) {
#ifdef DEBUG
			numProcessExactNode++;
			numProcessExactPredicate += l->rightChild->subids.size();
#endif
			for (auto &&id: l->rightChild->subids)
				gB[id] = 1;
		} else {
#ifdef DEBUG
			numProcessExactNode++;
			numProcessExactPredicate += l->rightChild->subids.size();
			numOROperation++;
#endif
			gB = gB | *l->rightChild->bst;
		}
		if (value < l->mid)
			backward_match_lgreenNode_native(l->leftChild, att, value, subList, gB);
#ifdef DEBUG
		if (value == l->mid)
			hit++;
#endif
	} else { // l->mid < value
		if (l->mid + 1 == value && l->rightChild->bst != nullptr) {
#ifdef DEBUG
			numProcessExactNode++;
			numProcessExactPredicate += l->rightChild->subids.size() - l->midv.size();
			numOROperation++;
			hit++;
#endif
			bitset<subs> bst_c = *l->rightChild->bst;
			for (auto &&id: l->midv)
				bst_c[id] = 0;
			gB = gB | bst_c;
		} else {
			backward_match_lgreenNode_native(l->rightChild, att, value, subList, gB);
		}
	}
}

void
BGTree_d::backward_match_hgreenNode_native(hgreennode_d *&r, const int &att, const int &value,
										   const vector<IntervalSub> &subList, bitset<subs> &gB) {
	if (r->leftChild == nullptr) { // 1. Ҷ�ӽڵ㱩������
#ifdef DEBUG
		numProcessOneCmpNode++;
		numProcessOneCmpPredicate += r->subids.size();
#endif
		for (auto &&id: r->subids) {
			for (auto &&pi: subList[id].constraints)
				if (att == pi.att) {
					if (pi.highValue < value)
						gB[id] = 1;
					break;
				}
		}
	} else if (value >= r->mid) { // 2. fall into right subInterval
		if (r->leftChild->bst == nullptr) { // leftChild must be all-unmatch
#ifdef DEBUG
			numProcessExactNode++;
			numProcessExactPredicate += r->leftChild->subids.size();
#endif
			for (auto &&id: r->leftChild->subids)
				gB[id] = 1;
		} else {
#ifdef DEBUG
			numProcessExactNode++;
			numProcessExactPredicate += r->leftChild->subids.size();
			numOROperation++;
#endif
			gB = gB | *r->leftChild->bst;
		}
		if (value > r->mid)
			backward_match_hgreenNode_native(r->rightChild, att, value, subList, gB);
#ifdef DEBUG
		if (value == r->mid)
			hit++;
#endif
	} else { // value < r->mid
		if (r->mid == value + 1 && r->leftChild->bst != nullptr) {
#ifdef DEBUG
			numProcessExactNode++;
			numProcessExactPredicate += r->leftChild->subids.size() - r->midv.size();
			numOROperation++;
			hit++;
#endif
			bitset<subs> bst_c = *r->leftChild->bst;
			for (auto &&id: r->midv)
				bst_c[id] = 0;
			gB = gB | bst_c;
		} else {
			backward_match_hgreenNode_native(r->leftChild, att, value, subList, gB);
		}
	}
}

int BGTree_d::calMemory() {
	double size = 0.0; // Byte
	size += sizeof(roots) + sizeof(bluenode_d *) * atts;
	bluenode_d *b;
	lgreennode_d *l;
	hgreennode_d *h;
	for (int i = 0; i < atts; i++) {
		queue<bluenode_d *> bq;
		queue<lgreennode_d *> lq;
		queue<hgreennode_d *> hq;
		bq.push(roots[i]);
		while (!bq.empty() || !lq.empty() || !hq.empty()) {
			if (!bq.empty()) {
				b = bq.front();
				bq.pop();
				size += sizeof(b) + (5 + b->subids.size() + b->lMidv.size() + b->rMidv.size()) * sizeof(int) +
						sizeof(vector<int>) * 3;
				size +=
					sizeof(lgreennode_d *) + sizeof(hgreennode_d *) + sizeof(bluenode_d *) * 2 + sizeof(bitset<subs> *);
				if (b->leftBlueChild) {
					bq.push(b->leftBlueChild);
					bq.push(b->rightBlueChild);
					lq.push(b->lowGreenChild);
					hq.push(b->highGreenChild);
				}
				if (b->bst) size += sizeof(bitset<subs>);
			}
			if (!lq.empty()) {
				l = lq.front();
				lq.pop();
				size += sizeof(l) + (5 + l->subids.size() + l->midv.size()) * sizeof(int) + sizeof(vector<int>) * 2 +
						sizeof(bitset<subs> *);
				if (l->leftChild != nullptr) {
					size += sizeof(lgreennode_d *) * 2;
					lq.emplace(l->leftChild);
					lq.emplace(l->rightChild);
				}
				if (l->bst)size += sizeof(bitset<subs>);
			}
			if (!hq.empty()) {
				h = hq.front();
				hq.pop();
				size += sizeof(h) + (5 + h->subids.size() + h->midv.size()) * sizeof(int) + sizeof(vector<int>) * 2 +
						sizeof(bitset<subs> *);
				if (h->leftChild != nullptr) {
					size += sizeof(hgreennode_d *) * 2;
					hq.emplace(h->leftChild);
					hq.emplace(h->rightChild);
				}
				if (h->bst)size += sizeof(bitset<subs>);
			}
		}
	}
	size += sizeof(int) * subs * 2; // subPredicate counter
	size += sizeof(nB) + sizeof(nB[0]) * atts; // nB or nnB
	size = size / 1024 / 1024; // MB
	return (int) size;
}

void BGTree_d::printBGTree() {
	vector<vector<vector<tuple<int, int, int, int>>>> nodeInfo(atts, vector<vector<tuple<int, int, int, int>>>());
	int height = 0;
	_for(i, 0, atts) {
		height = 0;
		queue<bluenode_d *> bNextLevel;
		queue<lgreennode_d *> lgNextLevel;
		queue<hgreennode_d *> hgNextLevel;
		bNextLevel.push(roots[i]);
		while (!bNextLevel.empty() || !lgNextLevel.empty() || !hgNextLevel.empty()) { // for each level
			nodeInfo[i].push_back(vector<tuple<int, int, int, int>>());
			int b = bNextLevel.size(), l = lgNextLevel.size(), r = hgNextLevel.size();
			_for(j, 0, b) {
				bluenode_d *t = bNextLevel.front();
				bNextLevel.pop();
				nodeInfo[i][height].push_back(make_tuple(1, t->l, t->h, t->subids.size()));
				if (t->leftBlueChild) bNextLevel.push(t->leftBlueChild);
				if (t->rightBlueChild) bNextLevel.push(t->rightBlueChild);
				if (t->lowGreenChild) lgNextLevel.push(t->lowGreenChild);
				if (t->highGreenChild) hgNextLevel.push(t->highGreenChild);
			}
			_for(j, 0, l) {
				lgreennode_d *t = lgNextLevel.front();
				lgNextLevel.pop();
				nodeInfo[i][height].push_back(make_tuple(2, t->l, t->h, t->subids.size()));
				if (t->leftChild) lgNextLevel.push(t->leftChild);
				if (t->rightChild) lgNextLevel.push(t->rightChild);
			}
			_for(j, 0, r) {
				hgreennode_d *t = hgNextLevel.front();
				hgNextLevel.pop();
				nodeInfo[i][height].push_back(make_tuple(3, t->l, t->h, t->subids.size()));
				if (t->leftChild) hgNextLevel.push(t->leftChild);
				if (t->rightChild) hgNextLevel.push(t->rightChild);
			}
			height++;
		}
	}
	_for(i, 0, 1) {
		_for(j, 0, nodeInfo[i].size()) {
			cout << "a" << i << ", h" << j << ", nodeNum= " << nodeInfo[i][j].size() << "\n";
			int nodeId = -1, lv = -1, hv = -1, maxNumNodeSub = -1;
			sort(nodeInfo[i][j].begin(), nodeInfo[i][j].end(),
				 [](const tuple<int, int, int, int> &a, const tuple<int, int, int, int> &b) {
					 return get<0>(a) == get<0>(b) ? get<3>(a) > get<3>(b) : get<0>(a) < get<0>(b);
				 });
			_for(k, 0, nodeInfo[i][j].size()) {
				cout << "(" << get<0>(nodeInfo[i][j][k]) << "," << get<1>(nodeInfo[i][j][k]) << ","
					 << get<2>(nodeInfo[i][j][k]) << ", " << get<3>(nodeInfo[i][j][k]) << "), ";
				if (k < nodeInfo[i][j].size() - 1 && (get<0>(nodeInfo[i][j][k]) != get<0>(nodeInfo[i][j][k + 1])))
					cout << "\n";
				if (maxNumNodeSub < get<3>(nodeInfo[i][j][k])) {
					nodeId = get<0>(nodeInfo[i][j][k]);
					lv = get<1>(nodeInfo[i][j][k]);
					hv = get<2>(nodeInfo[i][j][k]);
					maxNumNodeSub = get<3>(nodeInfo[i][j][k]);
				}
			}
			cout << "\nNodeID= " << nodeId << " [" << lv << ", " << hv << "], size= " << maxNumNodeSub << "\n\n";
		}
		cout << "\n";
	}
	cout << "ExactNode: " << numProcessExactNode << ", oneCmpNode: " << numProcessOneCmpNode << ", twoCmpNode: "
		 << numProcessTwoCmpNode \
 << "\nExactPredicate: " << numProcessExactPredicate << ", oneCmpPredicate: " << numProcessOneCmpPredicate
		 << ", twoCmpPredicate: " << numProcessTwoCmpPredicate \
 << "\nEffectivePredicate: " << numEffectivePredicate << ", effectiveRate: "
		 << (double) numEffectivePredicate /
			(double) (numProcessExactPredicate + numProcessOneCmpPredicate + numProcessTwoCmpPredicate + 1)
		 << "\nOR: " << numOROperation <<", AvgHit: "<<hit<< ".\n\n";

}

int BGTree_d::getHeight() {
	int height = 0;
	queue<bluenode_d *> bq;
	queue<lgreennode_d *> lq;
	queue<hgreennode_d *> hq;
	bluenode_d *b;
	lgreennode_d *l;
	hgreennode_d *h;
	_for(i, 0, atts) bq.emplace(roots[i]);
	while (!bq.empty() || !lq.empty() || !hq.empty()) {
		if (!bq.empty()) {
			b = bq.front();
			bq.pop();
			height = max(height, b->levelid);
			if (b->leftBlueChild != nullptr) {
				bq.emplace(b->leftBlueChild);
				bq.emplace(b->rightBlueChild);
				lq.emplace(b->lowGreenChild);
				hq.emplace(b->highGreenChild);
			}
		}
		if (!lq.empty()) {
			l = lq.front();
			lq.pop();
			height = max(height, l->levelid);
			if (l->leftChild != nullptr) {
				lq.emplace(l->leftChild);
				lq.emplace(l->rightChild);
			}
		}
		if (!hq.empty()) {
			h = hq.front();
			hq.pop();
			height = max(height, h->levelid);
			if (h->leftChild != nullptr) {
				hq.emplace(h->leftChild);
				hq.emplace(h->rightChild);
			}
		}
	}
	return height;
}