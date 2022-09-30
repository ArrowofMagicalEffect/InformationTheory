/*
Author:Ju5tice
Date:2022/9/22
*/
#include<stdio.h>
#include<string.h>
#define TRUE 1
#define FALSE 0
#define LEN 27
#define MAXNUM 200
#define codeLEN 50
#define word 50
#pragma warning(disable:4996)

const char leftCode = '0';
const char rightCode = '1';

int useableCount = LEN;

typedef struct HTNode{
	char data;//每个节点中存储的字符
	float freq;//符号对应的概率
	struct HTNode* lchild, * rchild, * parent;//左子、右子、父节点
	bool canSearch;//是否能够被搜到
	char code[codeLEN];//从父节点拿到的比特
	char check;//保存
}HTNode;

const float prob_dis[LEN] =
{ 0.2,0.105,0.071,0.0644,0.063,0.059,0.054,0.053,0.052,
0.047,0.035,0.029,0.023,0.0221,0.0225,0.021,0.0175,0.012,
0.012,0.011,0.0105,0.008,0.003,0.002,0.001,0.001,0.001 };

const char alphabet[LEN] =
{ ' ','E','T','O','A','N','I','R','S',
'H','D','L','C','F','U','M','P','Y',
'W','G','B','V','K','X','J','Q','Z' };

void initHuffmanNode(HTNode *NodeArray, int n);

void changeNode(HTNode* NodeArray, char data, float freq, HTNode* lchild,
	HTNode* rchild, HTNode* parent, bool canSearch);

void codeOrder(HTNode* NodeArray, int code);

HTNode* findMin(HTNode* NodeArray);

void linkHuffmanNode(HTNode* NodeArray);

int statisticNode(HTNode* NodeArray);

int findRoot(HTNode* NodeArray);

void makeHuffmanCode(HTNode* NodeArray);

void preOrderTraverse(HTNode* Node);

void arrayCopy(HTNode* NodeChild, HTNode* NodeParent);

void showCodeTable(HTNode* NodeArray);

void getWord(HTNode* NodeArray, char* Words, int n);

void getWordCode(HTNode* NodeArray, char* WordsCode, int n);

void DeCode(HTNode* NodeArray, char* str);

void main(void)
{
	HTNode aboveNode[MAXNUM];//创造一个aboveNode数组，内含MAXNUM个空白节点

	initHuffmanNode(aboveNode, MAXNUM);//将MAXNUM个结构体全部初始化
	
	for (int i = 0; i < LEN; i++)//对27个符号初始化
	{
		changeNode((aboveNode + i), alphabet[i], prob_dis[i], NULL, NULL, NULL, TRUE);
	}

	while (useableCount >= 2)
	{
		linkHuffmanNode(aboveNode);//节点制造
	}
	/*
	for (int i = 0; i < 70; i++)
	{
		printf("%c %lf %x %x %x %d %c %c\n", (aboveNode + i)->data, (aboveNode + i)->freq, (aboveNode + i)->lchild,
			(aboveNode + i)->rchild, (aboveNode + i)->parent, (aboveNode + i)->canSearch, (aboveNode + i)->code[0], (aboveNode + i)->check);
	}
	*/
	makeHuffmanCode(aboveNode);

	showCodeTable(aboveNode);

	//------------------------------------------码字分配完毕，下面开始获取输入，进行编码
	char Words[word];
	char WordsCode[word * codeLEN];
	
	getWord(aboveNode, Words, word);
	//getWord(aboveNode, Words, word);
	getWordCode(aboveNode, WordsCode, word * codeLEN);
}

void initHuffmanNode(HTNode *NodeArray, int n)
{
	for (int i = 0; i < n; i++)
	{
		(NodeArray + i)->data = '*';
		(NodeArray + i)->freq = i;
		(NodeArray + i)->lchild = (NodeArray + i)->rchild = (NodeArray + i)->parent = NULL;
		(NodeArray + i)->canSearch = FALSE;
		for(int j=0;j<codeLEN;j++)
			(NodeArray + i)->code[j] = '\0';
		(NodeArray + i)->check = '\0';
	}
}

void changeNode(HTNode* NodeArray, char data, float freq, HTNode* lchild,
	HTNode* rchild, HTNode* parent, bool canSearch)
{
	NodeArray->canSearch = canSearch;
	NodeArray->data = data;
	NodeArray->freq = freq;
	NodeArray->lchild = lchild;
	NodeArray->rchild = rchild;
	NodeArray->parent = parent;
}

void codeOrder(HTNode* NodeArray, char code)
{
	/*
	int temp = 0;
	for (int i = 0; i < codeLEN; i++)
	{
		if (NodeArray->code[i] == '\0')
			temp = i;
		break;
	}
	*/
	NodeArray->code[0] = code;//没毛病嗷
	//NodeArray->code[1] = '\0';
}

HTNode* findMin(HTNode* NodeArray)//找到所有节点中freq最小值，并且置其为不可搜索
{
	float min = 10.0;
	int minIndex = -1;
	for (int i = 0; i < MAXNUM; i++)
	{
		if (((NodeArray + i)->freq <= min)&& ((NodeArray + i)->canSearch == TRUE))
		{
			min = (NodeArray + i)->freq;
			minIndex = i;
		}
	}
	(NodeArray + minIndex)->canSearch = FALSE;
	//printf("找到最小值为%lf，索引为%d，对应字符%c\n", min, minIndex,(NodeArray + minIndex)->data);
	return (NodeArray + minIndex);
}

void linkHuffmanNode(HTNode* NodeArray)//链接两个子节点，以及初始化并链接一个父节点
{
	HTNode* minNode1 = findMin(NodeArray);
	HTNode* minNode2 = findMin(NodeArray);
	int useableIndex = -1;

	for (int i = 0; i < MAXNUM; i++)//找一个未用节点
	{
		if ((NodeArray + i)->data == '*')
		{
			useableIndex = i;
			break;//已经发现了索引为i的节点还没有被使用
		}
	}
	if (useableIndex > 0)//已经创建了一个新的父节点并且链接了两个子节点
	{
		changeNode((NodeArray + useableIndex), '#', minNode1->freq + minNode2->freq, minNode1, minNode2, NULL, TRUE);
		minNode1->parent = NodeArray + useableIndex;
		codeOrder(minNode1, rightCode);
		minNode1->check = rightCode;

		minNode2->parent = NodeArray + useableIndex;
		codeOrder(minNode2, leftCode);
		minNode2->check = leftCode;
		useableCount--;
	}

}

int statisticNode(HTNode* NodeArray)
{
	int count = 0;
	for (int i = 0; i < MAXNUM; i++)
	{
		if ((NodeArray + i)->canSearch = TRUE)
			count++;
	}
	return count;
}

int findRoot(HTNode* NodeArray)//返回根节点的索引下标
{
	int root = -1;
	for (int i = 0; i < MAXNUM; i++)//找一个未用节点
	{
		if ((NodeArray + i)->data == '#')
		{
			root = i;
		}
	}
	return root;
}

void makeHuffmanCode(HTNode* NodeArray)
{
	int root = findRoot(NodeArray);
	preOrderTraverse(NodeArray + root);
}

void preOrderTraverse(HTNode* Node)//先序遍历所有元素，找出leaf节点予以赋值
{
	
	if (Node->parent != NULL&&Node->parent->parent!=NULL)//不为根节点
	{
		arrayCopy(Node, Node->parent);
	}
	
	if (Node->lchild != NULL && Node->rchild != NULL)
	{
		//printf("先序遍历中，此时的字符为：%c，概率为%lf，编码(12形式)为%ld\n", Node->data,Node->freq, Node->code);
		
		preOrderTraverse(Node->lchild);
		preOrderTraverse(Node->rchild);
	}
	else//leaf节点
	{
		/*
		//Node->code += (10 * (Node->parent)->code);
		printf("已遍历到字符：%c，概率为%lf，而编码为：", Node->data, Node->freq);
		//for (int i = 0; Node->code[i] != '\0'; i++)
		for (int i = 0; Node->code[i] != '\0'; i++)
		{
			printf("%c ", Node->code[i]);
		}
		printf("\n");
		*/
	}
}

void arrayCopy(HTNode* NodeChild, HTNode* NodeParent)
{
	//获取两个数组的\0的索引位置
	int temp0 = 0, temp1 = 0;
	for (int i = 0; NodeChild->code[i] != '\0'; i++)
		temp0 = i + 1;//NodeChild中\0索引位置
	for (int i = 0; NodeParent->code[i] != '\0'; i++)
		temp1 = i + 1;//NodeParent中\0索引位置

	char tempArray[codeLEN];//中间变量数组

	for (int i = 0; i < codeLEN; i++)
	{
		tempArray[i] = NodeParent->code[i];
	}
	for (int i = 0,j = temp1; j < codeLEN&&NodeChild->code[i]!='\0'; i++,j++)
	{
		tempArray[j] = NodeChild->code[i];
	}
	
	for (int i = 0; i<codeLEN; i++)
	{
		NodeChild->code[i] = tempArray[i];
	}
}

void showCodeTable(HTNode* NodeArray)
{
	printf("           Huffman树码表\n-------------------------------------\n");
	printf("    符号     概率       码字\n");
	for (int i = 0; i < LEN; i++)
	{
		printf("%6c %11.4lf      ",(NodeArray+i)->data,(NodeArray+i)->freq);
		for (int j = 0; (NodeArray + i)->code[j] != '\0'; j++)
			printf("%c", (NodeArray + i)->code[j]);
		printf("\n");
	}
}

void getWord(HTNode* NodeArray,char* Words, int n)
{
	for (int i = 0; i < n; i++)
	{
		Words[i] = '\0';
	}
	printf("-------------------------------------\n\n输入英文字符(含空格):");
	int temp = 0;
	char ch;
	while ((ch = getchar()) != '\n')
		Words[temp++] = ch;
	printf("\"%s\"的编码结果是", Words);
	for (int i = 0; Words[i] != '\0'; i++)
	{
		for (int j = 0; j < LEN; j++)
		{
			if ((NodeArray + j)->data == Words[i] || (NodeArray + j)->data == (Words[i] - 32))
			{
				printf("%s", (NodeArray + j)->code);
				break;
			}
		}
	}
	putchar('\n');
}

void getWordCode(HTNode* NodeArray, char* WordsCode, int n)
{
	for (int i = 0; i < n; i++)
	{
		WordsCode[i] = '\0';
	}
	printf("\n-------------------------------------\n\n输入比特序列:");
	int temp = 0;
	char ch;
	while ((ch = getchar()) != '\n')
		WordsCode[temp++] = ch;
	printf("\"%s\"的译码结果是:", WordsCode);

	DeCode(NodeArray, WordsCode);
}

void DeCode(HTNode* NodeArray, char* str)
{
	int root = findRoot(NodeArray);
	HTNode* NodeArrayRoot = NodeArray + root;//保存根节点
	NodeArray += root;
	//printf("Root节点 %c %lf %x %x %x %d %c %c\n", (NodeArray)->data, (NodeArray)->freq, (NodeArray)->lchild,
	//	(NodeArray)->rchild, (NodeArray)->parent, (NodeArray)->canSearch, (NodeArray)->code[0], (NodeArray)->check);

	int ok = 0;
	for (int i = 0; ok != 1; i++)
	{
		if (str[i] == '\0')
			ok++;
		if (NodeArray->parent != NULL && NodeArray->lchild == NULL && NodeArray->rchild == NULL)//leaf
		{
			printf("%c", NodeArray->data);
			NodeArray = NodeArrayRoot;
			i--;
		}
		else//no leaf
		{
			if (NodeArray->lchild->check == str[i])
			{
				NodeArray = NodeArray->lchild;
			}
			else
				NodeArray = NodeArray->rchild;
		}
	}
}