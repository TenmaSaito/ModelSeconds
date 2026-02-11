// =================================================
// 
// モデル処理[model.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "main.h"
#include "camera.h"
#include "debugproc.h"
#include "field.h"
#include "model.h"
#include "input.h"
#include "mathUtil.h"
#include "3Dmodel.h"
#include "texture.h"
#include "modeldata.h"

using namespace MyMathUtil;

// =================================================
// マクロ定義
#define	MODEL_TXT	"data\\Scripts\\model.txt"	// モデルの外部ファイル
#define STR_SUCCESS(Realize)			(Realize != NULL)	// strstrでの成功判定

// =================================================
// グローバル変数
Model g_Model[MAX_MODEL];			// モデルの情報
int g_nNumTexture;					// テクスチャの数
char g_TextureName[MAX_MODEL][128];	// テクスチャのファイル名
int g_nNumModel;					// モデルの種類数
char g_ModelName[MAX_MODEL][128];	// モデルのファイル名
int g_nNumObj;						// オブジェクトの総数

// =================================================
// 初期化処理
// =================================================
void InitModel(void)
{
	
}

// =================================================
// 終了処理
// =================================================
void UninitModel(void)
{
	
}

// =================================================
// 更新処理
// =================================================
void UpdateModel(void)
{

}

// =================================================
// 描画処理
// =================================================
void DrawModel(void)
{
	
}

// =================================================
// モデルの設定処理
// =================================================
void SetModel(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nNumIdx, int nUseShadow)
{
	
}

// =================================================
// モデル情報の譲渡
// =================================================
Model* GetModel(void)
{
	return &g_Model[0];
}

// =================================================
// モデルの読み込む処理
// =================================================
bool LoadModel(const char *pFileName, int nThreadNum)
{	
	if (nThreadNum == 0)
	{
		// === 全項目共通で使用可能な変数 === //
		MyMathUtil::INT_VECTOR3 pos;
		MyMathUtil::INT_VECTOR3 rot;

		// === スカイ用の変数を用意 === //
		int nIdxTexSky = 0;					// 空用のテクスチャのインデックス
		float fMoveSky = 0.0f;				// 空の移動量
		int nXdevideSky, nYdevideSky = 0;	// 空の分割数
		int nRadiusSky = 0;					// 空のサイズ

		// === 雲用の変数を用意 === // 
		int nIdxTexCloud = 0;	// 雲のテクスチャのインデックス
		int nRadiusCloud = 0;	// 雲の半径
		int nHeightCloud = 0;	// 雲のテクスチャの高さ
		int nXdevideCloud, nYdevideCloud = 0;	// 雲のテクスチャの分割数

		// === フィールドの情報を格納する変数 === //
		D3DXVECTOR3 posField;
		int nIdxTexField = 0;	// テクスチャのタイプ
		int aIdxTexture[100] = {};	// テクスチャのタイプ
		int nXdevideField, nYdevideField = 0;	// フィールドの分割数
		int nXsize, nYsize = 0;		// フィールドのサイズ
		int nCntField = 0;

		// === モデル用の変数を用意 === //
		int nNumIdx = 0;			// モデルファイル名のインデックス
		int nNumModel = 0;			// モデル数
		char Realize[128];			// 判別用の変数
		char* pEqual = NULL;		// =を消去するためのポインタ
		int nCntModel = 0;			// 読み込むモデルファイル名をカウントする
		int nCntTexture = 0;		// 読み込むテクスチャのファイル名をカウントする
		int nUseShadow = 0;			// 影を使用するかどうか用の変数

		int nNumVtx[MAX_MODEL];		// 頂点数
		DWORD dwSizeFVF[MAX_MODEL];	// 頂点フォーマットのサイズ
		BYTE* pVtxBuff;					// 頂点バッファへのポインタ
		LPDIRECT3DDEVICE9 pDevice;		// デバイスへのポインタ
		D3DXMATERIAL* pMat;				// マテリアルへのポインタ

		// デバイスの取得
		pDevice = GetDevice();

		// ステージ情報を読み込み
		FILE* pFile;
		pFile = NULL;

		pFile = fopen(pFileName, "r");

		if (pFile != NULL)
		{// ファイルを開いた
			while (1)
			{
				fgets(&Realize[0], sizeof Realize, pFile);	// 最初に比較用文字の読み込み

				if (JudgeComent(&Realize[0]) == true)
				{// 何かの情報を読み込む合図を取得したとき
					if (strcmp(&Realize[0], "SCRIPT") == 0)
					{

					}
					if (strstr(&Realize[0], "NUM_TEXTURE") != NULL)
					{// テクスチャの数を代入
						// 「=」の場所を見つける
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// テクスチャの数を読み込む
							(void)sscanf(pEqual, "%d", &g_nNumTexture);
						}

					}
					if (strstr(&Realize[0], "TEXTURE_FILENAME") != NULL)
					{// テクスチャのファイル名を代入
						while (1)
						{
							if (nCntTexture < g_nNumTexture)
							{// 上で代入したテクスチャ数よりも、読み込んだファイル数が少ないとき
								// 「=」の場所を見つける
								pEqual = strstr(Realize, "=");

								if (pEqual != NULL)
								{
									// アドレスを1こずらす
									pEqual++;

									// テクスチャの数を読み込む
									(void)sscanf(pEqual, "%s", &g_TextureName[nCntTexture][0]);

									LoadTexture(&g_TextureName[nCntTexture][0], &aIdxTexture[nCntTexture]);

									// テクスチャのファイル名をカウント
									nCntTexture++;
									break;
								}

							}
						}
					}
				}

				if (strstr(&Realize[0], "FIELDSET") != NULL)
				{// フィールドの情報を代入
					while (1)
					{
						fgets(&Realize[0], sizeof Realize, pFile);	// 最初に比較用文字の読み込み

						if (strstr(&Realize[0], "TEXTYPE") != NULL)
						{
							// 「=」の場所を見つける
							pEqual = strstr(Realize, "=");

							if (pEqual != NULL)
							{
								// アドレスを1こずらす
								pEqual++;

								// テクスチャのインデックスを読み込む
								(void)sscanf(pEqual, "%d", &nIdxTexField);
							}
						}
						if (strstr(&Realize[0], "POS") != NULL)
						{
							pEqual = strstr(Realize, "=");

							if (pEqual != NULL)
							{
								// アドレスを1こずらす
								pEqual++;

								// 地面の位置を読み込む
								(void)sscanf(pEqual, "%f %f %f", &posField.x, &posField.y, &posField.z);
							}
						}
						if (strstr(&Realize[0], "ROT") != NULL)
						{
							pEqual = strstr(Realize, "=");

							if (pEqual != NULL)
							{
								// アドレスを1こずらす
								pEqual++;

								// 地面の向きを読み込む
								(void)sscanf(pEqual, "%d %d %d", &rot.x, &rot.y, &rot.z);
							}
						}
						if (strstr(&Realize[0], "BLOCK") != NULL)
						{
							pEqual = strstr(Realize, "=");

							if (pEqual != NULL)
							{
								// アドレスを1こずらす
								pEqual++;

								// 分割数を読み込む
								(void)sscanf(pEqual, "%d %d", &nXdevideField, &nYdevideField);
							}
						}
						if (strstr(&Realize[0], "SIZE") != NULL)
						{
							pEqual = strstr(Realize, "=");

							if (pEqual != NULL)
							{
								// アドレスを1こずらす
								pEqual++;

								// 縦横のサイズを読み込む
								(void)sscanf(pEqual, "%d %d", &nXsize, &nYsize);
							}
						}
						if (strstr(&Realize[0], "END_FIELDSET") != NULL)
						{
							D3DXVECTOR3 rotVec = DegreeToRadian(INTToFloat(rot));
							SetField(posField, VECNULL, rotVec, (float)nXsize, (float)nYsize, aIdxTexture[nIdxTexField], (float)nXdevideField, (float)nYdevideField, D3DCULL_CCW);
							nCntField++;
							break;
						}
					}

				}

				if (strstr(&Realize[0], "NUM_MODEL") != NULL)
				{// モデルの数を代入
					// 「=」の場所を見つける
					pEqual = strstr(Realize, "=");

					if (pEqual != NULL)
					{
						// アドレスを1こずらす
						pEqual++;

						// モデルの数を読み込む
						(void)sscanf(pEqual, "%d", &g_nNumModel);
					}

				}

				if (strstr(&Realize[0], "MODEL_FILENAME") != NULL)
				{// モデルのファイル名を代入
					// 「=」の場所を見つける
					pEqual = strstr(Realize, "=");

					if (pEqual != NULL)
					{
						HRESULT hr;		// エラー文用

						// アドレスを1こずらす
						pEqual++;

						// モデルの数を読み込む
						sscanf(pEqual, "%s", &g_ModelName[nCntModel][0]);

						// モデルを読み込む
						LoadModelData(&g_ModelName[nCntModel][0], &g_Model[nCntModel].nIdxModelData);
						g_Model[nCntModel].bUse = true;

						nCntModel++;
					}
				}
				if (STR_SUCCESS(strstr(&Realize[0], "MODELSET")))
				{
					D3DXVECTOR3 posF;

					while (1)
					{
						fgets(&Realize[0], sizeof Realize, pFile);	// 最初に比較用文字の読み込み

						if (JudgeComent(&Realize[0]) == true)
						{
							if (strstr(&Realize[0], "TYPE") != NULL)
							{
								pEqual = strstr(Realize, "=");

								if (pEqual != NULL)
								{
									// アドレスを1こずらす
									pEqual++;

									// モデルの数を読み込む
									(void)sscanf(pEqual, "%d", &nNumIdx);
								}
							}
							if (strstr(&Realize[0], "POS") != NULL)
							{
								pEqual = strstr(Realize, "=");

								if (pEqual != NULL)
								{
									// アドレスを1こずらす
									pEqual++;

									// モデルの数を読み込む
									(void)sscanf(pEqual, "%f %f %f", &posF.x, &posF.y, &posF.z);
								}
							}
							if (strstr(&Realize[0], "ROT") != NULL)
							{
								pEqual = strstr(Realize, "=");

								if (pEqual != NULL)
								{
									// アドレスを1こずらす
									pEqual++;

									// モデルの数を読み込む
									(void)sscanf(pEqual, "%d %d %d", &rot.x, &rot.y, &rot.z);
								}
							}
							if (strstr(&Realize[0], "SHADOW") != NULL)
							{
								pEqual = strstr(Realize, "=");

								if (pEqual != NULL)
								{
									// アドレスを1こずらす
									pEqual++;

									// モデルの数を読み込む
									(void)sscanf(pEqual, "%d", &nUseShadow);
								}
							}
							if (strstr(&Realize[0], "END_MODELSET") != NULL)
							{
								// Set3DModel
								D3DXVECTOR3 rotF = INTToFloat(rot);
								rotF = DegreeToRadian(rotF);
								Set3DModel(posF, rotF, g_Model[nNumIdx].nIdxModelData);
								break;
							}
						}
					}
				}
				else if (strcmp(&Realize[0], "END_SCRIPT") == 0)
				{// スクリプト読み込みを終了する
					break;
				}
			}
			fclose(pFile);	// ファイルを閉じる
		}
		else
		{// ファイルオープン失敗時
			// デバイスの破棄
			EndDevice();
			return false;
		}

		// デバイスの破棄
		EndDevice();

		return true;
	}
	else
	{
	// === 全項目共通で使用可能な変数 === //
	MyMathUtil::INT_VECTOR3 pos;
	MyMathUtil::INT_VECTOR3 rot;

	// === スカイ用の変数を用意 === //
	int nIdxTexSky = 0;					// 空用のテクスチャのインデックス
	float fMoveSky = 0.0f;				// 空の移動量
	int nXdevideSky, nYdevideSky = 0;	// 空の分割数
	int nRadiusSky = 0;					// 空のサイズ

	// === 雲用の変数を用意 === // 
	int nIdxTexCloud = 0;	// 雲のテクスチャのインデックス
	int nRadiusCloud = 0;	// 雲の半径
	int nHeightCloud = 0;	// 雲のテクスチャの高さ
	int nXdevideCloud, nYdevideCloud = 0;	// 雲のテクスチャの分割数

	// === フィールドの情報を格納する変数 === //
	D3DXVECTOR3 posField;
	int nIdxTexField = 0;	// テクスチャのタイプ
	int aIdxTexture[100] = {};	// テクスチャのタイプ
	int nXdevideField, nYdevideField = 0;	// フィールドの分割数
	int nXsize, nYsize = 0;		// フィールドのサイズ
	int nCntField = 0;

	// === モデル用の変数を用意 === //
	int nNumIdx = 0;			// モデルファイル名のインデックス
	int nNumModel = 0;			// モデル数
	char Realize[128];			// 判別用の変数
	char* pEqual = NULL;		// =を消去するためのポインタ
	int nCntModel = 0;			// 読み込むモデルファイル名をカウントする
	int nCntTexture = 0;		// 読み込むテクスチャのファイル名をカウントする
	int nUseShadow = 0;			// 影を使用するかどうか用の変数

	int nNumVtx[MAX_MODEL];		// 頂点数
	DWORD dwSizeFVF[MAX_MODEL];	// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;					// 頂点バッファへのポインタ
	LPDIRECT3DDEVICE9 pDevice;		// デバイスへのポインタ
	D3DXMATERIAL* pMat;				// マテリアルへのポインタ

	// デバイスの取得
	pDevice = GetDevicePrev();

	// ステージ情報を読み込み
	FILE* pFile;
	pFile = NULL;

	pFile = fopen(pFileName, "r");

	if (pFile != NULL)
	{// ファイルを開いた
		while (1)
		{
			fgets(&Realize[0], sizeof Realize, pFile);	// 最初に比較用文字の読み込み

			if (JudgeComent(&Realize[0]) == true)
			{// 何かの情報を読み込む合図を取得したとき
				if (strcmp(&Realize[0], "SCRIPT") == 0)
				{

				}

				if (strstr(&Realize[0], "NUM_MODEL") != NULL)
				{// モデルの数を代入
					// 「=」の場所を見つける
					pEqual = strstr(Realize, "=");

					if (pEqual != NULL)
					{
						// アドレスを1こずらす
						pEqual++;

						// モデルの数を読み込む
						(void)sscanf(pEqual, "%d", &g_nNumModel);
					}

				}

				if (strstr(&Realize[0], "MODEL_FILENAME") != NULL)
				{// モデルのファイル名を代入
					// 「=」の場所を見つける
					pEqual = strstr(Realize, "=");

					if (pEqual != NULL)
					{
						HRESULT hr;		// エラー文用

						// アドレスを1こずらす
						pEqual++;

						// モデルの数を読み込む
						sscanf(pEqual, "%s", &g_ModelName[nCntModel][0]);

						int n;

						// モデルを読み込む
						LoadModelData(&g_ModelName[nCntModel][0], &n, 1);
						g_Model[nCntModel].bUse = true;

						nCntModel++;
					}
				}

				if (strcmp(&Realize[0], "END_SCRIPT") == 0)
				{// スクリプト読み込みを終了する
					break;
				}
			}
		}

		fclose(pFile);	// ファイルを閉じる
	}
	else
	{// ファイルオープン失敗時
		// デバイスの破棄
		EndDevice();
		return false;
	}

	// デバイスの破棄
	EndDevice();

	return true;
	}
}

// =================================================
// スクリプト読み込みでのコメントアウトの処理
// =================================================
bool JudgeComent(char* pStr)
{
	// アドレス取得用のchar型のポインタを用意
	char* pTrash = {};

	if (strncmp(pStr, "#", 1) != 0 && strncmp(pStr, "\n", 1) != 0)
	{// 文頭が、コメントアウト、改行でなかった場合
		// #があるかを判別
		pTrash = strstr(pStr, "#");

		if (pTrash != NULL)
		{// #が存在する
			// それ以降の文字列を消去
			strcpy(pTrash, "");
		}

		while (1)
		{// tabが存在する場合回し続ける
			// tabのアドレスを取得
			pTrash = strstr(pStr, "\t");

			if (pTrash != NULL)
			{// tabが存在する

				if (pTrash + 1 == NULL)
				{// 文末にあった時
					// 後ろを空白にする
					strcpy(pTrash + 1, "");
				}
				else
				{// 文頭にあった時
					// tab以降の文字列をtabがあったところに代入
					strcpy(pTrash, pTrash + 1);
				}
			}
			else if (pTrash == NULL)
			{// tabが存在しなくなったとき
				break;
			}
		}

		return true;
	}
	else
	{// コメントアウト、改行が文頭だった場合
		return false;
	}
}

// =================================================
// モデルの種類数取得処理
// =================================================
int GetNumModel(void)
{
	return g_nNumModel;
}

// =================================================
// モデルファイル名取得処理
// =================================================
char* GetModelFileName(void)
{
	return &g_ModelName[0][0];
}

// =================================================
// テクスチャの種類数取得処理
// =================================================
int GetNumTexture(void)
{
	return g_nNumTexture;
}

// =================================================
// テクスチャファイル名取得処理
// =================================================
char* GetTextureFileName(void)
{
	return &g_TextureName[0][0];
}