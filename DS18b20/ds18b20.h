#ifndef __DS18B20_H
#define __DS18B20_H 
#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//�жϵ�ǰ�����¶�
//�������50(-1)�� ����CLOSE 1 ����CONTINUE 0
//////////////////////////////////////////////////////////////////////////////////
#define ERROR 0
#define CLOSE 1
#define CONTINUE 0
#define SKIP_ROM 0xcc
#define READ_SCRATCHPAD 0xbe
#define CONVERT_T 0x44
//IO��������
#define DS18B20_IO_IN()  {GPIOA->CRL&=0XFFFFFFF0;GPIOA->CRL|=8<<0;}
#define DS18B20_IO_OUT() {GPIOA->CRL&=0XFFFFFFF0;GPIOA->CRL|=3<<0;}
////IO��������											   
#define	DS18B20_DQ_OUT PAout(0) //���ݶ˿�	PA0
#define	DS18B20_DQ_IN  PAin(0)  //���ݶ˿�	PA0 
   	
u8 DS18B20_Init(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp(void);	//��ȡ�¶�
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);		//����һ���ֽ�
u8 DS18B20_Read_Bit(void);		//����һ��λ
u8 DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);			//��λDS18B20    
u8 IsTooHot(void);				//����¶ȸ���50�� ����CLOSE

#endif















