//=============================================================================
//
//	外部ファイル書き出し処理 [save.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "save.h"
#include "mathUtil.h"
#include "model.h"
#include "input.h"

using namespace MyMathUtil;

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_MODELTYPE	(64)			// モデルの最大数(仮)
#define SAVE_COOLTIME	(5000)			// 保存クールタイム

//*****************************************************************************
// グローバル変数
//*****************************************************************************
char g_aFileName[MAX_MODELTYPE][FILENAME_MAX] = {};					// パスの名前

//=============================================================================
//	外部ファイル書き出し処理
//=============================================================================
void SaveModelFile(const char* pSaveFileName)
{
	DWORD dwCurrentTime = timeGetTime();							// 入った時の時間

	static DWORD dwSaveTime;										// セーブした時の時間

	if (dwCurrentTime - dwSaveTime <= SAVE_COOLTIME)
	{// 時間がたっていなければ返す
		return;
	}

	_3DMODEL* p3DModel = Get3DModel(0);								// 設置モデルの先頭アドレス
	int nNumModel = GetNum3DModel();								// 設置モデルの数

	int nNumModelType = GetNumModel();								// モデル種類数
	int nNumTexture = GetNumTexture();								// テクスチャ種類数

	char* pModelFileName = GetModelFileName();						// モデルファイル名のポインタ
	char* pTextureFileName = GetTextureFileName();					// テクスチャファイル名のポインタ

	strcpy(&g_aFileName[0][0], "test.txt");							// モデルファイル名仮置き

	FILE* pFile = fopen(pSaveFileName, "w");						// ファイルを開く

	if (pFile == NULL)
	{// ファイルが開けなかったら
		MessageBox(NULL, TEXT("テキストデータの保存に失敗"), TEXT("Error"), MB_ICONHAND);
		return;
	}
	// ヘッダー
	fprintf(pFile, "#==============================================\n");
	fprintf(pFile, "#\n");
	fprintf(pFile, "# ModelSeconds [%s]\n", pSaveFileName);
	fprintf(pFile, "# Author : \n");
	fprintf(pFile, "#\n");
	fprintf(pFile, "#==============================================\n\n");

	fprintf(pFile, "SCRIPT\t\t# この行は消さないでね！\n\n");		// 開始の合図

	// テクスチャ数書き出し
	fprintf(pFile, "#------------------------------------------------------------------------------\n");
	fprintf(pFile, "# テクスチャ数\n");
	fprintf(pFile, "#------------------------------------------------------------------------------\n");

	fprintf(pFile, "NUM_TEXTURE = %d\n", nNumTexture);

	fprintf(pFile, "\n");

	// テクスチャファイル名書き出し
	fprintf(pFile, "#------------------------------------------------------------------------------\n");
	fprintf(pFile, "# テクスチャファイル名\n");
	fprintf(pFile, "#------------------------------------------------------------------------------\n");

	// テクスチャの種類数分だけ
	for (int nCntTexture = 0; nCntTexture < nNumTexture; nCntTexture++)
	{
		fprintf(pFile, "TEXTURE_FILENAME = %s\n", &pTextureFileName[nCntTexture * 128]);	// 文字数分ズラす
	}

	fprintf(pFile, "\n");

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
		fprintf(pFile, "MODEL_FILENAME = %s\n", &pModelFileName[nCntModel * 128]);	// 文字数分ズラす
	}

	fprintf(pFile, "\n");

	// モデルの配置情報書き出し
	fprintf(pFile, "#------------------------------------------------------------------------------\n");
	fprintf(pFile, "# モデル配置情報\n");
	fprintf(pFile, "#------------------------------------------------------------------------------\n");

	fprintf(pFile, "\n");

	// 配置モデル数分だけ
	for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++, p3DModel++)
	{
		fprintf(pFile, "MODELSET\n");
		fprintf(pFile, "\tTYPE = %d\n", p3DModel->nIdx3Dmodel);
		fprintf(pFile, "\tPOS = %.1f %.1f %.1f\n", p3DModel->pos.x, p3DModel->pos.y, p3DModel->pos.z);
		fprintf(pFile, "\tROT = %.1f %.1f %.1f\n", p3DModel->rot.x, p3DModel->rot.y, p3DModel->rot.z);
		fprintf(pFile, "END_MODELSET\n");
		fprintf(pFile, "\n");
	}

	fprintf(pFile, "\n");
	fprintf(pFile, "END_SCRIPT\t\t# この行は消さないでね！");		// 終わりの合図

	fclose(pFile);		// ファイルを閉じる

	dwSaveTime = dwCurrentTime;		// 時間を記録
	
	GenerateMessageBox(MB_ICONASTERISK, "Save", "モデル情報の保存が完了しました！");
}

//=============================================================================
//	配置モデル消去処理
//=============================================================================
void DeleteModel(_3DMODEL* p3DModel, int nNumModel, int nIdx)
{
	for (int nCnt3DModel = nIdx; nCnt3DModel < nNumModel; nCnt3DModel++)
	{
		p3DModel[nCnt3DModel] = p3DModel[nCnt3DModel + 1];
	}
}