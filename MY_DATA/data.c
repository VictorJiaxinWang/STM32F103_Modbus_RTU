#include "data.h"

InnerSaveTypeDef innerSaveTypeDef;//�ڲ��洢��Ϣ


const uint8 menu_row_1[]="   HDC Company  "; //���ﴨ�Ƽ�
const uint8 password[] = "password:----";	//�������
const uint8 passwordInput1[] = "password:*---";	//�����������һλ
const uint8 passwordInput2[] = "password:**--";	//������������λ
const uint8 passwordInput3[] = "password:***-";	//�������������λ
const uint8 passwordInput4[] = "password:****";	//�������������λ
const uint8 passwordError[] = "password:error";	//������������������

//���˵�
const uint8 programInfo[] = "1.prog info";		//������Ϣ
const uint8 bootInfo[] 	  = "2.boot info";		//BOOT��Ϣ
const uint8 slaveState[]  = "3.slave info";		//�ӻ���Ϣ
const uint8 canSetInfo[]  = "4.can info";		//CAN��Ϣ
const uint8 inputTest[]   = "5.input test";		//�������
const uint8 outputTest[]  = "6.output test";	//�������
const uint8 startConfig[] = "7.open config";	//��������
const uint8 netInfo[]	  = "8.net info";	    //������Ϣ
const uint8 point[]  = "->";	//ָ��
const uint8 nullPoint[]  = "  ";	//��ָ��


const unsigned char DEFAULT_IP[4]  ={192,168,1,88};											
const unsigned char DEFAULT_SUBNET[4]    ={255,255,255,0};										
const unsigned char DEFAULT_GATEWAY[4]   ={192,168,1,1};											
const unsigned char DEFAULT_DNS[4]={114,114,114,114};	
const unsigned char DEFAULT_MAC[6]={0x00,0x08,0xdc,0x11,0x11,0x11};
const unsigned char DEFAULT_DATE[6]={20,4,1,13,0,0};
const unsigned int 	DEFAULT_PORT=502;
const unsigned char DEFAULT_4851ID=0x01;
const unsigned char DEFAULT_4852ID=0x02;
const unsigned char DEFAULT_CANLIMIT=0x01;
const unsigned char DEFAULT_CANFILTER=0x02;
const unsigned char DEFAULT_SLAVADDRSTART=0x01;
const unsigned char DEFAULT_SLAVESTATE=0x00;

extern uint16 local_tcp_port;	

void read_parameter()
{
	uint8 i=0;
	uint8 buf[sizeof(innerSaveTypeDef)*2];
	memset(buf,0x00,sizeof(buf));
	// STMFLASH_Read(SAVE_DATA_ADDR,(uint16 *)buf,sizeof(innerSaveTypeDef));
	memcpy(&innerSaveTypeDef,buf,sizeof(innerSaveTypeDef));
	
	memset(innerSaveTypeDef.ProgVersion,0x00,sizeof(innerSaveTypeDef.ProgVersion));
	memset(innerSaveTypeDef.progDate,0x00,sizeof(innerSaveTypeDef.progDate));
	memset(innerSaveTypeDef.ProgSize,0x00,sizeof(innerSaveTypeDef.ProgSize));
	memset(innerSaveTypeDef.progMd5,0x00,sizeof(innerSaveTypeDef.progMd5));
	
	memcpy(innerSaveTypeDef.ProgVersion,"V001",strlen("V001"));
	memcpy(innerSaveTypeDef.progDate,"20.4.1.14.50.0",strlen("20.4.1.14.50.0"));
	memcpy(innerSaveTypeDef.ProgSize,"1111",strlen("1111"));
	memcpy(innerSaveTypeDef.progMd5,"abcdefghijklmnopqrstabcdefgjaaaa",strlen("abcdefghijklmnopqrstabcdefgjaaaa"));
	
	
	if(innerSaveTypeDef.configValueTypeDef.configedFlag!=0x01)			//����û�б���ʼ��������ʼ����Ĭ��ֵ
	{
		innerSaveTypeDef.configValueTypeDef.configedFlag=0x01;			//�����Ѿ����ù�
		memcpy(&innerSaveTypeDef.configValueTypeDef.localDate,DEFAULT_DATE,sizeof(DEFAULT_DATE));//��������
		memcpy(&innerSaveTypeDef.configValueTypeDef.mac,DEFAULT_MAC,sizeof(DEFAULT_MAC));//MAC��ַ
		memcpy(&innerSaveTypeDef.configValueTypeDef.lip,DEFAULT_IP,sizeof(DEFAULT_IP));//����IP��ַ
		memcpy(&innerSaveTypeDef.configValueTypeDef.sub,DEFAULT_SUBNET,sizeof(DEFAULT_SUBNET));//��������
		memcpy(&innerSaveTypeDef.configValueTypeDef.gw,DEFAULT_GATEWAY,sizeof(DEFAULT_GATEWAY));//����
		memcpy(&innerSaveTypeDef.configValueTypeDef.dns,DEFAULT_DNS,sizeof(DEFAULT_DNS));//DNS��������ַ
		memcpy(&innerSaveTypeDef.configValueTypeDef.rip,DEFAULT_IP,sizeof(DEFAULT_IP));//IPԶ��IP��ַ*/
		innerSaveTypeDef.configValueTypeDef.port485=DEFAULT_PORT;
		innerSaveTypeDef.configValueTypeDef.id4851=DEFAULT_4851ID;//485�ӻ�һID
		innerSaveTypeDef.configValueTypeDef.id4852=DEFAULT_4852ID;//485�ӻ���ID
		innerSaveTypeDef.configValueTypeDef.canLimit=DEFAULT_CANLIMIT;//CAN�޶�λ
		innerSaveTypeDef.configValueTypeDef.canFilter=DEFAULT_CANFILTER;//CAN����λ
		for(i=0;i<sizeof(innerSaveTypeDef.configValueTypeDef.slaveId);i++)//�ӻ�ID	
		{
			innerSaveTypeDef.configValueTypeDef.slaveId[i]=DEFAULT_SLAVADDRSTART+i;
		}		
		for(i=0;i<sizeof(innerSaveTypeDef.configValueTypeDef.slaveState);i++)//�ӻ�״̬
		{
			innerSaveTypeDef.configValueTypeDef.slaveState[i]=DEFAULT_SLAVESTATE;
		}
		memset(buf,0x00,sizeof(buf));
		memcpy(buf,&innerSaveTypeDef,sizeof(innerSaveTypeDef));
		// STMFLASH_Write(SAVE_DATA_ADDR,(uint16 *)buf,sizeof(innerSaveTypeDef));//���³ɹ��洢������Ϣ
			
	}
	// local_tcp_port = innerSaveTypeDef.configValueTypeDef.port485;
}



















