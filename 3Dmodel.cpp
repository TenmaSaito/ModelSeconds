//================================================================================================================
//
// DirectXの3Dモデル配置のcppファイル [3DModel.cpp]
// Author : TENMA
//
//================================================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "3DModel.h"
#include "modeldata.h"
#include "camera.h"
#include "mathUtil.h"
#include "save.h"

using namespace MyMathUtil;

//*************************************************************************************************
//*** マクロ定義 ***
//*************************************************************************************************
#define MAX_3DMODEL		(1024)		// 設置できるモデルの最大数

//*************************************************************************************************
//*** プロトタイプ宣言 ***
//*************************************************************************************************

//*************************************************************************************************
//*** グローバル変数 ***
//*************************************************************************************************
_3DMODEL g_aModel[MAX_3DMODEL];		// 3Dモデル情報
int g_nNum3DModel;					// 設置した3Dモデルの数

//=================================================================================================
// --- モデル初期化 ---
//=================================================================================================
void Init3DModel(void)
{
	// モデルを初期化
	ZeroMemory(&g_aModel[0], sizeof(_3DMODEL) * (MAX_3DMODEL));

	// 設置したモデルの数を初期化
	g_nNum3DModel = 0;
}

//=================================================================================================
// --- モデル終了 ---
//=================================================================================================
void Uninit3DModel(void)
{
	// 3Dmodel.cppにて動的にメモリを確保した場合(テクスチャ読み込み等)、ここで解放
}

//=================================================================================================
// --- モデル更新 ---
//=================================================================================================
void Update3DModel(void)
{

}

//=================================================================================================
// --- モデル描画 ---
//=================================================================================================
void Draw3DModel(void)
{
	// デバイスの取得開始
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	LP3DMODEL p3DModel = &g_aModel[0];	// 3Dモデルへのポインタ

	/*** アルファテストを有効にする ***/
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファテストを有効
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// 基準値よりも大きい場合にZバッファに書き込み
	pDevice->SetRenderState(D3DRS_ALPHAREF, 60);				// 基準値

	// 3Dモデルの描画
	for (int nCnt3DModel = 0; nCnt3DModel < MAX_3DMODEL; nCnt3DModel++, p3DModel++)
	{
		if (p3DModel->bUse != false)
		{ // もし使われていれば
			/*** ワールドマトリックスを計算 ***/
			CalcWorldMatrix(&p3DModel->mtxWorld,
				p3DModel->pos,
				p3DModel->rot);

			// モデルデータを取得
			LPMODELDATA pModelData = GetModelData(p3DModel->nIdx3Dmodel);
			if (p3DModel->bAlpha == true)
			{
				D3DCOLORVALUE diffuse = { 0.0f, 1.0f, 0.0f, 0.5f };

				Draw3DModelByCustomColorFromModelData(pDevice,
					pModelData,
					&p3DModel->mtxWorld,
					diffuse);
			}
			else if (nCnt3DModel == MAX_3DMODEL - 1)
			{
				D3DCOLORVALUE diffuse;

				if (g_aModel[nCnt3DModel].bAlpha == true)
				{
					diffuse = { 0.0f, 0.0f, 1.0f, 0.5f };
				}
				else
				{
					diffuse = { 1.0f, 0.0f, 0.0f, 0.5f };
				}

				Draw3DModelByCustomColorFromModelData(pDevice,
					pModelData,
					&p3DModel->mtxWorld,
					diffuse);
			}
			else
			{
				Draw3DModelFromModelData(pDevice,
					pModelData,
					&p3DModel->mtxWorld);
			}
		}
	}

	/*** アルファテストを無効にする ***/
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// アルファテストを無効化
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// 無条件にZバッファに書き込み
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// 基準値

	// デバイスの取得終了
	EndDevice();
}

//=================================================================================================
// --- モデル設置 ---
//=================================================================================================
int Set3DModel(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdxModelData)
{
	LP3DMODEL p3DModel = &g_aModel[0];
	int nCnt3DModel;

	// 3Dモデルの設置
	for (nCnt3DModel = 0; nCnt3DModel < MAX_3DMODEL; nCnt3DModel++, p3DModel++)
	{
		if (p3DModel->bUse == false)
		{ // もし使われていなければ
			p3DModel->pos = pos;
			p3DModel->rot = rot;
			p3DModel->nIdx3Dmodel = nIdxModelData;
			p3DModel->bUse = true;
			p3DModel->bAlpha = false;
			g_nNum3DModel++;

			break;
		}
	}

	// 最大数を超えていれば-1に変更
	if (nCnt3DModel >= MAX_3DMODEL) nCnt3DModel = -1;

	return nCnt3DModel;
}
//=================================================================================================
// --- 配列最終モデル設置 ---
//=================================================================================================
void Set3DModelLast(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	g_aModel[MAX_3DMODEL-1].pos = pos;
	g_aModel[MAX_3DMODEL-1].rot = rot;
	g_aModel[MAX_3DMODEL-1].nIdx3Dmodel = 0;
	g_aModel[MAX_3DMODEL - 1].bAlpha = true;
	g_aModel[MAX_3DMODEL-1].bUse = true;
}
//=================================================================================================
// --- 配列最終モデルポインタ取得 ---
//=================================================================================================
P3DMODEL GetLast3DModel(void)
{
	return &g_aModel[MAX_3DMODEL - 1];
}

//=================================================================================================
// --- ポインタ取得 ---
//=================================================================================================
LP3DMODEL Get3DModel(int nIdxModel)
{
	// もしインデックス外なら
	if (nIdxModel < 0 || nIdxModel >= MAX_3DMODEL) return NULL;
	return &g_aModel[nIdxModel];
}

//=================================================================================================
// --- 使用モデル数取得 ---
//=================================================================================================
int *GetNum3DModel(void)
{
	return &g_nNum3DModel;
}