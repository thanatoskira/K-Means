#include "newCommon.h"

//聚类中心类，用于记录聚类中心的各类信息
class Cluster{
public:
	Cluster(){
		this->Center = NULL;			//聚类中心点初始化NULL
		this->iNumMembers = 0;			//聚类成员数量初始化0
		this->NumDimensions = 0;		//聚类类型种类的数据量初始化0
		this->Precition = 1.0;			//聚类精度初始化1(不满足聚类精度)
	}

	strMyRecord *Center;				//记录聚类中心点
	list<strMyRecord *>MemberList;		//记录聚类成员链表
	unsigned int iNumMembers;			//记录聚类成员数量
	unsigned int NumDimensions;			//记录聚类类型的种类数量
	float Precition;					//记录聚类精度
};

//聚类树节点类
class ClusterNode{
public:
	//构造函数
	ClusterNode(){}
	ClusterNode(ClusterNode *pParentNode, int i, bool IsClusterOK, unsigned int IsLeaf, Cluster n_ClusterCenterNode, unsigned int id){
		this->n_ClusterNodeID = id;	//唯一标识ID
		this->n_ClusterNodeLevel = pParentNode->n_ClusterNodeLevel + 1;		//层级=父层级+1
		stringstream ss;
		ss<<i;
		this->strPath = pParentNode->strPath + "." + ss.str();
		this->pParentNode = pParentNode;		//记录父节点
		this->IsClusterOK = IsClusterOK;		//记录是否符合标准
		this->IsLeaf = IsLeaf;					//记录是否为叶子节点
		this->n_ClusterCenterNode = n_ClusterCenterNode;	//记录当前节点信息
		this->n_ChildCount = 0;					//当前节点孩子节点数量初始化为

		this->pParentNode->pChildNode[i] = this;//将当前节点加入父节点的pChildNode中
		pParentNode->n_ChildCount++;			//父节点的孩子节点个数+1

	}
	//计算该条记录到该节点中心的距离
	float CalCenterDistance(strMyRecord *pRecord);
	//递归函数, 在以节点为父亲的子树中,获得与数据记录距离最近的聚类节点
	ClusterNode * GetNearestCluseter(strMyRecord *pRecord);

public:
	unsigned int n_ClusterNodeID;				//聚类节点的唯一标识(ID)
	unsigned int n_ClusterNodeLevel;			//聚类节点的层级
	string strPath;								//聚类节点的路径
	ClusterNode *pParentNode;					//指向这个聚类节点的父节点的指针
	ClusterNode *pChildNode[MAXLABEL];			//指向这个聚类节点的孩子节点的指针
	bool IsClusterOK;							//聚合结果是否符合标准
	unsigned int IsLeaf;						//0:非叶子节点
												//1:叶子节点且聚类正常结束
	unsigned int n_ChildCount;					//记录孩子节点个数
	Cluster n_ClusterCenterNode;				//聚类节点的中心点信息
};

//聚类树类
class ClusterTree{
public:
	//构造函数
	ClusterTree(){
		this->pRootNode = new ClusterNode();
		this->pRootNode->n_ClusterNodeID = 0;					//rootID设置为0
		this->pRootNode->n_ClusterNodeLevel = 0;				//root层级设置为0
		this->pRootNode->strPath = "0";							//root路径设置为"0"
		this->pRootNode->pParentNode = NULL;					//root没有父节点设置为NULL
		this->pRootNode->IsClusterOK = false;					//root满足聚类精度
		this->pRootNode->IsLeaf = 0;							//root不为叶子节点
		this->pRootNode->n_ChildCount = 0;						//当前节点孩子节点数量初始化为
		this->TreeNumDimensions = 0;							//用于记录整个记录中的种类数量
		this->Count = 0;										//初始化为0
		this->RightLableCount = 0;								//初始化为0
		this->RightRate = 0;									//初始预测全错为0

		this->pRootNode->n_ClusterCenterNode.Center = NULL; 
		this->pRootNode->n_ClusterCenterNode.iNumMembers = 0;
		this->pRootNode->n_ClusterCenterNode.NumDimensions = 0;
		this->pRootNode->n_ClusterCenterNode.Precition = 0.0;
	}
	//打印聚类树
	void Print(ClusterNode *ChildNode, int **matrix);
	ClusterNode * FindNearestCluster(strMyRecord *pRecord);		//找到与给定记录距离最近的聚类节点
public:
	//根节点指针
	ClusterNode *pRootNode;	
	unsigned int TreeNumDimensions;					//记录聚类类型的种类数量
	unsigned int Count;								//记录总记录数量
	unsigned int RightLableCount;					//记录预测结果与原来类型相同的数量
	float RightRate;								//记录正确比率
};