/*
 * test.c
 *
 *  Created on: 2022/03/11
 *      Author: sssho
 */
#include"Controller/test.h"
#include"Controller/mode.h"
#include"Controller/state.h"
#include"Controller/callback.h"

#include"System/flags.h"
#include"System/sensing.h"
#include"System/motion.h"

#include"Hardware/basic_timer.h"
#include"Hardware/motor.h"
#include"Hardware/encoder.h"
#include"Hardware/interface_LED.h"

#include<stdio.h>

/*
 * @brief 	テストモードに向けた準備、無限待機編
 * @params1 フェイルセーフ動作のON/OFF,
 * @params2 モータ駆動のON/OFF
 */
void prepareForTest(uint8_t safety_flag,uint8_t motor_enable)
{
	  waitStarting();
	  getOffsets();
	  enableEncoder();
	  MF.FLAG.SAFETY = safety_flag & 0x01;
//	  basicTimerStart();
	  MF.FLAG.CALLBACK = 1;
	  if(motor_enable & 0x01)		enableMotors();
	  else if(motor_enable == 0) 	shutdownMotors();
}

/*
 * @brief 	壁センサの動作確認、printfでリアルタイム表示する仕様
 */
void testWallSensors(void)
{
	 prepareForTest(OFF, OFF);
	 uint8_t wall_info = 0x00;
	  while(1){
		  printf("FL: %4d L: %4d FF: %4d R: %4d FR: %4d\n",
				  sensor.wall_fl,sensor.wall_l,sensor.wall_ff,
				  sensor.wall_r,sensor.wall_fr);
		  waitMs(500);
		  wall_info = getWallInfo();
		  displayBinaryValueWithLEDs(wall_info);
	  }
}

/*
 * @brief 	エンコーダの動作確認、printfでリアルタイム表示する仕様
 */
void testEncoders(void)
{
	prepareForTest(OFF, OFF);
	  while(1){
			printf("dist : %.2lf,  L :%.2lf, R : %.2lf \n",
					mouse.mileage,
					sensor.mileage_l_mm,sensor.mileage_r_mm);
			waitMs(500);
	  }
}

/*
 * @brief 	IMUの動作確認（角度）、printfでリアルタイム表示する仕様
 */
void testIMU(void)
{
	prepareForTest(OFF,OFF);
	while(1){
		printf("Angle : %lf\n", mouse.angle);
		waitMs(500);
	 }
}

/*
 * @brief 	モータ関連のテスト全般
 */
void testMotions(void)
{
	uint8_t mode = modeSelect();
	displayBinaryValueWithLEDs(mode);
	prepareForTest(ON, ON);
	switch(mode){
		case 1:		//		6区画直線
	  		driveAccelMotion(SET_MM,max.velocity,OFF);
	  		moveHalfSectionAccel(ON, ON);
	  		moveOneSectionAccel(ON);
	  		moveOneSectionAccel(ON);
	  		moveOneSectionAccel(ON);
	  		moveOneSectionAccel(ON);
	  		moveOneSectionAccel(ON);
	  		moveHalfSectionDecel(OFF);
//	  		basicTimerPause();
	  		 MF.FLAG.CALLBACK = OFF;
	  		shutdownMotors();
	  		while(1){
	  			printf("dist: %.2f, angle: %.2f \n",mouse.mileage,mouse.angle);
	  			waitMs(1000);
	  		}
			break;
		case 2:		//		左無限スラローム
	  		  moveHalfSectionAccel(OFF, OFF);
	  		  tim_counter = 0;
			  moveSlalomL90();
			  moveOneSectionAccel(OFF);
	  		  for(uint8_t cnt=0;cnt<16;cnt++){
	  			  moveSlalomL90();
	  			  moveOneSectionAccel(OFF);
	  		  }
	  		  moveHalfSectionDecel(OFF);
	  		  spinRight180();

//	  		  basicTimerPause();
	  		 MF.FLAG.CALLBACK = OFF;
	  		  shutdownMotors();
			break;

		case 3:
			//	右無限スラローム
	  		  moveHalfSectionAccel(OFF, OFF);
	  		  tim_counter = 0;
	  		  for(uint8_t cnt=0;cnt<16;cnt++){
	  			  displayBinaryValueWithLEDs(0xff);
	  			  moveSlalomR90();
	  			  displayBinaryValueWithLEDs(0x00);
	  			  moveOneSectionAccel(OFF);
	  		  }
	  		  moveHalfSectionDecel(OFF);
	  		  spinRight180();

	  		 MF.FLAG.CALLBACK = OFF;
	  		  shutdownMotors();
	  		  break;
		case 4:
			tim_counter = 0;
	  		driveAccelMotion(SET_MM,max.velocity,OFF);
	  		moveHalfSectionAccel(ON, ON);
	  		moveHalfSectionDecel(OFF);

	  		 MF.FLAG.CALLBACK = OFF;
	  		shutdownMotors();
			MF.FLAG.NEW = 1;

			break;

		default:
			break;
	}

}
