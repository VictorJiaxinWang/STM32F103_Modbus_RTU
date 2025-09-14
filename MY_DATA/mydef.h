#ifndef _MYDEF_H_
#define _MYDEF_H_


#define SAVE_DATA_ADDR    0x08018800    //�����ڲ��洢��ʼ��ַ
#define SAVE_PROGRAM_ADDR 0x08019000	//�����ڲ��洢��ʼ��ַ

typedef enum
{
	MY_OK,								//�ɹ�
	MY_ERR								//ʧ��
}ResultState;							//״̬




//�������Ͷ���
typedef unsigned char uint8;		//�޷���8λ
typedef unsigned short uint16;		//�޷���16λ
typedef unsigned uint32;		//�޷���32λ
typedef unsigned long long uint64;		//�޷���64λ

typedef char int8;					//�з���8λ
typedef short int16;				//�з���16λ
typedef int int32;					//�з���32λ
typedef long long int64;					//�з���64λ


typedef struct
{
    unsigned char  b0:1; 
    unsigned char  b1:1; 
    unsigned char  b2:1; 
    unsigned char  b3:1; 
    unsigned char  b4:1;
    unsigned char  b5:1;
    unsigned char  b6:1;
    unsigned char  b7:1;
} BIT8;


#define CLOSE_TIME 10				//Ϩ����Ļʱ�䣬��λ��
#define PASSWORD 933				//��������ֵ
#define CMD_PASSWORD "1882598"		//������������ֵ

#endif

