#include "Rein.h"
#include "BIOP.h"
#include<iomanip>
#include <iostream>
using namespace std;

void print(char*, int = 0); // Ĭ��ֵӦ�ü��ں��������У���int���桰=0��

// Don't modify main function.
int main222()
{
	char str[12] = "OneTwoThree";
	print(str);
	print(str, 2);
	print(str, 0);
	print(str, 44);
	return 0;
}

// ��������ʵ��print����
void print(char str[], int p) {
	static int numCall = 0;
	numCall++;
	if (p != 0) {
		for (int i = 0; i < numCall; i++)
			cout << "��" << numCall << "�ε���:" << str << "\n";
	}
}