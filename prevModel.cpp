//==================================================================================
//
// DirectXのプレビューモデル表示用のcppファイル [prevModel.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "prevModel.h"
#include "modeldata.h"
#include "camera.h"
#include "mathUtil.h"
#include "thread.h"

using namespace MyMathUtil;

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
PREVMODEL g_modelPrev;		// プレビュー3Dモデル情報
MultiData g_mdPrev;			// クリティカルセクション

//==================================================================================
// --- モデル初期化 ---
//==================================================================================
void InitPrevModel(void)
{
	g_modelPrev = {};

	g_mdPrev.LockMultiData();

	LPPREVMODEL pPrevModel = &g_modelPrev;
	int nCntPrevModel;

	pPrevModel->pos = VECNULL;
	pPrevModel->rot = VECNULL;
	pPrevModel->nIdx3Dmodel = 5;
	pPrevModel->bUse = true;

	g_mdPrev.UnlockMultiData();
}

//==================================================================================
// --- モデル終了 ---
//==================================================================================
void UninitPrevModel(void)
{
	// 3Dmodel.cppにて動的にメモリを確保した場合(テクスチャ読み込み等)、ここで解放
}

//==================================================================================
// --- モデル更新 ---
//==================================================================================
void UpdatePrevModel(void)
{
	g_mdPrev.LockMultiData();

	LPPREVMODEL pPrevModel = &g_modelPrev;	// 3Dモデルへのポインタ

	pPrevModel->rot.y += 0.001f;
	pPrevModel->rot = RepairedRot(pPrevModel->rot);

	g_mdPrev.UnlockMultiData();
}

//==================================================================================
// --- モデル描画 ---
//==================================================================================
void DrawPrevModel(void)
{
	g_mdPrev.LockMultiData();

	// デバイスの取得開始
	LPDIRECT3DDEVICE9 pDevice = GetDevicePrev();
	LPPREVMODEL pPrevModel = &g_modelPrev;	// 3Dモデルへのポインタ

	/*** アルファテストを有効にする ***/
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファテストを有効
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// 基準値よりも大きい場合にZバッファに書き込み
	pDevice->SetRenderState(D3DRS_ALPHAREF, 60);				// 基準値

	// 3Dモデルの描画
	/*** ワールドマトリックスを計算 ***/
	CalcWorldMatrix(&pPrevModel->mtxWorld,
		pPrevModel->pos,
		pPrevModel->rot);

	// モデルデータを取得
	LPMODELDATA pModelData = GetModelData(pPrevModel->nIdx3Dmodel, 1);
	Draw3DModelFromModelData(pDevice,
		pModelData,
		&pPrevModel->mtxWorld);

	/*** アルファテストを無効にする ***/
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// アルファテストを無効化
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// 無条件にZバッファに書き込み
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// 基準値

	g_mdPrev.UnlockMultiData();
}

//==================================================================================
// --- モデル設置 ---
//==================================================================================
void SetPrevModel(int nIdxModelData)
{
	g_mdPrev.LockMultiData();

	LPPREVMODEL pPrevModel = &g_modelPrev;
	int nCntPrevModel;
	
	pPrevModel->pos = VECNULL;
	pPrevModel->nIdx3Dmodel = nIdxModelData;
	pPrevModel->bUse = true;

	g_mdPrev.UnlockMultiData();
}