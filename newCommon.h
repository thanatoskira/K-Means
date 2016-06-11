#include<iostream>
#include<sstream>
#include<list>
#include<vector>
#include<cmath>
#include <algorithm>
#include<string>
#include <fstream>
using namespace std;
typedef unsigned char BYTE;	//定义BYTE类型

#define DELIM ","			//以","划分数据
#define MAX_BUF_SIZE 256	//最大缓冲区大小
#define MAXLABEL 40			//最大LABEL数量

vector<char *> ProtocolType;	//存放所有的协议类型
vector<char *> Service;			//存放所有的服务类型
vector<char *> StatusFlag;		//存放所有的状态标志
vector<char *> Label;			//存放所有的类型标签
unsigned int ID = 0;		//全局标识ID
ofstream outFile;		//日志输出文件指针

//内联函数，将字符串数字转换成int整形
inline int Char2Int(char *s){
	stringstream ss;
	ss << s;
	int i;
	ss >> i;
	return i;
}

//内联函数，将字符串转换成BYTE类型
inline BYTE Char2Byte(char *s){
	BYTE b = atof(s)*100.0;
	return b;
}

//搜索Search类型在type数组中的相应下标
BYTE GetPosition(char *Search, vector<char *>&type){
	unsigned int index;	//用于记录下标

	//迭代器，用于指向type数组中各个类型
	vector<char *>::iterator tmp = type.begin();
	
	//遍历数组type中是否有Search类型，有则返回所对应的下表
	for(index = 0 ; tmp != type.end(); ++tmp, ++index){
		if(strcmp(*tmp, Search) == 0){
			return index;
		}
	}

	//type数组中未发现Search类型，则将Search类型加入type数组中，形成动态添加属性
	if(tmp == type.end()){
		unsigned int len = strlen(Search);	//获取Search字符串的长度
		char *tmpstr = new char(len);		//分配一块内存出来存放即将push_back的Search字符串
		strncpy(tmpstr, Search, len);		//将Search的内容拷贝到tmpstr中
		tmpstr[len] = '\0';			//字符串结束符
		type.push_back(tmpstr);			//将tmpstr也即Search添加到type数组中
	}
	return index;	//返回Search在type中的下标
}

//以delim分割字符串，并将结果存放至ret数组中
void Split(char *buff, char *delim, vector<char *> *ret){
	char *p = NULL;
	p = strtok(buff, delim);
	while(p != NULL){
		ret->push_back(p);
		p = strtok(NULL, delim);
	}
}

//结构体，用于存放处理后的数据记录
typedef struct STRMYRECORD{
	STRMYRECORD(){											//构造函数，初始化值为0
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
	

	STRMYRECORD & operator+=(STRMYRECORD *pRecord){				//重载+=运算符
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

	STRMYRECORD & operator=(STRMYRECORD *pRecord){				//重载=运算符
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
	
	STRMYRECORD & operator/=(unsigned int iNumMembers){				//重载/=运算符	
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

	bool operator!=(STRMYRECORD *pRecord){					//重载!=运算符
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

	bool operator==(STRMYRECORD *pRecord){					//重载==运算符
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
	BYTE iProtocolType;	//02 协议类型
	BYTE iService;		//03服务类型
	BYTE iStatusFlag;	//04状态标志
	int iSrcBytes;		//05源到目的字节数
	int iDestBytes;		//06目的到源字节数	
	int iFailedLogins;	//11登录失败次数
	int iNumofRoot;		//16root用户权限存取次数
	int iCount;			//232秒内连接相同主机数目
	int iSrvCount;		//242秒内连接相同端口数目
	BYTE iRerrorRate;	//27"REJ"错误的连接数比例
	BYTE iSameSrvRate;	//29连接到相同端口数比例
	BYTE iDiffSrvRate;	//30连接到不同端口数比例
	int iDstHostSrvCount;	//33相同目的地相同端口连接数
	BYTE iDstHostSameSrvRate;	//34相同目的地相同端口连接数比例
	BYTE iDstHostDiffSrvRate;	//35相同目的地不同端口连接数比例
	BYTE iDstHostSameSrcPortRate;	//36相同目的地相同源端口连接比例
	BYTE iDstHostSrvDiffHostRate;	//37不同主机连接相同端口比例
	BYTE iDstHostSrvSerrorRate;		//39连接当前主机有S0错误的比例
	BYTE iLabel;		//42类型标签
}strMyRecord;