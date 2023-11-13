/*
 * mpu9250.c
 *
 *  Created on: Oct 17, 2023
 *      Author: kaixuan-jiang
 */

#include "mpu9250.h"
#include "i2c.h"
#include "stdio.h"
#include <math.h>

/**
  * @brief: Initialize MPU9250
  * @retval:  Return value: 0 for success
             Other values represent error codes
  */
uint8_t MPU9250_Init(void)
{
	uint8_t res=0;

	uint8_t mode_addr_val[6] = {resetval,wakeval,closeval,modebypass,clesel,signlemes};

  HAL_I2C_Mem_Write(&hi2c1,MPU9250_ADDR,MPU_PWR_MGMT1_REG,1,mode_addr_val,1,1000);  //reset
  HAL_Delay(100);
  HAL_I2C_Mem_Write(&hi2c1,MPU9250_ADDR,MPU_PWR_MGMT1_REG,1,&mode_addr_val[1],1,1000); //wake up mpu9250

  MPU_Set_Gyro_Fsr(1);					        	//±500dps=±500°/s   ±32768
  MPU_Set_Accel_Fsr(0);					       	 	//±2g=±2*9.8m/s2    ±32768
  MPU_Set_Rate(50);						       	 	//50Hz


  HAL_I2C_Mem_Write(&hi2c1,MPU9250_ADDR,MPU_INT_EN_REG,1,&mode_addr_val[2],1,1000);   //Disable all interrupts
  HAL_I2C_Mem_Write(&hi2c1,MPU9250_ADDR,MPU_USER_CTRL_REG,1,&mode_addr_val[2],1,1000);//Disable I2C master mod
  HAL_I2C_Mem_Write(&hi2c1,MPU9250_ADDR,MPU_FIFO_EN_REG,1,&mode_addr_val[2],1,1000);//Disable FIFO
  HAL_I2C_Mem_Write(&hi2c1,MPU9250_ADDR,MPU_INTBP_CFG_REG,1,&mode_addr_val[3],1,1000);//Enable bypass mode

   HAL_I2C_Mem_Read(&hi2c1,MPU9250_ADDR,MPU_DEVICE_ID_REG,1,&res,1,1000);
    if(res==MPU6500_ID)
    {
       printf("read MPU9250ID OK\r\n");
       HAL_I2C_Mem_Write(&hi2c1,MPU9250_ADDR,MPU_PWR_MGMT1_REG,1,&mode_addr_val[4],1,1000);//Set CLKSEL with PLL X-axis as reference.
       HAL_I2C_Mem_Write(&hi2c1,MPU9250_ADDR,MPU_PWR_MGMT2_REG,1,&mode_addr_val[2],1,1000);//Acceleration and gyroscope are both operational
	   MPU_Set_Rate(50);						       	//Set the sampling rate to 50Hz.
    }
	else return 1;
  HAL_I2C_Mem_Read(&hi2c1,AK8963_ADDR,MAG_WIA,1,&res,1,1000);
    if(res==AK8963_ID)
    {
    	printf("read AK8963 ok\r\n");
        HAL_I2C_Mem_Write(&hi2c1,AK8963_ADDR,MAG_CNTL1,1,&mode_addr_val[5],1,1000);//Set AK8963 to single measurement mode.'
    }
	else return 1;

    return 0;
}

/**
  * @brief: Set the full-scale range for the MPU9250 gyroscope sensor
  * @param: fsr: 0 for ±250dps, 1 for ±500dps, 2 for ±1000dps, 3 for ±2000dps
  * @retval:  Return value: 0 for successful setting
              Other values indicate a failure to set
  */
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
	uint8_t fsrval = (fsr<<3) ;
	return  HAL_I2C_Mem_Write(&hi2c1,MPU9250_ADDR,MPU_GYRO_CFG_REG,1,&fsrval,1,1000);//设置陀螺仪满量程范围
}
//设置MPU9250加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
	uint8_t fsrval = (fsr<<3) ;
	return  HAL_I2C_Mem_Write(&hi2c1,MPU9250_ADDR,MPU_ACCEL_CFG_REG,1,&fsrval,1,1000);//设置加速度传感器满量程范围
}

/**
  * @brief: Set the digital low-pass filter for the MPU9250
  * @param: lpf: Digital low-pass filter frequency (Hz)
  * @retval: Return value: 0 for successful setting
  *               Other values indicate a failure to set
  */
uint8_t MPU_Set_LPF(uint16_t lpf)
{
	uint8_t data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6;
	return HAL_I2C_Mem_Write(&hi2c1,MPU9250_ADDR,MPU_CFG_REG,1,&data,1,1000);
}

/**
  * @brief Set the sampling rate for the MPU9250 (assuming Fs=1KHz)
  * @param rate: Sampling rate in the range of 4 to 1000 Hz
  * @retval 0 for successful setting
  *         Other values indicate a failure to set
  */
uint8_t MPU_Set_Rate(uint16_t rate)
{
	uint8_t data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data = HAL_I2C_Mem_Write(&hi2c1,MPU9250_ADDR,MPU_SAMPLE_RATE_REG,1,&data,1,1000);
 	return MPU_Set_LPF(rate/2);	//自动设置LPF为采样率的一半
}

/**
  * @brief Get the temperature value
  * @retval Temperature value (multiplied by 100)
  */
short MPU_Get_Temperature(void)
{
	uint8_t buf[2];
    short raw;
	float temp;
	HAL_I2C_Mem_Read(&hi2c1,MPU9250_ADDR,MPU_TEMP_OUTH_REG,1,buf,2,1000);
    raw=((uint16_t)buf[0]<<8)|buf[1];
    temp=21+((double)raw)/333.87;
    return temp*100;
}

/**
  * @brief Get the gyroscope values (raw data)
  * @param gyro[0]: Gyroscope raw reading for the x-axis (signed)
  * @param gyro[1]: Gyroscope raw reading for the y-axis (signed)
  * @param gyro[2]: Gyroscope raw reading for the z-axis (signed)
  * @retval 0 for success
  *         Other values indicate an error code
  */
uint8_t MPU_Get_Gyroscope(uint16_t *gyro)
{
    uint8_t buf[6];
    HAL_StatusTypeDef res = 0x00;
	res = HAL_I2C_Mem_Read(&hi2c1,MPU9250_ADDR,MPU_GYRO_XOUTH_REG,1,buf,6,1000);
	if(HAL_OK == res)
	{
		gyro[0] =(((uint16_t)buf[0]<<8)|buf[1]);
		gyro[1] =(((uint16_t)buf[2]<<8)|buf[3]);
		gyro[2] = (((uint16_t)buf[4]<<8)|buf[5]);
	}
/*	printf("Gyroscope Data:\r\n");
	printf("X: %d\r\n", gyro[0]);
	printf("Y: %d\r\n", gyro[1]);
	printf("Z: %d\r\n", gyro[2]);*/
    return res;
}
/**
  * @brief Get the accelerometer values (raw data)
  * @param accel[0]: Accelerometer raw reading for the x-axis (signed)
  * @param accel[1]: Accelerometer raw reading for the y-axis (signed)
  * @param accel[2]: Accelerometer raw reading for the z-axis (signed)
  * @retval 0 for success
  *         Other values indicate an error code
  */
uint8_t MPU_Get_Accelerometer(uint16_t *accel)
{
	uint8_t buf[6],res;
	res = HAL_I2C_Mem_Read(&hi2c1,MPU9250_ADDR,MPU_ACCEL_XOUTH_REG,1,buf,6,1000);
	if(HAL_OK == res)
	{
		accel[0]=((uint16_t)buf[0]<<8)|buf[1];
		accel[1]=((uint16_t)buf[2]<<8)|buf[3];
		accel[2]=((uint16_t)buf[4]<<8)|buf[5];
	}
	/*printf("Accelerometer Data:\r\n");
	printf("X: %d\r\n",  accel[0]);
	printf("Y: %d\r\n",  accel[1]);
	printf("Z: %d\r\n",  accel[2]);*/
    return res;
}
/**
  * @brief Get the magnetometer values (raw data)
  * @param magnet[0]: Magnetometer raw reading for the x-axis (signed)
  * @param magnet[1]: Magnetometer raw reading for the y-axis (signed)
  * @param magnet[2]: Magnetometer raw reading for the z-axis (signed)
  * @retval 0 for success
  *         Other values indicate an error code
  */
uint8_t MPU_Get_Magnetometer(uint16_t *magnet)
{
	uint8_t buf[6],res;
	uint8_t signlemess = 0x11 ;
	res = HAL_I2C_Mem_Read(&hi2c1,AK8963_ADDR,MAG_XOUT_L,1,buf,6,1000);
	if(HAL_OK == res)
	{
		magnet[0] =((uint16_t)buf[1]<<8)|buf[0];
		magnet[1]=((uint16_t)buf[3]<<8)|buf[2];
		magnet[2]=((uint16_t)buf[5]<<8)|buf[4];
	}
   HAL_I2C_Mem_Write(&hi2c1,AK8963_ADDR,MAG_CNTL1,1,&signlemess,1,1000);//AK8963每次读完以后都需要重新设置为单次测量模式

   printf("Magnetometer Data:\r\n");
   printf("X: %d\r\n", magnet[0]);
   printf("Y: %d\r\n", magnet[1]);
   printf("Z: %d\r\n", magnet[2]);
   return res;
}

/**
  * @brief Find offsets for each axis of the gyroscope.
  * @param I2Cx Pointer to the I2C structure configuration.
  * @param numCalPoints Number of data points to average.
  */
void MPU_calibrateGyro(uint16_t numCalPoints)
{
   // Init
   int32_t x = 0;
   int32_t y = 0;
   int32_t z = 0;

   // Zero guard
   if (numCalPoints == 0)
   {
       numCalPoints = 1;
   }

   // Save specified number of points
   for (uint16_t ii = 0; ii < numCalPoints; ii++)
   {
     //  MPU_readRawData(I2Cx);
       MPU_Get_Gyroscope(gyro);


       x += *(p_gyro+0);
       y += *(p_gyro+1);
       z += *(p_gyro+2);
       HAL_Delay(3);
   }

   // Average the saved data points to find the gyroscope offset
   gyroCal.x = (float)x / (float)numCalPoints;
   gyroCal.y = (float)y / (float)numCalPoints;
   gyroCal.z = (float)z / (float)numCalPoints;
}


/// @brief Calculate the real world sensor values.
void MPU_readProcessedData(void)
{
   // Get raw values from the IMU
   MPU_Get_Gyroscope(gyro);
   MPU_Get_Accelerometer(accel);

   // Convert accelerometer values to g's
   sensorData.ax = *(p_accel+0) / aScaleFactor;
   sensorData.ay = *(p_accel+1) / aScaleFactor;
   sensorData.az = *(p_accel+2) / aScaleFactor;

   // Compensate for gyro offset
   sensorData.gx = *(p_gyro+0) - gyroCal.x;
   sensorData.gy = *(p_gyro+1) - gyroCal.y;
   sensorData.gz = *(p_gyro+2) - gyroCal.z;

   // Convert gyro values to deg/s
   sensorData.gx /= gScaleFactor;
   sensorData.gy /= gScaleFactor;
   sensorData.gz /= gScaleFactor;
}

/**
  * @brief Calculate the attitude of the sensor in degrees using a complementary filter.
 */
void MPU_calcAttitude(void)
{
   // Read processed data
   MPU_readProcessedData();

   // Complementary filter
   float accelPitch = atan2(sensorData.ay, sensorData.az) * RAD2DEG;
   float accelRoll = atan2(sensorData.ax, sensorData.az) * RAD2DEG;
   float tau = 0.98;
   float dt  = 0.004;

   attitude.r = tau * (attitude.r - sensorData.gy * dt) + (1 - tau) * accelRoll; //roll
   attitude.p = tau * (attitude.p + sensorData.gx * dt) + (1 - tau) * accelPitch;//pitch
   attitude.y += sensorData.gz * dt;// yaw
}

