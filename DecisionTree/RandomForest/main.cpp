#include "Header.h"

int numCategory;
int numDimension;
vector<Vec> samples;

vector<Vec> ReadFile(const char *filePath)
{
	numCategory = 0;
	string line, number;
	vector<Vec> data;
	ifstream fin(filePath);

	while(getline(fin, line))
	{
		Vec lineVec;
		stringstream lineStream(line);
		while(getline(lineStream, number, ',')) lineVec.x.push_back(stod(number));
		assert(lineVec.x.size());
		lineVec.category = int(lineVec.x.back() + 0.5);
		lineVec.x.pop_back();
		numCategory = max(numCategory, lineVec.category+1);
		data.push_back(lineVec);
	}

	numDimension = data[0].x.size();
	for(int i=0; i<data.size(); i++) assert(data[i].x.size() == numDimension);
	return data;
}

pair<vector<Vec>, vector<Vec> > DivideDataSet(vector<Vec> samples, int train)
{
	assert(train <= samples.size());
	vector<Vec> trainData, testData;
	random_shuffle(samples.begin(), samples.end());
	for(int i=0; i<train; i++) trainData.push_back(samples[i]);
	for(int i=train; i<samples.size(); i++) testData.push_back(samples[i]);
	return make_pair(trainData, testData);
}

double TestAccuracy(Bag bag, vector<Vec> samples)
{
	int cor = 0;
	for(auto s : samples) cor += (s.category == BagClassify(bag, s));
	return 1.0 * cor / samples.size();
}

double AverageAccuracyOfParameters(vector<Vec> samples, int numTre, int maxDep, double partial, int numDim, int numCat, int numTryDim, int numRnd)
{
	double avg = 0;
	for(int i=0; i<numRnd; i++)
	{
		auto dataSet = DivideDataSet(samples, int(samples.size() * PartialTrain));
		Bag bag = Bagging(dataSet.first, 20, int(dataSet.first.size() * partial), maxDep, numDimension, numCategory, 7);
		avg += TestAccuracy(bag, dataSet.second);
	}
	return avg / numRnd;
}

void FindBestCombination(vector<Vec> samples,
						int numTre, int maxDep, int numDim, int numCat, int maxTryDim, int numStep,
						int iniDep, int iniTryDim, int &bestDep, int &bestTry, double &bestPartial)
{
	int dep = iniDep, tryDim = iniTryDim;
	double partial = 1.0;
	double step = 0.9;
	assert(dep <= maxDep && tryDim <= maxTryDim);
	double curAccuracy = AverageAccuracyOfParameters(samples, numTre, dep, partial, numDim, numCat, tryDim, 20);
	for(int i=0; i<numStep; i++)
	{
		printf("\tRound %d : Current Accuracy is %.4lf\n", i, curAccuracy);
		double nxtAccuracy = AverageAccuracyOfParameters(samples, numTre, dep, partial * step, numDim, numCat, tryDim, 20);
		if(nxtAccuracy > curAccuracy)
		{
			curAccuracy = nxtAccuracy;
			partial *= step;
		}
		for(int dDep=-1; dDep<=1; dDep++)
		{
			for(int dTry=-1; dTry<=1; dTry++)
			{
				int nDep = dep + dDep;
				int nTry = tryDim + dTry;
				if(dDep && dTry && 1<=nDep && nDep<=maxDep && 1<=nTry && nTry<=maxTryDim)
				{
					double nxtAccuracy = AverageAccuracyOfParameters(samples, numTre, nDep, partial, numDim, numCat, nTry, 20);
					if(nxtAccuracy > curAccuracy)
					{
						curAccuracy = nxtAccuracy;
						dep = nDep;
						tryDim = nTry;
					}
				}
			}
		}
	}
	bestDep = dep;
	bestTry = tryDim;
	bestPartial = partial;
}

int main()
{
	srand(time(0));
	samples = ReadFile("./data/sonar.csv");
	printf("Finding best combinatino of parameters.\n");
	int bestDep, bestTry;
	double bestPartial;
	FindBestCombination(samples, 20, 30, numDimension, numCategory, numDimension, 3, 6, 7, bestDep, bestTry, bestPartial);
	printf("Average accuracy: %.4lf\n", AverageAccuracyOfParameters(samples, 20, bestDep, bestPartial, numDimension, numCategory, bestTry, 40));
	printf("Parameters: TryDim = %d, MaxDep = %d, Partial = %.2lf .\n", bestTry, bestDep, bestPartial);
	return 0;
}