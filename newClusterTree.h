#include "newCommon.h"

//���������࣬���ڼ�¼�������ĵĸ�����Ϣ
class Cluster{
public:
	Cluster(){
		this->Center = NULL;			//�������ĵ��ʼ��NULL
		this->iNumMembers = 0;			//�����Ա������ʼ��0
		this->NumDimensions = 0;		//���������������������ʼ��0
		this->Precition = 1.0;			//���ྫ�ȳ�ʼ��1(��������ྫ��)
	}

	strMyRecord *Center;				//��¼�������ĵ�
	list<strMyRecord *>MemberList;		//��¼�����Ա����
	unsigned int iNumMembers;			//��¼�����Ա����
	unsigned int NumDimensions;			//��¼�������͵���������
	float Precition;					//��¼���ྫ��
};

//�������ڵ���
class ClusterNode{
public:
	//���캯��
	ClusterNode(){}
	ClusterNode(ClusterNode *pParentNode, int i, bool IsClusterOK, unsigned int IsLeaf, Cluster n_ClusterCenterNode, unsigned int id){
		this->n_ClusterNodeID = id;	//Ψһ��ʶID
		this->n_ClusterNodeLevel = pParentNode->n_ClusterNodeLevel + 1;		//�㼶=���㼶+1
		stringstream ss;
		ss<<i;
		this->strPath = pParentNode->strPath + "." + ss.str();
		this->pParentNode = pParentNode;		//��¼���ڵ�
		this->IsClusterOK = IsClusterOK;		//��¼�Ƿ���ϱ�׼
		this->IsLeaf = IsLeaf;					//��¼�Ƿ�ΪҶ�ӽڵ�
		this->n_ClusterCenterNode = n_ClusterCenterNode;	//��¼��ǰ�ڵ���Ϣ
		this->n_ChildCount = 0;					//��ǰ�ڵ㺢�ӽڵ�������ʼ��Ϊ

		this->pParentNode->pChildNode[i] = this;//����ǰ�ڵ���븸�ڵ��pChildNode��
		pParentNode->n_ChildCount++;			//���ڵ�ĺ��ӽڵ����+1

	}
	//���������¼���ýڵ����ĵľ���
	float CalCenterDistance(strMyRecord *pRecord);
	//�ݹ麯��, ���Խڵ�Ϊ���׵�������,��������ݼ�¼��������ľ���ڵ�
	ClusterNode * GetNearestCluseter(strMyRecord *pRecord);

public:
	unsigned int n_ClusterNodeID;				//����ڵ��Ψһ��ʶ(ID)
	unsigned int n_ClusterNodeLevel;			//����ڵ�Ĳ㼶
	string strPath;								//����ڵ��·��
	ClusterNode *pParentNode;					//ָ���������ڵ�ĸ��ڵ��ָ��
	ClusterNode *pChildNode[MAXLABEL];			//ָ���������ڵ�ĺ��ӽڵ��ָ��
	bool IsClusterOK;							//�ۺϽ���Ƿ���ϱ�׼
	unsigned int IsLeaf;						//0:��Ҷ�ӽڵ�
												//1:Ҷ�ӽڵ��Ҿ�����������
	unsigned int n_ChildCount;					//��¼���ӽڵ����
	Cluster n_ClusterCenterNode;				//����ڵ�����ĵ���Ϣ
};

//��������
class ClusterTree{
public:
	//���캯��
	ClusterTree(){
		this->pRootNode = new ClusterNode();
		this->pRootNode->n_ClusterNodeID = 0;					//rootID����Ϊ0
		this->pRootNode->n_ClusterNodeLevel = 0;				//root�㼶����Ϊ0
		this->pRootNode->strPath = "0";							//root·������Ϊ"0"
		this->pRootNode->pParentNode = NULL;					//rootû�и��ڵ�����ΪNULL
		this->pRootNode->IsClusterOK = false;					//root������ྫ��
		this->pRootNode->IsLeaf = 0;							//root��ΪҶ�ӽڵ�
		this->pRootNode->n_ChildCount = 0;						//��ǰ�ڵ㺢�ӽڵ�������ʼ��Ϊ
		this->TreeNumDimensions = 0;							//���ڼ�¼������¼�е���������
		this->Count = 0;										//��ʼ��Ϊ0
		this->RightLableCount = 0;								//��ʼ��Ϊ0
		this->RightRate = 0;									//��ʼԤ��ȫ��Ϊ0

		this->pRootNode->n_ClusterCenterNode.Center = NULL; 
		this->pRootNode->n_ClusterCenterNode.iNumMembers = 0;
		this->pRootNode->n_ClusterCenterNode.NumDimensions = 0;
		this->pRootNode->n_ClusterCenterNode.Precition = 0.0;
	}
	//��ӡ������
	void Print(ClusterNode *ChildNode, int **matrix);
	ClusterNode * FindNearestCluster(strMyRecord *pRecord);		//�ҵ��������¼��������ľ���ڵ�
public:
	//���ڵ�ָ��
	ClusterNode *pRootNode;	
	unsigned int TreeNumDimensions;					//��¼�������͵���������
	unsigned int Count;								//��¼�ܼ�¼����
	unsigned int RightLableCount;					//��¼Ԥ������ԭ��������ͬ������
	float RightRate;								//��¼��ȷ����
};