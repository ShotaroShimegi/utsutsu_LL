/*
 * route.c
 *
 *  Created on: 2022/03/06
 *      Author: sssho
 */
#include"Controller/state.h"

#include"System/route.h"
#include"System/map.h"
#include"System/sensing.h"

uint8_t route[1024];
uint16_t r_cnt;

void MakeRoute_NESW()
{
	//====変数宣言====
	uint8_t i = 0;
	uint8_t x, y;
	uint8_t dir_temp =  mouse_on_map.dir;
	uint8_t m_temp;					//マップデータ一時保持

	//====最短経路を初期化====
	do{
		route[i++] = 0xff;										//routeを初期化、iをインクリメント
	}while(i != 0);												//iが0でない間実行(iがオーバーフローして0になるまで実行？)

	//====歩数カウンタをセット====
	uint8_t m_step = step_map[mouse_on_map.y][mouse_on_map.x];

	//====x, yに現在座標を書き込み====
	x = mouse_on_map.x;
	y = mouse_on_map.y;

	//====最短経路を導出====
	do{
		m_temp = map[y][x];										//比較用マップ情報の格納
		if(MF.FLAG.SCND){										//二次走行用のマップを作成する場合(二次走行時はMF.FLAG.SCNDをTrueにする)
			m_temp >>= 4;										//上位4bitを使うので4bit分右にシフトさせる
		}

		//----東を見る----
		if(!(m_temp & 0x04) && (step_map[y][x+1] < m_step)){	//東側に壁が無く、現在地より小さい歩数マップ値であれば
					route[i] = (0x01 - mouse_on_map.dir) & 0x03;					//route配列に進行方向を記録
					m_step = step_map[y][x+1];								//最大歩数マップ値を更新
					x++;													//東に進んだのでX座標をインクリメント
		}
		//----北を見る----
		else if(!(m_temp & 0x08) && (step_map[y+1][x] < m_step)){		//北側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x00 - mouse_on_map.dir) & 0x03;					//route配列に進行方向を記録
			m_step = step_map[y+1][x];								//最大歩数マップ値を更新
			y++;													//北に進んだのでY座標をインクリメント
		}
		//----南を見る----
		else if(!(m_temp & 0x02) && (step_map[y-1][x] < m_step)){	//南側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x02 - mouse_on_map.dir) & 0x03;					//route配列に進行方向を記録
			m_step = step_map[y-1][x];								//最大歩数マップ値を更新
			y--;												//南に進んだのでY座標をデクリメント
		}
		//----西を見る----
		else if(!(m_temp & 0x01) && (step_map[y][x-1] < m_step)){	//西側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x03 - mouse_on_map.dir) & 0x03;					//route配列に進行方向を記録
			m_step = step_map[y][x-1];								//最大歩数マップ値を更新
			x--;												//西に進んだのでX座標をデクリメント
		}

		//----格納データ形式変更----
		switch(route[i]){										//route配列に格納した要素値で分岐
		case 0x00:												//前進する場合
			route[i] = 0x88;									//格納データ形式を変更
			break;
		case 0x01:												//右折する場合
			updateDirection(DIR_SPIN_R90);			//内部情報の方向を90度右回転
			route[i] = 0x44;									//格納データ形式を変更
			break;
		case 0x02:												//Uターンする場合
			updateDirection(DIR_SPIN_180);						//内部情報の方向を180度回転
			route[i] = 0x22;									//格納データ形式を変更
			break;
		case 0x03:												//左折する場合
			updateDirection(DIR_SPIN_L90);						//内部情報の方向を90度右回転
			route[i] = 0x11;									//格納データ形式を変更
			break;
		default:												//それ以外の場合
			route[i] = 0x00;									//格納データ形式を変更
			break;
		}
		i++;
	}while( step_map[y][x] != 0);					//進んだ先の歩数マップ値が0(=ゴール)になるまで実行
	mouse_on_map.dir = dir_temp;							//方向を始めの状態に戻す
}

void ConfRoute_NESW(uint8_t goal_size)
{
	//----壁情報書き込み----
	WriteMap();

	//----最短経路上に壁があれば進路変更----
	if(getWallInfo() & route[r_cnt]){
		MakeStepMap(goal_size);							//歩数マップを更新
		MakeRoute_NESW();									//最短経路を更新
		r_cnt = 0;											//経路カウンタを0に
	}
}


void updatePosition()
{
	switch(mouse_on_map.dir){
	case NORTH:
		mouse_on_map.y++;
		break;
	case EAST:
		mouse_on_map.x++;
		break;
	case SOUTH:
		mouse_on_map.y--;
		break;
	case WEST:
		mouse_on_map.x--;
		break;
	}
}

void updateDirection(uint8_t t_pat)
{
	mouse_on_map.dir = (mouse_on_map.dir + t_pat) & 0x03;
}
