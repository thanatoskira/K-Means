# K-Means
针对全班37个人(N)，每人4门课程成绩(T)，利用K-Means算法将其分成4类(K)


算法介绍
========

* 从N个文档随机选取K个文档作为质心
* 对剩余的每个文档测量其到每个质心的距离，并把它归到最近的质心的类
* 重新计算已经得到的各个类的质
* 迭代2～3步直至新的质心与原质心相等或小于指定阈值，算法结束

数据结构
========

![image](https://github.com/thanatoskira/K-Means/blob/master/KMeans-one/K-Means数据结构.jpg)

#K-Means入侵检测技术实现

* 语言: C++
* 输入数据: KDD Cup1999

##K-Means算法
* 基于划分的分类方法
    * 划分的步骤
        * 通过分割的方式将n个元素或者对象所组成的数据集,通过某种特定方式将其划分成k个部分,每一个部分称为一个簇,并且要求 k <= n
    * 满足条件
        * 每个簇中至少包含一个对象
        * 每个对象有且只能属于一个簇

##初始聚类中心的选取
* 数据结构属性
    * 根据 KDD Cup1999 所给的数据当中,我们将选取如下属性参与到聚类算法:
    ```c++
    typedef struct STRMYRECORD{
        public:
        BYTE iProtocolType; //02 协议类型
        BYTE iService;      //03服务类型
        BYTE iStatusFlag;   //04状态标志
        int iSrcBytes;      //05源到目的字节数
        int iDestBytes;     //06目的到源字节数 
        int iFailedLogins;  //11登录失败次数
        int iNumofRoot;     //16root用户权限存取次数
        int iCount;         //232秒内连接相同主机数目
        int iSrvCount;      //242秒内连接相同端口数目
        BYTE iRerrorRate;   //27"REJ"错误的连接数比例
        BYTE iSameSrvRate;  //29连接到相同端口数比例
        BYTE iDiffSrvRate;  //30连接到不同端口数比例
        int iDstHostSrvCount;   //33相同目的地相同端口连接数
        BYTE iDstHostSameSrvRate;   //34相同目的地相同端口连接数比例
        BYTE iDstHostDiffSrvRate;   //35相同目的地不同端口连接数比例
        BYTE iDstHostSameSrcPortRate;   //36相同目的地相同源端口连接比例
        BYTE iDstHostSrvDiffHostRate;   //37不同主机连接相同端口比例
        BYTE iDstHostSrvSerrorRate;     //39连接当前主机有S0错误的比例
        BYTE iLabel;        //42类型标签
    }strMyRecord;
    ```

* K值的选取
    * K值 = 数据类型种类个数(iLabel 属性种类个数)

* 相似度的计算
    * 采用“欧氏距离”公式
        * $$ d(i,j) = sqrt{} $$
        * 其中对象i = (x<sub>i1</sub>, x<sub>i2</sub>, ... , x<sub>ip</sub>);j = (y<sub>j1</sub>, y<sub>j2</sub>, ... , y<sub>jp</sub>)

Note
=======

* 20150701

    * 存在的缺陷:
        * 数据结构过于复杂
        * Cluster可直接使用Score替代，从而Flag可省去
        * ptr可以不经过Flag的判断直接初始化
        * IsClustOk()函数处写的过于复杂，可以自行修改(本文仅作为参考)

* 20150721

    * 代码已经重新修整

* 20160611

