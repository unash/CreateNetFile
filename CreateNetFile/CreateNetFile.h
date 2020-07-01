#include "stdafx.h"
#include <string.h>
#include <cstring>
#include <map>
//通用全局变量
const int LINE_LEN=5000;//文件的一行有200字节
const int LINE_NUM=1000000;//文件的最大行数
const int ARC_NUM=300000;//最大的节点个数
const int ARC_HAS_EDGE=15000;//一个节点拥有的最大的边数
const int ARC_PER_LINE=3000;//一行存在的节点数
const int EDGE_NUM=1000000;

const char COPYRIGHT[100]="作者: blog.exbye.com.";
const char CONTACT[100]="寻求帮助,请邮件至: admin@exbye.com.";
/*
如果是返回个数，则从1开始计数
如果是返回位置，则从0开始计数
*/

/*
此程序是典型的拿空间换时间的方案，是为了简化程序
但是当行数比较大的时候，消耗的内存上G
如果不使用空间换时间方案，也可以通过索引来降低计算复杂性；彼时则是一堆的指针操作。
*/

struct Edge;

struct Arc
{
	Edge **edgeIndex;//指向以这个节点为开始的边的指针的数组
	int edgeIndexNum;//数组个数
	char *word;//词语
};

struct Edge
{
	int arcBegin;//开始节点
	int arcEnd;//结束节点
	int value;//权值
};

struct cmp_str 
{
	bool operator()(char const *a, char const *b) const
	{
		return std::strcmp(a, b) < 0;
	}
};

struct cmp_edge//判断两条边是否相同 
{
	bool operator()(Edge const *a, Edge const *b) const
	{
		if(a->arcBegin>b->arcBegin){
			return true;
		}
		else if(a->arcBegin<b->arcBegin)
		{
			return false;
		}
		else if(a->arcEnd>b->arcEnd ){
			return true ;
		}
		else if(a->arcEnd<b->arcEnd){
			return false ;
		}
		else{
			return false;
		}
	}
};

//通用函数部分
int ReadTabFile(char *fileLoc,char **&pLine,int lineNum=LINE_NUM,int lineLen=LINE_LEN);
int SplitLine(char *pLine,char ** wordLine,int lineLen=LINE_LEN,int wordNumPerLine=ARC_PER_LINE,char s='\t');

//SaveTime方案部分
int SaveTimeMainProc(char **&wordAll,int **&matric,char **pLine,int lineNum=LINE_NUM,int lineLen=LINE_LEN,int wordNum=ARC_NUM,int wordNumPerLine=ARC_PER_LINE);
int FindWord(char **words,int num,const char *word);//从字符串数组中查找字符串是否存在
int FillMatric(int **matric,int *position,int wordLineNum,int wordAllNum);

int DeleteCharArray(char **pArray,int lineNum);
int DeleteIntArray(int **pArray,int lineNum);

void PrintWords(char **words,int num);
void PrintMatric(int **matric,int num);
void WriteNetFile(char **&wordAll,int **&matric,int wordAllNum,char *fileLoc);
void WriteFreqFile(char **&wordAll,int **&matric,int wordAllNum,char *fileLoc);

//Balance方案部分
int BalanceMainProc(char **lines,int lineNum,Arc* &arcs,Edge* &edges,int &setArcNum,int &setEdgeNum,int lineLen=LINE_LEN,int setArcPerLine=ARC_PER_LINE,int setArcHasEdge=ARC_HAS_EDGE);
int BalanceMatrixProc(char **lines,int lineNum,Arc* &arcs,Edge* &edges,int &setArcNum,int &setEdgeNum,int lineLen);
int FindArc(Arc *arcs,int num,const char *word);//从Arc数组中查找字符串是否存在
int FindArc(map<char *,int,cmp_str> &arcMap,char *word);
int FillEdge(Arc *arcs,Edge *edges,int arcNum,int &edgeNum,int edgeMaxNum,int *position,int wordLineNum,int setArcHasEdge,bool direct,map<Edge*,int,cmp_edge> &edgeMap);//direct 为1时，代表有向；为0时，无向
int FillEdge(Arc *arcs,Edge *edges,int arcNum,int &edgeNum,int *position,int wordLineNum,int setArcHasEdge,bool direct);
void DeleteArcs(Arc *arcs,int arcNum,int setArcHasEdge=ARC_HAS_EDGE);

void WriteArcFreqFile(char *fileLoc,Arc *arcs,int arcNum,Edge *edges,int edgeNum);
void WriteArcNetFile(char *fileLoc,Arc *arcs,int arcNum,Edge *edges,int edgeNum);