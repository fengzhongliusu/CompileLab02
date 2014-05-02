/*
 * =====================================================================================
 *
 *       Filename:  printerr.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/28/2014 09:00:11 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chengshuo (cshuo), chengshuo357951@gmail.com
 *        Company:  Nanjing Unversity
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include "sem.h"

/**
 * 打印错误信息
 **/

void print_err(int type,int lineno,char* err_msg)
{
	printf("Error type %d at line %d: ",type,lineno);
	switch(type){
		case 1:
			printf("Undefined variable \"%s\"\n",err_msg);
			break;
		case 2:
			printf("Undefined function \"%s\"\n",err_msg);
			break;
		case 3:
			printf("Redefined variable \"%s\"\n",err_msg);
			break;
		case 4:
			printf("Redefined function \"%s\"\n",err_msg);
			break;
		case 5:
			printf("Type mismatched!\n");
			break;
		case 6:
			printf("The left-hand side of an assignment must be a variable\n");
			break;
		case 7:
			printf("Operands type mismatched\n");
			break;
		case 8:
			printf("The return type mismatched\n");
			break;
		case 9:
			printf("mismatched arguments for function \"%s\"\n",err_msg);
			break;
		case 10:
			printf("variable is not an array\n");
			break;
		case 11:
			printf("\"%s\" must be a function\n",err_msg);
			break;
		case 12:
			printf("array index is not integer\n");
			break;
		case 13:
			printf("Illegal use of \"%s\"\n",err_msg);
			break;
		case 14:
			printf("Un-existed field \"%s\"\n",err_msg);
			break;
		case 15:
			printf("Redefined field \"%s\"\n",err_msg);
			break;
		case 16:
			printf("wrong type of expression in \"%s\"\n",err_msg);
			break;
		case 17:
			printf("no return statement in function \"%s\"\n",err_msg);
			break;
		case 18:
			printf("redefine struct \"%s\"\n",err_msg);
			break;
		case 19:
			printf("cannot do assignment in struct\n");
			break;
		default: 
			printf("wrong error type!!\n");
			break;
	}
}



