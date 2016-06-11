#include "newKmeans.h"

//读取训练数据
bool CKMeans::ReadTrainingRecords(){
	//打开输入文件
	char *filename = ".\\corrected_datastreat";
	if(!(this->pInFile = fopen(filename, "r"))){
		cout<<filename<<" Can not Open!"<<endl;
		exit(1);
	}

	cout<<"\nStart reading records from "<<filename<<"..."<<endl;
	outFile<<"Start reading records from "<<filename<<"...\n"<<endl;

	char *buff = new char[MAX_BUF_SIZE];						//读入每行时的缓冲区大小
	unsigned int count = 0;
	while(fgets(buff, MAX_BUF_SIZE, this->pInFile)){			//从文件中读取一行数据
		buff[strlen(buff)-1] = '\0';							//删除字符串最后的'\n' !此处可能存在问题，\r\n
		//cout<<buff<<endl;
		vector <char *> ALLChar;								//ALLChar存放拆分后的各个字符串
		Split(buff, DELIM, &ALLChar);							//拆分字符串

		strMyRecord *tmpMyRecord = new strMyRecord();			//要给结构体分配内存空间，不然赋值会发生错误

		//数据转换处理，并记录存放入tmpMyRecord链表中中
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

		//将处理后的记录加入链表中
		this->m_RecordsList.push_back(tmpMyRecord);

		this->pClusterTree->Count++;
		if(this->pClusterTree->Count % 1000 == 0){
			cout<<"----------"<<this->pClusterTree->Count<<" lines have written----------"<<endl;
		}   
	}

	//记录Label种类个数
	this->pClusterTree->TreeNumDimensions += Label.size();

	cout<<"\nALL Records have read! Total "<<this->pClusterTree->Count<<" records!\n"<<endl;
	outFile<<"ALL Records have read! Total "<<this->pClusterTree->Count<<" records!\n"<<endl;
	fclose(this->pInFile);
	return true;
}

//判断当前记录数值与之前的聚类中心的数值是否相同
bool CKMeans::IsSameAsCluster(int i, strMyRecord * pRecord){
	for(int j = 0; j < i; ++j){
		//if(*this->m_Cluster[j].Center == pRecord){						//根据记录中各个属性值的不同来选择
		if(this->m_Cluster[j].Center->iLabel == pRecord->iLabel){			//根据记录中iLabel的不同来选择
			return true; 
		}
	}
	return false;
}

void CKMeans::InitClusters(unsigned int NumClusters){					//初始化聚类中心
	list<strMyRecord *>::iterator RecdListIter;							//记录链表的迭代器

	this->m_iNumClusters = NumClusters;									//聚类的类别数
	RecdListIter = this->m_RecordsList.begin();							//将List迭代器指向链表头部

	strMyRecord * RecdCenter = new strMyRecord[this->m_iNumClusters];	//记录每个类别的平均值
	
	int *RecordCount = new int[this->m_iNumClusters];					//记录每种类别的总数
	int *CountMap = new int[this->m_iNumClusters];						//映射表，种类在映射表中的下标

	for(int i = 0; i < this->m_iNumClusters; ++i){	//初始化值
		RecordCount[i] = 0;
		CountMap[i] = -1;
	}
	
	for(strMyRecord *tmpRecord = NULL; RecdListIter != this->m_RecordsList.end(); ++RecdListIter){
		tmpRecord = (* RecdListIter);

		int index = -1;	//记录种类在RecordCount的下标
		int Negetive = -1;	//用于将类型插入映射表中
		for(int i = this->m_iNumClusters-1; i >= 0; --i){
			if(CountMap[i] == -1){
				Negetive = i;
				continue;
			}
			if(CountMap[i] == (int)tmpRecord->iLabel){	//如果映射表中存在该类型
				index = i;	//将该类型在RecordCount中的下标返回
				break;
			}
		}
		if(index == -1){	//说明映射表CountMap中不存在该类型
			CountMap[Negetive] = (int)tmpRecord->iLabel;	//将该类型插入到映射表中为-1的位置
			RecdCenter[Negetive] += tmpRecord;	//累加
			RecordCount[Negetive]++;	//然后该类别数量加1
		}
		else{	//说明类型存在于映射表中
			RecdCenter[index] += tmpRecord;	//累加
			RecordCount[index]++;	//将该类别在RecordCount中的值+1
		}
	}
	for(i = 0; i < this->m_iNumClusters; ++i){
		RecdCenter[i] /= RecordCount[i];
		this->m_Cluster[i].Center = &RecdCenter[i];
	}
}

//计算出当前记录到第id个聚类中心的欧氏距离
//为了计算方便，去除开根号部分
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

//找出与该条记录最近的聚类中心的ID
int CKMeans::FindClosestCluster(strMyRecord *pRecord){
	int MinID = 0;											//记录最小ID
	float MinDst = this->CalcEucNorm(pRecord, 0);			//记录最小ID的距离
	for(int id = 1; id < this->m_iNumClusters; ++id){		//依次计算该记录到每个聚类中心的距离
		float tmp = this->CalcEucNorm(pRecord, id);
		if(tmp < MinDst){
			MinDst = tmp;									//更新最小距离
			MinID = id; 									//更新最小ID
		}
	}
	return MinID;
}

void CKMeans::DistributeSamples(){									//找到该条记录最近的聚类中心的ID
	for(int i = 0 ; i < this->m_iNumClusters; ++i){					//清空所有聚类的成员列表
		this->m_Cluster[i].MemberList.clear();
		this->m_Cluster[i].iNumMembers = 0;
	}
	
	list<strMyRecord *>::iterator RecdListIter;						//迭代器RecdListiter指向链表m_RecordsList的头部
	RecdListIter = this->m_RecordsList.begin();

	for(strMyRecord *tmpRecord = NULL; RecdListIter != this->m_RecordsList.end(); ++RecdListIter){
		tmpRecord = (* RecdListIter);
		int MinID = FindClosestCluster(tmpRecord);					//找出与该条记录最近的聚类中心的ID
		this->m_Cluster[MinID].MemberList.push_back(tmpRecord);		//将该条记录插入到对应聚类中心的成员链表中
		this->m_Cluster[MinID].iNumMembers++;						//对应聚类中心成员数量加1
	}
}

bool CKMeans::CalcNewClustCenters(){						//重新计算聚类中心
	bool Judge = false;										//用于判断新聚类中心是否和旧聚类中心相同
	for(int i = 0; i < this->m_iNumClusters; ++i){
		if(this->m_Cluster[i].iNumMembers == 0)			//如果当前聚类中心为空则跳过此处判断
			continue;

		strMyRecord *TempCenter = new strMyRecord();		//将临时聚类中心TempCenter的每一个属性都为设置0	

		list<strMyRecord *>::iterator RecdListIter = this->m_Cluster[i].MemberList.begin();		//记录链表的迭代器
		
		for(strMyRecord *pRecord = NULL; RecdListIter != this->m_Cluster[i].MemberList.end(); ++RecdListIter){
			pRecord = (*RecdListIter); 

			*TempCenter += pRecord;							//将聚类中的每一个成员累加
		}
		
		*TempCenter /= this->m_Cluster[i].iNumMembers;		//将TempCenter的每一个属性除以聚类成员数，计算出平均值

		if(*TempCenter!= this->m_Cluster[i].Center){		//判断新聚类中心与原来的聚类中心是否相等
			this->m_Cluster[i].Center = TempCenter;			//不相等则将聚类中心赋值作为新的聚类中心
			Judge = true;									//将Judge置为true
		}
	}														//待所有的聚类都判断了新的聚类中心与之前的聚类中心是否相同
	if(Judge)												//当Judge为true时,说明存在某个聚类的新聚类中心与原来的聚类中心不相同
		return false;
	return true;											//当返回true时, 说明新的聚类中心与之前的聚类中心相同
}

bool CKMeans::IsClusterOK(int i){							//判断聚类i是否符合聚类精度要求
	unsigned int vLabel[MAXLABEL] = {0};					//记录当前聚类中的Label种类及个数
	int j;
	list<strMyRecord *>::iterator RecdListIter = this->m_Cluster[i].MemberList.begin();		//指向当前聚类链表

	//遍历当前聚类链表
	for(strMyRecord *tmpRecord = NULL; RecdListIter != this->m_Cluster[i].MemberList.end(); ++RecdListIter){
		tmpRecord = (* RecdListIter);
		vLabel[tmpRecord->iLabel]++;
	}
	//以上为统计聚类中Label种类及个数, 存于vLabel数组中

	this->m_Cluster[i].NumDimensions = 0;			//该聚类种类的类别
	unsigned int MasterLabel = 0, index = 0;
	for(j = 0; j < MAXLABEL; ++j){
		if(vLabel[j] != 0)
			this->m_Cluster[i].NumDimensions++;		//种类数量加1
		if(vLabel[j] > MasterLabel){				//记录主类型
			MasterLabel = vLabel[j];
			index = j;
		}
	}

	this->m_Cluster[i].Center->iLabel = index;		//将主类别的属性赋值给聚类中心点
	
	if(this->m_Cluster[i].NumDimensions == 0){
		return true;
	}

	//判断聚类精度是否小于0.3
	if((this->m_Cluster[i].Precition = (float)(this->m_Cluster[i].iNumMembers - vLabel[index]) / (float)this->m_Cluster[i].iNumMembers) < 1/(float)this->m_Cluster[i].NumDimensions){
		return true;
	}
	return false;
}

//对不满足聚类精度的聚类结果递归进行聚类计算
bool CKMeans::IsStopRecursion(int i){
	ID++;
	ClusterNode *pNode = new ClusterNode(this->pSelfClusterNode, i, false, 0, this->m_Cluster[i], ID);	//创建一个聚类节点
	
	//创建一个新的聚类树子节点
	CKMeans *nckmeans = new CKMeans(pNode, ID, this->m_ClusterLevel+1, &this->m_Cluster[i].MemberList);
	nckmeans->RunKMeans(this->m_Cluster[i].NumDimensions); 
	return true;
}

void CKMeans::RunKMeans(int Kvalue){								//运行KMeans算法
	bool IsFinish = false;											//用于判断聚类是否结束
 	this->InitClusters(Kvalue);										//初始化5个聚类中心

	while(!IsFinish){												//判断聚类是否结束
		this->DistributeSamples();									//将记录分配给最近的聚类中心
		IsFinish = this->CalcNewClustCenters();						//计算新的聚类中心, 如果所有聚类的新中心与旧中心都相同, 则把IsFinish设置为true
	}
	for(int i = 0; i < Kvalue; ++i){								//遍历判断各个聚类是否满足聚类精度0.1
		if(!this->IsClusterOK(i)){
			this->IsStopRecursion(i);								//如果不满足精度, 则继续划分
		}else{
			ID++;
			ClusterNode *tmpNode = new ClusterNode(this->pSelfClusterNode, i, true, 1, this->m_Cluster[i], ID);
		}
	}
}

//输出聚类结果
void CKMeans::GetClustersLabel(){
	strMyRecord *tmpRecord = NULL;
	list<strMyRecord *>::iterator RecdListIter;
	
	//遍历聚类结果
	for(int i = 0; i < m_iNumClusters; ++i){
		RecdListIter = this->m_Cluster[i].MemberList.begin();
		cout<<"第 "<<i<<" 类:"<<Label[this->m_Cluster[i].Center->iLabel]<<"\n\t";
		for(strMyRecord *tmpRecord = NULL; RecdListIter != this->m_Cluster[i].MemberList.end(); ++RecdListIter){
			tmpRecord = (*RecdListIter);
			cout<<Label[tmpRecord->iLabel]<<"->";
		}
		cout<<endl<<endl;
	}
	cout<<"\n************************************************\n\n";
}

//输出聚类树结构
void ClusterTree::Print(ClusterNode *ChildNode, int **matrix){
	if(ChildNode->IsLeaf){
		list<strMyRecord *>::iterator RecdListIter;								//遍历链表的迭代器
		RecdListIter = ChildNode->n_ClusterCenterNode.MemberList.begin();		//从链表头开始
		unsigned int count = 0;													//用于Path
		for(strMyRecord * tmp = NULL;RecdListIter != ChildNode->n_ClusterCenterNode.MemberList.end(); ++RecdListIter, ++count){
			stringstream ss;													//用于int->string类型转换
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

//计算该条记录到该节点中心的距离
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

//递归函数, 在以节点为父亲的子树中,获得与数据记录距离最近的聚类节点
ClusterNode * ClusterNode::GetNearestCluseter(strMyRecord *pRecord){
	ClusterNode * pNearestNode = NULL;												//距离最近节点指针
	ClusterNode * pTmpNode;													//临时节点
	float MinDistance, TmpDistance;											//最短距离,临时距离
	if(this->IsLeaf > 0){
		pNearestNode = this;												//判断当前节点如果为叶子节点, 则直接返回当前节点
	}
	else{																	//如果当前节点不是叶子节点,则对子节点进行计算
		if(this->pParentNode != NULL){										//由于root节点没有数据,所以要跳过root节点
			pNearestNode = this;											//将当前节点作为最近节点
			MinDistance = this->CalCenterDistance(pRecord);					//将pRecord与当前节点的距离最为最短距离
		}else{
			pNearestNode = NULL;
			MinDistance = 999999999999999;									//将pRecord到root节点距离设置为无穷大
		}
		for(int i = 0; i < this->n_ChildCount; ++i){
			pTmpNode = this->pChildNode[i]->GetNearestCluseter(pRecord);	//从孩子节点中找出最近节点
			TmpDistance = pTmpNode->CalCenterDistance(pRecord);
			if(TmpDistance < MinDistance){									//判断孩子节点和当前节点那个距离pRecord最近
				pNearestNode = pTmpNode;
				MinDistance = TmpDistance;
			}
		}
	}
	return pNearestNode;													//返回当前节点和子节点中距离pRecord距离最近的节点
}
//找到与给定记录距离最近的聚类节点
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
	strMyRecord *tmpMyRecord = new strMyRecord();							//要给结构体分配内存空间，不然赋值会发生错误
	//打开输入文件
	char *filename = ".\\pending.txt";
	int RightCount = 0;		//正确预判的个数
	int TotalCount = 0;		//总的数据个数
	int UnknowCount = 0;	//未知类型数据个数
	int index = GetPosition("normal", Label);	//记录正常类型的下标
	int BeforNormalCount = 0;	//预判前正确类型个数
	int AfterNormalCount = 0;	//预判后正确类型个数
	FILE *fp = NULL;
	if(!(fp = fopen(filename, "r"))){
		cout<<filename<<" Can not Open!"<<endl;
		exit(1);
	}

	cout<<"\nStart reading records from TestFile: "<<filename<<"..."<<endl;
	outFile<<"\nStart reading records from TestFile: "<<filename<<"..."<<endl;
	
	char *buff = new char[MAX_BUF_SIZE];									//读入每行时的缓冲区大小
	unsigned int count = 0;
	while(fgets(buff, MAX_BUF_SIZE, fp)){									//从文件中读取一行数据
		buff[strlen(buff)-1] = '\0';										//删除字符串最后的'\n' !此处可能存在问题，\r\n
		vector <char *> ALLChar;											//ALLChar存放拆分后的各个字符串
		Split(buff, DELIM, &ALLChar);										//拆分字符串

		//数据转换处理，并存放入tmpMyRecord中
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
			cout<<"\t\tClassified as : Unknow"<<endl;			//待测类型在不存在于已知类型中时预测为Unknow类型
			outFile<<"\t\tClassified as : Unknow"<<endl;
			TotalCount++;		//总数+1
			UnknowCount++;		//未知类型数据个数+1
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

//输出混淆矩阵
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
	outFile.open("LogOut.log", ios::out);														//输出日志文件指针
	ClusterTree *pClusterTree = new ClusterTree();												//创建聚类树root
	CKMeans *ckmeas = new CKMeans(pClusterTree, 0, 0);											//创建一个CKMeans对象
	ckmeas->ReadTrainingRecords();																//读取训练数据
	int count = pClusterTree->TreeNumDimensions;												//记录训练数据中的属性个数
	ckmeas->RunKMeans(count);																	//运行KMeans算法
	int **matrix = new int*[count];																//创建二维数组
	for(int i = 0; i < count; ++i){																//
		matrix[i] = new int[count];																//
		for(int j = 0; j < count; ++j){															//
			matrix[i][j] = 0;																	//
		}																						//
	}																							//
	cout<<"=========================Classification Result=========================="<<endl;
	outFile<<"=========================Classification Result=========================="<<endl;
	pClusterTree->Print(pClusterTree->pRootNode, matrix);
	PrintMatrix(matrix, pClusterTree);															//输出混淆矩阵
	ReadTestFile(pClusterTree, count);															//读取记录,并进行类别判别
	outFile.close();
	return 0;
}