//================================================================================================================
//
// DirectXのカメラ用ヘッダファイル [camera.h]
// Author : TENMA
//
//================================================================================================================
#ifndef _CAMERA_H_
#define _CAMERA_H_

//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "input.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
//*** カメラ情報構造体 ***
//**********************************************************************************
typedef struct
{
	D3DXVECTOR3 posV;			// 視点
	D3DXVECTOR3 posR;			// 注視点
	D3DXVECTOR3 vecU;			// 上方向ベクトル
	D3DXVECTOR3 rot;			// 向き
	D3DXMATRIX mtxProjection;	// プロジェクションマトリックス
	D3DXMATRIX mtxView;			// ビューマトリックス
	float fZlength;				// 注視点との距離
	D3DVIEWPORT9 viewport;		// ビューポート
	bool bUse;					// 使われているか
} Camera;

//*****************************************************************************
// 追加のカメラの構造体定義
//*****************************************************************************
namespace Shi
{
	//*****************************************************************************
	// カメラの構造体定義
	//*****************************************************************************
	typedef struct
	{
		D3DXVECTOR3 posV;				// 視点
		D3DXVECTOR3 posR;				// 注視点
		D3DXVECTOR3 posU;				// 上方向ベクトル
		D3DXVECTOR3 posVDest;			// 目的の視点
		D3DXVECTOR3 posRDest;			// 目的の注視点
		D3DXVECTOR3 move;				// 移動量
		D3DXVECTOR3 rot;				// 向き
		D3DXVECTOR3 rotDest;			// 目的の向き
		D3DXVECTOR3 rotOld;				// 過去の向き
		float fRadiusVertical;			// 縦軸距離(基本)
		float fRadiusHorizonttal;		// 横軸距離
		D3DXMATRIX mtxProjection;		// プロジェクションマトリックス
		D3DXMATRIX mtxView;				// ビューマトリックス
		D3DVIEWPORT9 viewport;			// ビューポート
		bool bUse;						// 使用状態
	}Camera;

	int AddCamera(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 rot, D3DVIEWPORT9 viewport);
	void SetCamera(int nIdxCamera);
}

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(int nIdxCamera);
void SetCamera(int nIdxCamera);

int AddCamera(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 rot, D3DVIEWPORT9 viewport);
void RemoveCamera(int nIdxCamera);
Camera *GetCamera(int nIdxCamera);
int GetCameraNum(void);
#endif
