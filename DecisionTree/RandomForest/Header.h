#ifndef DT
#define DT

#include <iostream>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <assert.h>

using namespace std;

struct Vec
{
	vector<double> x;
	int category;
};

struct Node
{
	Node *son[2];
	int dDim;
	double dVal;
	int ans;

	Node ()
	{
		son[0] = NULL;
		son[1] = NULL;
	}

	bool isLeaf() const
	{
		return son[0]==NULL && son[1]==NULL;
	}
};

struct Bag
{
	vector<Node *> root;
	int maxDep;
	int numDim;
	int numCat;
	int numTryDim;
};

Node *NewNode();
double GiniIndex(const vector<Vec> &sample);
pair<int, double> TryDivide(const vector<Vec> &sample);
Node *BuildTree(vector<Vec> sample, int dep);
int Classify(Node *a, Vec sample);
Bag Bagging(vector<Vec> sample, int numTre, int numSam, int maxDep, int numDim, int numCat, int numTryDim);
int BagClassify(Bag bag, Vec sample);

const double PartialTrain = 0.8;

#endif