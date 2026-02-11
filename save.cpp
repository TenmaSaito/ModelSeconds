//=============================================================================
//
//	外部ファイル書き出し処理 [save.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "save.h"
#include "mathUtil.h"
#include "model.h"
#include "field.h"
#include "input.h"

using namespace MyMathUtil;

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_MODELTYPE	(64)			// モデルの種類の最大数
#define SAVE_COOLTIME	(5000)			// 保存クールタイム

//*****************************************************************************
// グローバル変数
//*****************************************************************************

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

	D3DXVECTOR3 rot = VECNULL;										// 向き変換用

	_3DMODEL* p3DModel = Get3DModel(0);								// 設置モデルの先頭アドレス
	int *nNumModel = GetNum3DModel();								// 設置モデルの数

	Field* pField = GetField(0);									// 設置床の先頭アドレス
	int nNumFIeld = GetNumField();									// 設置床の数

	int nNumModelType = GetNumModel();								// モデル種類数
	int nNumTexture = GetNumTexture();								// テクスチャ種類数

	char* pModelFileName = GetModelFileName();						// モデルファイル名のポインタ
	char* pTextureFileName = GetTextureFileName();					// テクスチャファイル名のポインタ

	FILE* pFile = fopen(pSaveFileName, "w");						// ファイルを開く

	if (pFile == NULL)
	{// ファイルが開けなかったら
		MessageBox(NULL, TEXT("テキストデータの保存に失敗"), TEXT("Error"), MB_ICONHAND);
		return;
	}

	// ヘッダー
	fprintf(pFile, "#==============================================\n");
	fprintf(pFile, "#\n");
	fprintf(pFile, "# ModelSeconds [%s]\n", strrchr(pSaveFileName, '\\') + 1);
	fprintf(pFile, "# Author : \n");
	fprintf(pFile, "#\n");
	fprintf(pFile, "#==============================================\n\n");

	fprintf(pFile, "SCRIPT\t\t# この行は消さないでね！\n\n");		// 開始の合図

	// テクスチャ数書き出し
	fprintf(pFile, "#------------------------------------------------------------------------------\n");
	fprintf(pFile, "# テクスチャ数\n");
	fprintf(pFile, "#------------------------------------------------------------------------------\n");

	fprintf(pFile, "NUM_TEXTURE = %d\n", nNumTexture);

	fprintf(pFile, "\n");		// 改行

	// テクスチャファイル名書き出し
	fprintf(pFile, "#------------------------------------------------------------------------------\n");
	fprintf(pFile, "# テクスチャファイル名\n");
	fprintf(pFile, "#------------------------------------------------------------------------------\n");

	// テクスチャの種類数分だけ
	for (int nCntTexture = 0; nCntTexture < nNumTexture; nCntTexture++)
	{
		fprintf(pFile, "TEXTURE_FILENAME = %s\n", &pTextureFileName[nCntTexture * 128]);	// 文字数分ズラす
	}

	fprintf(pFile, "\n");		// 改行

	// モデル数書き出し
	fprintf(pFile, "#------------------------------------------------------------------------------\n");
	fprintf(pFile, "# モデル数\n");
	fprintf(pFile, "#------------------------------------------------------------------------------\n");

	fprintf(pFile, "NUM_MODEL = %d\n", nNumModelType);

	fprintf(pFile, "\n");		// 改行

	// モデルファイル名書き出し
	fprintf(pFile, "#------------------------------------------------------------------------------\n");
	fprintf(pFile, "# モデルファイル名\n");
	fprintf(pFile, "#------------------------------------------------------------------------------\n");

	// モデルの種類数分だけ
	for (int nCntModel = 0; nCntModel < nNumModelType; nCntModel++)
	{
		fprintf(pFile, "MODEL_FILENAME = %s\n", &pModelFileName[nCntModel * 128]);	// 文字数分ズラす
	}

	fprintf(pFile, "\n");		// 改行

	// 床数書き出し
	fprintf(pFile, "#------------------------------------------------------------------------------\n");
	fprintf(pFile, "# フィールド数\n");
	fprintf(pFile, "#------------------------------------------------------------------------------\n");

	fprintf(pFile, "NUM_FIELD = %d\n", nNumFIeld);

	fprintf(pFile, "\n");		// 改行

	// 床の配置情報書き出し
	fprintf(pFile, "#------------------------------------------------------------------------------\n");
	fprintf(pFile, "# 地面配置情報\n");
	fprintf(pFile, "#------------------------------------------------------------------------------\n");

	fprintf(pFile, "\n");		// 改行

	// 配置床数分だけ
	for (int nCntField = 0; nCntField < nNumFIeld; nCntField++, pField++)
	{
		rot = RadianToDegree(pField->rot);

		fprintf(pFile, "FIELDSET\n");
		fprintf(pFile, "\tTEXTYPE = %d\t\t# テクスチャの種類\n", pField->nIndexTexture);
		fprintf(pFile, "\tPOS = %.1f %.1f %.1f\t# 位置\n", pField->pos.x, pField->pos.y, pField->pos.z);
		fprintf(pFile, "\tROT = %d %d %d\t\t\t# 向き\n", (int)rot.x, (int)rot.y, (int)rot.z);
		fprintf(pFile, "\tBLOCK = %d %d\t# 分割数(X / Z)\n", pField->nXBlock, pField->nZBlock);
		fprintf(pFile, "\tSIZE = %.0f %.0f\t# 大きさ(X / Z)\n", pField->fWidth / pField->nXBlock, pField->fDepth / pField->nZBlock);
		fprintf(pFile, "END_FIELDSET\n");
		fprintf(pFile, "\n");
	}

	fprintf(pFile, "\n");		// 改行

	// モデルの配置情報書き出し
	fprintf(pFile, "#------------------------------------------------------------------------------\n");
	fprintf(pFile, "# モデル配置情報\n");
	fprintf(pFile, "#------------------------------------------------------------------------------\n");

	fprintf(pFile, "\n");		// 改行

	// 配置モデル数分だけ
	for (int nCntModel = 0; nCntModel < *nNumModel; nCntModel++, p3DModel++)
	{
		rot = RadianToDegree(p3DModel->rot);

		fprintf(pFile, "MODELSET\n");
		fprintf(pFile, "\tTYPE = %d\n", p3DModel->nIdx3Dmodel);
		fprintf(pFile, "\tPOS = %.1f %.1f %.1f\n", p3DModel->pos.x, p3DModel->pos.y, p3DModel->pos.z);
		fprintf(pFile, "\tROT = %d %d %d\n", (int)rot.x, (int)rot.y, (int)rot.z);
		fprintf(pFile, "END_MODELSET\n");
		fprintf(pFile, "\n");
	}

	fprintf(pFile, "\n");											// 改行
	fprintf(pFile, "END_SCRIPT\t\t# この行は消さないでね！");		// 終わりの合図

	fclose(pFile);					// ファイルを閉じる

	dwSaveTime = dwCurrentTime;		// 時間を記録
	
	GenerateMessageBox(MB_ICONASTERISK, "Save", "モデル情報の保存が完了しました！");	// 完了通知
}

//=============================================================================
//	配置モデル消去処理
//=============================================================================
void DeleteModel(_3DMODEL* p3DModel, int* nNumModel, int nIdx)
{
	for (int nCnt3DModel = nIdx; nCnt3DModel < *nNumModel; nCnt3DModel++)
	{// 消去したいモデルまで一個ずつ詰めていく
		p3DModel[nCnt3DModel] = p3DModel[nCnt3DModel + 1];
	}

	(*nNumModel)--;
}