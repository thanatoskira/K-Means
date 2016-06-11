#include "newClusterTree.h"

//KMeans�㷨��
class CKMeans{
public:
	//���캯��1���������ɾ������ĸ��ڵ�
	CKMeans(ClusterTree *pTree, int KmeansID, int Level){
		this->m_iNumClusters = 0;					//������������ʼ��0
		this->m_ClusterLevel = Level;				//������������Ĳ��
		this->m_KmeansID = KmeansID;				//CKMeans�����ID��
		this->pClusterTree = pTree;
		this->pSelfClusterNode = pTree->pRootNode;
	}
	//���캯��2���������ɾ��������ӽڵ�
	CKMeans(ClusterNode *pSelf, int KmeansID, int Level, list<strMyRecord *> *pDataList){
		this->m_KmeansID = KmeansID;
		this->m_ClusterLevel = Level;
		this->m_RecordsList = *pDataList;			//����һ�ξ������в�������ྫ�ȵľ�����Ϊ��ξ������������
		this->pSelfClusterNode = pSelf;
		this->pClusterTree = NULL;
	}
	//��ȡ��������Ԥ����ļ�¼
	bool ReadTrainingRecords();
	//K-means�㷨���������
	void RunKMeans(int Kvalue);
	//K-means�㷨�ĵ�һ��:��n�����ݶ�������ѡ��k��������Ϊ��ʼ��������
	void InitClusters(unsigned int NumClusters);
	//K-means�㷨�ĵڶ���:��ÿ������������֮��������ľ���
	void DistributeSamples();
	//�ҵ���������ݶ��������һ������
	int FindClosestCluster(strMyRecord *pRecord);
	//����ָ�����ݶ�����������ĵ�ŷ����þ���
	float CalcEucNorm(strMyRecord *pRecord, int id);
	//K-means�㷨�ĵ�����:���¼���ÿ�����������
	bool CalcNewClustCenters();
	//��KMeans�㷨����֮�󣬲�ѯ���о���ı�ǩ��
	void GetClustersLabel();
	//�������һ���еķ����Ƿ����
	bool IsClusterOK(int i);
	//�ж��Ƿ�����ݹ����
	bool IsStopRecursion(int i);
	
private:
	//�жϸ�����¼��֮ǰ�ľ��������Ƿ���ȫ��ͬ
	bool IsSameAsCluster(int i, strMyRecord *pRecord);


private:
	FILE *pInFile;							//�����ļ���ָ��
	list<strMyRecord *> m_RecordsList;		//���ݼ�¼����
	unsigned int m_iNumClusters;			//����������(��Kֵ)
	Cluster m_Cluster[MAXLABEL];			//��������
	int m_ClusterLevel;						//������������Ĳ��
	int m_KmeansID;							//CKMeans�����ID��
	ClusterTree *pClusterTree;				//��������ָ��
	ClusterNode *pSelfClusterNode;			//�뱾CKmeans������صľ���ڵ��ָ��
};
