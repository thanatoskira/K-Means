#include "newKmeans.h"

//��ȡѵ������
bool CKMeans::ReadTrainingRecords(){
	//�������ļ�
	char *filename = ".\\corrected_datastreat";
	if(!(this->pInFile = fopen(filename, "r"))){
		cout<<filename<<" Can not Open!"<<endl;
		exit(1);
	}

	cout<<"\nStart reading records from "<<filename<<"..."<<endl;
	outFile<<"Start reading records from "<<filename<<"...\n"<<endl;

	char *buff = new char[MAX_BUF_SIZE];						//����ÿ��ʱ�Ļ�������С
	unsigned int count = 0;
	while(fgets(buff, MAX_BUF_SIZE, this->pInFile)){			//���ļ��ж�ȡһ������
		buff[strlen(buff)-1] = '\0';							//ɾ���ַ�������'\n' !�˴����ܴ������⣬\r\n
		//cout<<buff<<endl;
		vector <char *> ALLChar;								//ALLChar��Ų�ֺ�ĸ����ַ���
		Split(buff, DELIM, &ALLChar);							//����ַ���

		strMyRecord *tmpMyRecord = new strMyRecord();			//Ҫ���ṹ������ڴ�ռ䣬��Ȼ��ֵ�ᷢ������

		//����ת����������¼�����tmpMyRecord��������
		tmpMyRecord->iProtocolType = GetPosition(ALLChar[0], ProtocolType);
		tmpMyRecord->iService = GetPosition(ALLChar[1], Service);
		tmpMyRecord->iStatusFlag = GetPosition(ALLChar[2], StatusFlag);
		tmpMyRecord->iSrcBytes = Char2Int(ALLChar[3]);
		tmpMyRecord->iDestBytes = Char2Int(ALLChar[4]);
		tmpMyRecord->iFailedLogins = Char2Int(ALLChar[5]);
		tmpMyRecord->iNumofRoot = Char2Int(ALLChar[6]);
		tmpMyRecord->iCount = Char2Int(ALLChar[7]);
		tmpMyRecord->iSrvCount = Char2Int(ALLChar[8]);
		tmpMyRecord->iRerrorRate = Char2Byte(ALLChar[9]);
		tmpMyRecord->iSameSrvRate = Char2Byte(ALLChar[10]);
		tmpMyRecord->iDiffSrvRate = Char2Byte(ALLChar[11]);
		tmpMyRecord->iDstHostSrvCount = Char2Int(ALLChar[12]);
		tmpMyRecord->iDstHostSameSrvRate = Char2Byte(ALLChar[13]);
		tmpMyRecord->iDstHostDiffSrvRate = Char2Byte(ALLChar[14]);
		tmpMyRecord->iDstHostSameSrcPortRate = Char2Byte(ALLChar[15]);
		tmpMyRecord->iDstHostSrvDiffHostRate = Char2Byte(ALLChar[16]);
		tmpMyRecord->iDstHostSrvSerrorRate = Char2Byte(ALLChar[17]);
		tmpMyRecord->iLabel = GetPosition(ALLChar[18], Label);

		//�������ļ�¼����������
		this->m_RecordsList.push_back(tmpMyRecord);

		this->pClusterTree->Count++;
		if(this->pClusterTree->Count % 1000 == 0){
			cout<<"----------"<<this->pClusterTree->Count<<" lines have written----------"<<endl;
		}   
	}

	//��¼Label�������
	this->pClusterTree->TreeNumDimensions += Label.size();

	cout<<"\nALL Records have read! Total "<<this->pClusterTree->Count<<" records!\n"<<endl;
	outFile<<"ALL Records have read! Total "<<this->pClusterTree->Count<<" records!\n"<<endl;
	fclose(this->pInFile);
	return true;
}

//�жϵ�ǰ��¼��ֵ��֮ǰ�ľ������ĵ���ֵ�Ƿ���ͬ
bool CKMeans::IsSameAsCluster(int i, strMyRecord * pRecord){
	for(int j = 0; j < i; ++j){
		//if(*this->m_Cluster[j].Center == pRecord){						//���ݼ�¼�и�������ֵ�Ĳ�ͬ��ѡ��
		if(this->m_Cluster[j].Center->iLabel == pRecord->iLabel){			//���ݼ�¼��iLabel�Ĳ�ͬ��ѡ��
			return true; 
		}
	}
	return false;
}

void CKMeans::InitClusters(unsigned int NumClusters){					//��ʼ����������
	list<strMyRecord *>::iterator RecdListIter;							//��¼����ĵ�����

	this->m_iNumClusters = NumClusters;									//����������
	RecdListIter = this->m_RecordsList.begin();							//��List������ָ������ͷ��

	strMyRecord * RecdCenter = new strMyRecord[this->m_iNumClusters];	//��¼ÿ������ƽ��ֵ
	
	int *RecordCount = new int[this->m_iNumClusters];					//��¼ÿ����������
	int *CountMap = new int[this->m_iNumClusters];						//ӳ���������ӳ����е��±�

	for(int i = 0; i < this->m_iNumClusters; ++i){	//��ʼ��ֵ
		RecordCount[i] = 0;
		CountMap[i] = -1;
	}
	
	for(strMyRecord *tmpRecord = NULL; RecdListIter != this->m_RecordsList.end(); ++RecdListIter){
		tmpRecord = (* RecdListIter);

		int index = -1;	//��¼������RecordCount���±�
		int Negetive = -1;	//���ڽ����Ͳ���ӳ�����
		for(int i = this->m_iNumClusters-1; i >= 0; --i){
			if(CountMap[i] == -1){
				Negetive = i;
				continue;
			}
			if(CountMap[i] == (int)tmpRecord->iLabel){	//���ӳ����д��ڸ�����
				index = i;	//����������RecordCount�е��±귵��
				break;
			}
		}
		if(index == -1){	//˵��ӳ���CountMap�в����ڸ�����
			CountMap[Negetive] = (int)tmpRecord->iLabel;	//�������Ͳ��뵽ӳ�����Ϊ-1��λ��
			RecdCenter[Negetive] += tmpRecord;	//�ۼ�
			RecordCount[Negetive]++;	//Ȼ������������1
		}
		else{	//˵�����ʹ�����ӳ�����
			RecdCenter[index] += tmpRecord;	//�ۼ�
			RecordCount[index]++;	//���������RecordCount�е�ֵ+1
		}
	}
	for(i = 0; i < this->m_iNumClusters; ++i){
		RecdCenter[i] /= RecordCount[i];
		this->m_Cluster[i].Center = &RecdCenter[i];
	}
}

//�������ǰ��¼����id���������ĵ�ŷ�Ͼ���
//Ϊ�˼��㷽�㣬ȥ�������Ų���
float CKMeans::CalcEucNorm(strMyRecord *pRecord, int id){
	double fDist;
	fDist = 0;

	fDist += pow((pRecord->iProtocolType - this->m_Cluster[id].Center->iProtocolType), 2);
	fDist += pow((pRecord->iService - this->m_Cluster[id].Center->iService), 2);
	fDist += pow((pRecord->iStatusFlag - this->m_Cluster[id].Center->iStatusFlag), 2);
	fDist += pow((pRecord->iSrcBytes - this->m_Cluster[id].Center->iSrcBytes), 2);
	fDist += pow((pRecord->iDestBytes - this->m_Cluster[id].Center->iDestBytes), 2);
	fDist += pow((pRecord->iFailedLogins - this->m_Cluster[id].Center->iFailedLogins), 2);
	fDist += pow((pRecord->iNumofRoot - this->m_Cluster[id].Center->iNumofRoot), 2);
	fDist += pow((pRecord->iCount - this->m_Cluster[id].Center->iCount),2);
	fDist += pow((pRecord->iSrvCount - this->m_Cluster[id].Center->iSrvCount), 2);
	fDist += pow((pRecord->iRerrorRate - this->m_Cluster[id].Center->iRerrorRate), 2);
	fDist += pow((pRecord->iSameSrvRate - this->m_Cluster[id].Center->iSameSrvRate),2);
	fDist += pow((pRecord->iDiffSrvRate - this->m_Cluster[id].Center->iDiffSrvRate),2);
	fDist += pow((pRecord->iDstHostSrvCount - this->m_Cluster[id].Center->iDstHostSrvCount), 2);
	fDist += pow((pRecord->iDstHostSameSrvRate - this->m_Cluster[id].Center->iDstHostSameSrvRate), 2);
	fDist += pow((pRecord->iDstHostDiffSrvRate - this->m_Cluster[id].Center->iDstHostDiffSrvRate), 2);
	fDist += pow((pRecord->iDstHostSameSrcPortRate - this->m_Cluster[id].Center->iDstHostSameSrcPortRate), 2);
	fDist += pow((pRecord->iDstHostSrvDiffHostRate - this->m_Cluster[id].Center->iDstHostSrvDiffHostRate) ,2);
	fDist += pow((pRecord->iDstHostSrvSerrorRate - this->m_Cluster[id].Center->iDstHostSrvSerrorRate), 2);
	//fDist += pow((pRecord->iLabel - this->m_Cluster[id].Center->iLabel), 2);
	return fDist;
}

//�ҳ��������¼����ľ������ĵ�ID
int CKMeans::FindClosestCluster(strMyRecord *pRecord){
	int MinID = 0;											//��¼��СID
	float MinDst = this->CalcEucNorm(pRecord, 0);			//��¼��СID�ľ���
	for(int id = 1; id < this->m_iNumClusters; ++id){		//���μ���ü�¼��ÿ���������ĵľ���
		float tmp = this->CalcEucNorm(pRecord, id);
		if(tmp < MinDst){
			MinDst = tmp;									//������С����
			MinID = id; 									//������СID
		}
	}
	return MinID;
}

void CKMeans::DistributeSamples(){									//�ҵ�������¼����ľ������ĵ�ID
	for(int i = 0 ; i < this->m_iNumClusters; ++i){					//������о���ĳ�Ա�б�
		this->m_Cluster[i].MemberList.clear();
		this->m_Cluster[i].iNumMembers = 0;
	}
	
	list<strMyRecord *>::iterator RecdListIter;						//������RecdListiterָ������m_RecordsList��ͷ��
	RecdListIter = this->m_RecordsList.begin();

	for(strMyRecord *tmpRecord = NULL; RecdListIter != this->m_RecordsList.end(); ++RecdListIter){
		tmpRecord = (* RecdListIter);
		int MinID = FindClosestCluster(tmpRecord);					//�ҳ��������¼����ľ������ĵ�ID
		this->m_Cluster[MinID].MemberList.push_back(tmpRecord);		//��������¼���뵽��Ӧ�������ĵĳ�Ա������
		this->m_Cluster[MinID].iNumMembers++;						//��Ӧ�������ĳ�Ա������1
	}
}

bool CKMeans::CalcNewClustCenters(){						//���¼����������
	bool Judge = false;										//�����ж��¾��������Ƿ�;ɾ���������ͬ
	for(int i = 0; i < this->m_iNumClusters; ++i){
		if(this->m_Cluster[i].iNumMembers == 0)			//�����ǰ��������Ϊ���������˴��ж�
			continue;

		strMyRecord *TempCenter = new strMyRecord();		//����ʱ��������TempCenter��ÿһ�����Զ�Ϊ����0	

		list<strMyRecord *>::iterator RecdListIter = this->m_Cluster[i].MemberList.begin();		//��¼����ĵ�����
		
		for(strMyRecord *pRecord = NULL; RecdListIter != this->m_Cluster[i].MemberList.end(); ++RecdListIter){
			pRecord = (*RecdListIter); 

			*TempCenter += pRecord;							//�������е�ÿһ����Ա�ۼ�
		}
		
		*TempCenter /= this->m_Cluster[i].iNumMembers;		//��TempCenter��ÿһ�����Գ��Ծ����Ա���������ƽ��ֵ

		if(*TempCenter!= this->m_Cluster[i].Center){		//�ж��¾���������ԭ���ľ��������Ƿ����
			this->m_Cluster[i].Center = TempCenter;			//������򽫾������ĸ�ֵ��Ϊ�µľ�������
			Judge = true;									//��Judge��Ϊtrue
		}
	}														//�����еľ��඼�ж����µľ���������֮ǰ�ľ��������Ƿ���ͬ
	if(Judge)												//��JudgeΪtrueʱ,˵������ĳ��������¾���������ԭ���ľ������Ĳ���ͬ
		return false;
	return true;											//������trueʱ, ˵���µľ���������֮ǰ�ľ���������ͬ
}

bool CKMeans::IsClusterOK(int i){							//�жϾ���i�Ƿ���Ͼ��ྫ��Ҫ��
	unsigned int vLabel[MAXLABEL] = {0};					//��¼��ǰ�����е�Label���༰����
	int j;
	list<strMyRecord *>::iterator RecdListIter = this->m_Cluster[i].MemberList.begin();		//ָ��ǰ��������

	//������ǰ��������
	for(strMyRecord *tmpRecord = NULL; RecdListIter != this->m_Cluster[i].MemberList.end(); ++RecdListIter){
		tmpRecord = (* RecdListIter);
		vLabel[tmpRecord->iLabel]++;
	}
	//����Ϊͳ�ƾ�����Label���༰����, ����vLabel������

	this->m_Cluster[i].NumDimensions = 0;			//�þ�����������
	unsigned int MasterLabel = 0, index = 0;
	for(j = 0; j < MAXLABEL; ++j){
		if(vLabel[j] != 0)
			this->m_Cluster[i].NumDimensions++;		//����������1
		if(vLabel[j] > MasterLabel){				//��¼������
			MasterLabel = vLabel[j];
			index = j;
		}
	}

	this->m_Cluster[i].Center->iLabel = index;		//�����������Ը�ֵ���������ĵ�
	
	if(this->m_Cluster[i].NumDimensions == 0){
		return true;
	}

	//�жϾ��ྫ���Ƿ�С��0.3
	if((this->m_Cluster[i].Precition = (float)(this->m_Cluster[i].iNumMembers - vLabel[index]) / (float)this->m_Cluster[i].iNumMembers) < 1/(float)this->m_Cluster[i].NumDimensions){
		return true;
	}
	return false;
}

//�Բ�������ྫ�ȵľ������ݹ���о������
bool CKMeans::IsStopRecursion(int i){
	ID++;
	ClusterNode *pNode = new ClusterNode(this->pSelfClusterNode, i, false, 0, this->m_Cluster[i], ID);	//����һ������ڵ�
	
	//����һ���µľ������ӽڵ�
	CKMeans *nckmeans = new CKMeans(pNode, ID, this->m_ClusterLevel+1, &this->m_Cluster[i].MemberList);
	nckmeans->RunKMeans(this->m_Cluster[i].NumDimensions); 
	return true;
}

void CKMeans::RunKMeans(int Kvalue){								//����KMeans�㷨
	bool IsFinish = false;											//�����жϾ����Ƿ����
 	this->InitClusters(Kvalue);										//��ʼ��5����������

	while(!IsFinish){												//�жϾ����Ƿ����
		this->DistributeSamples();									//����¼���������ľ�������
		IsFinish = this->CalcNewClustCenters();						//�����µľ�������, ������о����������������Ķ���ͬ, ���IsFinish����Ϊtrue
	}
	for(int i = 0; i < Kvalue; ++i){								//�����жϸ��������Ƿ�������ྫ��0.1
		if(!this->IsClusterOK(i)){
			this->IsStopRecursion(i);								//��������㾫��, ���������
		}else{
			ID++;
			ClusterNode *tmpNode = new ClusterNode(this->pSelfClusterNode, i, true, 1, this->m_Cluster[i], ID);
		}
	}
}

//���������
void CKMeans::GetClustersLabel(){
	strMyRecord *tmpRecord = NULL;
	list<strMyRecord *>::iterator RecdListIter;
	
	//����������
	for(int i = 0; i < m_iNumClusters; ++i){
		RecdListIter = this->m_Cluster[i].MemberList.begin();
		cout<<"�� "<<i<<" ��:"<<Label[this->m_Cluster[i].Center->iLabel]<<"\n\t";
		for(strMyRecord *tmpRecord = NULL; RecdListIter != this->m_Cluster[i].MemberList.end(); ++RecdListIter){
			tmpRecord = (*RecdListIter);
			cout<<Label[tmpRecord->iLabel]<<"->";
		}
		cout<<endl<<endl;
	}
	cout<<"\n************************************************\n\n";
}

//����������ṹ
void ClusterTree::Print(ClusterNode *ChildNode, int **matrix){
	if(ChildNode->IsLeaf){
		list<strMyRecord *>::iterator RecdListIter;								//��������ĵ�����
		RecdListIter = ChildNode->n_ClusterCenterNode.MemberList.begin();		//������ͷ��ʼ
		unsigned int count = 0;													//����Path
		for(strMyRecord * tmp = NULL;RecdListIter != ChildNode->n_ClusterCenterNode.MemberList.end(); ++RecdListIter, ++count){
			stringstream ss;													//����int->string����ת��
			ss<<count;
			tmp = (*RecdListIter);
			cout<<"True Label="<<Label[ChildNode->n_ClusterCenterNode.Center->iLabel];
			outFile<<"True Label="<<Label[ChildNode->n_ClusterCenterNode.Center->iLabel];

			cout<<"\tPre Label="<<Label[tmp->iLabel];
			outFile<<"\tPre Label="<<Label[tmp->iLabel];
			
			cout<<"\tCluster Path="<<ChildNode->strPath + "." + ss.str()<<endl;
			outFile<<"\tCluster Path="<<ChildNode->strPath + "." + ss.str()<<endl;

			matrix[ChildNode->n_ClusterCenterNode.Center->iLabel][tmp->iLabel]++;
			if(ChildNode->n_ClusterCenterNode.Center->iLabel == tmp->iLabel){
				this->RightLableCount++;
			}
		}
	}
	else{
		for(int i = 0; i < ChildNode->n_ChildCount; ++i){
			Print(ChildNode->pChildNode[i], matrix);
		}
	}
}

//���������¼���ýڵ����ĵľ���
float ClusterNode::CalCenterDistance(strMyRecord *pRecord){
	double fDist;
	fDist = 0;

	fDist += pow((pRecord->iProtocolType - this->n_ClusterCenterNode.Center->iProtocolType), 2);
	fDist += pow((pRecord->iService - this->n_ClusterCenterNode.Center->iService), 2);
	fDist += pow((pRecord->iStatusFlag - this->n_ClusterCenterNode.Center->iStatusFlag), 2);
	fDist += pow((pRecord->iSrcBytes - this->n_ClusterCenterNode.Center->iSrcBytes), 2);
	fDist += pow((pRecord->iDestBytes - this->n_ClusterCenterNode.Center->iDestBytes), 2);
	fDist += pow((pRecord->iFailedLogins - this->n_ClusterCenterNode.Center->iFailedLogins), 2);
	fDist += pow((pRecord->iNumofRoot - this->n_ClusterCenterNode.Center->iNumofRoot), 2);
	fDist += pow((pRecord->iCount - this->n_ClusterCenterNode.Center->iCount),2);
	fDist += pow((pRecord->iSrvCount - this->n_ClusterCenterNode.Center->iSrvCount), 2);
	fDist += pow((pRecord->iRerrorRate - this->n_ClusterCenterNode.Center->iRerrorRate), 2);
	fDist += pow((pRecord->iSameSrvRate - this->n_ClusterCenterNode.Center->iSameSrvRate),2);
	fDist += pow((pRecord->iDiffSrvRate - this->n_ClusterCenterNode.Center->iDiffSrvRate),2);
	fDist += pow((pRecord->iDstHostSrvCount - this->n_ClusterCenterNode.Center->iDstHostSrvCount), 2);
	fDist += pow((pRecord->iDstHostSameSrvRate - this->n_ClusterCenterNode.Center->iDstHostSameSrvRate), 2);
	fDist += pow((pRecord->iDstHostDiffSrvRate - this->n_ClusterCenterNode.Center->iDstHostDiffSrvRate), 2);
	fDist += pow((pRecord->iDstHostSameSrcPortRate - this->n_ClusterCenterNode.Center->iDstHostSameSrcPortRate), 2);
	fDist += pow((pRecord->iDstHostSrvDiffHostRate - this->n_ClusterCenterNode.Center->iDstHostSrvDiffHostRate) ,2);
	fDist += pow((pRecord->iDstHostSrvSerrorRate - this->n_ClusterCenterNode.Center->iDstHostSrvSerrorRate), 2);
	fDist += pow((pRecord->iLabel - this->n_ClusterCenterNode.Center->iLabel), 2);
	return fDist;
}

//�ݹ麯��, ���Խڵ�Ϊ���׵�������,��������ݼ�¼��������ľ���ڵ�
ClusterNode * ClusterNode::GetNearestCluseter(strMyRecord *pRecord){
	ClusterNode * pNearestNode = NULL;												//��������ڵ�ָ��
	ClusterNode * pTmpNode;													//��ʱ�ڵ�
	float MinDistance, TmpDistance;											//��̾���,��ʱ����
	if(this->IsLeaf > 0){
		pNearestNode = this;												//�жϵ�ǰ�ڵ����ΪҶ�ӽڵ�, ��ֱ�ӷ��ص�ǰ�ڵ�
	}
	else{																	//�����ǰ�ڵ㲻��Ҷ�ӽڵ�,����ӽڵ���м���
		if(this->pParentNode != NULL){										//����root�ڵ�û������,����Ҫ����root�ڵ�
			pNearestNode = this;											//����ǰ�ڵ���Ϊ����ڵ�
			MinDistance = this->CalCenterDistance(pRecord);					//��pRecord�뵱ǰ�ڵ�ľ�����Ϊ��̾���
		}else{
			pNearestNode = NULL;
			MinDistance = 999999999999999;									//��pRecord��root�ڵ��������Ϊ�����
		}
		for(int i = 0; i < this->n_ChildCount; ++i){
			pTmpNode = this->pChildNode[i]->GetNearestCluseter(pRecord);	//�Ӻ��ӽڵ����ҳ�����ڵ�
			TmpDistance = pTmpNode->CalCenterDistance(pRecord);
			if(TmpDistance < MinDistance){									//�жϺ��ӽڵ�͵�ǰ�ڵ��Ǹ�����pRecord���
				pNearestNode = pTmpNode;
				MinDistance = TmpDistance;
			}
		}
	}
	return pNearestNode;													//���ص�ǰ�ڵ���ӽڵ��о���pRecord��������Ľڵ�
}
//�ҵ��������¼��������ľ���ڵ�
ClusterNode * ClusterTree::FindNearestCluster(strMyRecord *pRecord){
	ClusterNode *pNearestNode = NULL;
	this->Count++;
	pNearestNode = this->pRootNode->GetNearestCluseter(pRecord);
	if(pNearestNode->n_ClusterCenterNode.Center->iLabel == pRecord->iLabel){
		this->RightLableCount++;
	}
	return pNearestNode;
}

void ReadTestFile(ClusterTree *pClusterTree, int DimensionsCount){
	strMyRecord *tmpMyRecord = new strMyRecord();							//Ҫ���ṹ������ڴ�ռ䣬��Ȼ��ֵ�ᷢ������
	//�������ļ�
	char *filename = ".\\pending.txt";
	int RightCount = 0;		//��ȷԤ�еĸ���
	int TotalCount = 0;		//�ܵ����ݸ���
	int UnknowCount = 0;	//δ֪�������ݸ���
	int index = GetPosition("normal", Label);	//��¼�������͵��±�
	int BeforNormalCount = 0;	//Ԥ��ǰ��ȷ���͸���
	int AfterNormalCount = 0;	//Ԥ�к���ȷ���͸���
	FILE *fp = NULL;
	if(!(fp = fopen(filename, "r"))){
		cout<<filename<<" Can not Open!"<<endl;
		exit(1);
	}

	cout<<"\nStart reading records from TestFile: "<<filename<<"..."<<endl;
	outFile<<"\nStart reading records from TestFile: "<<filename<<"..."<<endl;
	
	char *buff = new char[MAX_BUF_SIZE];									//����ÿ��ʱ�Ļ�������С
	unsigned int count = 0;
	while(fgets(buff, MAX_BUF_SIZE, fp)){									//���ļ��ж�ȡһ������
		buff[strlen(buff)-1] = '\0';										//ɾ���ַ�������'\n' !�˴����ܴ������⣬\r\n
		vector <char *> ALLChar;											//ALLChar��Ų�ֺ�ĸ����ַ���
		Split(buff, DELIM, &ALLChar);										//����ַ���

		//����ת�������������tmpMyRecord��
		tmpMyRecord->iProtocolType = GetPosition(ALLChar[0], ProtocolType);
		tmpMyRecord->iService = GetPosition(ALLChar[1], Service);
		tmpMyRecord->iStatusFlag = GetPosition(ALLChar[2], StatusFlag);
		tmpMyRecord->iSrcBytes = Char2Int(ALLChar[3]);
		tmpMyRecord->iDestBytes = Char2Int(ALLChar[4]);
		tmpMyRecord->iFailedLogins = Char2Int(ALLChar[5]);
		tmpMyRecord->iNumofRoot = Char2Int(ALLChar[6]);
		tmpMyRecord->iCount = Char2Int(ALLChar[7]);
		tmpMyRecord->iSrvCount = Char2Int(ALLChar[8]);
		tmpMyRecord->iRerrorRate = Char2Byte(ALLChar[9]);
		tmpMyRecord->iSameSrvRate = Char2Byte(ALLChar[10]);
		tmpMyRecord->iDiffSrvRate = Char2Byte(ALLChar[11]);
		tmpMyRecord->iDstHostSrvCount = Char2Int(ALLChar[12]);
		tmpMyRecord->iDstHostSameSrvRate = Char2Byte(ALLChar[13]);
		tmpMyRecord->iDstHostDiffSrvRate = Char2Byte(ALLChar[14]);
		tmpMyRecord->iDstHostSameSrcPortRate = Char2Byte(ALLChar[15]);
		tmpMyRecord->iDstHostSrvDiffHostRate = Char2Byte(ALLChar[16]);
		tmpMyRecord->iDstHostSrvSerrorRate = Char2Byte(ALLChar[17]);
		tmpMyRecord->iLabel = GetPosition(ALLChar[18], Label);
		if(tmpMyRecord->iLabel == index){
			BeforNormalCount++;
		}
		cout<<"True Label is :"<<Label[tmpMyRecord->iLabel];
		outFile<<"True Label is :"<<Label[tmpMyRecord->iLabel];
		
		if(tmpMyRecord->iLabel > DimensionsCount-1){	
			cout<<"\t\tClassified as : Unknow"<<endl;			//���������ڲ���������֪������ʱԤ��ΪUnknow����
			outFile<<"\t\tClassified as : Unknow"<<endl;
			TotalCount++;		//����+1
			UnknowCount++;		//δ֪�������ݸ���+1
			continue;
		}
		cout<<"\t\tClassified as :";
		outFile<<"\t\tClassified as :";
		BYTE PreLabel = pClusterTree->FindNearestCluster(tmpMyRecord)->n_ClusterCenterNode.Center->iLabel;
		if(PreLabel == index){
			AfterNormalCount++;
		}
		cout<<Label[PreLabel]<<endl;
		outFile<<Label[PreLabel]<<endl;
		if(tmpMyRecord->iLabel == PreLabel){
			RightCount++;
		}
		TotalCount++;
	}
	fclose(fp);
	cout<<"\nAdd "<<TotalCount<<" Record...!"<<endl;
	outFile<<"\nAdd "<<TotalCount<<" Record...!"<<endl;
	
	cout<<endl<<"==============================Total Result=============================="<<endl;
	outFile<<endl<<"==============================Total Result=============================="<<endl;
	cout<<"Total Test Records="<<TotalCount<<"\tRight Records="<<RightCount;
	outFile<<"Total Test Records="<<TotalCount<<"\tRight Records="<<RightCount;
	
	cout<<"\tRight Rate="<<(float)RightCount/TotalCount<<endl;
	outFile<<"\tRight Rate="<<(float)RightCount/TotalCount<<endl;

	cout<<"Unknow Records="<<UnknowCount<<endl;
	outFile<<"Unknow Records="<<UnknowCount<<endl;

	cout<<"BeforeNormal Records="<<BeforNormalCount<<"\tBeforeNormal Rate="<<(float)BeforNormalCount/TotalCount<<endl;
	outFile<<"BeforeNormal Records="<<BeforNormalCount<<"BeforeNormal Rate="<<(float)BeforNormalCount/TotalCount<<endl;
	
	cout<<"AfterNormal Records="<<AfterNormalCount<<"\tAfterNormal Rate="<<(float)AfterNormalCount/TotalCount<<endl;
	outFile<<"AfterNormal Records="<<AfterNormalCount<<"\tAfterNormal Rate="<<(float)AfterNormalCount/TotalCount<<endl;

	cout<<endl<<"==============================Total Result=============================="<<endl;
	outFile<<endl<<"==============================Total Result=============================="<<endl;
}

//�����������
void PrintMatrix(int **matrix, ClusterTree *pClusterTree){

	cout<<endl<<"===========================New Confusion Matrix==========================="<<endl;
	outFile<<endl<<"===========================New Confusion Matrix==========================="<<endl;
	int RightCount = 0;
	int TotalCount = 0;
	for(int i = 0; i <  pClusterTree->TreeNumDimensions; ++i){
		outFile<<"\t\t"<<Label[i]<<"\t\t";
	}
	cout<<endl;
	outFile<<endl;
	for(i = 0; i < pClusterTree->TreeNumDimensions; ++i){
		cout<<Label[i]<<"\t";
		outFile<<Label[i]<<"\t\t";
		for(int j = 0; j < pClusterTree->TreeNumDimensions; ++j){
			cout<<matrix[i][j]<<"\t";
			outFile<<matrix[i][j]<<"\t\t";
			TotalCount += matrix[i][j];
			if(i == j)
				RightCount += matrix[i][j];
		}
		cout<<endl;
		outFile<<endl;
	}
	
	cout<<endl<<"==============================Total Result=============================="<<endl;
	outFile<<endl<<"==============================Total Result=============================="<<endl;
	pClusterTree->RightRate = (float)pClusterTree->RightLableCount / (float)pClusterTree->Count;
	cout<<"Total Test Records="<<TotalCount<<"\tRight Records="<<RightCount;
	outFile<<"Total Test Records="<<TotalCount<<"\tRight Records="<<RightCount;

	cout<<"\tRight Rate="<<(float)RightCount/TotalCount<<endl;
	outFile<<"\tRight Rate="<<(float)RightCount/TotalCount<<endl;

	cout<<endl<<"==============================Total Result=============================="<<endl;
	outFile<<endl<<"==============================Total Result=============================="<<endl;

}

int main(){
	outFile.open("LogOut.log", ios::out);														//�����־�ļ�ָ��
	ClusterTree *pClusterTree = new ClusterTree();												//����������root
	CKMeans *ckmeas = new CKMeans(pClusterTree, 0, 0);											//����һ��CKMeans����
	ckmeas->ReadTrainingRecords();																//��ȡѵ������
	int count = pClusterTree->TreeNumDimensions;												//��¼ѵ�������е����Ը���
	ckmeas->RunKMeans(count);																	//����KMeans�㷨
	int **matrix = new int*[count];																//������ά����
	for(int i = 0; i < count; ++i){																//
		matrix[i] = new int[count];																//
		for(int j = 0; j < count; ++j){															//
			matrix[i][j] = 0;																	//
		}																						//
	}																							//
	cout<<"=========================Classification Result=========================="<<endl;
	outFile<<"=========================Classification Result=========================="<<endl;
	pClusterTree->Print(pClusterTree->pRootNode, matrix);
	PrintMatrix(matrix, pClusterTree);															//�����������
	ReadTestFile(pClusterTree, count);															//��ȡ��¼,����������б�
	outFile.close();
	return 0;
}