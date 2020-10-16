#include "Header.h"

int _maxDep;
int _numDim;
int _numCat;
int _numTryDim;

Node *NewNode()
{
	Node *ret = new Node;
	*ret = Node();
	return ret;
}

double GiniIndex(const vector<Vec> &sample)
{
	if(!sample.size()) return 1;
	double G = 1;
	vector<int> buk(_numCat, 0);
	for(auto s : sample) buk[s.category]++;
	for(auto x : buk) G -= 1.0*x*x/sample.size()/sample.size();
	return G;
}

pair<int, double> TryDivide(const vector<Vec> &sample)
{
	double minG = +1e20;
	int bestDim;
	double bestVal;
	for(int i=0; i<_numTryDim; i++)
	{
		int d = rand() % _numDim;
		for(auto s : sample)
		{
			vector<Vec> ls, rs;
			for(auto t : sample)
			{
				if(t.x[d] < s.x[d]) ls.push_back(t);
				else rs.push_back(t);
			}
			double Gl = GiniIndex(ls) * ls.size() / sample.size();
			double Gr = GiniIndex(rs) * rs.size() / sample.size();
			double after = Gl+Gr;
			if(after < minG)
			{
				minG = after;
				bestDim = d;
				bestVal = s.x[d];
			}
		}
	}
	return make_pair(bestDim, bestVal);
}

Node *BuildTree(vector<Vec> sample, int dep)
{
	if(!sample.size()) return NULL;
	if(dep > _maxDep) return NULL;
	
	Node *cur = NewNode();
	auto divide = TryDivide(sample);
	cur->dDim = divide.first;
	cur->dVal = divide.second;
	vector<Vec> ls, rs;
	for(auto s : sample)
		if(s.x[cur->dDim] < cur->dVal)
			ls.push_back(s);
		else rs.push_back(s);

	if(ls.size() && rs.size())
	{
		cur->son[0] = BuildTree(ls, dep+1);
		cur->son[1] = BuildTree(rs, dep+1);
	}

	if(cur->isLeaf())
	{
		vector<int> buk(_numCat, 0);
		for(auto s : sample) buk[s.category] += 1;
		cur->ans = max_element(buk.begin(), buk.end()) - buk.begin();
	}
	return cur;
}

int Classify(Node *a, Vec sample)
{
	if(a->isLeaf()) return a->ans;
	else if(sample.x[a->dDim] < a->dVal) return Classify(a->son[0], sample);
	else return Classify(a->son[1], sample);
}

Bag Bagging(vector<Vec> sample, int numTre, int numSam, int maxDep, int numDim, int numCat, int numTryDim)
{
	Bag ret;
	_maxDep = ret.maxDep = maxDep;
	_numDim = ret.numDim = numDim;
	_numCat = ret.numCat = numCat;
	_numTryDim = ret.numTryDim = numTryDim;
	for(int i=0; i<numTre; i++)
	{
		vector<Vec> tmpSample;
		for(int j=0; j<numSam; j++)
			tmpSample.push_back(sample[rand()%sample.size()]);

		ret.root.push_back(BuildTree(tmpSample, 0));
	}
	return ret;
}

int BagClassify(Bag bag, Vec sample)
{
	_maxDep = bag.maxDep;
	_numDim = bag.numDim;
	_numCat = bag.numCat;
	_numTryDim = bag.numTryDim;
	vector<int> buk(_numCat, 0);
	for(auto r : bag.root) buk[Classify(r, sample)]++;
	return max_element(buk.begin(), buk.end()) - buk.begin();
}