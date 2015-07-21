#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#define N  37 //学生人数
#define K  4 //簇类个数
#define T  4 //成绩门数



struct Stu{ //学生成绩结构体
	int num;    //学号
	double Chinese; //语文
	double Math;   //数学
	double English;  //英语
	double CLanguage; //C语言
	struct Stu *next; //尾指针
};

struct Stu Stus[N];
struct Stu Clusters[K]; //4个簇类
struct Stu *ptr[K] = {NULL,NULL,NULL,NULL}; //指向每个簇类的最后一个结构体



//输出各个簇
void OutPut(){
	for(int i = 0; i < K; ++i){
		struct Stu *top = Clusters[i].next;
		printf("第%d簇:\n中心点:(%.6lf, %.6lf, %.6lf, %.6lf)\n成员个数:%d\n成员:\n", i+1, Clusters[i].Chinese, Clusters[i].Math, Clusters[i].English, Clusters[i].CLanguage, Clusters[i].num);
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
		for(int i = 0; i < N; i++){
			fscanf(fp, "%d", &Stus[i].num);
			fscanf(fp, "%lf", &Stus[i].Chinese);
			fscanf(fp, "%lf", &Stus[i].Math);
			fscanf(fp, "%lf", &Stus[i].English);
			fscanf(fp, "%lf", &Stus[i].CLanguage);
			Stus[i].next = NULL;
		}
	}
	fclose(fp);
}

//初始化簇中心点,随机从N=37个数据中选取K=4个数据作为初始化中心点,
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
			Clusters[i].next = NULL;
			Clusters[i].num = 0; //该簇人数
			Clusters[i].Chinese = Stus[index].Chinese;
			Clusters[i].Math = Stus[index].Math;
			Clusters[i].English = Stus[index].English;
			Clusters[i].CLanguage = Stus[index].CLanguage;
			ptr[i] = &Clusters[i];
			printf("第%d簇初始化中心点:(%.6lf, %.6lf, %.6lf, %.6lf)\n", i+1, Clusters[i].Chinese, Clusters[i].Math, Clusters[i].English, Clusters[i].CLanguage);
		}
	}
}

//计算数据到各个簇中心的距离并且进行分类
void Calculator(struct Stu *s){
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
    Clusters[min_index].num++;
	ptr[min_index]->next = s;    //s地址0x0043222 struct Stu *Stus???
	ptr[min_index] = ptr[min_index]->next;
	ptr[min_index]->next = NULL;

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
	struct Stu *tmp = NULL;
	double newCenter[K][T]; //记录新的中心点
	for(int i = 0; i < K; ++i){
		double avgC = 0.0, avgM = 0.0, avgE = 0.0, avgCL = 0.0;
		tmp = Clusters[i].next;
		while(tmp){
			avgC += tmp->Chinese;
			avgM += tmp->Math;
			avgE += tmp->English;
			avgCL += tmp->CLanguage;
			tmp = tmp->next;
		}
		newCenter[i][0] = avgC/Clusters[i].num;
		newCenter[i][1] = avgM/Clusters[i].num;
		newCenter[i][2] = avgE/Clusters[i].num;
		newCenter[i][3] = avgCL/Clusters[i].num;
	}
	if(!IsClustOK(newCenter)){
		for(int i = 0; i < K; ++i){
			Clusters[i].Chinese = newCenter[i][0];
			Clusters[i].Math = newCenter[i][1];
			Clusters[i].English = newCenter[i][2];
			Clusters[i].CLanguage = newCenter[i][3];
			Clusters[i].next = NULL;
			Clusters[i].num = 0;
			ptr[i] = &Clusters[i];
		}
	}
	else{
		OutPut();
		printf("end of program ！\n");
		exit(0);
	}
}


int main(){
	LoadScoreData();  //读文件导入数据
	InitCluses();    //初始化簇类中心
	while(1){
		for(int i = 0; i < N; ++i){  // N=37 学生人数
			Calculator(&Stus[i]);   //计算37个点到4个簇的距离并归类
		};
		Average();    //计算新中心点，并判断是否满足条件结束循环
	}
	return 0;
}

