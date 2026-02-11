//================================================================================================================
//
// DirectXのカメラ処理 [camera.cpp]
// Author : TENMA
//
//================================================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "camera.h"
#include "mathUtil.h"
#include "debugproc.h"

//*************************************************************************************************
//*** マクロ定義 ***
//*************************************************************************************************
#define CAMERA_ZLENGTH		(300.0f)		// カメラの注視点との距離
#define CAMERA_SPD			(2.0f)			// カメラの移動速度
#define CAMERA_ROTSPD		(0.05f)			// カメラの移動速度
#define MAX_CAMERA			(4)				// カメラの最大数

//*************************************************************************************************
//*** グローバル変数 ***
//*************************************************************************************************
Camera g_aCamera[MAX_CAMERA];		// カメラの情報
int g_nNumCamera;					// カメラの数
float g_fSpdCamera;					// カメラの速度

//================================================================================================================
// --- カメラの初期化 ---
//================================================================================================================
void InitCamera(void)
{
	Camera* pCamera = &g_aCamera[0];

	// カメラの初期化
	ZeroMemory(&g_aCamera[0], sizeof(Camera) * MAX_CAMERA);

	g_fSpdCamera = CAMERA_SPD;

	// カメラの数の初期化
	g_nNumCamera = 0;
}

//================================================================================================================
// --- カメラの終了 ---
//================================================================================================================
void UninitCamera(void)
{

}

//================================================================================================================
// --- カメラの更新 ---
//================================================================================================================
void UpdateCamera(int nIdxCamera)
{
	Camera *pCamera = &g_aCamera[nIdxCamera];

#if 1
	/*** 視点の平行移動！ ***/
	if (GetKeyboardPress(DIK_W))
	{ // Wを押したとき
		/** 追加入力 **/
		if (GetKeyboardPress(DIK_A))
		{// Aを押したとき
			pCamera->posV.x += cosf(pCamera->rot.y + (D3DX_PI * 1.25f)) * g_fSpdCamera;
			pCamera->posV.z += sinf(pCamera->rot.y + (D3DX_PI * 1.25f)) * g_fSpdCamera;
			pCamera->posR.x += cosf(pCamera->rot.y + (D3DX_PI * 1.25f)) * g_fSpdCamera;
			pCamera->posR.z += sinf(pCamera->rot.y + (D3DX_PI * 1.25f)) * g_fSpdCamera;
		}
		else if (GetKeyboardPress(DIK_D))
		{ // Dを押したとき
			pCamera->posV.x += cosf(pCamera->rot.y + (D3DX_PI * 0.75f)) * g_fSpdCamera;
			pCamera->posV.z += sinf(pCamera->rot.y + (D3DX_PI * 0.75f)) * g_fSpdCamera;
			pCamera->posR.x += cosf(pCamera->rot.y + (D3DX_PI * 0.75f)) * g_fSpdCamera;
			pCamera->posR.z += sinf(pCamera->rot.y + (D3DX_PI * 0.75f)) * g_fSpdCamera;
		}
		else
		{ // 純粋なW入力時
			pCamera->posV.x += cosf(pCamera->rot.y + D3DX_PI) * g_fSpdCamera;
			pCamera->posV.z += sinf(pCamera->rot.y + D3DX_PI) * g_fSpdCamera;
			pCamera->posR.x += cosf(pCamera->rot.y + D3DX_PI) * g_fSpdCamera;
			pCamera->posR.z += sinf(pCamera->rot.y + D3DX_PI) * g_fSpdCamera;
		}
	}
	else if (GetKeyboardPress(DIK_S))
	{ // Sを押したとき
		/** 追加入力 **/
		if (GetKeyboardPress(DIK_A))
		{// Aを押したとき
			pCamera->posV.x += cosf(pCamera->rot.y + (D3DX_PI * 1.75f)) * g_fSpdCamera;
			pCamera->posV.z += sinf(pCamera->rot.y + (D3DX_PI * 1.75f)) * g_fSpdCamera;
			pCamera->posR.x += cosf(pCamera->rot.y + (D3DX_PI * 1.75f)) * g_fSpdCamera;
			pCamera->posR.z += sinf(pCamera->rot.y + (D3DX_PI * 1.75f)) * g_fSpdCamera;

		}
		else if (GetKeyboardPress(DIK_D))
		{ // Dを押したとき
			pCamera->posV.x += cosf(pCamera->rot.y + (D3DX_PI * 0.25f)) * g_fSpdCamera;
			pCamera->posV.z += sinf(pCamera->rot.y + (D3DX_PI * 0.25f)) * g_fSpdCamera;
			pCamera->posR.x += cosf(pCamera->rot.y + (D3DX_PI * 0.25f)) * g_fSpdCamera;
			pCamera->posR.z += sinf(pCamera->rot.y + (D3DX_PI * 0.25f)) * g_fSpdCamera;
		}
		else
		{ // 純粋なS入力時
			pCamera->posV.x += cosf(pCamera->rot.y) * g_fSpdCamera;
			pCamera->posV.z += sinf(pCamera->rot.y) * g_fSpdCamera;
			pCamera->posR.x += cosf(pCamera->rot.y) * g_fSpdCamera;
			pCamera->posR.z += sinf(pCamera->rot.y) * g_fSpdCamera;
		}
	}
	else if (GetKeyboardPress(DIK_A))
	{ // Aを押したとき
		pCamera->posV.x += cosf(pCamera->rot.y + (D3DX_PI * 1.5f)) * g_fSpdCamera;
		pCamera->posV.z += sinf(pCamera->rot.y + (D3DX_PI * 1.5f)) * g_fSpdCamera;
		pCamera->posR.x += cosf(pCamera->rot.y + (D3DX_PI * 1.5f)) * g_fSpdCamera;
		pCamera->posR.z += sinf(pCamera->rot.y + (D3DX_PI * 1.5f)) * g_fSpdCamera;
	}
	else if (GetKeyboardPress(DIK_D))
	{ // Dを押したとき
		pCamera->posV.x += cosf(pCamera->rot.y + (D3DX_PI * 0.5f)) * g_fSpdCamera;
		pCamera->posV.z += sinf(pCamera->rot.y + (D3DX_PI * 0.5f)) * g_fSpdCamera;
		pCamera->posR.x += cosf(pCamera->rot.y + (D3DX_PI * 0.5f)) * g_fSpdCamera;
		pCamera->posR.z += sinf(pCamera->rot.y + (D3DX_PI * 0.5f)) * g_fSpdCamera;
	}

	/*** カメラの移動！(上下) ***/
	if (GetKeyboardPress(DIK_LCONTROL))
	{
		pCamera->posV.y -= g_fSpdCamera;
		pCamera->posR.y -= g_fSpdCamera;
	}
	else if (GetKeyboardPress(DIK_SPACE))
	{
		pCamera->posV.y += g_fSpdCamera;
		pCamera->posR.y += g_fSpdCamera;
	}

	if (GetKeyboardTrigger(DIK_LSHIFT))
	{
		g_fSpdCamera *= 2.0f;
	}
	else if (GetKeyboardRelease(DIK_LSHIFT))
	{
		g_fSpdCamera *= 0.5f;
	}

#endif // 0

	/*** カメラの回転！(注視点中心) ***/
	if (GetKeyboardPress(DIK_E))
	{
		pCamera->rot.y -= CAMERA_ROTSPD;

		if (pCamera->rot.y < -D3DX_PI)
		{
			pCamera->rot.y += D3DX_PI * 2.0f;
		}
		else if (pCamera->rot.y >= D3DX_PI)
		{
			pCamera->rot.y -= D3DX_PI * 2.0f;
		}
	}
	else if (GetKeyboardPress(DIK_Q))
	{
		pCamera->rot.y += CAMERA_ROTSPD;

		if (pCamera->rot.y < -D3DX_PI)
		{
			pCamera->rot.y += D3DX_PI * 2.0f;
		}
		else if (pCamera->rot.y >= D3DX_PI)
		{
			pCamera->rot.y -= D3DX_PI * 2.0f;
		}
	}

	// カメラの上下回転！
	if (GetKeyboardPress(DIK_Y))
	{
		pCamera->rot.z -= CAMERA_ROTSPD;

		if (pCamera->rot.z < 0.05f)
		{
			pCamera->rot.z = 0.05f;
		}
	}
	else if (GetKeyboardPress(DIK_N))
	{
		pCamera->rot.z += CAMERA_ROTSPD;

		if (pCamera->rot.z >= D3DX_PI * 0.9f)
		{
			pCamera->rot.z = D3DX_PI * 0.9f;
		}
	}

	/*** ホイールでの注視点間距離変更処理！ ***/
	if (GetMouseMove(MOUSESLOPE_WHEEL) != 0 && pCamera->fZlength >= CAMERA_ZLENGTH * 0.01f)
	{
		pCamera->fZlength -= (float)GetMouseMove(MOUSESLOPE_WHEEL) * 0.1f;
		if (pCamera->fZlength <= CAMERA_ZLENGTH * 0.01f)
		{
			pCamera->fZlength = CAMERA_ZLENGTH * 0.01f;
		}
	}

	PrintDebugProc(nIdxCamera, "ZLen : %f\n", pCamera->fZlength);

	pCamera->posV.x = pCamera->posR.x + (sinf(pCamera->rot.z) * cosf(pCamera->rot.y) * pCamera->fZlength);
	pCamera->posV.y = pCamera->posR.y + (cosf(pCamera->rot.z) * pCamera->fZlength);
	pCamera->posV.z = pCamera->posR.z + (sinf(pCamera->rot.z) * sinf(pCamera->rot.y) * pCamera->fZlength);
}

//================================================================================================================
// --- カメラの設置 ---
//================================================================================================================
void SetCamera(int nIdxCamera)
{
	Camera* pCamera = &g_aCamera[nIdxCamera];

	/*** デバイスの取得 ***/
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	if (nIdxCamera == 1)
	{
		pDevice = GetDevicePrev();
	}

	// ビューポート設定
	pDevice->SetViewport(&pCamera->viewport);

	/*** プロジェクションマトリックスの初期化 ***/
	D3DXMatrixIdentity(&pCamera->mtxProjection);

	/*** プロジェクションマトリックスの作成 ***/
	D3DXMatrixPerspectiveFovLH(&pCamera->mtxProjection,
		D3DXToRadian(45.0f),							// 視野角
		(float)pCamera->viewport.Width / (float)pCamera->viewport.Height,		// モニターのアスペクト比
		10.0f,					// カメラの描画最小範囲
		10000.0f);				// カメラの描画最大範囲

	/*** プロジェクションマトリックスの設定 ***/
	pDevice->SetTransform(D3DTS_PROJECTION, &pCamera->mtxProjection);

	/*** ビューマトリックスの初期化 ***/
	D3DXMatrixIdentity(&pCamera->mtxView);

	/*** ビューマトリックスの作成 ***/
	D3DXMatrixLookAtLH(&pCamera->mtxView,
		&pCamera->posV,		// 視点
		&pCamera->posR,		// 注視点
		&pCamera->vecU);	// 上方向ベクトル

	/*** ビューマトリックスの設定 ***/
	pDevice->SetTransform(D3DTS_VIEW, &pCamera->mtxView);

	// デバイスの取得の終了
	EndDevice();
}

//================================================================================================================
// --- カメラの追加 ---
//================================================================================================================
int AddCamera(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 rot, D3DVIEWPORT9 viewport)
{
	Camera *pCamera = &g_aCamera[0];
	D3DXVECTOR3 diff = {};		// 2点間の差分
	int nCntCamera;

	for (nCntCamera = 0; nCntCamera < MAX_CAMERA; nCntCamera++, pCamera++)
	{
		if (pCamera->bUse == false)
		{ // カメラが使われていなければ
			pCamera->posV = posV;			// カメラの位置
			pCamera->posR = posR;			// 注視点の位置
			pCamera->vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);	// 上方向ベクトル
			pCamera->rot = rot;				// カメラの向き
			diff = posV - posR;				// 差分

			pCamera->fZlength = D3DXVec3Length(&diff);		// 長さを計算
			pCamera->viewport = viewport;	// ビューポート
			pCamera->bUse = true;			// カメラを使用済みに
			g_nNumCamera++;					// カメラ設置数を増加

			break;
		}
	}
	
	if (nCntCamera >= MAX_CAMERA) nCntCamera = -1;

	return nCntCamera;
}

//================================================================================================================
// --- カメラの削除 ---
//================================================================================================================
void RemoveCamera(int nIdxCamera)
{
	if (nIdxCamera < 0 || nIdxCamera >= MAX_CAMERA) return;
	if (g_nNumCamera <= 0) return;

	g_aCamera[nIdxCamera].bUse = false;		// 未使用に
	g_nNumCamera--;							// カメラの設置数を減らす
}

//================================================================================================================
// --- カメラの取得 ---
//================================================================================================================
Camera *GetCamera(int nIdxCamera)
{
	if (nIdxCamera < 0 || nIdxCamera >= MAX_CAMERA) return NULL;

	return &g_aCamera[nIdxCamera];
}

//================================================================================================================
// --- カメラの数 ---
//================================================================================================================
int GetCameraNum(void)
{
	return 1;
}