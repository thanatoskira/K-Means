#include "newClusterTree.h"

//KMeans算法类
class CKMeans{
public:
	//构造函数1，用于生成聚类树的根节点
	CKMeans(ClusterTree *pTree, int KmeansID, int Level){
		this->m_iNumClusters = 0;					//聚类的类别数初始化0
		this->m_ClusterLevel = Level;				//聚类对象所处的层次
		this->m_KmeansID = KmeansID;				//CKMeans对象的ID号
		this->pClusterTree = pTree;
		this->pSelfClusterNode = pTree->pRootNode;
	}
	//构造函数2，用于生成聚类树的子节点
	CKMeans(ClusterNode *pSelf, int KmeansID, int Level, list<strMyRecord *> *pDataList){
		this->m_KmeansID = KmeansID;
		this->m_ClusterLevel = Level;
		this->m_RecordsList = *pDataList;			//将上一次聚类结果中不满足聚类精度的聚类作为这次聚类的输入数据
		this->pSelfClusterNode = pSelf;
		this->pClusterTree = NULL;
	}
	//读取经过数据预处理的记录
	bool ReadTrainingRecords();
	//K-means算法的总体过程
	void RunKMeans(int Kvalue);
	//K-means算法的第一步:从n个数据对象任意选择k个对象作为初始聚类中心
	void InitClusters(unsigned int NumClusters);
	//K-means算法的第二步:把每个对象分配给与之距离最近的聚类
	void DistributeSamples();
	//找到离给定数据对象最近的一个聚类
	int FindClosestCluster(strMyRecord *pRecord);
	//计算指定数据对象与聚类中心的欧几里得距离
	float CalcEucNorm(strMyRecord *pRecord, int id);
	//K-means算法的第三步:重新计算每个聚类的中心
	bool CalcNewClustCenters();
	//在KMeans算法运行之后，查询所有聚类的标签数
	void GetClustersLabel();
	//检查聚类后一类中的分类是否合理
	bool IsClusterOK(int i);
	//判断是否结束递归过程
	bool IsStopRecursion(int i);
	
private:
	//判断该条记录与之前的聚类中心是否完全相同
	bool IsSameAsCluster(int i, strMyRecord *pRecord);


private:
	FILE *pInFile;							//输入文件的指针
	list<strMyRecord *> m_RecordsList;		//数据记录链表
	unsigned int m_iNumClusters;			//聚类的类别数(即K值)
	Cluster m_Cluster[MAXLABEL];			//子类数组
	int m_ClusterLevel;						//聚类对象所处的层次
	int m_KmeansID;							//CKMeans对象的ID号
	ClusterTree *pClusterTree;				//聚类树的指针
	ClusterNode *pSelfClusterNode;			//与本CKmeans对象相关的聚类节点的指针
};
