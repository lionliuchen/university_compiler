// cffx.cpp : Defines the entry point for the console application.
//




/*
* �������ƣ�����ԭ��֮�ʷ�������
* ��    �룺Test.c C����Դ�����ļ�
* ��    ����Const.txt ������ Sign.txt ��ʶ���� Result.txt ��Ԫʽ���
*
* ���뻷����VC6.0_CN
* ����ϵͳ��WinXP_SP2
*
* ��    ע���˳������һ���巽ʽ�������Դ�룬���Ƚ���Ԥ����ȥ��ע�ͺ���Ч�ո�
*           Ȼ���ٽ�����ϸ�Ĵʷ�������Ϊ�˱��ں����������������˼���������
*           ��ͳ�����
* ע    �⣺�˷�����ֻ��C�����Ӽ����д���
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
    int kind;  //���ʵ�����
    char value[20]; //���ʵ�ֵ
}*pDualistic, Dualistic;
    int iPos;
    int ListLength=0;//�������ĳ���
int  errorLine=0;
char proBuffer[PRO_MAX_LEN] = "";
char ch;
int row,col;    //���ڴ洢״̬ת��������к�row���к�col
int StateTbl[50][100];  //�洢״̬ת������Ķ�ά����
char wordget[STR_MAX_LEN]; //������ʱ�����ȡ���ʵ��ַ�����

int k;
int  point = 0;
struct VarInfo//������Ϣ����
{
    int StoreType; //��������
    char content[STB_MAX_LEN]; //��������
}signTab[100];

struct ConstInfo //������Ϣ����
{
  int StoreType;//��������
  char name[20]; //��������
  int Value; //����
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
    /*0*/"δ֪����",    /*1*/"�Ƿ����ַ�",    /*2*/"����ȷ���ַ��������",
    /*3*/"����ȷ���ַ������",    /*4*/"����ȷ�����ֱ��",    /*5*/"ע�Ͷ�ʧ'*/'"};


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
int SearchSymbol(char code[],char Symbol);//��code�ַ����в��ҵ�һ�γ���ch�ַ����±�
bool wordAnalyse(pDualistic pDu);
void ReadToStaArray();
int main(int argc, char* argv[])
{


    Dualistic  TokenList[100];
    pDualistic ptmp = TokenList;
    FILE *fin, *fout;
    int i;
    char c;

    printf("Դ�������\n");

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
            printf("\n�������̫�����޷�����\a");
            return 0;
        }
        proBuffer[i++] = c;
    }
    fclose(fin);
    proBuffer[i++] = '\0';
    printf("\n***************************\nԴ�������ɹ���Դ��������:\n%s",proBuffer);
    printf("\n�����������\n");    getch();

    printf("\nԤ����\n");
    pretreatment();
    printf("\n***************************\nԤ����ɹ���ȥ��ע�ͺ��Դ����Ϊ:\n%s*",proBuffer);
    printf("\n�����������\n");    getch();
    printf("\n�ʷ�����\n");
    point = 0;
    if ((fout=fopen("Result.txt","wb")) == NULL)
    {
        printf("�����ļ�Result.txtʧ�ܡ�\n");
        return 0;
    }
    i = 0;
    errorLine = 0;
    wordAnalyse(ptmp);//�ʷ������������еĵ�����ȡ���������뵥����TokenList����
         //���������е�ÿһ������д���ļ�Result.txt��
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
            fprintf(fout, "(��  ��%s)",wordget);
            break;
        case ID:
            fprintf(fout, "(��ʶ����%s)",wordget);
            break;
        case CONST:
            fprintf(fout, "(��  ����%s)",wordget);
            break;
        case OPERAT:
            fprintf(fout, "(�������%s)",wordget);
            break;
        case DIVIDE:
            fprintf(fout, "(��  ����%s)",wordget);
            break;
        default:;
        }
        if(ptmp->kind >= 1 && ptmp->kind <= KEYWORD_LEN)
            fprintf(fout, "(�ؼ��֣�%s)",kwTab[ptmp->kind-1]);
        fprintf(fout, "\r\n");
    }while(1);
*/
    fclose(fout);

    printf("д�س�����ͱ�ʶ����\n");

    if ((fout=fopen("Const.txt","wb")) == NULL)
    {
        printf("�����ļ�Const.txtʧ�ܡ�\n");
        return 0;
    }
    for(i = 0; i < pointCTB; ++i)
        fprintf(fout, "%3d %s\r\n",i, constTab[i]);
    fclose(fout);

    if ((fout=fopen("Sign.txt","wb")) == NULL)
    {
        printf("�����ļ�Sign.txtʧ�ܡ�\n");
        return 0;
    }
    for(i = 0; i < pointSTB; ++i)
        fprintf(fout, "%3d %s\r\n",i, signTab[i]);
    fclose(fout);
    printf("\nд�����\n�����������\n");    getch();

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
    printf("\nError:��%d��,%s\n",errorLine, errorTab[id]);
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


int InsertConst(char *str,int value)//�����ͳ������볣����Ϣ����
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
        printf("�ļ������ڣ�");
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
int SearchSymbol(char code[],char ch)  //��code�ַ����в��ҵ�һ�γ���ch�ַ����±�
{
  for(int i=0;i<strlen(code);i++)
      if(code[i]==ch) break;
  return i;
}
bool Process(int *iState,pDualistic ptmp)
{

  if(*iState==0)//�����ǰ״̬Ϊ0����˵������ƴд����
    return false;
  else if(*iState<0)//�����ǰ״̬<0����˵���õ����Ѿ���ȡ��ϣ���Ҫ����״̬��ֵ�жϸõ��ʵ�����
  {

      wordget[k-1]='\0';


      iPos--;
      if(*iState==-1) //��ʶ��
      {
            //��wordget[]�еĵ��ʴ��뵥����
            ptmp=ptmp+ListLength;
            ptmp->kind=1;
            strcpy(ptmp->value,wordget);

            //����ʶ�����ʴ��������Ϣ��
            InsertId(ptmp->kind,ptmp->value);
            ListLength++;

      }
      else if(*iState>=-64&&*iState<=-54) //�����
      {
            //��wordget[]�еĵ��ʴ��뵥����
            ptmp=ptmp+ListLength;
            ptmp->kind=-(*iState);
            strcpy(ptmp->value,wordget);
            ListLength++;
      }
      else if((*iState)>=-84&&(*iState)<=-70) //���
      {
            //��wordget[]�еĵ��ʴ��뵥����
            ptmp=ptmp+ListLength;
            ptmp->kind=-(*iState);
            strcpy(ptmp->value,wordget);
            ListLength++;
      }
      else if((*iState)>=-87&&(*iState)<=-85) //����
      {
            //��wordget[]�еĵ��ʴ��뵥����
            ptmp=ptmp+ListLength;
            ptmp->kind=-(*iState);
            strcpy(ptmp->value,wordget);


            //�����ʴ��볣����Ϣ��
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
  //�����ַ�����Symbol[]���ڴ洢���е��ַ�
    char SymbolArr[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/{}+-*%&|!><=();,:.\"\r\n \t[]~`@#$^_?\\";
    k=0;
    bool Tag=true;
    iPos=0;//���ڴ洢��ǰ�ַ���Դ�����е�λ���
    int iSymbol;//�洢��ǰ�ַ�

//���ú���ReadToStaArray()�������ļ�zhjz.txt�����ļ��洢����״̬ת�����󣩵��ڴ�StateTbl[][]��
    ReadToStaArray();


    int iState=1;//�������ͱ���iState������ʼ��Ϊ��ʼ״̬1


    //ѭ������Դ�����е�ÿһ���ַ����Ե��ʽ�����ȡ
    while(iPos<strlen(proBuffer)&&Tag) //�����ǰ�ַ���λ���iС��Դ����ĳ��Ȳ��ҵ�ǰ״̬TagΪ��
    {
      iSymbol=proBuffer[iPos++];
      wordget[k++]=iSymbol; //��Դ�����еĵ�i���ַ�׷�ӵ�wordget[]�����ĩβ��wordget[]�����ڴ洢������ȡ�ĵ��ʵĻ�����
      row=iState;
      col=SearchSymbol(SymbolArr,iSymbol);//��λ��ǰ�ַ����ַ�����Symbol[]�е��±��,������±�Ŵ������ͱ���col�У����ڼ�¼״̬ת��������к�
      iState=StateTbl[row-1][col];//��״̬ת������StateTbl[][]�в���iState״̬�ڶ����±�Ϊcol���ַ�ʱҪת���ɵ�״̬
      Tag=Process(&iState,pDu);
      if(!Tag) return false;

    }
    return true;
}
