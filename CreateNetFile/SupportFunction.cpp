#include "stdafx.h"
#include "CreateNetFile.h"


//读取tab分隔的文件，返回值为行数;只能读取ANSI文件
//需要手动delete指针空间
//如果错误，返回-1

int ReadTabFile(char *fileLoc,char **&lines,int lineNum,int lineLen)
{
	clock_t start=clock();
	ifstream ifs(fileLoc);
	int currLineNum=0;//当前行数
	try{
		if(!ifs){
			cout<<"文件打开失败,请检查文件名是否为:";cout<<fileLoc<<"."<<endl;
			return -1;
		}
		lines=new char*[lineNum];//考虑到'\0'符号

		while(!ifs.eof() && currLineNum<lineNum)
		{
			lines[currLineNum]=new char[lineLen+1];
			ifs.getline(lines[currLineNum],lineLen+1);
			long char_num=strlen(lines[currLineNum]);
            for (long i=char_num-1; i>=0; i--) { //Process with \r\n on non-windows
                if (lines[currLineNum][i]!='\r') break;
                lines[currLineNum][i]='\0';
                char_num--;
            }
			if(strlen(lines[currLineNum])==lineLen){
				cout<<"在读取文件第"<<currLineNum+1<<"行的时候,字符长度超过限额:"<<lineLen<<"; 请扩大源码参数:"<<"LINE_LEN"<<endl;
				DeleteCharArray(lines,currLineNum);
				return -1;
			}
			currLineNum++;
		}

		clock_t end=clock();

		if(!ifs.eof())
		{
            cout<<"文件"<<fileLoc<<"行数超过最大限制:"<<lineNum<<",强烈推荐扩大源码参数:"<<"LINE_NUM"<<";目前读入行数为:"<<currLineNum<<";使用时间为"<<double(end-start)/CLOCKS_PER_SEC<<"秒,程序将继续执行"<<endl;
        }
		else
		{
            cout<<"文件"<<fileLoc<<"读取完毕,读入行数为:"<<currLineNum<<";使用时间为"<<double(end-start)/CLOCKS_PER_SEC<<"秒"<<endl;
        }

		ifs.close();
		return currLineNum;
	}
	catch(...){
		if(ifs.is_open()){ifs.close();}
		if(lines!=NULL){
			DeleteCharArray(lines,currLineNum);
		}
		cout<<"文件读取失败,程序运行终止."<<endl;
		return -1;
	}
};

//删除字符二维数组,正确返回0
int DeleteCharArray(char **lines,int lineNum)
{
	int i;
	if(lines)
	{
		for(i=0;i<lineNum;i++)
		{
			if(lines[i])
			{
				delete []lines[i];
			}
		}
		delete []lines;
	}
	//cout<<"二维数组删除成功"<<endl;
	return 0;
};

//删除整型二维数组,正确返回0；注意和删除二维字符数组不太一样
int DeleteIntArray(int **pArray,int lineNum)
{
	int i;
	if(pArray)
	{
		for(i=0;i<lineNum;i++)
		{
			if(pArray[i])
			{
				delete []pArray[i];
			}
		}
		delete []pArray;
	}
	//cout<<"二维数组删除成功"<<endl;
	return 0;
};

//读取所有行，并分裂词语,返回值为词语的个数
int SaveTimeMainProc(char **&wordAll,int **&matric,char **pLine,int lineNum,int lineLen,int wordAllNum,int wordLineNum)
{
	clock_t start=clock();

	int iLine=0;//当前行

	char **wordLine=new char*[wordLineNum];//每一行的词语数组
	int *position=new int[wordLineNum];

	wordAll=new char*[wordAllNum];
	matric=new int*[wordAllNum];

	for(int i=0;i<wordAllNum;i++)
	{
		matric[i]=new int[wordAllNum];
		for(int j=0;j<wordAllNum;j++)
		{
			matric[i][j]=0;
		}
	}

	//对于总词语数组，wordAllNum=当前词语个数
	//对于每一行词语数组，wordLineNum=当前词语个数
	wordAllNum=0;
	for(iLine=0;iLine<lineNum;iLine++)//遍历所有行
	{
		wordLineNum=SplitLine(pLine[iLine],wordLine,lineLen,wordLineNum,'\t');
		for(int i=0;i<wordLineNum;i++)
		{
			position[i]=FindWord(wordAll,wordAllNum,wordLine[i]);
			if(position[i]<0)//没有找到这个单词
			{
				wordAll[wordAllNum]=wordLine[i];
				position[i]=wordAllNum;
				wordAllNum++;
			}

		}
		FillMatric(matric,position,wordLineNum,wordAllNum);
	}
	//PrintWords(wordAll,wordAllNum);
	delete []wordLine;
	delete []position;

	clock_t end=clock();

	cout<<"网络矩阵生成完毕，使用时间为"<<double(end-start)/CLOCKS_PER_SEC<<"秒."<<endl;

	return wordAllNum;
};

//查找数组中的词语，返回-1代表没有找到；否则返回找到的位置
int FindWord(char **words,int num,const char *word)
{
	int i=0;
	for(i=0;i<num;i++)
	{
		if(!strcmp(words[i],word)){return i;}
	}
	return -1;
};

//将词语插入到数组中，如果存在，则返回位置
//如果遇到空单词，则停止；返回新插入的地点
int InsertWord(char * const *words,int arrNum,const char *word)
{
	int i=0;
	while(i<arrNum && words[i][0]!='\0')
	{
		if(!strcmp(words[i],word)){return i;}
		i++;
	}
	if(i==arrNum){cout<<"越界"<<endl;return -1;}
	if(i<arrNum-1)
	{
		strcpy_x(words[i],strlen(words[i])+1,word);
		//strcpy(words[i],word);
		words[i+1][0]='\0';
	}
	else
	{
		cout<<"词语数组已经到末尾,注意！"<<endl;
		strcpy_x(words[i],strlen(words[i])+1,word);
		//strcpy(words[i],word);
	}
	return i;
};

//输入为指针数组，此数组在外部已经申请了空间
//将行转换成数组，赋予指针数组；返回值为数组词语的个数
int SplitLine(char *pLine,char ** wordLine,int lineLen,int wordNumPerLine,char s)
{
	int i=0;//当前词语指针的数目
	int k=0;//对于pLine,游标位置
	for(i=0;i<wordNumPerLine;i++){wordLine[i]=NULL;}//初始化输入的单词数组
	i=0;
	char *tmpNewWord=pLine;//第一个word
	for(k=0;k<lineLen;k++)
	{
		if(pLine[k]=='\0')//说明到了行的末尾，标志一个词语的形成
		{
			//探测形成的词语是否重复
			if(strcmp(tmpNewWord,"") && FindWord(wordLine,i,tmpNewWord)<0)//没有找到了相同词并且词不为空
			{
				if (i<wordNumPerLine)
				{
					wordLine[i]=tmpNewWord;
					i++;
				}
				else{
					//cout<<"某一行的词语数超过限制:"<<wordNumPerLine<<"; 请扩大源码参数:"<<"ARC_PER_LINE"<<endl;
					return -1;
				}
			}
			break;
		}
		if(pLine[k]!=s)
		{
			if(k==0 || pLine[k-1]=='\0')
			{
				tmpNewWord=pLine+k;
			}
		}
		else//标志一个词语的形成
		{
			pLine[k]='\0';
			if(strcmp(tmpNewWord,"") && FindWord(wordLine,i,tmpNewWord)<0)//没有找到了相同词并且词不为空
			{
				if (i<wordNumPerLine)
				{
					wordLine[i]=tmpNewWord;
					i++;
				}
				else{
					//cout<<"某一行的词语数超过限制:"<<wordNumPerLine<<"; 请扩大源码参数:"<<"ARC_PER_LINE"<<endl;
					return -1;
				}
			}
		}

	}
	return i;
};

char *splitWeigth(char *pLine, int weights[], int count, char s = '\t') {
    int anchor = 0;
    if (strlen(pLine) == 0) {
        return nullptr;
    }
    char *name = nullptr;
    int index = -1;
    int weight = 0;
    while (true) {
        if (pLine[anchor] != s && pLine[anchor] != '\0') {
            if (index < 0) {
                anchor++;
            } else {
                weight = 10 * weight + pLine[anchor] - 48;
                anchor++;
            }
            continue;
        }
        if (index < 0) {
            pLine[anchor] = '\0';
            name = pLine;
        } else {
            weights[index] = weight;
            weight = 0;
        }
        index++;
        anchor++;
        if (index == count) {
            break;
        }
    }
    return name;
}

//打印词语数组
void PrintWords(char **words,int num)
{
	for(int i=0;i<num;i++)
	{
		cout<<words[i]<<'\t';
	}
	cout<<endl;
};

//使用整型数组填充网络矩阵
//但是matric必须初始化
int FillMatric(int **matric,int *position,int wordLineNum,int wordAllNum)
{
	for(int i=0;i<wordLineNum;i++)
	{
		for(int j=0;j<wordLineNum;j++)
		{
			matric[position[i]][position[j]]++;
		}
	}
	return 0;
};

void PrintMatric(int **matric,int num)
{
	for(int i=0;i<num;i++)
	{
		for(int j=0;j<num;j++)
		{
			cout<<matric[i][j];
		}
		cout<<endl;
	}
};

void WriteNetFile(char **&wordAll,int **&matric,int wordAllNum,char *fileLoc)
{
	clock_t start=clock();
	ofstream ofs(fileLoc);
	ofs<<"*Vertices "<<wordAllNum<<endl;//第一个文本段
	for(int i=0;i<wordAllNum;i++)
	{
		ofs<<i+1<<" \""<<wordAll[i]<<"\""<<endl;
	}

	ofs<<"*Edges"<<endl;//第二个文本段
	for(int i=0;i<wordAllNum;i++)
	{
		for(int j=0;j<i;j++)
		{
			if(matric[i][j]>0){ofs<<i+1<<" "<<j+1<<" "<<matric[i][j]<<endl;}
		}
	}
	ofs.close();
	clock_t end=clock();
	cout<<"写入.net文件完毕，文件路径为：";cout<<fileLoc;cout<<"使用时间为"<<double(end-start)/CLOCKS_PER_SEC<<"秒"<<endl;
};

void WriteFreqFile(char **&wordAll,int **&matric,int wordAllNum,char *fileLoc)
{
	clock_t start=clock();
	ofstream ofs(fileLoc);
	ofs<<"Order\tWord\tFreqence"<<endl;//第一个文本段
	for(int i=0;i<wordAllNum;i++)
	{
		ofs<<i+1<<"\t"<<wordAll[i]<<"\t"<<matric[i][i]<<endl;
	}

	ofs.close();
	clock_t end=clock();
	cout<<"写入词频文件完毕，文件路径为：";cout<<fileLoc;cout<<"使用时间为"<<double(end-start)/CLOCKS_PER_SEC<<"秒."<<endl;
}

//读取所有行，并分裂词语,返回值为词语的个数
int BalanceMainProc(char **lines,int lineNum,Arc* &arcs,Edge* &edges,int &setArcNum,int &setEdgeNum,int lineLen,int setArcPerLine,int setArcHasEdge)
{
	map<char *,int,cmp_str> arcMap;//点的map
	arcMap.clear();
	map<Edge*,int,cmp_edge> edgeMap;//边的map
	edgeMap.clear();

	clock_t start=clock();
	arcs=new Arc[setArcNum];
	edges=new Edge[setEdgeNum];
	int iEdge=0;
	int iArc=0;
	int iLine=0;
	char **wordLine=new char*[setArcPerLine];
	int *position=new int[setArcPerLine];
	for(iLine=0;iLine<lineNum;iLine++)//遍历所有行
	{		
		if((iLine+1)%1000==0)
		{
			cout<<"已经处理了"<<iLine+1<<"行..."<<endl;
		}
		int wordLineNum=SplitLine(lines[iLine],wordLine,lineLen,setArcPerLine,'\t');
		if(wordLineNum==-1){
			cout<<"文件第"<<iLine+1<<"行的词语数目超过限额:"<<setArcPerLine<<"; 请扩大源码参数:"<<"ARC_PER_LINE"<<endl;
			delete []wordLine;
			wordLine=NULL;
			delete []position;
			position=NULL;
			return -1;
		}

		for(int i=0;i<wordLineNum;i++)
		{
			//position[i]=FindArc(arcs,iArc,wordLine[i]);
			position[i]=FindArc(arcMap,wordLine[i]);
			if(position[i]<0)//没有找到这个单词,将此单词添加成节点
			{
				if(iArc>=setArcNum){
					cout<<"从第"<<iLine+1<<"行开始超出最大节点数:"<<setArcNum<<";请扩大源码参数:"<<"ARC_NUM"<<endl;
					delete []wordLine;
					wordLine=NULL;
					delete []position;
					position=NULL;
					return -1;
				}
				arcs[iArc].edgeIndex=NULL;//尚未存在边
				arcs[iArc].edgeIndexNum=0;
				arcs[iArc].word=wordLine[i];
				position[i]=iArc;
				arcMap[wordLine[i]]=iArc;
				iArc++;
			}
		}
		//点集合，边集合，点数，边数，点的位置数组，位置数组的个数，点已经拥有的边数
		if(FillEdge(arcs,edges,iArc,iEdge,setEdgeNum,position,wordLineNum,setArcHasEdge,false,edgeMap)==-1){//必然会找到这些节点
			cout<<"在文件第"<<iLine+1<<"行中，处理边的时候出错,程序将终止."<<endl;
			delete []wordLine;
			wordLine=NULL;
			delete []position;
			position=NULL;
			return -1;
		}
	}
	setArcNum=iArc;
	delete []wordLine;
	wordLine=NULL;
	delete []position;
	position=NULL;
	setEdgeNum=iEdge;
	clock_t end=clock();

	cout<<"网络构建完毕，使用时间为"<<double(end-start)/CLOCKS_PER_SEC<<"秒."<<endl;
	return iArc;
}

/*
 * @param lines, 文本行
 * @param lineNum, 行数
 * @parma arcs, 顶点集合
 * @parma edges, 边集合
 * @param setArcNum, 顶点个数
 * @param setEdgeNum, 边个数
 * @param lineLen, 行最大长度
 */
int BalanceMatrixProc(char **lines,int lineNum,Arc* &arcs,Edge* &edges,int &setArcNum,int &setEdgeNum,int lineLen)
{
    setArcNum = lineNum - 1;
    setEdgeNum = EDGE_NUM; // 待定
    
    map<char *,int,cmp_str> arcMap;//点的map
    arcMap.clear();
    map<Edge*,int,cmp_edge> edgeMap;//边的map
    edgeMap.clear();
    
    clock_t start=clock();
    
    arcs=new Arc[setArcNum];
    char **wordLine=new char*[setArcNum];
    cout << "遍历首行" << endl;
    int row = 0;
    char *currentLine = lines[row];
    int wordLineNum = SplitLine(currentLine, wordLine, lineLen, setArcNum+1, '\t');
    setArcNum = wordLineNum;
    for (int i = 0; i < wordLineNum; i++) {
        Arc *arc = arcs + i;
        arc->word = wordLine[i];
        arc->edgeIndex = nullptr;
        arc->edgeIndexNum = 0;
        arcMap[arc->word] = i;
    }
    
    edges=new Edge[setEdgeNum];
    int *weights = new int(setArcNum);
    int edgeCount = 0;
    for (; row < lineNum-1; row++) {
        if((row+1)%1000==0)
        {
            cout<<"已经处理了"<<row+1<<"行..."<<endl;
        }
        currentLine = lines[row+1];
        if (strlen(currentLine) == 0) {
            break;
        }
        char *name = splitWeigth(currentLine, weights, setArcNum);
        for (int j = 0; j <= row; j++) {
            int weight = weights[j];
            if (weight <= 0) {
                continue;
            }
            Edge &edge = edges[edgeCount];
            int begin = arcMap[name];
            edge.arcBegin = begin;
            edge.arcEnd = j;
            edge.value = weights[j];
            edgeCount++;
        }
    }
    delete weights;
    
    delete []wordLine;
    wordLine=NULL;
    
    setEdgeNum = edgeCount;
    
    clock_t end=clock();

    cout<<"网络构建完毕，使用时间为"<<double(end-start)/CLOCKS_PER_SEC<<"秒."<<endl;
    
    return 0;
}

int FindArc(Arc *arcs,int num,const char *word)
{
	int i=0;
	for(i=0;i<num;i++)
	{
		if(!strcmp(arcs[i].word,word)){return i;}
	}
	return -1;
}
int FindArc(map<char *,int,cmp_str> &arcMap,char *word){
	map<char * ,int,cmp_str>::iterator it=arcMap.find(word);
	if(it==arcMap.end()){

		return -1;
	}
	else{

		return int(it->second);

	}
}
int FillEdge(Arc *arcs,Edge *edges,int arcNum,int &edgeNum,int *position,int wordLineNum,int setArcHasEdge,bool direct)//direct 为1时，代表有向；为0时，无向
{//点集合，边集合，点数，边数，点的位置数组，位置数组的个数，点已经拥有的边数
	for(int i=0;i<wordLineNum;i++)
	{
		Arc *temArc=&arcs[position[i]];//当前节点
		if(temArc->edgeIndexNum==0)//这一行存在共线，而且节点的边索引尚未构建;如果只有一个节点，则认为它和自己共线
		{
			temArc->edgeIndex=new Edge *[setArcHasEdge];
			temArc->edgeIndexNum=0;
		}
		if(!direct)//如果是无向网络
		{
			for(int j=i;j<wordLineNum;j++)
			{
				int k;
				//这一块代码的作用：判断某一个边是否存在
				for(k=0;k<temArc->edgeIndexNum;k++)
				{
					if((temArc->edgeIndex[k]->arcBegin==position[i] && temArc->edgeIndex[k]->arcEnd==position[j]) ||
						(temArc->edgeIndex[k]->arcEnd==position[i] && temArc->edgeIndex[k]->arcBegin==position[j]) )//已经存在这条边了
					{
						temArc->edgeIndex[k]->value++;
						break;
					}
				}

				if(k==temArc->edgeIndexNum)//如果目前还不存在这条边
				{
					edges[edgeNum].arcBegin=position[i];
					edges[edgeNum].arcEnd=position[j];
					edges[edgeNum].value=1;

					//填充索引
					temArc->edgeIndex[temArc->edgeIndexNum]=&edges[edgeNum];
					temArc->edgeIndexNum++;

					if(i!=j)
					{
						if(arcs[position[j]].edgeIndexNum==0)//后者还没有索引
						{
							arcs[position[j]].edgeIndex=new Edge*[setArcHasEdge];
							arcs[position[j]].edgeIndexNum=0;
						}
						arcs[position[j]].edgeIndex[arcs[position[j]].edgeIndexNum]=&edges[edgeNum];
						arcs[position[j]].edgeIndexNum++;
					}
					edgeNum++;
				}

			}
		}
		else//这是有向网络
		{
			cout<<"有向网络的处理方法还没有确定"<<endl;
		}
	}
	return 0;
}
int FillEdge(Arc *arcs,Edge *edges,int arcNum,int &edgeNum,int edgeMaxNum,int *position,int wordLineNum,int setArcHasEdge,bool direct,map<Edge*,int,cmp_edge> &edgeMap)//direct 为1时，代表有向；为0时，无向
{//点集合，边集合，点数，边数，点的位置数组，位置数组的个数，点已经拥有的边数
	for(int i=0;i<wordLineNum;i++)
	{
		Arc *temArc=&arcs[position[i]];//当前节点
		if(temArc->edgeIndexNum==0)//这一行存在共线，而且节点的边索引尚未构建;如果只有一个节点，则认为它和自己共线
		{
			temArc->edgeIndex=new Edge *[setArcHasEdge];
			temArc->edgeIndexNum=0;
		}
		if(!direct)//如果是无向网络
		{
			for(int j=i;j<wordLineNum;j++)
			{
				int k;
				//这一块代码的作用：判断某一个边是否存在
				Edge tmpEdge={position[i],position[j],1};
				map<Edge*,int,cmp_edge>::iterator it;
				it=edgeMap.find(&tmpEdge);
				if(it!=edgeMap.end())//有找到相同边
				{
					k=int(it->second);
					edges[k].value++;
				}
				else//没有找到相同边
				{
					if(edgeNum>=edgeMaxNum){
						cout<<"边的个数超过限额:"<<edgeMaxNum<<"; 请扩大源码参数:"<<"EDGE_NUM"<<endl;
						return -1;
					}
					edges[edgeNum].arcBegin=position[i];
					edges[edgeNum].arcEnd=position[j];
					edges[edgeNum].value=1;
					edgeMap[&edges[edgeNum]]=edgeNum;
					//填充索引
					if(temArc->edgeIndexNum>=setArcHasEdge){
						cout<<"节点\""<<temArc->word<<"\"拥有边的个数超过限额:"<<setArcHasEdge<<"; 请扩大源码参数:"<<"ARC_HAS_EDGE"<<endl;
						return -1;
					}
					temArc->edgeIndex[temArc->edgeIndexNum]=&edges[edgeNum];
					temArc->edgeIndexNum++;

					if(i!=j)
					{
						if(arcs[position[j]].edgeIndexNum==0)//后者还没有索引
						{
							arcs[position[j]].edgeIndex=new Edge*[setArcHasEdge];
							arcs[position[j]].edgeIndexNum=0;
						}
						if(arcs[position[j]].edgeIndexNum>=setArcHasEdge){
							cout<<"节点\""<<arcs[position[j]].word<<"\"拥有边的个数超过限额:"<<setArcHasEdge<<"; 请扩大源码参数:"<<"ARC_HAS_EDGE"<<endl;
							return -1;
						}
						arcs[position[j]].edgeIndex[arcs[position[j]].edgeIndexNum]=&edges[edgeNum];
						arcs[position[j]].edgeIndexNum++;
					}
					edgeNum++;
				}
			}
		}
		else//这是有向网络
		{
			cout<<"有向网络的处理方法还没有确定."<<endl;
		}
	}
	return 0;
}
void DeleteArcs(Arc *arcs,int arcNum,int setArcHasEdge)
{
	if(arcs!=NULL){
		for(int i=0;i<arcNum;i++)
		{
			if(arcs[i].edgeIndexNum>0)
			{
				delete [] arcs[i].edgeIndex;
			}
		}
		delete [] arcs;
	}
	cout<<"节点销毁完毕."<<endl;
}

void WriteArcNetFile(char *fileLoc,Arc *arcs,int arcNum,Edge *edges,int edgeNum)
{
	clock_t start=clock();
	ofstream ofs(fileLoc);
	ofs<<"*Vertices "<<arcNum<<endl;//第一个文本段
	for(int i=0;i<arcNum;i++)
	{
		ofs<<i+1<<" \""<<arcs[i].word<<"\""<<endl;
	}

	ofs<<"*Edges"<<endl;//第二个文本段
	for(int i=0;i<edgeNum;i++)
	{
		if(edges[i].arcBegin!=edges[i].arcEnd )
		{
			ofs<<edges[i].arcBegin+1<<" "<<edges[i].arcEnd+1 <<" "<<edges[i].value<<endl;
		}
	}
	ofs.close();
	clock_t end=clock();
	cout<<"写入.net文件完毕,文件路径为:";cout<<fileLoc;cout<<";使用时间为"<<double(end-start)/CLOCKS_PER_SEC<<"秒."<<endl;
};

void WriteArcFreqFile(char *fileLoc,Arc *arcs,int arcNum,Edge *edges,int edgeNum)
{
	clock_t start=clock();
	ofstream ofs(fileLoc);
	ofs<<"Order\tWord\tFreqence"<<endl;//第一个文本段
	for(int i=0;i<edgeNum;i++)
	{
		if(edges[i].arcBegin==edges[i].arcEnd)
		{
			ofs<<edges[i].arcBegin+1<<"\t"<<arcs[edges[i].arcBegin].word<<"\t"<<edges[i].value<<endl;
		}
	}

	ofs.close();
	clock_t end=clock();
	cout<<"写入词频文件完毕,文件路径为:";cout<<fileLoc;cout<<";使用时间为"<<double(end-start)/CLOCKS_PER_SEC<<"秒."<<endl;
}
