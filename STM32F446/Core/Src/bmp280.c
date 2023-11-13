/*
 * bmp280.c
 *
 *  Created on: Oct 10, 2023
 *      Author: kaixuan jiang
 */

#include "bmp280.h"
#include "i2c.h"
#include "stdio.h"


static bmp280_calib_param_t g_bmp280_calib_table;

/**
  * @brief: Initialize BMP280
  */
void BMP280_Init()
{
	bmp280_id_check();
	bmp280_set_power_mode(NORMAL_MODE);
	bmp280_set_Pressure_oversampling(PRESS_OVERSAMP_16X);
	bmp280_set_Temperature_oversampling(TEMP_OVERSAMP_2X);
	bmp280_get_calib_param();
	printf("successfully BMP280 init\r\n");

}
/**
  * @brief: bmp280_id_check
  */
uint8_t bmp280_id_check()
{
	HAL_StatusTypeDef hi2c2_status = 0x00;
	/*ID check*/
	//BMP280_CHIP_ID_REG = 0XD0,BMP280_CHIP_ID_VAL = 0X58
	uint8_t id_addr_val[3] = {0xD0,0x58,0x00};
	hi2c2_status = HAL_I2C_Mem_Read(&hi2c1,BMP280_I2C_ADDR,id_addr_val[0],1,&id_addr_val[2],1,1000);
	if(HAL_OK!=hi2c2_status){
		printf("get part_id error\r\n");
		return 1;
	}
	if(id_addr_val[1]!=id_addr_val[2]){
		printf("bmp280_validate_id is error\r\n");
		return 1;
	}
		printf("bmp280_id:%02X\r\n",id_addr_val[2]);
	return 0;
}
/**
  * @brief: bmp280_reset
  */
uint8_t bmp280_reset()
{
	HAL_StatusTypeDef hi2c2_status = 0x00;
	/*reset*/
	//BMP280_RST_REG = 0XE0,BMP280_SOFT_RESRT_VALUE=0XB6
	uint8_t addr_val[2] = {BMP280_RST_REG,BMP280_SOFT_RESRT_VALUE};
	hi2c2_status = HAL_I2C_Mem_Write(&hi2c1,BMP280_I2C_ADDR,addr_val[0],1,&addr_val[1],1,1000);
	if(HAL_OK!=hi2c2_status){
		printf("bmp280_reset error\r\n");
		return 1;
	}
	return 0;
}
/**
  * @brief: bmp280_set_power_mode
  */
uint8_t bmp280_set_power_mode(bmp280_power_mode mode)
{
	uint8_t dev_mode = 0x00;
	    switch (mode) {
	        case FORCED_MODE:{
	        	dev_mode = (uint8_t)BMP280_FORCED_MODE;
	            break;
	        }
	        case SLEEP_MODE: {
	            dev_mode = (uint8_t)BMP280_SLEEP_MODE;
	            break;
	        }
	        case NORMAL_MODE: {
	            dev_mode = (uint8_t)BMP280_NORMAL_MODE;
	            break;
	        }
	        default:
	            return 1;
	    }

		HAL_StatusTypeDef hi2c1_status = 0x00;
		//BMP280_CTRL_MEAS_REG=0XF4
		uint8_t mode_addr_val[3] = {BMP280_CTRL_MEAS_REG,0x00,dev_mode};
		uint8_t reg_value;

		hi2c1_status = HAL_I2C_Mem_Read(&hi2c1,BMP280_I2C_ADDR,mode_addr_val[0],1,&mode_addr_val[1],1,1000);
		if(HAL_OK!=hi2c1_status){
			printf("get bmp280_power_mode error\r\n");
			return 1;
		}
			//printf("bmp280 current value in regs 0xF4:%02X\r\n",mode_addr_val[1]);

			// clear bit 0 1 and set new value of mode
			reg_value = (mode_addr_val[1] & 0xFC) | (mode_addr_val[2] & 0x03);

		hi2c1_status = HAL_I2C_Mem_Write(&hi2c1,BMP280_I2C_ADDR,mode_addr_val[0],1,&reg_value,1,1000);
		if(HAL_OK!=hi2c1_status){
			printf("bmp280_set_power_mode error\r\n");
			return 1;
		}
			//printf("bmp280_set_power_mode success\r\n");

		hi2c1_status = HAL_I2C_Mem_Read(&hi2c1,BMP280_I2C_ADDR,mode_addr_val[0],1,&mode_addr_val[1],1,1000);
		if(HAL_OK!=hi2c1_status){
			printf("get bmp280_power_mode error after configure\r\n");
			return 1;
			}
			//printf("bmp280  value in regs after configure :%02X\r\n",mode_addr_val[1]);
			return 0;

}
/**
  * @brief: bmp280_set_Pressure_oversampling
  */
uint8_t bmp280_set_Pressure_oversampling(bmp280_pressure_mode mode )
{
	   uint8_t value = 0;
	   uint8_t temp  = 0;
	   uint8_t reg_value  = 0;

	    switch (mode) {
	    case PRESS_OVERSAMP_1X:
	        temp = BMP280_OVERSAMP_1X;
	        break;

	    case PRESS_OVERSAMP_2X:
	        temp = BMP280_OVERSAMP_2X;
	        break;

	    case PRESS_OVERSAMP_4X:
	        temp = BMP280_OVERSAMP_4X;
	        break;

	    case PRESS_OVERSAMP_8X:
	        temp = BMP280_OVERSAMP_8X;
	        break;

	    case PRESS_OVERSAMP_16X:
	        temp = BMP280_OVERSAMP_16X;
	        break;

	    default:
	        return 1;
	    }
	    HAL_StatusTypeDef hi2c1_status = 0x00;

		hi2c1_status = HAL_I2C_Mem_Read(&hi2c1,BMP280_I2C_ADDR,BMP280_CTRL_MEAS_REG,1,&value,1,1000);
		if(HAL_OK!=hi2c1_status){
			printf("get bmp280_Pressure_oversampling_param error\r\n");
			return 1;
		}

		// clear bit 2 3 4  and set new value of mode
		reg_value = ( value & 0xE3) | ( (temp<<2) & 0x1C);

		hi2c1_status = HAL_I2C_Mem_Write(&hi2c1,BMP280_I2C_ADDR,BMP280_CTRL_MEAS_REG,1,&reg_value,1,1000);
		if(HAL_OK!=hi2c1_status){
			printf("bmp280_set_Pressure_oversampling error\r\n");
			return 1;
		}
		//printf("successfully BMP280 set Pressure_oversampling\r\n");

		hi2c1_status = HAL_I2C_Mem_Read(&hi2c1,BMP280_I2C_ADDR,BMP280_CTRL_MEAS_REG,1,&value,1,1000);
		if(HAL_OK!=hi2c1_status){
		printf("get bmp280_Pressure_oversampling_param after configure error\r\n");
		return 1;
		}
		//printf("bmp280_set_Pressure_oversampling value in regs after configure :%02X\r\n",value);

		return 0;

}
/**
  * @brief: bmp280_set_Temperature_oversampling
  */
uint8_t bmp280_set_Temperature_oversampling(bmp280_temperature_mode mode )
{
		   uint8_t value = 0;
		   uint8_t temp  = 0;
		   uint8_t reg_value  = 0;

		    switch (mode) {
		    case TEMP_OVERSAMP_1X:
		        temp = BMP280_OVERSAMP_1X;
		        break;

		    case TEMP_OVERSAMP_2X:
		        temp = BMP280_OVERSAMP_2X;
		        break;

		    case TEMP_OVERSAMP_4X:
		        temp = BMP280_OVERSAMP_4X;
		        break;

		    case TEMP_OVERSAMP_8X:
		        temp = BMP280_OVERSAMP_8X;
		        break;

		    case TEMP_OVERSAMP_16X:
		        temp = BMP280_OVERSAMP_16X;
		        break;

		    default:
		        return 1;
		    }
		    HAL_StatusTypeDef hi2c1_status = 0x00;

			hi2c1_status = HAL_I2C_Mem_Read(&hi2c1,BMP280_I2C_ADDR,BMP280_CTRL_MEAS_REG,1,&value,1,1000);
			if(HAL_OK!=hi2c1_status){
				printf("get bmp280_calib_param error\r\n");
				return 1;
			}

			// clear bit 5 6 7  and set new value of mode
			reg_value = ( value & 0x1F) | ( (temp<<5) & 0xE0);

			hi2c1_status = HAL_I2C_Mem_Write(&hi2c1,BMP280_I2C_ADDR,BMP280_CTRL_MEAS_REG,1,&reg_value,1,1000);
			if(HAL_OK!=hi2c1_status){
				printf("bmp280_set_Temperature_oversampling error\r\n");
				return 1;
			}
			//printf("successfully BMP280 set_Temperature_oversampling\r\n");

			hi2c1_status = HAL_I2C_Mem_Read(&hi2c1,BMP280_I2C_ADDR,BMP280_CTRL_MEAS_REG,1,&value,1,1000);
			if(HAL_OK!=hi2c1_status){
			printf("get bmp280_set_Temperature_oversampling after configure error\r\n");
			return 1;
			}
			//printf("bmp280_set_Temperature_oversampling value in regs after configure :%02X\r\n",value);



			return 0;

}
/**
  * @brief: bmp280_get_calib_param
  */
uint8_t bmp280_get_calib_param()
{
	HAL_StatusTypeDef hi2c1_status = 0x00;
	uint8_t data_u8[BMP280_CALIB_DATA_SIZE] = { 0x00 };
	hi2c1_status = HAL_I2C_Mem_Read(&hi2c1,BMP280_I2C_ADDR,BMP280_TEMPERATURE_CALIB_DIG_T1_LSB_REG
			,1,data_u8,BMP280_PRESSURE_TEMPERATURE_CALIB_DATA_LENGTH,1000);
	if(HAL_OK!=hi2c1_status){
		printf("get bmp280_calib_param error\r\n");
		return 1;
	}
	g_bmp280_calib_table.dig_T1 = (uint16_t)(
			(((uint16_t)((uint8_t)data_u8[BMP280_TEMPERATURE_CALIB_DIG_T1_MSB])) << BMP280_SHIFT_BY_08_BITS)
			| data_u8[BMP280_TEMPERATURE_CALIB_DIG_T1_LSB]);
	g_bmp280_calib_table.dig_T2 = (int16_t)(
	      (((int16_t)((int8_t)data_u8[BMP280_TEMPERATURE_CALIB_DIG_T2_MSB])) << BMP280_SHIFT_BY_08_BITS)
		   | data_u8[BMP280_TEMPERATURE_CALIB_DIG_T2_LSB]);
	g_bmp280_calib_table.dig_T3 = (int16_t)(
	      (((int16_t)((int8_t)data_u8[BMP280_TEMPERATURE_CALIB_DIG_T3_MSB])) << BMP280_SHIFT_BY_08_BITS)
		  | data_u8[BMP280_TEMPERATURE_CALIB_DIG_T3_LSB]);
	g_bmp280_calib_table.dig_P1 = (uint16_t)(
	      (((uint16_t)((uint8_t)data_u8[BMP280_PRESSURE_CALIB_DIG_P1_MSB])) << BMP280_SHIFT_BY_08_BITS)
		  | data_u8[BMP280_PRESSURE_CALIB_DIG_P1_LSB]);
	g_bmp280_calib_table.dig_P2 = (int16_t)(
	      (((int16_t)((int8_t)data_u8[BMP280_PRESSURE_CALIB_DIG_P2_MSB])) << BMP280_SHIFT_BY_08_BITS)
		  | data_u8[BMP280_PRESSURE_CALIB_DIG_P2_LSB]);
	g_bmp280_calib_table.dig_P3 = (int16_t)(
	      (((int16_t)((int8_t)data_u8[BMP280_PRESSURE_CALIB_DIG_P3_MSB])) << BMP280_SHIFT_BY_08_BITS)
		  | data_u8[BMP280_PRESSURE_CALIB_DIG_P3_LSB]);
	g_bmp280_calib_table.dig_P4 = (int16_t)(
	      (((int16_t)((int8_t)data_u8[BMP280_PRESSURE_CALIB_DIG_P4_MSB])) << BMP280_SHIFT_BY_08_BITS)
		  | data_u8[BMP280_PRESSURE_CALIB_DIG_P4_LSB]);
	g_bmp280_calib_table.dig_P5 = (int16_t)(
	      (((int16_t)((int8_t)data_u8[BMP280_PRESSURE_CALIB_DIG_P5_MSB])) << BMP280_SHIFT_BY_08_BITS)
		  | data_u8[BMP280_PRESSURE_CALIB_DIG_P5_LSB]);
	g_bmp280_calib_table.dig_P6 = (int16_t)(
	      (((int16_t)((int8_t)data_u8[BMP280_PRESSURE_CALIB_DIG_P6_MSB])) << BMP280_SHIFT_BY_08_BITS)
		  | data_u8[BMP280_PRESSURE_CALIB_DIG_P6_LSB]);
	g_bmp280_calib_table.dig_P7 = (int16_t)(
	      (((int16_t)((int8_t)data_u8[BMP280_PRESSURE_CALIB_DIG_P7_MSB])) << BMP280_SHIFT_BY_08_BITS)
		  | data_u8[BMP280_PRESSURE_CALIB_DIG_P7_LSB]);
	g_bmp280_calib_table.dig_P8 = (int16_t)(
	      (((int16_t)((int8_t)data_u8[BMP280_PRESSURE_CALIB_DIG_P8_MSB])) << BMP280_SHIFT_BY_08_BITS)
		  | data_u8[BMP280_PRESSURE_CALIB_DIG_P8_LSB]);
	g_bmp280_calib_table.dig_P9 = (int16_t)(
	      (((int16_t)((int8_t)data_u8[BMP280_PRESSURE_CALIB_DIG_P9_MSB])) << BMP280_SHIFT_BY_08_BITS)
		  | data_u8[BMP280_PRESSURE_CALIB_DIG_P9_LSB]);
	return 0;
}
/**
  * @brief: read_bmp280_temperature with compensation
  */
uint8_t read_bmp280_temperature(uint32_t *comp_temp)
{
	*comp_temp = 0;
	uint8_t data_u8[BMP280_TEMPERATURE_DATA_SIZE] = { 0 };
	int32_t temp;
	HAL_StatusTypeDef hi2c2_status = 0x00;

	hi2c2_status = HAL_I2C_Mem_Read(&hi2c1,BMP280_I2C_ADDR,BMP280_TEMPERATURE_MSB_REG,1,data_u8,BMP280_TEMPERATURE_DATA_SIZE,1000);
	if(HAL_OK!=hi2c2_status){
		printf("get read_bmp280_temperature error\r\n");
		return 1;
	}
	temp = (int)((((uint32_t)(data_u8[BMP280_TEMPERATURE_MSB_DATA]))
			<< BMP280_SHIFT_BY_12_BITS) | (((uint32_t)(data_u8[BMP280_TEMPERATURE_LSB_DATA]))
			<< BMP280_SHIFT_BY_04_BITS) | ((uint32_t)data_u8[BMP280_TEMPERATURE_XLSB_DATA]
			>> BMP280_SHIFT_BY_04_BITS));
	//
    int v_x1_u32r = 0;
    int v_x2_u32r = 0;
    v_x1_u32r = ((((temp >> BMP280_SHIFT_BY_03_BITS) -
    		((int)g_bmp280_calib_table.dig_T1 << BMP280_SHIFT_BY_01_BIT))) *
    		((int)g_bmp280_calib_table.dig_T2)) >> BMP280_SHIFT_BY_11_BITS;

    v_x2_u32r = (((((temp >> BMP280_SHIFT_BY_04_BITS) -
                    ((int)g_bmp280_calib_table.dig_T1)) *
                   ((temp >> BMP280_SHIFT_BY_04_BITS) -
                    ((int)g_bmp280_calib_table.dig_T1))) >>
                  BMP280_SHIFT_BY_12_BITS) *
                 ((int)g_bmp280_calib_table.dig_T3)) >>
                BMP280_SHIFT_BY_14_BITS;

    g_bmp280_calib_table.t_fine = v_x1_u32r + v_x2_u32r;

    temp = (g_bmp280_calib_table.t_fine * 5 + 128) >> BMP280_SHIFT_BY_08_BITS;
  //  printf("comp_temp: %d.%d\n", (int)temp/100, (int)(temp % 100));
      *comp_temp = temp;
    return 0;
}
/**
  * @brief: read_bmp280_pressure with compensation
  */
uint8_t read_bmp280_presse(uint32_t *comp_presse)
{
	*comp_presse = 0;
	uint8_t data_u8[BMP280_PRESSURE_DATA_SIZE] = { 0 };
	int32_t temp;
	HAL_StatusTypeDef hi2c1_status = 0x00;
	//BMP280_PRESSURE_MSB_REG=0XF7,BMP280_PRESSURE_DATA_SIZE=0X03
	hi2c1_status = HAL_I2C_Mem_Read(&hi2c1,BMP280_I2C_ADDR,BMP280_PRESSURE_MSB_REG,1,data_u8,BMP280_PRESSURE_DATA_SIZE,1000);
	if(HAL_OK!=hi2c1_status){
		printf("get read_bmp280_presse error\r\n");
		return 1;
	}
	temp = (int)(
			(((uint32_t)(data_u8[BMP280_PRESSURE_MSB_DATA])) << BMP280_SHIFT_BY_12_BITS)
			| (((uint32_t)(data_u8[BMP280_PRESSURE_LSB_DATA])) << BMP280_SHIFT_BY_04_BITS)
			| ((uint32_t)data_u8[BMP280_PRESSURE_XLSB_DATA]	>> BMP280_SHIFT_BY_04_BITS)
			);
	//
	int      v_x1_u32r = 0;
	int      v_x2_u32r = 0;

	v_x1_u32r = (((int)g_bmp280_calib_table.t_fine) >> BMP280_SHIFT_BY_01_BIT) - (int)64000;
	v_x2_u32r = (((v_x1_u32r >> BMP280_SHIFT_BY_02_BITS) * (v_x1_u32r >> BMP280_SHIFT_BY_02_BITS)) >>
	                 BMP280_SHIFT_BY_11_BITS) * ((int)g_bmp280_calib_table.dig_P6);
	v_x2_u32r = v_x2_u32r + ((v_x1_u32r * ((int)g_bmp280_calib_table.dig_P5)) << BMP280_SHIFT_BY_01_BIT);
	v_x2_u32r = (v_x2_u32r >> BMP280_SHIFT_BY_02_BITS) + (((int)g_bmp280_calib_table.dig_P4) << BMP280_SHIFT_BY_16_BITS);
	v_x1_u32r = (((g_bmp280_calib_table.dig_P3 *  (((v_x1_u32r >> BMP280_SHIFT_BY_02_BITS) *
			(v_x1_u32r >> BMP280_SHIFT_BY_02_BITS)) >> BMP280_SHIFT_BY_13_BITS)) >> BMP280_SHIFT_BY_03_BITS) +
	        ((((int)g_bmp280_calib_table.dig_P2) * v_x1_u32r) >> BMP280_SHIFT_BY_01_BIT)) >> BMP280_SHIFT_BY_18_BITS;
	v_x1_u32r = ((((32768 + v_x1_u32r)) * ((int)g_bmp280_calib_table.dig_P1)) >> BMP280_SHIFT_BY_15_BITS);
	if (v_x1_u32r == 0)
	{
		return 1; // avoid exception caused by division by zero
	}
	//
	uint32_t val_baro = 0;
	val_baro = (((uint32_t)(((int)1048576) - temp) - (v_x2_u32r >> BMP280_SHIFT_BY_12_BITS))) * 3125;

	if (val_baro < 0x80000000) {
		if (v_x1_u32r != 0) {
			val_baro =	(val_baro << BMP280_SHIFT_BY_01_BIT) / ((uint32_t)v_x1_u32r);
		} else {
			return 1;
		}
	} else if (v_x1_u32r != 0) {
		val_baro = (val_baro / (uint32_t)v_x1_u32r) * 2;
	} else {
	        return 1;
	}
	v_x1_u32r = (((int)g_bmp280_calib_table.dig_P9) * ((int)(((val_baro >> BMP280_SHIFT_BY_03_BITS) *
	    		(val_baro >> BMP280_SHIFT_BY_03_BITS)) >> BMP280_SHIFT_BY_13_BITS))) >> BMP280_SHIFT_BY_12_BITS;

	v_x2_u32r = (((int)(val_baro >> BMP280_SHIFT_BY_02_BITS)) * ((int)g_bmp280_calib_table.dig_P8)) >>	BMP280_SHIFT_BY_13_BITS;

	val_baro = (uint32_t)((int)val_baro +
	    		((v_x1_u32r + v_x2_u32r + g_bmp280_calib_table.dig_P7) >> BMP280_SHIFT_BY_04_BITS));

	//printf("comp_presse:%d.%d\r\n",(int)val_baro/100, (int)(val_baro % 100));
	*comp_presse = val_baro;
	return 0;
}
