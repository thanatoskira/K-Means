#include<iostream>
#include<sstream>
#include<list>
#include<vector>
#include<cmath>
#include <algorithm>
#include<string>
#include <fstream>
using namespace std;
typedef unsigned char BYTE;	//����BYTE����

#define DELIM ","			//��","��������
#define MAX_BUF_SIZE 256	//��󻺳�����С
#define MAXLABEL 40			//���LABEL����

vector<char *> ProtocolType;	//������е�Э������
vector<char *> Service;			//������еķ�������
vector<char *> StatusFlag;		//������е�״̬��־
vector<char *> Label;			//������е����ͱ�ǩ
unsigned int ID = 0;		//ȫ�ֱ�ʶID
ofstream outFile;		//��־����ļ�ָ��

//�������������ַ�������ת����int����
inline int Char2Int(char *s){
	stringstream ss;
	ss << s;
	int i;
	ss >> i;
	return i;
}

//�������������ַ���ת����BYTE����
inline BYTE Char2Byte(char *s){
	BYTE b = atof(s)*100.0;
	return b;
}

//����Search������type�����е���Ӧ�±�
BYTE GetPosition(char *Search, vector<char *>&type){
	unsigned int index;	//���ڼ�¼�±�

	//������������ָ��type�����и�������
	vector<char *>::iterator tmp = type.begin();
	
	//��������type���Ƿ���Search���ͣ����򷵻�����Ӧ���±�
	for(index = 0 ; tmp != type.end(); ++tmp, ++index){
		if(strcmp(*tmp, Search) == 0){
			return index;
		}
	}

	//type������δ����Search���ͣ���Search���ͼ���type�����У��γɶ�̬�������
	if(tmp == type.end()){
		unsigned int len = strlen(Search);	//��ȡSearch�ַ����ĳ���
		char *tmpstr = new char(len);		//����һ���ڴ������ż���push_back��Search�ַ���
		strncpy(tmpstr, Search, len);		//��Search�����ݿ�����tmpstr��
		tmpstr[len] = '\0';			//�ַ���������
		type.push_back(tmpstr);			//��tmpstrҲ��Search��ӵ�type������
	}
	return index;	//����Search��type�е��±�
}

//��delim�ָ��ַ�����������������ret������
void Split(char *buff, char *delim, vector<char *> *ret){
	char *p = NULL;
	p = strtok(buff, delim);
	while(p != NULL){
		ret->push_back(p);
		p = strtok(NULL, delim);
	}
}

//�ṹ�壬���ڴ�Ŵ��������ݼ�¼
typedef struct STRMYRECORD{
	STRMYRECORD(){											//���캯������ʼ��ֵΪ0
		this->iProtocolType = 0;
		this->iService = 0;
		this->iStatusFlag = 0;
		this->iSrcBytes = 0;
		this->iDestBytes = 0;
		this->iFailedLogins = 0;
		this->iNumofRoot = 0;
		this->iCount = 0;
		this->iSrvCount = 0;
		this->iRerrorRate = 0;
		this->iSameSrvRate = 0;
		this->iDiffSrvRate = 0;
		this->iDstHostSrvCount = 0;
		this->iDstHostSameSrvRate = 0;
		this->iDstHostDiffSrvRate = 0;
		this->iDstHostSameSrcPortRate = 0;
		this->iDstHostSrvDiffHostRate = 0;
		this->iDstHostSrvSerrorRate = 0;
		this->iLabel = 0;
	}
	

	STRMYRECORD & operator+=(STRMYRECORD *pRecord){				//����+=�����
		this->iProtocolType += pRecord->iProtocolType;
		this->iService += pRecord->iService;
		this->iStatusFlag += pRecord->iStatusFlag;
		this->iSrcBytes += pRecord->iSrcBytes;
		this->iDestBytes += pRecord->iDestBytes;
		this->iFailedLogins += pRecord->iFailedLogins;
		this->iNumofRoot += pRecord->iNumofRoot;
		this->iCount += pRecord->iCount;
		this->iSrvCount += pRecord->iSrvCount;
		this->iRerrorRate += pRecord->iRerrorRate;
		this->iSameSrvRate += pRecord->iSameSrvRate;
		this->iDiffSrvRate += pRecord->iDiffSrvRate;
		this->iDstHostSrvCount += pRecord->iDstHostSrvCount;
		this->iDstHostSameSrvRate += pRecord->iDstHostSameSrvRate;
		this->iDstHostDiffSrvRate += pRecord->iDstHostDiffSrvRate;
		this->iDstHostSameSrcPortRate += pRecord->iDstHostSameSrcPortRate;
		this->iDstHostSrvDiffHostRate += pRecord->iDstHostSrvDiffHostRate;
		this->iDstHostSrvSerrorRate += pRecord->iDstHostSrvSerrorRate;
		this->iLabel += pRecord->iLabel;
		return *this;
	}

	STRMYRECORD & operator=(STRMYRECORD *pRecord){				//����=�����
		this->iProtocolType = pRecord->iProtocolType;
		this->iService = pRecord->iService;
		this->iStatusFlag = pRecord->iStatusFlag;
		this->iSrcBytes = pRecord->iSrcBytes;
		this->iDestBytes = pRecord->iDestBytes;
		this->iFailedLogins = pRecord->iFailedLogins;
		this->iNumofRoot = pRecord->iNumofRoot;
		this->iCount = pRecord->iCount;
		this->iSrvCount = pRecord->iSrvCount;
		this->iRerrorRate = pRecord->iRerrorRate;
		this->iSameSrvRate = pRecord->iSameSrvRate;
		this->iDiffSrvRate = pRecord->iDiffSrvRate;
		this->iDstHostSrvCount = pRecord->iDstHostSrvCount;
		this->iDstHostSameSrvRate = pRecord->iDstHostSameSrvRate;
		this->iDstHostDiffSrvRate = pRecord->iDstHostDiffSrvRate;
		this->iDstHostSameSrcPortRate = pRecord->iDstHostSameSrcPortRate;
		this->iDstHostSrvDiffHostRate = pRecord->iDstHostSrvDiffHostRate;
		this->iDstHostSrvSerrorRate = pRecord->iDstHostSrvSerrorRate;
		this->iLabel = pRecord->iLabel;
		return *this;
	}
	
	STRMYRECORD & operator/=(unsigned int iNumMembers){				//����/=�����	
		this->iProtocolType /= iNumMembers;
		this->iService /= iNumMembers;
		this->iStatusFlag /= iNumMembers;
		this->iSrcBytes /= iNumMembers;
		this->iDestBytes /= iNumMembers;
		this->iFailedLogins /= iNumMembers;
		this->iNumofRoot /= iNumMembers;
		this->iCount /= iNumMembers;
		this->iSrvCount /= iNumMembers;
		this->iRerrorRate /= iNumMembers;
		this->iSameSrvRate /= iNumMembers;
		this->iDiffSrvRate /= iNumMembers;
		this->iDstHostSrvCount /= iNumMembers;
		this->iDstHostSameSrvRate /= iNumMembers;
		this->iDstHostDiffSrvRate /= iNumMembers;
		this->iDstHostSameSrcPortRate /= iNumMembers;
		this->iDstHostSrvDiffHostRate /= iNumMembers;
		this->iDstHostSrvSerrorRate /= iNumMembers;
		this->iLabel /= iNumMembers;
		return *this;
	}

	bool operator!=(STRMYRECORD *pRecord){					//����!=�����
		if(this->iProtocolType == pRecord->iProtocolType &&
		this->iService == pRecord->iService &&
		this->iStatusFlag == pRecord->iStatusFlag &&
		this->iSrcBytes == pRecord->iSrcBytes &&
		this->iDestBytes == pRecord->iDestBytes &&
		this->iFailedLogins == pRecord->iFailedLogins &&
		this->iNumofRoot == pRecord->iNumofRoot &&
		this->iCount == pRecord->iCount &&
		this->iSrvCount == pRecord->iSrvCount &&
		this->iRerrorRate == pRecord->iRerrorRate &&
		this->iSameSrvRate == pRecord->iSameSrvRate &&
		this->iDiffSrvRate == pRecord->iDiffSrvRate &&
		this->iDstHostSrvCount == pRecord->iDstHostSrvCount &&
		this->iDstHostSameSrvRate == pRecord->iDstHostSameSrvRate &&
		this->iDstHostDiffSrvRate == pRecord->iDstHostDiffSrvRate &&
		this->iDstHostSameSrcPortRate == pRecord->iDstHostSameSrcPortRate &&
		this->iDstHostSrvDiffHostRate == pRecord->iDstHostSrvDiffHostRate &&
		this->iDstHostSrvSerrorRate == pRecord->iDstHostSrvSerrorRate &&
		this->iLabel == pRecord->iLabel){
			return false;
		}
		return true;		
	}

	bool operator==(STRMYRECORD *pRecord){					//����==�����
		if(this->iProtocolType == pRecord->iProtocolType &&
		this->iService == pRecord->iService &&
		this->iStatusFlag == pRecord->iStatusFlag &&
		this->iSrcBytes == pRecord->iSrcBytes &&
		this->iDestBytes == pRecord->iDestBytes &&
		this->iFailedLogins == pRecord->iFailedLogins &&
		this->iNumofRoot == pRecord->iNumofRoot &&
		this->iCount == pRecord->iCount &&
		this->iSrvCount == pRecord->iSrvCount &&
		this->iRerrorRate == pRecord->iRerrorRate &&
		this->iSameSrvRate == pRecord->iSameSrvRate &&
		this->iDiffSrvRate == pRecord->iDiffSrvRate &&
		this->iDstHostSrvCount == pRecord->iDstHostSrvCount &&
		this->iDstHostSameSrvRate == pRecord->iDstHostSameSrvRate &&
		this->iDstHostDiffSrvRate == pRecord->iDstHostDiffSrvRate &&
		this->iDstHostSameSrcPortRate == pRecord->iDstHostSameSrcPortRate &&
		this->iDstHostSrvDiffHostRate == pRecord->iDstHostSrvDiffHostRate &&
		this->iDstHostSrvSerrorRate == pRecord->iDstHostSrvSerrorRate &&
		this->iLabel == pRecord->iLabel){
			return true;
		}
		return false;		
	}

public:
	BYTE iProtocolType;	//02 Э������
	BYTE iService;		//03��������
	BYTE iStatusFlag;	//04״̬��־
	int iSrcBytes;		//05Դ��Ŀ���ֽ���
	int iDestBytes;		//06Ŀ�ĵ�Դ�ֽ���	
	int iFailedLogins;	//11��¼ʧ�ܴ���
	int iNumofRoot;		//16root�û�Ȩ�޴�ȡ����
	int iCount;			//232����������ͬ������Ŀ
	int iSrvCount;		//242����������ͬ�˿���Ŀ
	BYTE iRerrorRate;	//27"REJ"���������������
	BYTE iSameSrvRate;	//29���ӵ���ͬ�˿�������
	BYTE iDiffSrvRate;	//30���ӵ���ͬ�˿�������
	int iDstHostSrvCount;	//33��ͬĿ�ĵ���ͬ�˿�������
	BYTE iDstHostSameSrvRate;	//34��ͬĿ�ĵ���ͬ�˿�����������
	BYTE iDstHostDiffSrvRate;	//35��ͬĿ�ĵز�ͬ�˿�����������
	BYTE iDstHostSameSrcPortRate;	//36��ͬĿ�ĵ���ͬԴ�˿����ӱ���
	BYTE iDstHostSrvDiffHostRate;	//37��ͬ����������ͬ�˿ڱ���
	BYTE iDstHostSrvSerrorRate;		//39���ӵ�ǰ������S0����ı���
	BYTE iLabel;		//42���ͱ�ǩ
}strMyRecord;