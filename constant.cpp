#include "constant.h"
string expID = "323";       // ��¼ʵ��id
bool display = 0;           // �Ƿ��ӡӳ���ϵ
const int interval = 1000;   // ÿƥ����ô����¼����һ����־��Ϣ
//const int subs = 1000000; // Number of subscriptions.
const int pubs = 500;       // Number of publications.
int atts = 5000;            // Total number of attributes, i.e. dimensions.
const int cons = 10;        // Number of constraints(predicates) in one sub.
int m = 5000;                 // Number of constraints in one pub.
const int attDis = 0;       // The distribution of attributes in subs and pubs. 0:uniform distribution | 1:Zipf distribution
const int valDis = 0;       // The distribution of values in subs and pubs. 0:uniform �̶����; 1: ������ν��(��widthΪ��С���)
const int valDom = 1000000; // Cardinality of values.
const double alpha = 0;     // Parameter for Zipf distribution.
double width = 0.3;         // Width of a predicate.

// Rein
const int buks = 1000;      // Number of buckets.

// BIOP��Rein2
int be = 0;                 // ÿ��ά��0�� 2 * 2^bits��bits����, -1��ʾ��Ͱ����Ϊbits�������� bitExponent
const int lvls = 2;         // ����
const int gs = 5;           // ��Ĵ�С     

// Tama
const int level = 11;
