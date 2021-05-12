#include "ds18b20.h"
#include "delay.h"	

//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
u8 DS18B20_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PORTA口时钟 
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		//PORTA0 推挽输出
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOA,GPIO_Pin_0);    //输出1

	DS18B20_Rst();

	return DS18B20_Check();
} 
//复位DS18B20
void DS18B20_Rst(void)	   
{                 
    	DS18B20_IO_OUT(); //SET PA0 OUTPUT
    	DS18B20_DQ_OUT=0; //总线拉低
    	delay_us(750);    //拉低750us  至少480us
    	DS18B20_DQ_OUT=1; //释放总线 等待DS18B20回复一个脉冲
	delay_us(68);    //60-75 
}
//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
u8 DS18B20_Check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();//SET PA0 INPUT	 
   	 while (DS18B20_DQ_IN&&retry<200)//读DQ
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
	while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}
//从DS18B20读取一个位
//返回值：1/0
u8 DS18B20_Read_Bit(void) 			 // read one bit
{
    	u8 data;
	DS18B20_IO_OUT();//SET PA0 OUTPUT
	DS18B20_DQ_OUT=0; 
	delay_us(2);
    	DS18B20_DQ_OUT=1; 
	DS18B20_IO_IN();//SET PA0 INPUT
	delay_us(12);
	if(DS18B20_DQ_IN)data=1;
    	else data=0;	 
    	delay_us(50);           
    	return data;
}
//从DS18B20读取一个字节
//返回值：读到的数据
u8 DS18B20_Read_Byte(void)    // read one byte
{        
    	u8 i,j,data;
    	data=0;
	for (i=1;i<=8;i++) 
	{
        	j=DS18B20_Read_Bit();
        	data=(j<<7)|(data>>1);
   	 }						    
    	return data;
}
//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_Write_Byte(u8 dat)     
 {             
  	u8 j;
    	u8 testb;
	DS18B20_IO_OUT();//SET PA0 OUTPUT;
    	for (j=1;j<=8;j++) 
	{
      		testb=dat&0x01;
        	dat=dat>>1;
        	if (testb) 
        	{
            		DS18B20_DQ_OUT=0;// Write 1
            		delay_us(2);                            
            		DS18B20_DQ_OUT=1;
            		delay_us(60);             
        	}
        else 
        {
            	DS18B20_DQ_OUT=0;// Write 0
            	delay_us(60);             
            	DS18B20_DQ_OUT=1;
            	delay_us(2);                          
        }
    }
}
//开始温度转换
void DS18B20_Start(void)// ds1820 start convert
{   						               
    	DS18B20_Rst();	   
	DS18B20_Check();	 		//初始化
    	DS18B20_Write_Byte(SKIP_ROM);// skip rom
    	DS18B20_Write_Byte(CONVERT_T);// convert T 命令 开始测量周围环境变量
} 
 
//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DS18B20_Get_Temp(void)
{
    	u8 temp;
    	u8 TL,TH;
	short tem;
    	DS18B20_Start ();//测量的过程完成之后有一定的时间 再去读scratchpad
    	DS18B20_Rst();
    	DS18B20_Check(); //最多750ms 
    	DS18B20_Write_Byte(SKIP_ROM);// skip rom
    	DS18B20_Write_Byte(READ_SCRATCHPAD);// read scratchpad     
    	TL=DS18B20_Read_Byte(); // LSB   read byte0
    	TH=DS18B20_Read_Byte(); // MSB   read byte1
	    	  
    	if(TH>7)//00000111
    	{
        	TH=~TH;
        	TL=~TL; 
        	temp=0;//温度为负  
    	}else temp=1;//温度为正	  	  
    	tem=TH; //获得高八位
    	tem<<=8;    
    	tem+=TL;//获得低八位
    	tem=(float)tem*0.625;//转换 右移4位    
	if(temp)return tem; //返回温度值
	else return -tem;    
} 
 
u8 IsTooHot(void){
	short temperature;
	temperature=DS18B20_Get_Temp();
	if(temperature<0){
		temperature=-temperature;
	}

	if(temperature/10>49)return CLOSE;
	 return CONTINUE;
}
