#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#define N  37 //ѧ������
#define K  4 //�������
#define T  4 //�ɼ�����



struct Stu{ //ѧ���ɼ��ṹ��
	int num;    //ѧ��
	double Chinese; //����
	double Math;   //��ѧ
	double English;  //Ӣ��
	double CLanguage; //C����
	struct Stu *next; //βָ��
};

struct Stu Stus[N];
struct Stu Clusters[K]; //4������
struct Stu *ptr[K] = {NULL,NULL,NULL,NULL}; //ָ��ÿ����������һ���ṹ��



//���������
void OutPut(){
	for(int i = 0; i < K; ++i){
		struct Stu *top = Clusters[i].next;
		printf("��%d��:\n���ĵ�:(%.6lf, %.6lf, %.6lf, %.6lf)\n��Ա����:%d\n��Ա:\n", i+1, Clusters[i].Chinese, Clusters[i].Math, Clusters[i].English, Clusters[i].CLanguage, Clusters[i].num);
		while(top){
			printf("\t\tѧ��:%d\t(%.6lf, %.6lf, %.6lf, %.6lf)\n", top->num, top->Chinese, top->Math, top->English, top->CLanguage);
			top = top->next;
		}
		printf("\n");
	}
}

//��������
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

//��ʼ�������ĵ�,�����N=37��������ѡȡK=4��������Ϊ��ʼ�����ĵ�,
//���ҽ���ʼ���ĵ����ݴ�ŵ�Clusters[i]���ݽṹ��
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
			Clusters[i].num = 0; //�ô�����
			Clusters[i].Chinese = Stus[index].Chinese;
			Clusters[i].Math = Stus[index].Math;
			Clusters[i].English = Stus[index].English;
			Clusters[i].CLanguage = Stus[index].CLanguage;
			ptr[i] = &Clusters[i];
			printf("��%d�س�ʼ�����ĵ�:(%.6lf, %.6lf, %.6lf, %.6lf)\n", i+1, Clusters[i].Chinese, Clusters[i].Math, Clusters[i].English, Clusters[i].CLanguage);
		}
	}
}

//�������ݵ����������ĵľ��벢�ҽ��з���
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
	}//�ҳ����ݵ������������ĵ���̵Ĵ���
    Clusters[min_index].num++;
	ptr[min_index]->next = s;    //s��ַ0x0043222 struct Stu *Stus???
	ptr[min_index] = ptr[min_index]->next;
	ptr[min_index]->next = NULL;

}

//�жϾ������
bool IsClustOK(double newCenter[K][T])
{
	double ext=0.000005;  //���ֵ
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

//����س�Ա��ƽ��ֵ
void Average(){
	struct Stu *tmp = NULL;
	double newCenter[K][T]; //��¼�µ����ĵ�
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
		printf("end of program ��\n");
		exit(0);
	}
}


int main(){
	LoadScoreData();  //���ļ���������
	InitCluses();    //��ʼ����������
	while(1){
		for(int i = 0; i < N; ++i){  // N=37 ѧ������
			Calculator(&Stus[i]);   //����37���㵽4���صľ��벢����
		};
		Average();    //���������ĵ㣬���ж��Ƿ�������������ѭ��
	}
	return 0;
}

