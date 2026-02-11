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

using namespace MyMathUtil;

//*************************************************************************************************
//*** マクロ定義 ***
//*************************************************************************************************

//*************************************************************************************************
//*** グローバル変数 ***
//*************************************************************************************************
 LP3DMODEL g_pSelectModel;			//選択中のモデルの情報
 ControlType g_ControlType;			//コントローラーの状態
 int g_nSelectNum;

//=================================================================================================
// --- コントローラー初期化 ---
//=================================================================================================
void InitController(void) 
{
	g_nSelectNum = 0;
	g_pSelectModel = Get3DModel(g_nSelectNum);
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
	PrintDebugProc("SELECT NUMBER : %d\n", g_nSelectNum);

	//================================
	// -- コントロールタイプ選択 --
	//================================
	if (GetKeyboardTrigger(DIK_1))
	{//モデル選択
		g_ControlType = CONTROLTYPE_SELECT;
		PrintDebugProc("CONTROLTYPE_SELECT\n");
	}
	else if (GetKeyboardTrigger(DIK_2))
	{//モデル位置移動
		g_ControlType = CONTROLTYPE_POS;
		PrintDebugProc("CONTROLTYPE_POS\n");
	}
	else if (GetKeyboardTrigger(DIK_3))
	{//モデル角度変更
		g_ControlType = CONTROLTYPE_ROT;
		PrintDebugProc("CONTROLTYPE_ROT\n");
	}

	//================================
	// -- コントロール --
	//================================
	if (g_ControlType == CONTROLTYPE_SELECT)
	{
		if (GetKeyboardTrigger(DIK_RIGHT))
		{//一つ上のモデルを見る
			g_nSelectNum++;
			g_pSelectModel = Get3DModel(g_nSelectNum);

			if (g_pSelectModel->bUse == false)
			{//選択された番号のモデルが不使用
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

			g_pSelectModel = Get3DModel(g_nSelectNum);

			if (g_pSelectModel->bUse == false)
			{
				SearchUseModel(false);
			}
		}
		if (GetKeyboardTrigger(DIK_UP))
		{

		}
		if (GetKeyboardTrigger(DIK_DOWN))
		{

		}
	}
	else if (g_ControlType == CONTROLTYPE_POS)
	{

	}
	else if (g_ControlType == CONTROLTYPE_ROT)
	{

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
	if (bInDe == true)
	{
		while (1)
		{
			g_nSelectNum++;
			g_pSelectModel = Get3DModel(g_nSelectNum);
			if (g_pSelectModel->bUse == true)
			{//使用されているモデル発見
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
				GenerateMessageBox(MB_ICONERROR, "Error", "もう無いです！！！！");
				break;
			}
		}
	}
}