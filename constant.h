//#pragma once
#ifndef _CONST_H
#define _CONST_H
#include<iostream>
using namespace std;

extern string expID;       // ��¼ʵ��id
extern const bool display; // �Ƿ��ӡӳ���ϵ
extern const bool verifyID;// whether to verify Insertion and Deletion function
extern const int interval; // ÿƥ����ô����¼����һ����־��Ϣ
extern const double subp;  // subp�����Ķ��ĵ�ν�ʶ�����ǰcons��ά����
const int subs = 1000000;  // Number of subscriptions.
extern const int pubs;     // Number of publications.
extern int atts;           // Total number of attributes, i.e. dimensions.
extern int cons;     // Number of constraints(predicates) in one sub.
extern int m;              // Number of constraints in one pub.
extern const int attDis;   // The distribution of attributes in subs and pubs. 0:uniform distribution | 1:Zipf distribution
extern const int valDis;   // The distribution of values in subs and pubs. 0:uniform
extern const int valDom;   // Cardinality of values.
extern const double alpha; // Parameter for Zipf distribution.
extern double width;       // Width of a predicate.

// Rein
extern const int buks;     // Number of buckets.

// BIOP
extern int be;             // ÿ��ά���� 2 * 2^bits��bits����
extern int be2;            // be=-1ʱÿ��ά����be2��bits����
extern const int lvls;     // ����
extern int gs;             // һ���ж��ٸ�ά��

// Tama
extern const int level;

// AdaRein
//const int MAX_SUBS = 1000000;
//const int MAX_ATTS = 20; // Tama
//const int MAX_BUCKS = 1000;
extern const double falsePositiveRate;

// OpIndex
const int SEGMENTS = 32;
const int MAX_SIGNATURE = 61;
#endif

