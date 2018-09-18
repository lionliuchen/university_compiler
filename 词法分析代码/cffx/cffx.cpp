// cffx.cpp : Defines the entry point for the console application.
//




/*
* 程序名称：编译原理之词法分析器
* 输    入：Test.c C语言源程序文件
* 输    出：Const.txt 常量表 Sign.txt 标识符表 Result.txt 二元式结果
*
* 编译环境：VC6.0_CN
* 操作系统：WinXP_SP2
*
* 备    注：此程序采用一缓冲方式读入程序源码，首先进行预处理去掉注释和无效空格
*           然后再进行详细的词法分析，为了便于后续处理，程序设置了几个表：符号
*           表和常数表。
* 注    意：此分析器只对C语言子集进行处理
*/
//#include <stdafx.h>

#include "stdio.h"
#include "ctype.h"
#include "string.h"
#include "conio.h"
#include "stdlib.h"

#define KEYWORD_LEN 32
#define STR_MAX_LEN 300
#define PRO_MAX_LEN 20480
#define STB_MAX_LEN 1000
#define CTB_MAX_LEN 1000

#define ERROR    0
#define ID        (KEYWORD_LEN+1)
#define CONST    (KEYWORD_LEN+2)
#define OPERAT    (KEYWORD_LEN+3)
#define DIVIDE    (KEYWORD_LEN+4)

typedef struct signDuality
{
    int kind;  //单词的类型
    char value[20]; //单词的值
}*pDualistic, Dualistic;
    int iPos;
    int ListLength=0;//单词流的长度
int  errorLine=0;
char proBuffer[PRO_MAX_LEN] = "";
char ch;
int row,col;    //用于存储状态转换矩阵的行号row和列号col
int StateTbl[50][100];  //存储状态转换矩阵的二维数组
char wordget[STR_MAX_LEN]; //用于临时存放提取单词的字符数组

int k;
int  point = 0;
struct VarInfo//变量信息表定义
{
    int StoreType; //变量类型
    char content[STB_MAX_LEN]; //变量内容
}signTab[100];

struct ConstInfo //常量信息表定义
{
  int StoreType;//常量类型
  char name[20]; //常量名字
  int Value; //常量
}constTab[100];

int  pointSTB = 0;

int  pointCTB = 0;
char kwTab[KEYWORD_LEN][10]={ "auto",        "break",    "case",        "char",        "const",
        "continue",    "default",    "do",        "double",    "else",
        "enum",        "extern",    "float",    "for",        "goto",
        "if",        "int",        "long",        "register",    "return",
        "short",    "signed",    "sizeof",    "static",    "struct",
        "switch",    "typedef",    "union",    "unsigned",    "void",
        "volatile",    "while"};
char errorTab[][50]={
    /*0*/"未知错误",    /*1*/"非法的字符",    /*2*/"不正确的字符常量表达",
    /*3*/"不正确的字符串表达",    /*4*/"不正确的数字表达",    /*5*/"注释丢失'*/'"};


void pretreatment();
void ProcError(int id);
bool GetChar();
bool GetBC();
void Concat(char *str);
int  Reserve(char *str);
void Retract();
int InsertId(int kind,char *str);
int InsertConst(char *str,int value);
bool Process(int *iState,pDualistic ptmp,int iPos);
int SearchSymbol(char code[],char Symbol);//在code字符串中查找第一次出现ch字符的下标
bool wordAnalyse(pDualistic pDu);
void ReadToStaArray();
int main(int argc, char* argv[])
{


    Dualistic  TokenList[100];
    pDualistic ptmp = TokenList;
    FILE *fin, *fout;
    int i;
    char c;

    printf("源代码读入\n");

    if ((fin=fopen("Test.c","r")) == NULL)
    {
        printf("Cannot open infile\n");
        return 0;
    }
    i = 0;
    while((c = fgetc(fin)) != EOF)
    {
        if(i >= PRO_MAX_LEN-1)
        {
            printf("\n程序代码太长，无法处理\a");
            return 0;
        }
        proBuffer[i++] = c;
    }
    fclose(fin);
    proBuffer[i++] = '\0';
    printf("\n***************************\n源代码读入成功，源代码如下:\n%s",proBuffer);
    printf("\n按任意键继续\n");    getch();

    printf("\n预处理\n");
    pretreatment();
    printf("\n***************************\n预处理成功，去掉注释后的源代码为:\n%s*",proBuffer);
    printf("\n按任意键继续\n");    getch();
    printf("\n词法分析\n");
    point = 0;
    if ((fout=fopen("Result.txt","wb")) == NULL)
    {
        printf("建立文件Result.txt失败。\n");
        return 0;
    }
    i = 0;
    errorLine = 0;
    wordAnalyse(ptmp);//词法分析，将所有的单词提取出来并存入单词流TokenList【】
         //将单词流中的每一个单词写入文件Result.txt中
/*    do
    {
        if(i++ > PRO_MAX_LEN)
            break;
        if(!wordAnalyse(ptmp))
        {
            break;
        }
 //       if (ptmp->value == -1)
  //          fprintf(fout, "<%3d,  ->\t",ptmp->kind);

            fprintf(fout, "<%3d,%3d>\t",ptmp->kind, ptmp->value);
        switch(ptmp->kind)
        {
        case ERROR:
            fprintf(fout, "(出  错：%s)",wordget);
            break;
        case ID:
            fprintf(fout, "(标识符：%s)",wordget);
            break;
        case CONST:
            fprintf(fout, "(常  量：%s)",wordget);
            break;
        case OPERAT:
            fprintf(fout, "(运算符：%s)",wordget);
            break;
        case DIVIDE:
            fprintf(fout, "(界  符：%s)",wordget);
            break;
        default:;
        }
        if(ptmp->kind >= 1 && ptmp->kind <= KEYWORD_LEN)
            fprintf(fout, "(关键字：%s)",kwTab[ptmp->kind-1]);
        fprintf(fout, "\r\n");
    }while(1);
*/
    fclose(fout);

    printf("写回常量表和标识符表\n");

    if ((fout=fopen("Const.txt","wb")) == NULL)
    {
        printf("建立文件Const.txt失败。\n");
        return 0;
    }
    for(i = 0; i < pointCTB; ++i)
        fprintf(fout, "%3d %s\r\n",i, constTab[i]);
    fclose(fout);

    if ((fout=fopen("Sign.txt","wb")) == NULL)
    {
        printf("建立文件Sign.txt失败。\n");
        return 0;
    }
    for(i = 0; i < pointSTB; ++i)
        fprintf(fout, "%3d %s\r\n",i, signTab[i]);
    fclose(fout);
    printf("\n写入完毕\n按任意键继续\n");    getch();

    return 0;
}


void pretreatment()
{int lines=0;
    char tmp[PRO_MAX_LEN];

    int tmpp = 0;
    bool flg;
    char tmpc;


    point = 0;
    do
    {
        flg = GetChar();

        if(ch == '/')
        {
            flg = GetChar();
            switch(ch)
            {
            case '/':
                do
                {
                    flg = GetChar();
                }while(!(ch == '\n' || flg == false));
                if(ch == '\n')
                    Retract();
                break;
            case '*':
                do
                {
                    flg = GetChar();
                    tmpc = ch;

                    if(tmpc == '\n')
                        tmp[tmpp++] = tmpc;
                    flg = GetChar();
                    Retract();
                }while(flg && !(flg && tmpc == '*' && ch == '/'));
                flg = GetChar();
                if (!flg)
                {
                    ProcError(5);
                }
                break;
            default:

                Retract();
                Retract();
                GetChar();
                tmp[tmpp++] = ch;
                flg = GetChar();
                tmp[tmpp++] = ch;
            }
        }
        else
        {
            tmp[tmpp++] = ch;
        }
    }while(flg);
    tmp[tmpp] = '\0';

     strcpy(proBuffer,tmp);//
}


void ProcError(int id)
{
    printf("\nError:第%d行,%s\n",errorLine, errorTab[id]);
}


bool GetChar()
{
    if(point < PRO_MAX_LEN && proBuffer[point] != '\0')
    {
        ch = proBuffer[point++];
        if (ch == '\n')
            errorLine ++;
        return true;
    }
    ch = '\0';
    return false;
}


bool GetBC()
{
    do
    {
        if(!GetChar())
        {
            ch = '\0';
            return false;
        }
    }while(isspace(ch));
    return true;
}


void Concat(char *str)
{
    int i;
    for(i=0; str[i]; ++i);
    str[i] = ch;
    str[i+1] = '\0';
}


int Reserve(char *str)
{
    int i;
    for(i=0; i<KEYWORD_LEN; ++i)
    {
        if(0 == strcmp(kwTab[i], str))
            return i+1;
    }
    return 0;
}

void Retract()
{
    if(proBuffer[point] == '\n' && errorLine > 0)
        errorLine --;
    point --;
}


int InsertId(int kind,char *str)
{
    int i;
    for(i=0; i < pointSTB; ++i)
        if(0 == strcmp(signTab[i].content, str))
            return i;
    signTab[pointSTB++].StoreType=kind;
    strcpy(signTab[pointSTB++].content, str);
    return (pointSTB-1);
}


int InsertConst(char *str,int value)//将整型常量插入常量信息表中
{
    int i;
    for(i=0; i < pointCTB; ++i)
        if(0 == strcmp(constTab[i].name, str))
            return i;
    constTab[pointCTB++].Value=value;
    return (pointCTB-1);
}
void ReadToStaArray()
{

    FILE *fp;
    row=0;col=0;
    if((fp=fopen("zhjz.txt","r")) == NULL)
    {
        printf("文件不存在！");
        exit(0);
    }


    char ch;
    ch=fgetc(fp);
    while(!feof(fp))
    {
        int i=0;
        char s[]="";

        while(ch!='\t'&&ch!='\r'&&ch!='\n')
        {
            s[i++]=ch;
            ch=fgetc(fp);
        }
        s[i]='\0';
        if(ch =='\t'||ch =='\r')
            StateTbl[row][col++]=atoi(s);
        else if(ch == '\n')
        {
            row++;col=0;
        }
        ch=fgetc(fp);
    }
    fclose(fp);

}
int SearchSymbol(char code[],char ch)  //在code字符串中查找第一次出现ch字符的下标
{
  for(int i=0;i<strlen(code);i++)
      if(code[i]==ch) break;
  return i;
}
bool Process(int *iState,pDualistic ptmp)
{

  if(*iState==0)//如果当前状态为0，则说明单词拼写错误
    return false;
  else if(*iState<0)//如果当前状态<0，则说明该单词已经提取完毕，需要根据状态的值判断该单词的类型
  {

      wordget[k-1]='\0';


      iPos--;
      if(*iState==-1) //标识符
      {
            //将wordget[]中的单词存入单词流
            ptmp=ptmp+ListLength;
            ptmp->kind=1;
            strcpy(ptmp->value,wordget);

            //将标识符单词存入变量信息表
            InsertId(ptmp->kind,ptmp->value);
            ListLength++;

      }
      else if(*iState>=-64&&*iState<=-54) //运算符
      {
            //将wordget[]中的单词存入单词流
            ptmp=ptmp+ListLength;
            ptmp->kind=-(*iState);
            strcpy(ptmp->value,wordget);
            ListLength++;
      }
      else if((*iState)>=-84&&(*iState)<=-70) //界符
      {
            //将wordget[]中的单词存入单词流
            ptmp=ptmp+ListLength;
            ptmp->kind=-(*iState);
            strcpy(ptmp->value,wordget);
            ListLength++;
      }
      else if((*iState)>=-87&&(*iState)<=-85) //常量
      {
            //将wordget[]中的单词存入单词流
            ptmp=ptmp+ListLength;
            ptmp->kind=-(*iState);
            strcpy(ptmp->value,wordget);


            //将单词存入常量信息表
            InsertConst(wordget,atoi(ptmp->value));
            ListLength++;
      }

      strcpy(wordget,"");
      k=0;
      *iState=1;
  }
  else
      row=*iState;
  return true;
}

bool wordAnalyse(pDualistic pDu)
{
  //定义字符数组Symbol[]用于存储所有的字符
    char SymbolArr[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/{}+-*%&|!><=();,:.\"\r\n \t[]~`@#$^_?\\";
    k=0;
    bool Tag=true;
    iPos=0;//用于存储当前字符在源代码中的位序号
    int iSymbol;//存储当前字符

//调用函数ReadToStaArray()，读入文件zhjz.txt（该文件存储的是状态转换矩阵）到内存StateTbl[][]中
    ReadToStaArray();


    int iState=1;//定义整型变量iState，并初始化为初始状态1


    //循环读入源代码中的每一个字符，对单词进行提取
    while(iPos<strlen(proBuffer)&&Tag) //如果当前字符的位序号i小于源代码的长度并且当前状态Tag为真
    {
      iSymbol=proBuffer[iPos++];
      wordget[k++]=iSymbol; //将源代码中的第i个字符追加到wordget[]数组的末尾，wordget[]是用于存储正在提取的单词的缓冲区
      row=iState;
      col=SearchSymbol(SymbolArr,iSymbol);//定位当前字符在字符数组Symbol[]中的下标号,将这个下标号存入整型变量col中，用于记录状态转换矩阵的列号
      iState=StateTbl[row-1][col];//从状态转换矩阵StateTbl[][]中查找iState状态在读入下标为col的字符时要转换成的状态
      Tag=Process(&iState,pDu);
      if(!Tag) return false;

    }
    return true;
}
