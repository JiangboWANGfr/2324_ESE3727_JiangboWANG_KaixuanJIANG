/*
 * adxl345.c
 *
 *  Created on: Oct 17, 2023
 *      Author: jkx
 */

#include "adxl345.h"
#include "i2c.h"
#include "math.h"
#include "stdio.h"

/*init ADXL345  success 0 ; fail 1 */
uint8_t ADXL345_Init(void)
{
	uint8_t id,val;
	HAL_I2C_Mem_Read(&hi2c2,ADXL_READ,DEVICE_ID,I2C_MEMADD_SIZE_8BIT,&id,1,0xff);
	if(id ==0XE5)
	{
		val = 0x2B;		//avtive low , full resolution mode, right-justified mode, 16g Range Setting
		HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,DATA_FORMAT,I2C_MEMADD_SIZE_8BIT,&val,1,0xFF);

		val = 0x0A;		//100 Hz output data rate
		HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,BW_RATE,I2C_MEMADD_SIZE_8BIT,&val,1,0xFF);

		val = 0x28;		//link bit , measurement mode
		HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,POWER_CTL,I2C_MEMADD_SIZE_8BIT,&val,1,0xFF);

		val = 0x00;
		HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,INT_ENABLE,I2C_MEMADD_SIZE_8BIT,&val,1,0xFF);
		HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,OFSX,I2C_MEMADD_SIZE_8BIT,&val,1,0xFF);
		HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,OFSY,I2C_MEMADD_SIZE_8BIT,&val,1,0xFF);
		HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,OFSZ,I2C_MEMADD_SIZE_8BIT,&val,1,0xFF);

		printf("successfully ADXL345 init\r\n");
		return 0;
	}
	printf("ADXL345 init Error!\r\n");
	return 1;
}


/* get  x,y,z average of ten times */
void ADXL345_RD_Avval(short *x,short *y,short *z)
{
	short tx=0,ty=0,tz=0;
	uint8_t i;
	for(i=0;i<10;i++)
	{
		ADXL345_RD_XYZ(x,y,z);
		HAL_Delay(10);
		tx+=(short)*x;
		ty+=(short)*y;
		tz+=(short)*z;
	}
	*x=tx/10;
	*y=ty/10;
	*z=tz/10;
}

/* get x,y,z datas of one time */
void ADXL345_RD_XYZ(short *x,short *y,short *z)
{
	uint8_t buf[6];

	HAL_I2C_Mem_Read(&hi2c2,ADXL_READ,DATA_X0,I2C_MEMADD_SIZE_8BIT,&buf[0],1,0xFF);
	HAL_I2C_Mem_Read(&hi2c2,ADXL_READ,DATA_X1,I2C_MEMADD_SIZE_8BIT,&buf[1],1,0xFF);
	HAL_I2C_Mem_Read(&hi2c2,ADXL_READ,DATA_Y0,I2C_MEMADD_SIZE_8BIT,&buf[2],1,0xFF);
	HAL_I2C_Mem_Read(&hi2c2,ADXL_READ,DATA_Y1,I2C_MEMADD_SIZE_8BIT,&buf[3],1,0xFF);
	HAL_I2C_Mem_Read(&hi2c2,ADXL_READ,DATA_Z0,I2C_MEMADD_SIZE_8BIT,&buf[4],1,0xFF);
	HAL_I2C_Mem_Read(&hi2c2,ADXL_READ,DATA_Z1,I2C_MEMADD_SIZE_8BIT,&buf[5],1,0xFF);

	*x=(short)(((uint16_t)buf[1]<<8)+buf[0]);
	*y=(short)(((uint16_t)buf[3]<<8)+buf[2]);
	*z=(short)(((uint16_t)buf[5]<<8)+buf[4]);
}

/*auto-calibration*/
void ADXL345_AUTO_Adjust(char *xval,char *yval,char *zval)
{
	short tx,ty,tz;
	uint8_t i, val;
	short offx=0,offy=0,offz=0;
	val = 0x00;		//sleep mode first .
	HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,POWER_CTL,I2C_MEMADD_SIZE_8BIT,&val,1,0xFF);
	//ADXL345_WR_Reg(POWER_CTL,0x00);
	HAL_Delay(100);

	val = 0x2B;		//avtive low , full resolution mode, right-justified mode, 16g Range Setting
	HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,DATA_FORMAT,I2C_MEMADD_SIZE_8BIT,&val,1,0xFF);

	val = 0x0A;		//100 Hz output data rate
	HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,BW_RATE,I2C_MEMADD_SIZE_8BIT,&val,1,0xFF);

	val = 0x28;		//link bit , measurement mode
	HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,POWER_CTL,I2C_MEMADD_SIZE_8BIT,&val,1,0xFF);

	val = 0x00;
	HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,INT_ENABLE,I2C_MEMADD_SIZE_8BIT,&val,1,0xFF);
	HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,OFSX,I2C_MEMADD_SIZE_8BIT,&val,1,0xFF);
	HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,OFSY,I2C_MEMADD_SIZE_8BIT,&val,1,0xFF);
	HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,OFSZ,I2C_MEMADD_SIZE_8BIT,&val,1,0xFF);

	HAL_Delay(12);
	for(i=0;i<10;i++)
	{
		ADXL345_RD_Avval(&tx,&ty,&tz);
		offx+=tx;
		offy+=ty;
		offz+=tz;
	}
	offx/=10;
	offy/=10;
	offz/=10;
	*xval=-offx/4;
	*yval=-offy/4;
	*zval=-(offz-256)/4;
	HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,OFSX,I2C_MEMADD_SIZE_8BIT,(uint8_t *)xval,1,0xFF);
	HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,OFSY,I2C_MEMADD_SIZE_8BIT,(uint8_t *)yval,1,0xFF);
	HAL_I2C_Mem_Write(&hi2c2,ADXL_WRITE,OFSZ,I2C_MEMADD_SIZE_8BIT,(uint8_t *)zval,1,0xFF);
}

/* Read ADXL345 data 'times' times and then calculate the average.
   x, y, z: Data read.
   times: Number of readings. */
void ADXL345_Read_Average(short *x,short *y,short *z,uint8_t times)
{
	uint8_t i;
	short tx,ty,tz;
	*x=0;
	*y=0;
	*z=0;
	if(times)
	{
		for(i=0;i<times;i++)//read times times in succession
		{
			ADXL345_RD_XYZ(&tx,&ty,&tz);
			*x+=tx;
			*y+=ty;
			*z+=tz;
			HAL_Delay(5);
		}
		*x/=times;
		*y/=times;
		*z/=times;
	}
}


/* Get the angle
   x, y, z: Components of gravity acceleration in the x, y, and z directions (numeric values without units).
   dir: Desired angle - 0 for angle with the Z-axis, 1 for angle with the X-axis, 2 for angle with the Y-axis.
   Return value: Angle value in tenths of a degree.
   'res' is in radians, and convert it to degrees by multiplying it by 180/3.14.*/
short ADXL345_Get_Angle(float x,float y,float z,uint8_t dir)
{
	float temp;
 	float res=0;
	switch(dir)
	{
		case 0:// 0 for angle with the Z-axis
 			temp=sqrt((x*x+y*y))/z;
 			res=atan(temp);
 			break;
		case 1:// 1 for angle with the X-axis
 			temp=x/sqrt((y*y+z*z));
 			res=atan(temp);
 			break;
 		case 2:// 2 for angle with the Y-axis
 			temp=y/sqrt((x*x+z*z));
 			res=atan(temp);
 			break;
 	}
	return res*180/3.14;
}


/*show the data*/
void ADXL345_Data_Show()
{
	short x,y,z;
	short xang,yang,zang;

	ADXL345_Read_Average(&x,&y,&z,10);  //读取x,y,z 3个方向的加速度值 总共10次
	printf("X Val: %d.%d\n", x/100, x%100);
	printf("Y Val: %d.%d\n", y/100, y%100);
	printf("Z Val: %d.%d\n", z/100, z%100);

	xang=ADXL345_Get_Angle(x,y,z,1);
	yang=ADXL345_Get_Angle(x,y,z,2);
	zang=ADXL345_Get_Angle(x,y,z,0);
	printf("X Ang: %d.%d\n", xang/10, xang%10);
	printf("X Ang: %d.%d\n", yang/10, yang%10);
	printf("X Ang: %d.%d\n", zang/10, zang%10);

}
