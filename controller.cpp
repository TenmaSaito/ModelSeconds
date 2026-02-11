//================================================================================================================
//
// 3Dモデル操作のcppファイル [controller.cpp]
// Author : Keitaro Nagate
//
//================================================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "model.h"
#include "3Dmodel.h"
#include "controller.h"
#include "input.h"
#include "debugproc.h"
#include "mathUtil.h"
#include "camera.h"

using namespace MyMathUtil;

//*************************************************************************************************
//*** マクロ定義 ***
//*************************************************************************************************
#define MOVE_POS	(1.0f)		//入力一回のPOS移動量
#define MOVE_ROT	(0.1f)		//入力一回のROT変更量
#define SETPOS_SPD	(2.0f)			// カメラの移動速度

//*************************************************************************************************
//*** グローバル変数 ***
//*************************************************************************************************
 LP3DMODEL g_pSelectModel;			// 選択中のモデルの情報
 ControlType g_ControlType;			// コントローラーの状態
 int g_nSelectNum;					// 選択中のモデルの番号
 D3DXVECTOR3 SetPos;				// 設置位置
 float g_fSpdPOSR;					//注視点移動速度

//=================================================================================================
// --- コントローラー初期化 ---
//=================================================================================================
void InitController(void) 
{
	g_nSelectNum = 0;
	g_pSelectModel = Get3DModel(g_nSelectNum);
	g_pSelectModel->bAlpha = true;
	SetPos = VECNULL;
	g_fSpdPOSR = SETPOS_SPD;
}
//=================================================================================================
// --- コントローラー終了 ---
//=================================================================================================
void UninitController(void)
{

}
//=================================================================================================
// --- コントローラー更新 ---
//=================================================================================================
void UpdateController(void)
{
	Camera* pCamera = GetCamera(0);
	pCamera->posR = SetPos;

	//================================
	// -- 画面表示 --
	//================================
	if (g_ControlType == CONTROLTYPE_SELECT)
	{
		PrintDebugProc("CONTROLTYPE_SELECT\n");
	}
	else if(g_ControlType == CONTROLTYPE_POS)
	{
		PrintDebugProc("CONTROLTYPE_POS\n");
	}
	else if (g_ControlType == CONTROLTYPE_ROT)
	{
		PrintDebugProc("CONTROLTYPE_ROT\n");
	}
	else if (g_ControlType == CONTROLTYPE_SET)
	{
		PrintDebugProc("CONTROLTYPE_SET\n");
	}
	PrintDebugProc("SELECT NUMBER : %d\n", g_nSelectNum);
	PrintDebugProc("POS.X : %f\nPOS.Y : %f\nPOS.Z : %f\n",g_pSelectModel->pos.x, g_pSelectModel->pos.y, g_pSelectModel->pos.z);
	PrintDebugProc("ROT.X : %f\nROT.Y : %f\nROT.Z : %f\n", g_pSelectModel->rot.x, g_pSelectModel->rot.y, g_pSelectModel->rot.z);

	//================================
	// -- コントロールタイプ選択 --
	//================================
	if (GetKeyboardTrigger(DIK_1))
	{//モデル選択
		g_ControlType = CONTROLTYPE_SELECT;
	}
	else if (GetKeyboardTrigger(DIK_2))
	{//モデル位置移動
		g_ControlType = CONTROLTYPE_POS;
	}
	else if (GetKeyboardTrigger(DIK_3))
	{//モデル角度変更
		g_ControlType = CONTROLTYPE_ROT;
	}

	//================================
	// -- コントロール --
	//================================
	MovePosR();

	if (g_ControlType == CONTROLTYPE_SELECT)
	{//モデル選択//
		if (GetKeyboardTrigger(DIK_RIGHT))
		{//一つ上のモデルを見る
			g_pSelectModel->bAlpha = false;
			g_nSelectNum++;
			g_pSelectModel = Get3DModel(g_nSelectNum);
			g_pSelectModel->bAlpha = true;

			if (g_pSelectModel->bUse == false)
			{//選択された番号のモデルが不使用
				g_pSelectModel->bAlpha = false;
				SearchUseModel(true);
			}
		}
		if (GetKeyboardTrigger(DIK_LEFT))
		{
			g_nSelectNum--;
			if (g_nSelectNum < 0)
			{//セレクトが0番以下を指定
				g_nSelectNum = MAX_MODEL;
			}

			g_pSelectModel->bAlpha = false;
			g_pSelectModel = Get3DModel(g_nSelectNum);
			g_pSelectModel->bAlpha = true;

			if (g_pSelectModel->bUse == false)
			{//選択された番号のモデルが不使用
				g_pSelectModel->bAlpha = false;
				SearchUseModel(false);
			}
		}
		if (GetKeyboardTrigger(DIK_UP))
		{//10ずつずらす
			g_pSelectModel->bAlpha = false;
			g_nSelectNum += 10;
			g_pSelectModel = Get3DModel(g_nSelectNum);
			SearchUseModel(false);
		}
		if (GetKeyboardTrigger(DIK_DOWN))
		{//10ずつずらす
			g_pSelectModel->bAlpha = false;
			g_nSelectNum += 10;
			g_pSelectModel = Get3DModel(g_nSelectNum);
			SearchUseModel(true);
		}
	}
	else if (g_ControlType == CONTROLTYPE_POS)
	{//位置移動//
		if (GetKeyboardPress(DIK_RIGHT))
		{//右
			if (GetKeyboardPress(DIK_LSHIFT))
			{//LSHIFTで上
				g_pSelectModel->pos.y += MOVE_POS;
			}
			else
			{
				g_pSelectModel->pos.x += MOVE_POS;
			}
		}
		if (GetKeyboardPress(DIK_LEFT))
		{//左
			if (GetKeyboardPress(DIK_LSHIFT))
			{//LSHIFTで下
				g_pSelectModel->pos.y -= MOVE_POS;
			}
			else
			{
				g_pSelectModel->pos.x -= MOVE_POS;
			}
		}
		if (GetKeyboardPress(DIK_UP))
		{//奥
			if (GetKeyboardPress(DIK_LSHIFT))
			{//LSHIFTで上
				g_pSelectModel->pos.y += MOVE_POS;
			}
			else
			{
				g_pSelectModel->pos.z += MOVE_POS;
			}
		}
		if (GetKeyboardPress(DIK_DOWN))
		{//手前
			if (GetKeyboardPress(DIK_LSHIFT))
			{//LSHIFTで下
				g_pSelectModel->pos.y -= MOVE_POS;
			}
			else
			{
				g_pSelectModel->pos.z -= MOVE_POS;
			}
		}
	}
	else if (g_ControlType == CONTROLTYPE_ROT)
	{//角度変更//
		if (GetKeyboardPress(DIK_RIGHT))
		{//右
			if (GetKeyboardPress(DIK_LSHIFT))
			{//LSHIFTで上
				g_pSelectModel->rot.x += MOVE_ROT;
			}
			else
			{
				g_pSelectModel->rot.y += MOVE_ROT;
			}
		}
		if (GetKeyboardPress(DIK_LEFT))
		{//左
			if (GetKeyboardPress(DIK_LSHIFT))
			{//LSHIFTで下
				g_pSelectModel->rot.x -= MOVE_ROT;
			}
			else
			{
				g_pSelectModel->rot.y -= MOVE_ROT;
			}
		}
		if (GetKeyboardPress(DIK_UP))
		{//奥
			if (GetKeyboardPress(DIK_LSHIFT))
			{//LSHIFTで上
				g_pSelectModel->rot.z += MOVE_ROT;
			}
			else
			{
				g_pSelectModel->rot.x += MOVE_ROT;
			}
		}
		if (GetKeyboardPress(DIK_DOWN))
		{//手前
			if (GetKeyboardPress(DIK_LSHIFT))
			{//LSHIFTで下
				g_pSelectModel->rot.z -= MOVE_ROT;
			}
			else
			{
				g_pSelectModel->rot.y -= MOVE_ROT;
			}
		}
	}
}
//=================================================================================================
// --- コントローラー描画 ---
//=================================================================================================
void DrawController(void)
{

}
//=================================================================================================
// --- 使用されているモデルを探す ---
// TRUE = ++ / FALSE = --
//=================================================================================================
void SearchUseModel(bool bInDe)
{
	int nSaveNum = g_nSelectNum;
	if (g_pSelectModel->bUse == true)
	{
		return;
	}

	if (bInDe == true)
	{
		while (1)
		{
			g_nSelectNum++;
			g_pSelectModel = Get3DModel(g_nSelectNum);
			if (g_pSelectModel->bUse == true)
			{//使用されているモデル発見
				g_pSelectModel->bAlpha = true;
				break;
			}
			else if (g_nSelectNum >= MAX_MODEL)
			{//見つけらずに上限
				//0番へ
				g_nSelectNum = 0;
				g_pSelectModel = Get3DModel(g_nSelectNum);
			}
			else if (g_nSelectNum == nSaveNum)
			{//見つからない
				g_nSelectNum = nSaveNum;
				g_pSelectModel = Get3DModel(g_nSelectNum);
				g_pSelectModel->bAlpha = true;
				GenerateMessageBox(MB_ICONERROR, "Error", "もう無いです！！！！");
				break;
			}
		}
	}
	else if (bInDe == false)
	{
		while (1)
		{
			g_nSelectNum--;
			g_pSelectModel = Get3DModel(g_nSelectNum);
			if (g_pSelectModel->bUse == true)
			{//使用されているモデル発見
				g_pSelectModel->bAlpha = true;
				break;
			}
			else if (g_nSelectNum <= 0)
			{//見つけられずに下限へ
				g_nSelectNum = MAX_MODEL;
				g_pSelectModel = Get3DModel(g_nSelectNum);
			}
			else if (g_nSelectNum == nSaveNum)
			{//見つからない
				g_nSelectNum = nSaveNum;
				g_pSelectModel = Get3DModel(g_nSelectNum);
				g_pSelectModel->bAlpha = true;
				GenerateMessageBox(MB_ICONERROR, "Error", "もう無いです！！！！");
				break;
			}
		}
	}
}
//=================================================================================================
// --- 注視点移動 ---
//=================================================================================================
void MovePosR(void)
{
	Camera* pCamera = GetCamera(0);
	
	if (GetKeyboardPress(DIK_W))
	{ // Wを押したとき
		/** 追加入力 **/
		if (GetKeyboardPress(DIK_A))
		{// Aを押したとき
			SetPos.x += cosf(pCamera->rot.y + (D3DX_PI * 1.25f)) * g_fSpdPOSR;
			SetPos.z += sinf(pCamera->rot.y + (D3DX_PI * 1.25f)) * g_fSpdPOSR;
		}
		else if (GetKeyboardPress(DIK_D))
		{ // Dを押したとき
			SetPos.x += cosf(pCamera->rot.y + (D3DX_PI * 0.75f)) * g_fSpdPOSR;
			SetPos.z += sinf(pCamera->rot.y + (D3DX_PI * 0.75f)) * g_fSpdPOSR;
		}
		else
		{ // 純粋なW入力時
			SetPos.x += cosf(pCamera->rot.y + D3DX_PI) * g_fSpdPOSR;
			SetPos.z += sinf(pCamera->rot.y + D3DX_PI) * g_fSpdPOSR;
		}
	}
	else if (GetKeyboardPress(DIK_S))
	{ // Sを押したとき
		/** 追加入力 **/
		if (GetKeyboardPress(DIK_A))
		{// Aを押したとき
			pCamera->posR.x += cosf(pCamera->rot.y + (D3DX_PI * 1.75f)) * g_fSpdPOSR;
			pCamera->posR.z += sinf(pCamera->rot.y + (D3DX_PI * 1.75f)) * g_fSpdPOSR;

		}
		else if (GetKeyboardPress(DIK_D))
		{ // Dを押したとき
			
			pCamera->posR.x += cosf(pCamera->rot.y + (D3DX_PI * 0.25f)) * g_fSpdPOSR;
			pCamera->posR.z += sinf(pCamera->rot.y + (D3DX_PI * 0.25f)) * g_fSpdPOSR;
		}
		else
		{ // 純粋なS入力時
			pCamera->posR.x += cosf(pCamera->rot.y) * g_fSpdPOSR;
			pCamera->posR.z += sinf(pCamera->rot.y) * g_fSpdPOSR;
		}
	}
	else if (GetKeyboardPress(DIK_A))
	{ // Aを押したとき
		pCamera->posR.x += cosf(pCamera->rot.y + (D3DX_PI * 1.5f)) * g_fSpdPOSR;
		pCamera->posR.z += sinf(pCamera->rot.y + (D3DX_PI * 1.5f)) * g_fSpdPOSR;
	}
	else if (GetKeyboardPress(DIK_D))
	{ // Dを押したとき
		pCamera->posR.x += cosf(pCamera->rot.y + (D3DX_PI * 0.5f)) * g_fSpdPOSR;
		pCamera->posR.z += sinf(pCamera->rot.y + (D3DX_PI * 0.5f)) * g_fSpdPOSR;
	}

	/*** カメラの移動！(上下) ***/
	if (GetKeyboardPress(DIK_LCONTROL))
	{
		pCamera->posV.y -= g_fSpdPOSR;
		pCamera->posR.y -= g_fSpdPOSR;
	}
	else if (GetKeyboardPress(DIK_SPACE))
	{
		pCamera->posV.y += g_fSpdPOSR;
		pCamera->posR.y += g_fSpdPOSR;
	}

	if (GetKeyboardTrigger(DIK_LSHIFT))
	{
		g_fSpdPOSR *= 2.0f;
	}
	else if (GetKeyboardRelease(DIK_LSHIFT))
	{
		g_fSpdPOSR *= 0.5f;
	}

	pCamera->posR = SetPos;
}