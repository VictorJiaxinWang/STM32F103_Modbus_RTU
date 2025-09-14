#ifndef _DATA_H_
#define _DATA_H_
#include "mydef.h"
//#include "stmflash.h"
#include "string.h"

typedef struct											
{
	uint8 configedFlag;						//�Ƿ��Ѿ�����
	uint8 localDate[6];						//��������
	uint8 mac[6];							/*MAC��ַ*/
	uint8 lip[4];							/*local IP����IP��ַ*/
	uint8 sub[4];							/*��������*/
	uint8 gw[4];							/*����*/	
	uint8 dns[4];							/*DNS��������ַ*/
	uint8 rip[4];							/*remote IPԶ��IP��ַ*/
	uint16 port485;							//485�˿�
	uint8 id4851;							//485�ӻ�һID
	uint8 id4852;							//485�ӻ���ID
	uint8 canLimit;							//CAN�޶�λ
	uint8 canFilter;						//CAN����λ
	uint8 slaveId[20];						//�ӻ�ID		
	uint8 slaveState[20];					//�ӻ�״̬
}ConfigValueTypeDef;						//������Ϣ


typedef struct
{
	uint8 ProgVersion[5];						//�汾��		
	uint8 ProgSize[7];							//�ֽ���
	uint8 progMd5[33];							//MD5ֵ
	uint8 progDate[20];							//��������
	ConfigValueTypeDef configValueTypeDef;		//��������
}InnerSaveTypeDef;

typedef enum
{
	size_12=12,
	size_16=16,
	size_24=24
}OledSize;		//OLED����ߴ�

typedef struct
{
	uint8 row;									//��ʾ��
	uint8 col;									//��ʾ��
	OledSize size;									//�����С
	uint8 oledShowData[17];		 				//OLED��ʾ����
}OledShowTypeDef;            	 			    //OLED���ƽṹ��


typedef enum
{
	no_key,										//�ް���
	up_key,										//�ϼ�
	down_key,									//�¼�
	ok_key,										//ȷ��
	esc_key,									//�˳�
	//���������ⰴ��
	open_screen,								//����Ļ
	close_screen,								//Ϩ����Ļ
}KeyEnum;										//����ö��
typedef enum
{
	subMenu,
	noSubMenu
}MenuIsFirsr;									//�˵���ҳ
typedef enum
{
	firstSubMenu,
	secondSubMenu,
	thirdSubMenu,
	fourthSubMenu,
	maxSubMenu
}MenuPoint;										//�Ӳ˵�
typedef enum
{
	firstPage,
	secondPage,
	maxPage
}MenuPage;
typedef struct
{
	MenuIsFirsr isSubMenu;								//�Ƿ�����ҳ
	MenuPoint pointSubMenu;								//��ҳ�ڼ����˵�
	MenuPage menuPage;
}MenuInfoTypeDef;								//�˵������ṹ��


extern InnerSaveTypeDef innerSaveTypeDef;//�ڲ��洢��Ϣ


extern const uint8 menu_row_1[];				//���ﴨ�Ƽ�
extern const uint8 password[]; 					//�������
extern const uint8 passwordInput1[];	//�����������һλ
extern const uint8 passwordInput2[];	//������������λ
extern const uint8 passwordInput3[];	//�������������λ
extern const uint8 passwordInput4[];   	//�������������λ
extern const uint8 passwordError[];	//������������������

//���˵�
extern const uint8 programInfo[];		//������Ϣ
extern const uint8 bootInfo[];		//������Ϣ
extern const uint8 slaveState[];		//�ӻ���Ϣ
extern const uint8 canSetInfo[];		//CAN��Ϣ
extern const uint8 inputTest[];		//�������
extern const uint8 outputTest[];	//�������
extern const uint8 startConfig[];		//�����˵�
extern const uint8 netInfo[];	    //�����˵�
extern const uint8 point[];			//ָ��
extern const uint8 nullPoint[];     //��ָ��

void read_parameter(void);
#endif


