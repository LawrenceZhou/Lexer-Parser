#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAXSIZE 2048   //缓冲区最大容量2KB
#define HALFMAXSIZE 1024  //半个缓冲区容量1KB
#define FILENAMESIZE 128   //文件名大小128B
#define TOKENSIZE 32       //token大小32B

char keywordstable[32][9]={"auto","break","case","char","const","continue",
                           "default","do","double","else","enum","extern",
						   "float","for","goto","if","int","long","register",
						   "return","short","signed","sizeo","fstatic",
						   "struct","switch","typedef","union","unsigned",
						   "void","volatile","while"};

char buffer[MAXSIZE],file_name[FILENAMESIZE],CH,first_unit=0,second_unit=0;  //buffer为缓冲区，file_name为文件名，CH为词法分析器当前所读取字符
int row_number=0,column_number=0,char_number=0,word_number=0, left__right_flag=0,flag=0,forward=0,chinese_flag=0;   //row_number行数，char_number字符数，word_number单词数，left__right_flag标志判断填充左右缓冲区
long Current_Pos=0;           //Current_Pos当前读到的文件的位置

void WelcomeInterface()
{
	printf("--------------------------------------------------\n");
	printf("-------------欢迎使用zyj词法分析器！--------------\n");
    printf("--------------------------------------------------\n");
}

void Initial()
{
	memset(buffer,0,MAXSIZE);
	memset(file_name,0,FILENAMESIZE);

	CH='\0';
    row_number=0;
	char_number=0;
	word_number=0;
	left__right_flag=0;
	flag=0;
	chinese_flag=0;
	forward=0;
    Current_Pos=0;
}

int FiletoBuffer()   //FiletoBuffer函数填充缓冲区
{
	FILE *filepointer;
	filepointer=fopen(file_name,"r"); 
	if (filepointer==NULL)     //若不存在文件，重新输入
	{
		printf("Can't find the file! Input a valid one!\n");
     	return 0;
	}
	else                            //若存在该文件
	{	
		fseek(filepointer,Current_Pos,0);   //定位到上次文件读到的位置
		int i;
        char c;
       if(left__right_flag==0)   //left__right_flag为0时，填充左半部分缓冲区
		{	
			i=0;

			while(i<HALFMAXSIZE)
			{
				c=fgetc(filepointer);
            
				if (c==EOF)  //若是'\0'直接返回
                {
					buffer[i++]=c;
					//printf("%c",buffer[i]);
                    return 0;
				}
				else{ 
					buffer[i++]=c;
					//printf("%c",buffer[i]);
                    //i++;
					}
			}

			Current_Pos=ftell(filepointer);    //找到目前所读到的位置
			return 1;

		}
		else if(left__right_flag==1)    //left__right_flag为1时，填充右半部分缓冲区
		{
		    i=HALFMAXSIZE;

			while(i<MAXSIZE)
			{
			    c=fgetc(filepointer);
           
				if (c==EOF)    //若是'\0'直接返回
				{
					buffer[i++]=c;
                    return 0;
				}
		    	else
					buffer[i++]=c;
			}

            Current_Pos=ftell(filepointer);    //找到目前所读到的位置
			return 1;
		}
        fclose(filepointer);     //关闭文件
	}
}

void Get_Nbc()       //检测当前字符是否为空格，若是空格及相关转义字符继续读入字符
{
	while((CH==' ')||(CH=='\t')||(CH=='\n'))    
		Get_Char();
} 

int Get_Char()
{
	CH=buffer[forward];
	 column_number++; 

	if(CH==EOF)
	
	    
		return 0;               //结束
    
	if(CH=='\n')
	{
		column_number=0;
		row_number++;
	}//统计行数和字符数
	else if((CH!='\n')&&(CH!=' ')&&(CH!='\t')&&(CH!=EOF)&&(CH<0||CH>32))
	{
	 
		char_number++;
        if(CH<0)
        chinese_flag++;        

    }
    if(forward==MAXSIZE-1)
		forward=0;
	else
		forward++;

	if(forward==HALFMAXSIZE)
	{
		left__right_flag=1;
		FiletoBuffer(file_name);
	}
	else if(forward==0)
	{
		left__right_flag=0;
		FiletoBuffer(file_name);
	}
	return 1;
}

void Retract()
{
	if(forward==0)
		forward=MAXSIZE-1;
	else
	forward--;
	if(CH=='\n')
	    row_number--;
	else if((CH!='\n')&&(CH!=' ')&&(CH!='\t')&&(CH!=EOF)&&(CH<0||CH>32))
	{
		if(CH<0)
		    chinese_flag--;
	    char_number--;
	}
}

void Analyze()
{
	char token[TOKENSIZE]=" ";
	FILE *fp1;
	fp1=fopen("analyze_result.txt","w+");//以追加的方式建立或打开analyze_result.txt 

	int i,j=0;
Get_Char();
	Get_Nbc();
	while(CH!=EOF)
	{
     
	if((CH>=65&&CH<=90)||(CH>=97&&CH<=122)||(CH=='_'))                  //关键字和标识符
	{
		memset(token,0,TOKENSIZE); 
		while((CH>=48&&CH<=57)||(CH>=65&&CH<=90)||(CH>=97&&CH<=122)||(CH=='_'))
		{
			token[j++]=CH;
			Get_Char();
		}
		Retract();
		word_number++;

		for(i=0;i<32;i++)
			if(!strcmp(token,keywordstable[i]))
			{
				printf("< %s, - >\n",token);
				fprintf(fp1,"< %s, - >\n",token);
				flag=1;
				break;
			}
        j=0;
        if(flag!=1)
        {
		 printf("< ID, 指向%s在符号表的入口指针 >\n",token);
		 fprintf(fp1,"< ID, 指向%s在符号表的入口指针 >\n",token);
    	}
		else
		flag=0;

    }
	else if((CH>=48)&&(CH<=57))                                //无符号数
	{
		memset(token,0,TOKENSIZE);
		while((CH>=48)&&(CH<=57))
		{
			token[j++]=CH;
			Get_Char();
		 }
		if((CH=='.')||(CH=='E')||(CH=='e'))                    //小数和指数
		{
			token[j++]=CH;
			Get_Char();
			while((CH>=48)&&(CH<=57))
			{
				token[j++]=CH;
			    Get_Char();
			}
		}
		Retract();
        j=0;
		printf("< num, %s >\n",token);
		fprintf(fp1,"< num, %s >\n",token);
	}
	else if(CH=='#')                                //预处理
	{
		printf("preproccessing in row %d: #", row_number);
    	fprintf(fp1,"preproccessing in row %d: #", row_number);
		Get_Char();
		printf("%c", CH);
		fprintf(fp1,"%c", CH);
		while(CH!='\n')
		{
			Get_Char();
		    printf("%c", CH);
		    fprintf(fp1,"%c", CH);
		}
		printf("%c", CH);
		fprintf(fp1,"%c", CH);
	 }
	else if(CH=='/')
	{
		 Get_Char();
		if(CH=='/')                                  //注释
		{
			printf("annotation in row %d :// ",row_number+1);
			fprintf(fp1,"annotation in row %d :// ",row_number+1);
			Get_Char();
			while(CH!='\n')
			{
				printf("%c",CH);
				fprintf(fp1,"%c", CH);
				Get_Char();
			}
		    printf("\n");
		    fprintf(fp1,"\n");
		}
		else if(CH=='*')                               //注释
		{
			printf("annotation in row %d :/* ",row_number+1);
			fprintf(fp1,"annotation in row %d :/* ",row_number+1);
			Get_Char();
			while(1)
			{
				while(CH!='*')
				{
				    printf("%c", CH);
				    fprintf(fp1,"%c", CH);
				    Get_Char();
				}
				Get_Char();
				if(CH=='/')
				{
					printf("%c", CH);
				    fprintf(fp1,"%c", CH);
					break;
				}
				else
				{
				    printf("*%c",CH);
				    fprintf(fp1,"*%c",CH);
				}
			}
			printf("\n");
		    fprintf(fp1,"\n");
		}
		else if(CH=='=')
		{
			printf("< /=, - >\n");
			fprintf(fp1,"< /=, - >\n");
		}
		else
		{
			printf("< /, - >\n");
			fprintf(fp1,"< /, - >\n");
			Retract();
		}
	}                                    //比较，赋值，运算符号
	else if(CH=='<')
	{
		Get_Char();
		if(CH=='=')
		{
			printf("< relop, LE >\n");
			fprintf(fp1,"< relop, LE >\n");
		}
		else if(CH=='>')
		{
			printf("< relop, NE >\n");
			fprintf(fp1,"< relop, NE >\n");
		}
		else
		{
			printf("< relop, LT >\n");
			fprintf(fp1,"< relop, LT >\n");
			Retract();
		}
	}
	else if(CH=='>')
	{
		Get_Char();
		if(CH=='=')
		{
			printf("< relop, GE >\n");
			fprintf(fp1,"< relop, GE >\n");
		}
		else
		{
			printf("< relop, GT >\n");
			fprintf(fp1,"< relop, GT >\n");
			Retract();
		}
	}
	else if(CH=='=')
	{
		Get_Char();
		if(CH=='=')
		{
			printf("< ==, - >\n");
			fprintf(fp1,"< ==, - >\n");
		}
		else
		{
			printf("< relop, EQ >\n");
			fprintf(fp1,"< relop, EQ >\n");
			Retract();
		}
	}
	else if(CH=='+')
	{
		Get_Char();
		if(CH=='=')
		{
			printf("< +=, - >\n");
			fprintf(fp1,"< +=, - >\n");
		}
		else if(CH=='+')
		{
			printf("< ++, - >\n");
			fprintf(fp1,"< ++, - >\n");
		}
		else
		{
			printf("< +, - >\n");
			fprintf(fp1,"< +, - >\n");
			Retract();
		}
	}
	else if(CH=='-')
	{
		Get_Char();
		if(CH=='=')
		{
			printf("< -=, - >\n");
			fprintf(fp1,"< -=, - >\n");
		}
		else if(CH=='-')
		{
			printf("< --, - >\n");
			fprintf(fp1,"< --, - >\n");
		}
		else
		{
			printf("< -, - >\n");
			fprintf(fp1,"< -, - >\n");
			Retract();
		}
	}
	else if(CH=='*')
	{
		Get_Char();
		if(CH=='=')
		{
			printf("< *=, - >\n");
			fprintf(fp1,"< *=, - >\n");
		}
		else
		{
			printf("< *, - >\n");
			fprintf(fp1,"< *, - >\n");
			Retract();
		}
	}
	else if(CH=='!')
	{
		Get_Char();
		if(CH=='=')
		{
			printf("< !=, NE >\n");
			fprintf(fp1,"< !=, NE >\n");
		}
		else
		{
			printf("< !, - >\n");
			fprintf(fp1,"< !, - >\n");
		}
	}
	else if(CH==':')
	{
		Get_Char();
		if(CH=='=')
		{
			printf("< assign-op, - >\n");
			fprintf(fp1,"< assign-op, - >\n");
		}
		else
		{
			printf("< :, - >\n");
			fprintf(fp1,"< :, - >\n");
			Retract();
		}
	}
	else if(CH=='&')     //且 
	{
		Get_Char();
		if(CH=='&')
		{
			printf("< &&, - >\n");
			fprintf(fp1,"< &&, - >\n");
		}
		else
		{
			printf("< &, - >\n");
			fprintf(fp1,"< &, - >\n");
			Retract();
		}
	}
	else if(CH=='|')            //或 
	{
		Get_Char();
		if(CH=='|')
		{
			printf("< ||, - >\n");
			fprintf(fp1,"< ||, - >\n");
		}
		else
		{
			printf("< |, - >\n");
			fprintf(fp1,"< |, - >\n");
			Retract();
		}
	}
	else if(CH=='\"')                //反斜杠系列 
	{
		printf("Content in\"\" in row %d : ", row_number+1 );
		fprintf(fp1,"Content in\"\" in row %d : ", row_number+1 );
	    Get_Char();
		while(CH!='\"')
		{
		    if(CH=='\\')
		    {
		        Get_Char();
		        
		            printf("\\%c",CH);
		            fprintf(fp1,"\\%c",CH);
		        
		    }
		    else
		    {
			printf("%c",CH);
		    fprintf(fp1,"%c",CH);
		    } 
		    Get_Char();
		}
		printf("\n");
		fprintf(fp1,"\n");
	}
	else if(CH=='\'')
	{
		     printf("< \', - >\n");
		     fprintf(fp1,"< \', - >\n");
	}
	else if(CH=='?')
	{ 
		printf("< ?, - >\n"); 
		fprintf(fp1,"< ?, - >\n"); 
	}
	else if(CH=='%')
	{ 
		printf("< %%, - >\n"); 
		fprintf(fp1,"< %%, - >\n"); 
	}
	else if(CH=='(')
	{ 
		printf("< (, - >\n"); 
		fprintf(fp1,"< (, - >\n"); 
	}
    else if(CH==')')
	{ 
		printf("< ), - >\n"); 
		fprintf(fp1,"< ), - >\n"); 
	}
	else if(CH=='[')
	{ 
		printf("< [, - >\n"); 
		fprintf(fp1,"< [, - >\n"); 
	}
	else if(CH==']')
	{ 
		printf("< ], - >\n"); 
		fprintf(fp1,"< ], - >\n"); 
	}
	else if(CH=='{')
    { 
		printf("< {, - >\n"); 
		fprintf(fp1,"< {, - >\n"); 
	}
	else if(CH=='}')
    { 
		printf("< }, - >\n"); 
		fprintf(fp1,"< }, - >\n"); 
	}
	else if(CH==';')
	{ 
		printf("< ;, - >\n"); 
		fprintf(fp1,"< ;, - >\n"); 
	}
	else if(CH==',')
	{ 
		printf("< ,, - >\n"); 
		fprintf(fp1,"< ,, - >\n"); 
	}
	else if(CH=='.')
	{ 
		printf("< ., - >\n"); 
		fprintf(fp1,"< ., - >\n"); 
	}
	else if((CH<0)&&(chinese_flag%2==0))
	{ 
		printf("< 中文字符, - >\n"); 
		fprintf(fp1,"< 中文字符, - >\n"); 
	}
	else if(CH==' ')
	{}
	else if(CH=='\\')
	{
		Get_Char();
		if(CH=='o')
		{
			printf("< \\o, - >\n");
			fprintf(fp1,"< \\o, - >\n");
		}
		else if(CH=='n')
		{
			printf("< \\n, - >\n");
			fprintf(fp1,"< \\n, - >\n");
		}
		else if(CH=='r')
		{
			printf("< \\r, - >\n");
			fprintf(fp1,"< \\r, - >\n");
		}
		else if(CH=='t')
		{
			printf("< \\t, - >\n");
			fprintf(fp1,"< \\t, - >\n");
		}
		else if(CH=='v')
		{
			printf("< \\v, - >\n");
			fprintf(fp1,"< \\v, - >\n");
		}
		else if(CH=='a')
		{
			printf("< \\a, - >\n");
			fprintf(fp1,"< \\a, - >\n");
		}
		else if(CH=='b')
		{
			printf("< \\b, - >\n");
			fprintf(fp1,"< \\b, - >\n");
		}
		else if(CH=='f')
		{
			printf("< \\f, - >\n");
			fprintf(fp1,"< \\f, - >\n");
		}
		else if(CH=='\'')
		{
		     printf("< \\\', - >\n");
		     fprintf(fp1,"< \\\', - >\n");
		 }

		else if(CH=='\\')
		{
		     printf("< \\\\, - >\n");
		     fprintf(fp1,"< \\\\, - >\n");
		 }
		else if(CH=='?')
		{
			printf("< \\?, - >\n");
			fprintf(fp1,"< \\?, - >\n");
		}
		else if(CH=='x')
		{
			Get_Char();
			if((CH>=48&&CH<=57)||(CH>=97&&CH<=102))
			{
				first_unit=CH; 
				Get_Char();
				if((CH>=48&&CH<=57)||(CH>=97&&CH<=102))
				{
					printf("< \\x%c%c二位十六进制, - >\n",first_unit,CH);
					fprintf(fp1,"< \\x%c%c二位十六进制, - >\n",first_unit,CH);
				}
				else
			    {
			    	printf("< \\x0%c二位十六进制, - >\n",first_unit);
			    	fprintf(fp1,"< \\x0%c二位十六进制, - >\n",first_unit);
				    Retract();
			    }			
			}
			else
			{
				printf("< \\x00二位十六进制, - >\n");
				fprintf(fp1,"< \\x00二位十六进制, - >\n");
				Retract();
			}
		}
		else if((CH>=48)&&(CH<=55))
		{
			first_unit=CH;
			if(CH==48)
			{
				Get_Char();
			    if((CH>=48)&&(CH<=55))
			    {
				    second_unit=CH; 
				    Get_Char();
				    if((CH>=48)&&(CH<=55))
				    {
					    printf("< \\%c%c%c三位八进制, - >\n",first_unit,second_unit,CH);
					    fprintf(fp1,"< \\%c%c%c三位八进制, - >\n",first_unit,second_unit,CH);
					}
				    else
			         {
			    	    printf("< \\0%c%c三位八进制, - >\n",first_unit,CH);
			    	    fprintf(fp1,"< \\0%c%c三位八进制, - >\n",first_unit,CH);
				        Retract();
			         }			
			    }
			    else
			    {
			    	printf("<\0, - >\n");
			    	fprintf(fp1,"<\0, - >\n");
			    	Retract();
			    }
			}
			else
			{
			    Get_Char();
			    if((CH>=48)&&(CH<=55))
			    {  
				    second_unit=CH; 
				    Get_Char();
				    if((CH>=48)&&(CH<=55))
				    {
					    printf("< \\%c%c%c三位八进制, - >\n",first_unit,second_unit,CH);
					    fprintf(fp1,"< \\%c%c%c三位八进制, - >\n",first_unit,second_unit,CH);
					}
				    else
			        { 
			    	    printf("< \\0%c%c三位八进制, - >\n",first_unit,second_unit);
			    	    fprintf(fp1,"< \\0%c%c三位八进制, - >\n",first_unit,second_unit);
				        Retract();
			        }			
			    }
			    else
			    {
				    printf("< \\00%c三位八进制, - >\n",first_unit);
				    fprintf(fp1,"< \\00%c三位八进制, - >\n",first_unit);
				    Retract();
			    }
			}
		}
		else
		{
			printf("< \\%c, - >\n",CH);
			fprintf(fp1,"< \\%c, - >\n",CH);
		}
	}
	
	else
	{
		printf("There's an error in row %d, column %d.\n",row_number+1, column_number);
		fprintf(fp1,"There's an error in row %d, column %d.\n",row_number+1, column_number);
	}
	Get_Char();
	Get_Nbc();
	}
	printf("The character number is %d, Chinese character is %d. \n",char_number-(chinese_flag/2),chinese_flag/2);
	printf("The word number is %d\n", word_number);
	printf("The row number is %d\n", row_number);
	fprintf(fp1,"The character number is %d, Chinese character is %d. \n",char_number-(chinese_flag/2),chinese_flag/2);
	fprintf(fp1,"The word number is %d\n", word_number);
	fprintf(fp1,"The row number is %d\n", row_number);
	fclose(fp1);
}

int main()
{
	WelcomeInterface();
	Initial();

	while(1)
	{
		printf("Please input the file name which you want to analyze:\n");
		scanf("%s", &file_name);

	 	FILE *fp;
    	fp=fopen(file_name,"r"); 
    	if (fp==NULL)     //若不存在文件，重新输入
			printf("Can't find the file! Input a valid one!\n");
		else
		{
		    FiletoBuffer(file_name);
			Analyze();
			fclose(fp);
		}
		int command;
		printf("Do you wanna analyze another one? If so,please input 1; if not, input 0. \n");
		scanf("%d",&command);
			
		if(command==0)
		{
			printf("THX for using!\n");
			return 0;
		}else if(command==1)
		{}
	}
	return 0;
}
