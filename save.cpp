//=============================================================================
//
//	外部ファイル書き出し処理 [save.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "save.h"
#include "mathUtil.h"

using namespace MyMathUtil;

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_MODEL		(64)			// モデルの最大数(仮)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
char g_aFileName[MAX_MODEL][FILENAME_MAX] = {};					// パスの名前

//=============================================================================
//	外部ファイル書き出し処理
//=============================================================================
void SaveModelFile(void)
{
	const char* pFileName[] = { "data\\Scripts\\file.txt" };		// 仮置きファイル名
	const char aSaveFileName[FILENAME_MAX] = { "model.txt" };		// 

	int nNumModelType = 1;											// 仮置きモデル種類数
	int nNumModel = 2;												// 仮置きモデル数
	int nType = 1;													// 仮置き種類
	D3DXVECTOR3 pos, rot = {};										// 位置と向き仮置き

	pos = rot = VECNULL;											// 初期化

	strcpy(&g_aFileName[0][0], "test.txt");							// モデルファイル名仮置き

	FILE* pFile = fopen(pFileName[0], "w");							// ファイルを開く

	if (pFile == NULL)
	{// ファイルが開けなかったら
		MessageBox(NULL, TEXT("テキストデータの保存に失敗"), TEXT("Error"), MB_ICONHAND);
		return;
	}

	// ヘッダー
	fprintf(pFile, "#==============================================\n");
	fprintf(pFile, "#\n");
	fprintf(pFile, "# ModelSeconds [%s]\n", &aSaveFileName[0]);
	fprintf(pFile, "# Author : \n");
	fprintf(pFile, "#\n");
	fprintf(pFile, "#==============================================\n\n");

	fprintf(pFile, "SCRIPT\n\n");		// 開始の合図

	// モデル数書き出し
	fprintf(pFile, "#------------------------------------------------------------------------------\n");
	fprintf(pFile, "# モデル数\n");
	fprintf(pFile, "#------------------------------------------------------------------------------\n");

	fprintf(pFile, "NUM_MODEL = %d\n", nNumModelType);

	fprintf(pFile, "\n");

	// モデルファイル名書き出し
	fprintf(pFile, "#------------------------------------------------------------------------------\n");
	fprintf(pFile, "# モデルファイル名\n");
	fprintf(pFile, "#------------------------------------------------------------------------------\n");

	// モデルの種類数分だけ
	for (int nCntModel = 0; nCntModel < nNumModelType; nCntModel++)
	{
		fprintf(pFile, "MODEL_FILENAME = %s\n", &g_aFileName[nCntModel][0]);
	}

	fprintf(pFile, "\n");

	// モデルの配置情報書き出し
	fprintf(pFile, "#------------------------------------------------------------------------------\n");
	fprintf(pFile, "# モデル配置情報\n");
	fprintf(pFile, "#------------------------------------------------------------------------------\n");

	fprintf(pFile, "\n");

	// 配置モデル数分だけ
	for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
	{
		fprintf(pFile, "MODELSET\n");
		fprintf(pFile, "\tTYPE = %d\n", nType);
		fprintf(pFile, "\tPOS = %.1f %.1f %.1f\n", pos.x, pos.y, pos.z);
		fprintf(pFile, "\tROT = %.1f %.1f %.1f\n", rot.x, rot.y, rot.z);
		fprintf(pFile, "END_MODELSET\n");
		fprintf(pFile, "\n");
	}

	fprintf(pFile, "\n");
	fprintf(pFile, "END_SCRIPT\t\t# この行は消さないでね！");		// 終わりの合図

	fclose(pFile);

	// 保留！！
	//if ()											// ファイルを閉じる
	//{
	//	GenerateMessageBox(MB_ICONASTERISK, "Save", "モデル情報の保存が完了しました！");
	//}

}

//=============================================================================
//	配置モデル消去処理
//=============================================================================
void DeleteModel(void)
{

}