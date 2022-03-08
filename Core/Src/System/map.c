/*
 * map.c
 *
 *  Created on: Feb 21, 2022
 *      Author: sssho
 */
#include <stdio.h>

#include"System/map.h"
#include"System/sensing.h"
#include"System/flags.h"

#include"Hardware/flash.h"

MAP_Mouse_Typedef point;
MAP_Mouse_Typedef goal;
MAP_Count_Typedef map_count;

uint16_t wall_vertical[MAZE_SIZE + 1];
uint16_t wall_horizontal[MAZE_SIZE + 1];
uint8_t wall_map[MAZE_SIZE][MAZE_SIZE];
uint16_t step_map[MAZE_SIZE][MAZE_SIZE];
uint8_t route[MAX_NODE];

void ConvertMapIntoWall() {
	uint8_t x,y;
	//絶対壁を配置
	wall_horizontal[MAZE_SIZE] = 0xffff;
	wall_vertical[MAZE_SIZE] = 0xffff;

	for(y=0;y<MAZE_SIZE;y++){
		for(x=0;x<MAZE_SIZE;x++){
			wall_horizontal[y] += (((wall_map[y][x] & 0x02) >> 1) << x);
			wall_vertical[x] +=  ((wall_map[y][x] & 0x01) << y);
		}
	}
}
void printHorizontalWallData() {
	uint8_t y;
	printf("horizontal\n");
	for(y=16;y>=0;y--){
		printf("%d, %#x\n",y,wall_horizontal[y]);
	}
}
void printVerticalWallData() {
	uint8_t x;
	printf("vertical\n");
		for(x=0;x<17;x++){
			printf("%d, %#x\n",x,wall_vertical[x]);
		}
}

/*
 * @brief 壁情報表示用関数
 */
void printWallData()
{
	int8_t x,y;
	ConvertMapIntoWall();
	for(y=MAZE_SIZE;y>0;y--){
		printf("+");
		for(x=0;x<MAZE_SIZE;x++){
			if(wall_horizontal[y] & (0x01 << x))		printf("---");
			else													printf("   ");
			printf("+");
		}
		printf("\n");
		for(x=0;x<MAZE_SIZE+1;x++){
			if(wall_vertical[x] & (0x01 << (y-1)))	printf("|   ");
			else													printf("    ");
		}
		printf("\n");
	}
	printf("+");
	for(x=0;x<MAZE_SIZE;x++){
		if(wall_horizontal[0] & (0x01 << x))	printf("---");
		else												printf("   ");
		printf("+");
	}
	printf("\n");
}

/*
 * @brief マップ関連初期化
 */
void deleteWallMap()
{
	uint8_t x, y;

	//マップのクリア
	for(y = 0; y <= 0x0f; y++){
		for(x = 0; x <= 0x0f; x++){
			wall_map[y][x] = 0xf0;			//上位(2次走行時)を壁あり、下位(1次走行時)を壁なしとする。
		}
	}
	//外壁の配置
	for(y = 0; y <= 0x0f; y++){
		wall_map[y][0] |= 0xf1;				//最西に壁を配置
		wall_map[y][15] |= 0xf4;			//最東に壁を配置
	}
	for(x = 0; x <= 0x0f; x++){
		wall_map[0][x] |= 0xf2;				//最南に壁を配置
		wall_map[15][x] |= 0xf8;				//最北に壁を配置
	}
	//スタート壁の配置
	wall_map[0][0] = 0xf7;
	wall_map[0][1] = 0xf3;
	//ゴール座標の設定
	setMapStruct(GOAL_X, GOAL_Y, 0x01);
}

/*
 * @brief マップ関連再計算
 */
void updateWallMap(uint8_t wall_info)
{
	//====壁情報の補正格納====
	uint8_t wall_temp = (wall_info>> point.dir) & 0x0f;		//センサ壁情報をpoint.DIRで向きを補正させて下位4bit分を残す
	wall_temp |= (wall_temp << 4);						//上位4bitに下位4bitをコピー。この作業でm_tempにNESW順で壁が格納

	//====マップへの書込み====
	wall_map[point.y][point.x] = wall_temp;

	//----周辺に書き込む----
	//北側区画が存在する時、壁情報を上書き
	if(point.y != 15){														//北側区画が存在
		if(wall_temp & 0x88)	wall_map[point.y + 1][point.x] |= 0x22;	//壁アリ
		else							wall_map[point.y + 1][point.x] &= 0xDD;	//壁ナシ
	}
	//東側区画が存在する時、壁情報を上書き
	if(point.x != 15){														//東側区画が存在
		if(wall_temp & 0x44)	wall_map[point.y][point.x + 1] |= 0x11;	//壁アリ
		else							wall_map[point.y][point.x + 1] &= 0xEE;	//壁ナシ
	}
	//南側区画が存在する時、壁情報を上書き
	if(point.y != 0){															//南側区画が存在
		if(wall_temp & 0x22)	wall_map[point.y - 1][point.x] |= 0x88;	//壁アリ
		else							wall_map[point.y - 1][point.x] &= 0x77;	//壁ナシ
	}
	//西側区画が存在する時、壁情報を上書き
	if(point.x != 0){															//西側区画が存在
		if(wall_temp & 0x11)	wall_map[point.y][point.x - 1] |= 0x44;	//壁アリ
		else							wall_map[point.y][point.x - 1] &= 0xBB;	//壁ナシ
	}
}

/*
 * @brief 歩数マップ製作
 */
uint16_t MakeStepMap(uint8_t goal_length)
{
	uint8_t x, y;
	//====Clear Step Map====
	for(y = 0; y <= 0x0f; y++){
		for( x = 0; x <= 0x0f; x++){
			step_map[y][x] = 0xff;
		}
	}

	//====Set Step Number in Goal Axis====
	if(goal_length == 2) {
		step_map[goal.y + 1][goal.x] = 0;
		step_map[goal.y][goal.x + 1] = 0;
		step_map[goal.y + 1][goal.x + 1] = 0;
		step_map[goal.y][goal.x] = 0;
	}else if(goal_length == 1){
		step_map[goal.y][goal.x] = 0;
	}

	uint16_t step_count = 0;
	uint8_t wall_temp = 0x00;

	//====自分の座標にたどり着くまでループ====
	do{
		//----マップ全域を捜索----
		for( y = 0; y <= 0x0f; y++){
			for( x = 0; x <= 0x0f; x++){
				//----現在最大の歩数を発見したとき----
				if( step_map[y][x] == step_count){
					wall_temp = wall_map[y][x];						//map配列からマップデータを取り出す
					if(MF.FLAG.SCND)		wall_temp >>= 4;		//上位4bitを使うので4bit分右にシフトさせる
					//----北壁についての処理----
					if(!(wall_temp & 0x08) && y != 0x0f){				//北壁がなく現在最北端でないとき
						if(step_map[y+1][x] == 0xff){					//北側が未記入なら
							step_map[y+1][x] = step_count + 1;	//次の歩数を書き込む
						}
					}
					//----東壁についての処理----
					if(!(wall_temp & 0x04) && x != 0x0f){			//東壁がなく現在最東端でないとき
						if(step_map[y][x+1] == 0xff){					//東側が未記入なら
							step_map[y][x+1] = step_count + 1;	//次の歩数を書き込む
						}
					}
					//----南壁についての処理----
					if(!(wall_temp & 0x02) && y != 0){				//南壁がなく現在最南端でないとき
						if(step_map[y-1][x] == 0xff){					//南側が未記入なら
							step_map[y-1][x] = step_count + 1;		//次の歩数を書き込む
						}
					}
					//----西壁についての処理----
					if(!(wall_temp & 0x01) && x != 0){				//西壁がなく現在最西端でないとき
						if(step_map[y][x-1] == 0xff){					//西側が未記入なら
							step_map[y][x-1] = step_count + 1;		//次の歩数を書き込む
						}
					}
				}
			}
		}
		step_count++;
	}while(step_map[point.y][point.x] == 0xff);		//現在座標が未記入ではなくなるまで実行
	return step_count;
}
/*
 * @brief route配列の計算
 */
void MakeRoute_NESW()
{
	uint8_t i = 0;
	uint8_t wall_temp;
	uint8_t dir_temp =  point.dir;

	//====最短経路を初期化====
	do{
		route[i++] = 0xff;										//routeを初期化、iをインクリメント
	}while(i < MAX_NODE);												//iが0でない間実行(iがオーバーフローして0になるまで実行？)

	//====現在地情報を保存====
	uint8_t step_count = step_map[point.y][point.x];
	uint8_t x = point.x;
	uint8_t y = point.y;

	//====最短経路を導出====
	do{
		wall_temp = wall_map[y][x];			//比較用マップ情報の格納
		if(MF.FLAG.SCND)	wall_temp >>= 4;
		//----東を見る----
		if(!(wall_temp & 0x04) && (step_map[y][x+1] < step_count)){//東側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x01 - point.dir) & 0x03;							//route配列に進行方向を記録
			step_count = step_map[y][x+1];								//最大歩数マップ値を更新
			x++;																		//東に進んだのでX座標をインクリメント
		}
		//----北を見る----
		else if(!(wall_temp & 0x08) && (step_map[y+1][x] < step_count)){		//北側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x00 - point.dir) & 0x03;						//route配列に進行方向を記録
			step_count = step_map[y+1][x];								//最大歩数マップ値を更新
			y++;																	//北に進んだのでY座標をインクリメント
		}
		//----南を見る----
		else if(!(wall_temp & 0x02) && (step_map[y-1][x] < step_count)){	//南側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x02 - point.dir) & 0x03;						//route配列に進行方向を記録
			step_count = step_map[y-1][x];								//最大歩数マップ値を更新
			y--;																	//南に進んだのでY座標をデクリメント
		}
		//----西を見る----
		else if(!(wall_temp & 0x01) && (step_map[y][x-1] < step_count)){	//西側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x03 - point.dir) & 0x03;					//route配列に進行方向を記録
			step_count = step_map[y][x-1];								//最大歩数マップ値を更新
			x--;												//西に進んだのでX座標をデクリメント
		}

		//----格納データ形式変更----
		switch(route[i]){										//route配列に格納した要素値で分岐
		case 0x00:												//前進する場合
			route[i] = 0x88;									//格納データ形式を変更
			break;
		case 0x01:												//右折する場合
			changeDirection(DIR_SPIN_R90);			//内部情報の方向を90度右回転
			route[i] = 0x44;									//格納データ形式を変更
			break;
		case 0x02:												//Uターンする場合
			changeDirection(DIR_SPIN_180);			//内部情報の方向を180度回転
			route[i] = 0x22;									//格納データ形式を変更
			break;
		case 0x03:												//左折する場合
			changeDirection(DIR_SPIN_L90);			//内部情報の方向を90度右回転
			route[i] = 0x11;									//格納データ形式を変更
			break;
		default:												//それ以外の場合
			route[i] = 0x00;								//格納データ形式を変更
			break;
		}
		i++;
	}while( step_map[y][x] != 0);					//進んだ先の歩数マップ値が0(=ゴール)になるまで実行
	point.dir = dir_temp;				//方向を始めの状態に戻す
}

void ConfRoute_NESW(uint8_t goal_size, uint8_t wall_data)
{
	updateWallMap(wall_data);

	//----最短経路上に壁があれば進路変更----
	if(getWallInfo() & route[map_count.route]){
		MakeStepMap(goal_size);							//歩数マップを更新
		MakeRoute_NESW();								//最短経路を更新
		map_count.route = 0;
	}
}

void advancePosition()
{
	switch(point.dir){
	case NORTH:
		point.y++;
		break;
	case EAST:
		point.x++;
		break;
	case SOUTH:
		point.y--;
		break;
	case WEST:
		point.x--;
		break;
	}
}

void changeDirection(uint8_t rotate_hex)
{
	if(rotate_hex == 0x00)					return;
	point.dir = (point.dir + rotate_hex) & 0x03;
}

MAP_Mouse_Typedef setMapStruct(uint8_t x, uint8_t y, uint8_t dir)
{
	MAP_Mouse_Typedef instance;
	instance.x = x;
	instance.y = y;
	instance.dir = dir;
	return instance;
}


void prepareMapForSearch(void)
{
	if(MF.FLAG.FIRST)	deleteWallMap();
	map_count.step = 0;
	map_count.route = 0;
	makeStepMap(GOAL_LENGTH);
	MakeRoute_NESW();
}

void updateMapAndRoute(void)
{

}

void saveWallMap(void)
{
	FLASH_Erase();

	uint8_t i,j;
	  for(i = 0; i < 16; i++){
	    for(j = 0; j < 16; j++){
	    	FLASH_WriteData((i*16 + j), wall_map[i][j]);
	    }
	  }
}

void loadWallMap(void)
{
	  int i,j;

	  for(i = 0; i < 16; i++){
	    for(j = 0; j < 16; j++){
	      wall_map[i][j] = FLASH_ReadData(i*16 + j);
	    }
	  }
}
