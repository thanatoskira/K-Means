#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#define N  37 //数据个数
#define K  4 //集合个数
#define T  4 //成绩门数

int Flag[4] = {1,1,1,1}; //记录各类簇有无成员,有为0,无为1

struct Score{ //成绩结构体
	int num;
	double Chinese;
	double Math;
	double English;
	double CLanguage;
	struct Score *next;
};

struct Cluster{
	int length; //每个集合成员个数
	double Chinese;
	double Math;
	double English;
	double CLanguage;
	struct Score *Center;
};

struct Score Scores[N];
struct Cluster Clusters[K];
struct Score *ptr[K] = {NULL,NULL,NULL,NULL}; //指向每个簇类的最后一个结构体

//输出各个簇
void OutPut(){/
	for(int i = 0; i < K; ++i){
		struct Score *top = Clusters[i].Center;
		printf("第%d簇:\n中心点:(%.6lf, %.6lf, %.6lf, %.6lf)\n成员个数:%d\n成员:\n", i+1, Clusters[i].Chinese, Clusters[i].Math, Clusters[i].English, Clusters[i].CLanguage, Clusters[i].length);
		while(top){
			printf("\t\t学号:%d\t(%.6lf, %.6lf, %.6lf, %.6lf)\n", top->num, top->Chinese, top->Math, top->English, top->CLanguage);
			top = top->next;
		}
		printf("\n");
	}
}

//加载数据
void LoadScoreData(){
	FILE *fp = fopen("Score.txt", "r");
	while(!feof(fp)){
		for(int i = 0; i < N; ++i){
			fscanf(fp, "%d", &Scores[i].num);
			fscanf(fp, "%lf", &Scores[i].Chinese);
			fscanf(fp, "%lf", &Scores[i].Math);
			fscanf(fp, "%lf", &Scores[i].English);
			fscanf(fp, "%lf", &Scores[i].CLanguage);
			Scores[i].next = NULL;
		}
	}
	fclose(fp);
}

//初始化簇中心点,随机从N个数据中选取K个数据作为初始化中心点,
//并且将初始中心点数据存放到Clusters[i]数据结构中
void InitCluses(){
	srand((unsigned)time(NULL));
	int tmp[K] = {-1};
	for(int i = 0; i < K; ++i){
		int index = rand()%N;
		for(int j = 0; j < i; ++j){
			if(tmp[j] == index)
				break;
		}
		if(j < i)
			i--;
		else{
			tmp[i] = index;
			Clusters[i].Center = NULL;
			Clusters[i].length = 0;
			Clusters[i].Chinese = Scores[index].Chinese;
			Clusters[i].Math = Scores[index].Math;
			Clusters[i].English = Scores[index].English;
			Clusters[i].CLanguage = Scores[index].CLanguage;
			printf("第%d簇初始化中心点:(%.6lf, %.6lf, %.6lf, %.6lf)\n", i+1, Clusters[i].Chinese, Clusters[i].Math, Clusters[i].English, Clusters[i].CLanguage);
		}
	}
}

//计算数据到各个簇中心的距离并且进行分类
void Calculator(struct Score *s){
	int min_index = 0;
	double distance, min;
	for(int i = 0; i < K; ++i){
		distance = sqrt(pow(s->Chinese - Clusters[i].Chinese, 2) + pow(s->Math - Clusters[i].Math, 2) + pow(s->English - Clusters[i].English, 2) + pow(s->CLanguage - Clusters[i].CLanguage, 2));
		if(i == 0)
			min = distance;
		if(distance < min){
            min_index = i;
			min = distance;
		}
	}//找出数据到各个簇类中心点最短的簇类

    //将数据存放入最短距离的簇类
	if(Flag[min_index]){ //判断当前簇类有无成员
		Clusters[min_index].Center = s;
		Clusters[min_index].length++;
		Flag[min_index] = 0;
		ptr[min_index] = s;
		ptr[min_index]->next = NULL;
	}
	else{
		Clusters[min_index].length++;
		ptr[min_index]->next = s;
		ptr[min_index] = ptr[min_index]->next;
		ptr[min_index]->next = NULL;
	}
}

//判断聚类结束
bool IsClustOK(double newCenter[K][T])
{
	double ext=0.000005;  //误差值
	for(int i=0;i<K;i++)
	{
		if(fabs(Clusters[i].Chinese - newCenter[i][0]) <= ext && fabs(Clusters[i].Math - newCenter[i][1]) <= ext
			&& fabs(Clusters[i].English - newCenter[i][2]) <= ext && fabs(Clusters[i].CLanguage - newCenter[i][3]) <= ext)
			continue;
		else 
			return false;
	}
	return true;
}

//计算簇成员的平均值
void Average(){
	struct Score *tmp = NULL;
	double newCenter[K][T]; //记录新的中心点
	for(int i = 0; i < K; ++i){
		double avgC = 0.0, avgM = 0.0, avgE = 0.0, avgCL = 0.0;
		tmp = Clusters[i].Center;
		while(tmp){
			avgC += tmp->Chinese;
			avgM += tmp->Math;
			avgE += tmp->English;
			avgCL += tmp->CLanguage;
			tmp = tmp->next;
		}
		newCenter[i][0] = avgC/Clusters[i].length;
		newCenter[i][1] = avgM/Clusters[i].length;
		newCenter[i][2] = avgE/Clusters[i].length;
		newCenter[i][3] = avgCL/Clusters[i].length;
	}
	if(!IsClustOK(newCenter)){
		for(int i = 0; i < K; ++i){
			Clusters[i].Chinese = newCenter[i][0];
			Clusters[i].Math = newCenter[i][1];
			Clusters[i].English = newCenter[i][2];
			Clusters[i].CLanguage = newCenter[i][3];
			Clusters[i].Center = NULL;
			Clusters[i].length = 0;
			Flag[i] = 1;
		}
	}
	else{
		OutPut();
		printf("end of program~\n");
		exit(0);
	}
}


int main(){
	LoadScoreData(); //导入数据
	InitCluses(); //初始化簇类中心
	while(1){
		for(int i = 0; i < N; ++i){
			Calculator(&Scores[i]); //重新计算各个点到各个簇的距离并且进行分类
		};
		Average(); //计算新中心点，并判断是否满足条件结束循环
	}
	return 0;
}



