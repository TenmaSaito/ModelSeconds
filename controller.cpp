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
#include "save.h"
#include "prevModel.h"

using namespace MyMathUtil;

//*************************************************************************************************
//*** マクロ定義 ***
//*************************************************************************************************
#define MOVE_POS	(1.0f)		//入力一回のPOS移動量
#define MOVE_ROT	(0.1f)		//入力一回のROT変更量
#define SETPOS_SPD	(2.0f)			// カメラの移動速度
#define TAGESPEED	(0.02f)			// タゲへの移動速度割合
#define TAGE_HEIGHT	(200.0f)		//　タゲ位置の高さ追加分

//*************************************************************************************************
//*** グローバル変数 ***
//*************************************************************************************************
 LP3DMODEL g_pSelectModel;			// 選択中のモデルの情報
 ControlType g_ControlType;			// コントローラーの状態
 int g_nSelectNum;					// 選択中のモデルの番号
 D3DXVECTOR3 SetPos;				// 設置位置
 D3DXVECTOR3 SetRot;				// 設置角度
 float g_fSpdPOSR;					//注視点移動速度
 bool bTarget;						//RSHIFTによる対象への視点移動
 bool g_bPosRot;					//TRUE POS / FALSE ROT

//=================================================================================================
// --- コントローラー初期化 ---
//=================================================================================================
void InitController(void) 
{
	g_nSelectNum = 0;
	g_pSelectModel = Get3DModel(g_nSelectNum);
	g_pSelectModel->bAlpha = true;
	SetPos = VECNULL;
	SetRot = VECNULL;
	g_fSpdPOSR = SETPOS_SPD;
	bTarget = false;
	g_bPosRot = true;

	Set3DModelLast(SetPos, SetRot);
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
	else if (GetKeyboardTrigger(DIK_4))
	{
		g_ControlType = CONTROLTYPE_SET;
	}

	//================================
	// -- コントロール --
	//================================
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
	else if (g_ControlType == CONTROLTYPE_SET)
	{// -- モデルセット -- //
		P3DMODEL pLast = GetLast3DModel();
		if (GetKeyboardTrigger(DIK_RSHIFT))
		{
			g_bPosRot ^= true;
			pLast->bAlpha ^= true;
		}

		if (GetKeyboardTrigger(DIK_P))
		{
			pLast->nIdx3Dmodel++;
			if (pLast->nIdx3Dmodel > GetNumModelIdx() - 1)
			{
				pLast->nIdx3Dmodel = 0;
			}
		}
		else if (GetKeyboardTrigger(DIK_O))
		{
			pLast->nIdx3Dmodel--;
			if (pLast->nIdx3Dmodel < 0)
			{
				pLast->nIdx3Dmodel = GetNumModelIdx() - 1;
			}
		}

		if (g_bPosRot == true)
		{
			if (GetKeyboardPress(DIK_RIGHT))
			{//右
				if (GetKeyboardPress(DIK_LSHIFT))
				{//LSHIFTで上
					SetPos.y += MOVE_POS;
				}
				else
				{
					SetPos.x += MOVE_POS;
				}
			}
			if (GetKeyboardPress(DIK_LEFT))
			{//左
				if (GetKeyboardPress(DIK_LSHIFT))
				{//LSHIFTで下
					SetPos.y -= MOVE_POS;
				}
				else
				{
					SetPos.x -= MOVE_POS;
				}
			}
			if (GetKeyboardPress(DIK_UP))
			{//奥
				if (GetKeyboardPress(DIK_LSHIFT))
				{//LSHIFTで上
					SetPos.y += MOVE_POS;
				}
				else
				{
					SetPos.z += MOVE_POS;
				}
			}
			if (GetKeyboardPress(DIK_DOWN))
			{//手前
				if (GetKeyboardPress(DIK_LSHIFT))
				{//LSHIFTで下
					SetPos.y -= MOVE_POS;
				}
				else
				{
					SetPos.z -= MOVE_POS;
				}
			}
		}
		else if (g_bPosRot == false)
		{
			if (GetKeyboardPress(DIK_RIGHT))
			{//右
				if (GetKeyboardPress(DIK_LSHIFT))
				{//LSHIFTで上
					SetRot.x += MOVE_ROT;
				}
				else
				{
					SetRot.y += MOVE_ROT;
				}
			}
			if (GetKeyboardPress(DIK_LEFT))
			{//左
				if (GetKeyboardPress(DIK_LSHIFT))
				{//LSHIFTで下
					SetRot.x -= MOVE_ROT;
				}
				else
				{
					SetRot.y -= MOVE_ROT;
				}
			}
			if (GetKeyboardPress(DIK_UP))
			{//奥
				if (GetKeyboardPress(DIK_LSHIFT))
				{//LSHIFTで上
					SetRot.z += MOVE_ROT;
				}
				else
				{
					SetRot.x += MOVE_ROT;
				}
			}
			if (GetKeyboardPress(DIK_DOWN))
			{//手前
				if (GetKeyboardPress(DIK_LSHIFT))
				{//LSHIFTで下
					SetRot.z -= MOVE_ROT;
				}
				else
				{
					SetRot.y -= MOVE_ROT;
				}
			}
		}
		pLast->pos = SetPos;
		pLast->rot = SetRot;
		SetPrevModel(pLast->nIdx3Dmodel);

		if (GetKeyboardPress(DIK_TAB))
		{
			Set3DModel(SetPos, SetRot,pLast->nIdx3Dmodel);
		}
	}

	//=================================
	//	     -- 常設コマンド --
	// ================================
	if (GetKeyboardTrigger(DIK_DELETE))
	{
		DeleteModel(Get3DModel(0), GetNum3DModel(), g_nSelectNum);
		if (&g_nSelectNum > GetNum3DModel())
		{
			g_nSelectNum = *GetNum3DModel();
			g_pSelectModel = Get3DModel(g_nSelectNum);
		}
		g_pSelectModel->bAlpha = true;
	}

	//選択中のモデルの位置へ移動
	if (GetKeyboardTrigger(DIK_RSHIFT))
	{
		bTarget ^= true;
	}

	if (bTarget == true)
	{
		pCamera->posR.x += (g_pSelectModel->pos.x - pCamera->posR.x) * TAGESPEED;
		pCamera->posR.y += (g_pSelectModel->pos.y + 200.0f - pCamera->posR.y) * TAGESPEED;
		pCamera->posR.z += (g_pSelectModel->pos.z - pCamera->posR.z) * TAGESPEED;
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