// CreateNetFile.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CreateNetFile.h"

struct globalArgs_t {
    char *input;                    /* -f option */
    char *output_freq;				/* - option */
    char *output_net;				/* - option */
} globalArgs;

static const char *optString = "f:";

void SaveTimeProc(); //费空间方案，使用二维整型数组来存储边权值
bool BalanceProc();  //平衡方案，使用边结构体数组存储
void orgnizeParameters(int argc, _TCHAR* const argv[]);

char *const d_inputFilePath = "./tab_file_test.txt";  // input file path
char *const d_freqFilePath  = "./FreqFile.txt"; // 注：纯C++没有创建目录的方法
char *const d_netFilePath   = "./NetFile.net";  //

int MainX(int argc, _TCHAR* argv[])
{
    orgnizeParameters(argc, argv);
	clock_t start=clock();
	bool status=BalanceProc();
	clock_t end=clock();
	cout<<"程序运行结束,总共使用时间为"<<double(end-start)/CLOCKS_PER_SEC<<"秒."<<endl;
	if(!status)
	{
		cout<<CONTACT<<endl;
	}
	system("pause");

	return 0;
}

void orgnizeParameters(int argc, _TCHAR* const argv[])
{

	globalArgs.input = d_inputFilePath;
	globalArgs.output_freq = d_freqFilePath;
	globalArgs.output_net = d_netFilePath;

#ifndef PLATFORM_WINNT
	// cout<<argc<<"\n"<<argv<<endl;
	int opt = getopt(argc, argv, optString);
	while (opt != -1) {
		switch (opt) {
		case 'f':
			globalArgs.input = optarg;
			break;

		default:
			break;
		}
		opt = getopt(argc, argv, optString);
	}
#endif // !PLATFORM_WINNT

}

bool BalanceProc()
{
	char **lines=NULL;
	int lineNum=ReadTabFile(globalArgs.input,lines);//记得之后需要delete []lines.
	if(lineNum==-1){//没有成功读取文件
		return false;
	}
	Arc *arcs=NULL;
	Edge *edges=NULL;

	int arcNum=ARC_NUM;
	int edgeNum=EDGE_NUM;

	if(BalanceMainProc(lines,lineNum,arcs,edges,arcNum,edgeNum)!=-1){
		WriteArcFreqFile(globalArgs.output_freq,arcs,arcNum,edges,edgeNum);
		WriteArcNetFile(globalArgs.output_net,arcs,arcNum,edges,edgeNum);
		DeleteArcs(arcs,arcNum);
		delete []edges;
        cout<<"边销毁完毕."<<endl;
		DeleteCharArray(lines,lineNum);
		return true;
	}
	else{
		DeleteCharArray(lines,lineNum);
		return false;
	}
}
void SaveTimeProc()//省时间的处理方案
{
	char **lines=NULL;
	int lineNum=ReadTabFile(globalArgs.input,lines);

	int **matric=NULL;
	char **wordAll;//这是指针数组,指向的是lines的内容，所以1级指针目标不需要单独再delete
	int wordAllNum=ARC_NUM;
	wordAllNum=SaveTimeMainProc(wordAll,matric,lines,lineNum,wordAllNum);

	//PrintMatric(matric,wordAllNum);
	WriteNetFile(wordAll,matric,wordAllNum,globalArgs.output_net);
	WriteFreqFile(wordAll,matric,wordAllNum,globalArgs.output_freq);

	DeleteCharArray(lines,lineNum);
	DeleteIntArray(matric,wordAllNum);
	delete []wordAll;
}