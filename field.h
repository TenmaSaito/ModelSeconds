//================================================================================================================
//
// DirectXのゲームフィールド表示用ヘッダファイル [field.h]
// Author : TENMA
//
//================================================================================================================
#ifndef _FIELD_H_
#define _FIELD_H_

//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************

//*************************************************************************************************
//*** 床構造体の定義 ***
//*************************************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;							// ポリゴンの位置
	D3DXVECTOR3 move;							// 移動量
	D3DXVECTOR3 rot;							// ポリゴンの角度
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
	float fWidth;								// ポリゴンの横幅
	float fDepth;								// ポリゴンの奥行
	int nXBlock;								// 拡大倍率X
	int nZBlock;								// 拡大倍率Y
	int nIndexTexture;							// テクスチャの種類
	int nCounterAnim;							// アニメーションカウンター
	int nPatternAnim;							// アニメーションパターン
	char aCodename[MAX_PATH];					// 判定用名称
	D3DCULL type;								// カリングタイプ
	bool bUse;									// 使われているか
}Field;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void InitField(void);
void UninitField(void);
void UpdateField(void);
void DrawField(void);
void ResetField(bool bLost);

void SetField(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXVECTOR3 rot, float fWidth, float fDepth, int nIndexTexture, int nXblock, int nZblock, D3DCULL type);
bool CollisionFloor(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fHeight);
Field* GetField(int nIdx);
int GetNumField(void);
#endif