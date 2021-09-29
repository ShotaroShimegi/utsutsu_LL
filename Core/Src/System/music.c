/*
 * music.c
 *
 *  Created on: 2021/07/25
 *      Author: sssho
 */


#include"tim.h"

#include"Hardware/buzzer.h"
#include"Hardware/basic_timer.h"

#include"System/music.h"

void MelodySummer(void)
{
	Melody(g6,100);
	Melody(c7,100);
	Melody(d7,100);
	Melody(e7,100);
	Melody(d7,100);
	waitMs(50);
	Melody(c7,50);
	waitMs(50);
	Melody(c7,200);
}

void MelodyRayearth(void){
	Melody(f5,100);
	waitMs(10);
	Melody(f5,600);
	Melody(c6,400);
	Melody(d6,200);
	Melody(a5h,600);

}

void MelodyGoal(void)
{
	Melody(g6,300);
	Melody(f6,300);
	Melody(e6,300);

}

void MelodyUrara(void){
	Melody(d7,100);
	Melody(a6,100);
	waitMs(10);
	Melody(a6,200);
	waitMs(50);

	Melody(d7,100);
	Melody(a6,100);
	waitMs(10);
	Melody(a6,200);
	waitMs(50);

	Melody(d7,100);
	Melody(c7,100);
	Melody(d7,100);
	Melody(c7,100);
	Melody(a6,200);
	waitMs(50);

}

void MelodyMrLawrence()
{
	Melody(d7,100);
	Melody(e7,100);
	Melody(d7,100);
	Melody(a6,100);
	Melody(d7,100);

	waitMs(400);

	Melody(d7,100);
	Melody(e7,100);
	Melody(d7,100);
	Melody(e6,100);
	Melody(g7,100);
	Melody(e6,100);
	Melody(d7,100);
	Melody(e7,100);
	Melody(d7,100);
	Melody(c7,100);
	Melody(a6,100);

}

void MelodyNatsumatsuri()
{
	Melody(d6,200);
	Melody(f6,200);
	Melody(g6,200);
	Melody(f6,100);
	Melody(g6,100);
	Melody(a6,200);
	Melody(c6,200);
	Melody(d6,200);


}

void MelodyKurenai(void){
	Melody(c7,300);
	waitMs(10);
	Melody(c7,100);
	Melody(b6,100);
	Melody(c7,300);
	waitMs(10);
	Melody(d6,100);
	Melody(e7,300);
	Melody(d7,100);
	Melody(c7,100);
	Melody(b6,300);
}

void MelodyYamato(){
	Melody(e6,100);
	waitMs(10);
	Melody(e6,200);
	Melody(f6,100);
	Melody(e6,200);
	Melody(c6,200);

	Melody(f6,400);
	Melody(e6,400);
	Melody(d6,400);

}
