# 聚类算法

## 作业内容

本次作业实现了两种基础聚类算法：

- K-Means Clustering
  - `void K_Means_Clustering(int numClusters, int numFeatures, int numTexts, int numCategories, Text *texts, Cluster *clusters)`
  - 给定需要的簇数`numClusters`、样本的特征数`numFeatures`、样本的数量`numTexts`、人工标记的簇数`numCategories`、样本集合`*texts`、分类结果的保存指针`*clusters`，函数将会求解聚类，并保存在`*clusters`中
- Bisectional K-Means Clustering
  - `void Bisectional_K_Means_Clustering(int numClusters, int numFeatures, int numTexts, int numCategories, Text *texts, Cluster *clusters)`
  - 参数的意义同上

以及两种评价指标：

- Purity
  - `double Purity(Text *text, int numTexts, Cluster *cluster, int numCluster, int numCategories)`
  - 参数解读同上，函数将会计算出所分的每一个类和人工标定的类的纯度，并用 `KM` 算法求解最大匹配作为聚类的总体纯度。
- F-Measure
  - `double FMeasure(Text *text, int numTexts, Cluster *cluster, int numCluster, int numCategories)`
  - 参数解读同上，函数将会计算出所分的每一个类和人工标定的类的F值，并用 `KM` 算法求解最大匹配作为聚类的总体F值。

其中部分代码来自下发的模板，并对模块之间的组织结构进行了优化，使得每个聚类算法可以当成一个只依赖于输入的黑盒进行使用，便于重用和扩展。

## 实现过程

在实现代码的过程中，遇到了一个非常神奇的错误：一个数组的大小设置过小，使得中途写入此数组的时候发生了越界。然而越界并没有使得程序直接段错误，而程序仍然可以继续运行。当定义了越界数组的函数准备返回时，程序会莫名其妙的跳到第80行继续运行，而不是按计划返回，在执行第80行时，系统终于检测到了问题，程序发生段错误。思考其原因，可能是写入那个过小的数组时，修改了程序的栈空间，而栈空间直接关系到了函数的调用，因此再函数返回时才会发生错误。在这个问题上花费了特别长的时间进行调试，为了避免下次遇到同类问题依旧不知所措，我不禁好奇这类问题应该如何调试呢？

## 算法性能对比

为了测试两种聚类算法的性能，给定输入数据，让两个算法分别运行100次，每10次求最大F值，再求这些最大值的平均数：

- K-Means Clustering: F-Means = 0.8173
- Bisectional K-Means Clustering: F-Means = 0.8434

通过对比发现，Bisectional K-Means Clustering 算法略优于 K-Means Clustering 算法。

# 分类树

## 作业内容

本次作业实现了一个 Random-Forest 分类树，并与给出的 Decision-Tree 分类树做了简单对比。

此分类树有如下几个接口：

- `Node *BuildTree(vector<Vec> sample, int dep)`

  - 给定标定好的数据集 `sample` ，以及树的初始深度 `dep` ，算法将会递归构建一棵分类树，并返回根指针。
- `int Classify(Node *a, Vec sample)`

  - 给定分类树的根指针，以及需要分类的数据 `sample` ，分类树将会返回数据所属的类别。
- `Bag Bagging(vector<Vec> sample, int numTre, int numSam, int maxDep, int numDim, int numCat, int numTryDim)`
- 给定标定好的数据集 `sample` ，需要构建的树的数量 `numTre`，选择构建每棵树的数据的随机抽样次数 `numSam`，树的最大深度 `maxDep` ，数据集的维度数 `numDim`，数据集的标定类数 `numCat` ，每一层分类用的维度数 `numTryDim`，算法将会返回一个结构体，这个结构体里面保存了通过 `Bagging` 法构建的 `numTre` 棵分类树。
- `void FindBestCombination(vector<Vec> samples, int numTre, int maxDep, int numDim, int numCat, int maxTryDim, int numStep, int iniDep, int iniTryDim, int &bestDep, int &bestTry, double &bestPartial)`
  - 给定数据集，以及参数的范围，该函数将会通过爬山算法（随机梯度下降？）找到最佳的参数组合并返回，使得在该参数组合下，分类树的准确率最高。

## 实现过程

在实现过程中没有遇到太多问题，唯独有一次把 `double` 型数据定义为了 `int` ，导致分类树的准确率为 $45\%$ ，甚至没有随机猜的准确率高。

在评判参数组合的优秀程度的过程中，算法是这样实现的，不知道是否合理：

- 根据参数组合构建 $20$ 组分类森林，每一组分类森林包含 $20$ 棵分类树。
- 对于每一组分类森林，都分别将数据集打乱，其中 $80\%$ 的数据于训练分类树， $20\%$ 的数据用于打分。
- 参数组合的得分即为 $20$ 组分类森林的得分的平均值。

## 算法性能

对于 $20$ 棵树，最大深度 $6$，每一层分类用维度 $7$ 的结果，得到的一组分类树的准确率为：$77.86\%$

对于给定的“声呐数据集”，通过爬山算法找到的最优参数组合为：

- 每层用于分类的维度数为 $4$
- 最大深度为 $5$
- 用于构建每棵树的数据为有放回抽取 $0.9$ 倍数据集大小个数据后得到的

在此组合下的准确率为：$80.12\%$

