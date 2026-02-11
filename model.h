// =================================================
// 
// モデル処理[model.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _MODEL_H_
#define _MODEL_H_

#include"main.h"

// =================================================
// マクロ定義
#define	MAX_TEX		(25)	// テクスチャの最大数
#define	MAX_MODEL	(512)	// モデルの最大数
#define	PLAYER_XFIX	(30)	// プレイヤーのXサイズの補正
#define	PLAYER_YFIX	(0)		// プレイヤーのYサイズの補正
#define	PLAYER_ZFIX	(30)	// プレイヤーのZサイズの補正

// =================================================
// モデルの構造体を定義
typedef struct
{
	int nIdxModelData;		// モデルデータのインデックス
	bool bUse;
}Model;

// =================================================
// モデルの種類を列挙型で定義
typedef enum
{
	
	MODELTYPE_MAX
}ModelType;

// =================================================
// プロトタイプ宣言
void InitModel(void);	// 初期化処理
void UninitModel(void);	// 終了処理
void UpdateModel(void);	// 更新処理
void DrawModel(void);	// 描画処理
void SetModel(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nType, int nUseShadow);		// モデルの設定処理
Model* GetModel(void);			// モデル情報の譲渡(種類毎)
bool LoadModel(const char *pFileName);			// モデルの読み込み処理
bool JudgeComent(char* pStr);	// スクリプト読み込みでのコメントアウトの処理
int GetNumModel(void);
char* GetModelFileName(void);
int GetNumTexture(void);
char* GetTextureFileName(void);
bool CollisionModel(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove);	// モデルとの当たり判定

#endif