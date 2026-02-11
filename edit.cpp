//==================================================================================
//
// DirectXのゲーム関連用のcppファイル [game.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "edit.h"
#include "input.h"
#include "camera.h"
#include "modeldata.h"
#include "3Dmodel.h"
#include "model.h"
#include "light.h"
#include "thread.h"
#include "skybox.h"
#include "field.h"
#include "Texture.h"
#include "mode.h"
#include "save.h"
#include "controller.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
char g_pFileName[MAX_PATH];
int g_nIdxCamera;		// 設置したカメラのインデックス

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitEdit(void)
{
	/*** Aの初期化 ***/

	/*** カメラの初期化 ***/
	InitCamera();

	/*** テクスチャの初期化 ***/
	InitTexture();

	/*** モデルデータの初期化 ***/
	InitModelData();

	/*** 3Dモデルの初期化 ***/
	Init3DModel();

	/*** ライトの初期化 ***/
	InitLight();

	/*** 床の初期化 ***/
	InitField();

	/*** スカイボックスの初期化 ***/
	InitSkybox();

	/*** モデル配置 ***/
	InitModel();
	LoadModel(g_pFileName);

	/*** コントローラー初期化 ***/
	InitController();

	D3DVIEWPORT9 viewport;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;
	viewport.X = 0.0f;
	viewport.Y = 0.0f;

	g_nIdxCamera = AddCamera(VEC_Z(-100.0f), VECNULL, VEC_Z(D3DX_HALFPI), viewport);

	SetSkybox(SKYBOX_NORMAL);
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitEdit(void)
{
	/*** Aの終了 ***/

	/*** カメラの終了 ***/
	UninitCamera();

	/*** テクスチャの終了 ***/
	UninitTexture();

	/*** モデルデータの終了 ***/
	UninitModelData();

	/*** 3Dモデルの終了 ***/
	Uninit3DModel();

	/*** ライトの終了 ***/
	UninitLight();

	/*** 床の終了 ***/
	UninitField();

	/*** スカイボックスの終了 ***/
	UninitSkybox();

	/*** コントローラー終了 ***/
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateEdit(void)
{
	/*** Aの更新 ***/
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		SetMode(MODE_EDIT);
	}

	/*** カメラの更新 ***/
	UpdateCamera(g_nIdxCamera);

	/*** 3Dモデルの更新 ***/
	Update3DModel();

	/*** ライトの更新 ***/
	UpdateLight();

	/*** 床の更新 ***/
	UpdateField();

	/*** スカイボックスの更新 ***/
	UpdateSkybox();

	/*** コントローラーの更新 ***/
	UpdateController();

	if (GetKeyboardTrigger(DIK_BACK) == true)
	{
		SaveModelFile();
	}
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawEdit(void)
{
	// カメラの数分だけ描画
	for (int nCntDraw = 0; nCntDraw < GetCameraNum(); nCntDraw++)
	{
		/*** カメラの設置 ***/
		SetCamera(nCntDraw);

		/*** スカイボックスの描画 ***/
		DrawSkybox();

		// VERTEX_3D ============================================
		/*** Aの描画 ***/

		/*** 床の描画 ***/
		DrawField();

		/*** 3Dモデルの描画 ***/
		Draw3DModel();

		/*** コントローラーの描画 ***/
		DrawController();

		// VERTEX_2D ============================================
		/*** Aの描画 ***/
	}
}

//==================================================================================
// --- デバイスリセット ---
//==================================================================================
void ResetEdit(bool bLost)
{
	if (bLost)
	{ // Uninit
		ResetTexture(bLost);
		ResetModelData(bLost);
		ResetSkybox(bLost);
		ResetField(bLost);
		ResetLight(bLost);
	}
	else
	{ // ReCreateBuffer
		ResetTexture(bLost);
		ResetModelData(bLost);
		ResetSkybox(bLost);
		ResetField(bLost);
		ResetLight(bLost);
	}
}

//==================================================================================
// --- 現在の配置情報の書き出し処理 ---
//==================================================================================
void SaveEditFile(void)
{
	//SaveModelFile();
}

//==================================================================================
// --- 読み込むスクリプトファイルの設定 ---
//==================================================================================
void SetModelFileName(const char* pFileName)
{
	ZeroMemory(g_pFileName, sizeof(g_pFileName));

	strcpy(g_pFileName, pFileName);
}