//================================================================================================================
//
// windowsアプリケーションの作成->ウィンドウの表示処理 [main.cpp]
// Author : TENMA
//
//================================================================================================================
//**********************************************************************************
//*** インクルード ***
//**********************************************************************************
#include "main.h"
#include "input.h"
#include "mathUtil.h"
#include "debugproc.h"
#include "thread.h"
#include "resource.h"

// MODE
#include "mode.h"
#include "edit.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define CLASS_NAME		"WindowClass"				// ウィンドウクラスの名前
#define WINDOW_NAME		"ウィンドウ表示処理"		// キャプションに表示される名前
#define SCREEN_WIDTH		(1280)					// ウィンドウの幅
#define SCREEN_HEIGHT		(720)					// ウィンドウの高さ
#define MODE_ON										// モード切り替え
#define CREATE_DATE		"2026/02/..."				// 制作日
#define CREATERS_NAME	"TENMA SAITO\n..."			// 制作者名
#define	MODEL_TXT		"data\\Scripts\\model.txt"	// デフォルトの外部ファイル

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
bool InitWindow(HINSTANCE hInstance, RECT rect, LPWNDCLASSEX lpWcex, HWND *phWnd);
void UninitWindow(LPWNDCLASSEX lpWcex);
HWND InitDlgBox(HINSTANCE hInstance, int nIDD, HWND hWnd, DLGPROC dlgProc);
void UninitDlgBox(HWND hDlgWnd);
void UpdateDlgBoxPos(HWND hWnd, HWND hDlgWnd);
void MessageLoop(LPMSG lpMsg);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);
void ToggleFullscreen(HWND hWnd);
CREATE_LOOPID(loopThread);
void SetDeviceReset(void);

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
LPDIRECT3D9				g_pD3D = NULL;					// Direct3Dオブジェクトへのポインタ
LPDIRECT3DDEVICE9		g_pD3DDevice = NULL;			// Direct3Dデバイスへのポインタ
HWND g_hWnd = NULL;										// 獲得したウィンドウハンドル
int g_nCountFPS = 0;									// FPSカウンタ
bool g_isFullscreen = false;							// フルスクリーンの使用状況
RECT g_windowRect;										// ウィンドウサイズ
bool g_bMainThread;										// メインスレッドの実行状況
MultiData g_mdDirect3DDevice;							// スレッドセーフ

// デバイスリセット関連
bool g_bDeviceLost;										// デバイスの状態
D3DPRESENT_PARAMETERS g_resetParam;						// デバイスリセット用

// オートセーブ関連
char g_aLastSaveTime[MAX_PATH];							// 最終保存時間

//================================================================================================================
// --- メイン関数 ---
//================================================================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);			// メモリリーク検知用のフラグ
#endif // _DEBUG
	HWND hWnd = NULL;							// ウィンドウハンドル
	MSG msg = {};								// メッセージを格納する変数
	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	WNDCLASSEX wcex = {};

	// ウィンドウ作成
	if (!InitWindow(hInstance, rect, &wcex, &hWnd))
	{ // ウィンドウ作成失敗
		UninitWindow(&wcex);

		return -1;
	}

	g_hWnd = hWnd;
	g_bMainThread = true;

	// 初期化処理
	if (FAILED(Init(hInstance, hWnd, TRUE)))
	{// 初期化処理が失敗した場合
		MessageBox(hWnd, "Initに失敗しました。", "Error (0)", MB_ICONERROR);
		return -1;
	}

	// ウィンドウの表示
	ShowWindow(hWnd, nCmdShow);					// ウィンドウの表示状態を設定
	UpdateWindow(hWnd);							// クライアント領域を更新

	SetNewSaveTime();							// 保存時間を更新

	// メッセージループ
	MessageLoop(&msg);
	
	// DirectXの終了
	Uninit();

	// ウィンドウの削除
	UninitWindow(&wcex);

	// マルチスレッドのクリティカルセクションの終了
	g_mdDirect3DDevice.~MultiData();

	return (int)msg.wParam;
}

//================================================================================================================
// --- メッセージループ ---
//================================================================================================================
void MessageLoop(LPMSG lpMsg)
{
	DWORD dwCurrentTime;						// 現在時刻
	DWORD dwExecLastTime;						// 最後に処理した時刻
	DWORD dwCurrentTimeAutoSave;				// 現在時刻
	DWORD dwExecLastTimeAutoSave;				// 最後に処理した時刻
	DWORD dwFrameCount;							// フレームカウント
	DWORD dwFPSLastTime;						// 最後にFPSを計測した時刻

	//分解能を設定
	timeBeginPeriod(1);

	dwCurrentTime = 0;							// 初期化
	dwExecLastTime = timeGetTime();				// 現在時刻を取得
	dwFrameCount = 0;
	dwFPSLastTime = timeGetTime();
	dwCurrentTimeAutoSave = 0;							// 初期化
	dwExecLastTimeAutoSave = timeGetTime();				// 現在時刻を取得

	// メッセージループ
	while (lpMsg != NULL)
	{
		if (PeekMessage(lpMsg, NULL, 0, 0, PM_REMOVE) != 0)
		{// windowsの処理
			if (lpMsg->message == WM_QUIT)
			{// WM_QUITメッセージを受けると、メッセージループを抜ける
				break;
			}
			else
			{
				// メッセージの設定
				TranslateMessage(lpMsg);				// 仮想キーメッセージを文字メッセージへ変換

				DispatchMessage(lpMsg);					// ウィンドウプロシージャへメッセージを送出
			}
		}
		else
		{// DirectXの処理
			dwCurrentTime = timeGetTime();				// 現在時刻を取得
			dwCurrentTimeAutoSave = timeGetTime();				// 現在時刻を取得

			if ((dwCurrentTime - dwFPSLastTime) >= 500)
			{// 0.5秒経過
				// FPSを計測
				g_nCountFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);

				dwFPSLastTime = dwCurrentTime;			// FPSを計測した時刻を取得
				dwFrameCount = 0;						// フレームカウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{//60分の1秒経過

				dwExecLastTime = dwCurrentTime;			//処理開始時刻[現在時刻]を保存

				// 更新処理
				Update();

				// 描画処理
				Draw();

				dwFrameCount++;							// フレームカウントを加算
			}

			if ((dwCurrentTimeAutoSave - dwExecLastTimeAutoSave) >= 300000)
			{//5分経過
				dwExecLastTimeAutoSave = dwCurrentTimeAutoSave;

				SaveEditFile();

				SetNewSaveTime();
			}
		}
	}

	g_bMainThread = false;
}

//================================================================================================================
// ウィンドウプロシージャ
//================================================================================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nID;

	//ファイルオープン関係
	OPENFILENAME oFileName = {};
	TCHAR szFileName[MAX_PATH] = TEXT("");
	TCHAR tszFilePath[MAX_PATH] = { 0 };		// ファイルパスを格納するTCHAR型配列(長さ260)も0で初期化.
	TCHAR szFileTitle[MAX_PATH] = TEXT("");
	TCHAR szBuffer[MAX_PATH] = TEXT("");
	HANDLE hFile = NULL;						// ファイルハンドルhFileをNULLに初期化.
	DWORD err;

	switch (uMsg)
	{
	case WM_DESTROY:		// ウィンドウ破棄のメッセージ
		// WM_QUITメッセージを送る
		PostQuitMessage(0);
		break;

		// キー押下のメッセージ
	case WM_KEYDOWN:
		// 押されたキーの判定
		switch (wParam)
		{
		case VK_ESCAPE:
			// 終了確認
			nID = MessageBox(hWnd, "終了しますか？", "終了確認メッセージ", (MB_YESNO | MB_ICONINFORMATION));
			if (nID == IDYES)
			{// もしYESだった場合
				// ウィンドウを破棄する(WM_DESTROYメッセージを送る)
				DestroyWindow(hWnd);
			}

			break;

		case VK_F11:
			// F11でフルスクリーン
			ToggleFullscreen(hWnd);
			break;
		}
		break;

	case WM_COMMAND: //コントロールの操作
		switch (LOWORD(wParam))
		{
		case ID_CREATEFILE:		// 新規作成

			break;

		case ID_NAMEDSAVE:		// 名前を付けて保存

			break;

		case ID_UPDATE:			// 上書き保存

			SaveEditFile();

			break;

		case ID_OPEN:			// 開く

			//開く(O)
			ZeroMemory(&oFileName, sizeof(OPENFILENAME));								// 初期化
			oFileName.lStructSize = sizeof(OPENFILENAME);								// サイズ設定
			oFileName.hwndOwner = hWnd;													// ウィンドウハンドル
			oFileName.lpstrFilter = TEXT("テキストファイル (*.txt)\0*.txt;\0\0");		// 開くファイルの種類
			oFileName.nFilterIndex = 1;													// ファイルフィルター
			oFileName.lpstrFile = szFileName;											// 取得したファイルパス
			oFileName.nMaxFile = MAX_PATH;												// 上記の文字列のデータの最大数
			oFileName.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;	// フラグ(開く際の動作)
			oFileName.lpstrDefExt = TEXT("");											// 拡張子の自動追加
			oFileName.nMaxFileTitle = 64;												// ファイル名の最大データ数
			oFileName.lpstrFileTitle = szFileTitle;										// ファイル名
			oFileName.lpstrTitle = NULL;												// ダイアログボックスの名前

			//［ファイルを開く］ダイアログボックスを作成
			if (!GetOpenFileName(&oFileName))
			{
				MyMathUtil::GenerateMessageBox(MB_ICONINFORMATION,
					"Error!",
					"アドレス取得に失敗しました...");
				
				break;
			}

			err = CommDlgExtendedError();

			SetModelFileName(oFileName.lpstrFile);
			SetMode(MODE_EDIT);

			break;

		case ID_VERSION:		// バージョン情報

			MyMathUtil::GenerateMessageBox(MB_ICONINFORMATION,
				"バージョン情報！",
				"アプリ名 : %s\n制作者 : %s\n制作日 : %s",
				WINDOW_NAME,
				CREATERS_NAME,
				CREATE_DATE);

			break;
		}

		break;

	case WM_CLOSE:		// 閉じるボタン押下のメッセージ

		// ウィンドウを破棄する(WM_DESTROYメッセージを送る)
		DestroyWindow(hWnd);

		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//================================================================================================================
// ウィンドウ作成
//================================================================================================================
bool InitWindow(HINSTANCE hInstance, RECT rect, LPWNDCLASSEX lpWcex, HWND* phWnd)
{
	HWND hWnd = NULL;

	*lpWcex =
	{
		sizeof(WNDCLASSEX),						// ウィンドウクラスのメモリサイズ
		CS_CLASSDC,								// ウィンドウのスタイル
		WindowProc,								// ウィンドウプロシージャ
		0,										// 0
		0,										// 0
		hInstance,								// インスタンスハンドル
		LoadIcon(NULL,IDI_APPLICATION),			// タスクバーのアイコン
		LoadCursor(NULL,IDC_ARROW),				// マウスカーソル
		(HBRUSH)(COLOR_WINDOW + 1),				// クライアント領域の背景色
		MAKEINTRESOURCE(IDR_MENU1),				// メニューバー
		CLASS_NAME,								// ウィンドウクラスの名前
		NULL									// ファイルのアイコン
	};

	// ウィンドウクラスの登録
	RegisterClassEx(lpWcex);

	// クライアント領域を指定のサイズに調整
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// ウィンドウの生成
	hWnd = CreateWindowEx(
		0,										// 拡張ウィンドウスタイル
		CLASS_NAME,								// ウィンドウクラスの名前
		WINDOW_NAME,							// ウィンドウの名前
		WS_OVERLAPPEDWINDOW,					// ウィンドウスタイル
		CW_USEDEFAULT,							// ウィンドウの左上X座標
		CW_USEDEFAULT,							// ウィンドウの左上Y座標
		(rect.right - rect.left),				// ウィンドウの幅
		(rect.bottom - rect.top),				// ウィンドウの高さ
		NULL,									// 親ウィンドウのハンドル
		NULL,									// メニュー(もしくは子ウィンドウ)ハンドル
		hInstance,								// インスタンスハンドル
		NULL);									// ウィンドウ作成データ

	if (hWnd == NULL)
	{
		MessageBox(NULL, "ウィンドウハンドルが確保されていません!", "警告！", MB_ICONWARNING);
		return false;
	}

	if (phWnd != NULL) *phWnd = hWnd;

	return true;
}

//================================================================================================================
// --- エディットボックス作成 ---
//================================================================================================================
HWND InitDlgBox(HINSTANCE hInstance, int nIDD, HWND hWnd, DLGPROC dlgProc)
{
	HWND hDlgWnd = NULL;

	hDlgWnd = CreateDialog(hInstance, MAKEINTRESOURCE(nIDD), hWnd, dlgProc);

	UpdateDlgBoxPos(hWnd, hDlgWnd);

	return hDlgWnd;
}

//================================================================================================================
// --- エディットボックス削除 ---
//================================================================================================================
void UninitDlgBox(HWND hDlgWnd)
{
	ShowWindow(hDlgWnd, SW_HIDE);

	DestroyWindow(hDlgWnd);
}

//================================================================================================================
// --- エディットボックスの位置更新 ---
//================================================================================================================
void UpdateDlgBoxPos(HWND hWnd, HWND hDlgWnd)
{
	WINDOWINFO wInfo = {};
	wInfo.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(hWnd, &wInfo);

	MoveWindow(hDlgWnd,
		wInfo.rcWindow.left + 10,
		wInfo.rcWindow.bottom - 50,
		300,
		40,
		true);
}

//================================================================================================================
// --- ウィンドウ削除 ---
//================================================================================================================
void UninitWindow(LPWNDCLASSEX lpWcex)
{
	// ウィンドウクラスの登録解除
	UnregisterClass(CLASS_NAME, lpWcex->hInstance);
}

//================================================================================================================
// --- 初期化処理 ---
//================================================================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;			// ディスプレイモード
	D3DPRESENT_PARAMETERS d3dpp;	// プレゼンテーションパラメータ		

	// Direct3Dオブジェクトの作成
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL)
	{
		MessageBox(hWnd, "Direct3Dオブジェクトの作成に失敗しました。", "Error (-1)", MB_ICONERROR);
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,
		&d3ddm)))
	{
		MessageBox(hWnd, "ディスプレイモードの取得に失敗しました。", "Error (-1)", MB_ICONERROR);
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));			// パラメータのゼロクリア

	d3dpp.BackBufferWidth = SCREEN_WIDTH;		// ゲームの画面サイズ(横)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;		// ゲームの画面サイズ(高さ)
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;	// バックバッファの形式
	d3dpp.BackBufferCount = 1;					// バックバッファの数
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// ダブルバッファの切り替え(映像信号と同期)
	d3dpp.EnableAutoDepthStencil = TRUE;		// デプスバッファとステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;	// デプスバッファとして16bitを使う
	d3dpp.Windowed = bWindow;					// ウィンドウモード
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;			// リフレッシュレート
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;			// インターバル

	// Direct3Dデバイスの作成
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		&d3dpp,
		&g_pD3DDevice)))
	{
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
			&d3dpp,
			&g_pD3DDevice)))
		{
			if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
				&d3dpp,
				&g_pD3DDevice)))
			{
				MessageBox(hWnd, "Direct3Dデバイスの作成に失敗しました。", "Error (-1)", MB_ICONERROR);
				return E_FAIL;
			}
		}
	}

	// リセット時のコピー
	g_resetParam = d3dpp;

	// レンダーステートの設定(消さないこと！ALPHA値の設定を適用する為の設定！)
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// サンプラーステートの設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);		// テクスチャの拡縮補間の設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);		// テクスチャの繰り返しの設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// テクスチャステージステートの設定(テクスチャのアルファブレンドの設定[テクスチャとポリゴンのALPHA値を混ぜる！])
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	// キーボードの初期化処理
	if (FAILED(InitKeyboard(hInstance, hWnd)))
	{
		MessageBox(hWnd, "InitKeyboardに失敗しました。", "Error (0)", MB_ICONERROR);
		return E_FAIL;
	}

	// ジョイパッドの初期化処理
	if (FAILED(InitJoypad()))
	{
		MessageBox(hWnd, "InitJoypadに失敗しました。", "Error (1)", MB_ICONERROR);
		return E_FAIL;
	}

	// マウスの初期化処理
	if (FAILED(InitMouse(hWnd)))
	{
		MessageBox(hWnd, "InitMouseに失敗しました。", "Error (2)", MB_ICONERROR);
		return E_FAIL;
	}

	// デバッグフォント作成
	InitDebugProc();

#ifdef MODE_ON

	SetModelFileName(MODEL_TXT);
	SetMode(MODE_EDIT);

#endif // MODE_ON

	return S_OK;
}

//================================================================================================================
// --- 終了処理 ---
//================================================================================================================
void Uninit(void)
{
#ifdef MODE_ON
	// モードの解放
	UninitMode();
#endif
	// キーボードの終了処理
	UninitKeyboard();

	// ジョイパッドの終了処理
	UninitJoypad();

	// マウスの終了処理
	UninitMouse();

	// デバッグフォント削除
	UninitDebugProc();

	// Direct3Dデバイスの破棄
	RELEASE(g_pD3DDevice);

	// Direct3Dオブジェクトの破棄
	RELEASE(g_pD3D);
}

//================================================================================================================
// --- 更新処理 ---
//================================================================================================================
void Update(void)
{
	CHAR aStr[MAX_PATH] = {};
	static int nCount = 0;
	static int nID[3] = {};

	PrintDebugProc("FPS : %d\n", g_nCountFPS);

	PrintDebugProc("最終保存時刻 : [%s]\n", g_aLastSaveTime);

	//キーボードの更新処理(これより上に書くな)
	UpdateKeyboard();

	//ジョイパッドの更新処理
	UpdateJoypad();

	// マウスの更新処理
	UpdateMouse();

	// デバッグ表示更新
	UpdateDebugProc();

	/*** 現在の画面(モード)の更新処理 ***/
	UpdateMode();
}

//================================================================================================================
// --- 描画処理 ---
//================================================================================================================
void Draw(void)
{
	HRESULT hr = S_OK;
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	if (g_bDeviceLost != true)
	{
		// 画面クリア(バックバッファとZバッファのクリア)
		pDevice->Clear(0, NULL,
			(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
			D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

		// 描画開始
		if (SUCCEEDED(pDevice->BeginScene()))
		{// 描画開始が成功した場合
#ifdef MODE_ON
			DrawMode();
#endif
			// デバック表示描画
			DrawDebugProc();

			// 描画終了
			g_pD3DDevice->EndScene();
		}

		// バックバッファとフロントバッファの入れ替え
		hr = g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
		EndDevice();
	}
	else
	{
		SetDeviceReset();
	}

	if (hr == D3DERR_DEVICELOST)
	{
		int nYN = MyMathUtil::GenerateMessageBox((MB_ICONERROR | MB_YESNO),
			"Error!",
			"デバイスロストが発生しました！\nデバイスを復旧しますか？");

		if (nYN == IDOK)
		{
			g_bDeviceLost = true;
		}
		else if (nYN == IDCANCEL)
		{
			PostQuitMessage(0);
		}
	}
}

//================================================================================================================
// --- デバイスのリセット ---
//================================================================================================================
void SetDeviceReset(void)
{
	HRESULT hr = g_pD3DDevice->TestCooperativeLevel();
	if (hr == D3DERR_DEVICELOST)
	{
		return;
	}
	else if (hr == D3DERR_DEVICENOTRESET)
	{
		ResetMode(true);
		ResetDebugProc(true);

		hr = g_pD3DDevice->Reset(&g_resetParam);
		if (hr == D3D_OK)
		{ // デバイス復旧時
			ResetDebugProc(false);
			ResetMode(false);
			g_bDeviceLost = false;

			int nID = 0;

			nID = MyMathUtil::GenerateMessageBox(MB_ICONINFORMATION | MB_YESNO,
				"Success!",
				"≧∀≦) < デバイスの復旧に成功したよ！\n褒めてあげてね！");

			if (nID == IDNO)
			{
				int nRand = rand() % 10;
				if (nRand < 9)
				{
					MyMathUtil::GenerateMessageBox(MB_ICONINFORMATION,
						"しょぼーん",
						"(´・ω・`)");
				}
				else
				{
					MyMathUtil::GenerateMessageBox(MB_ICONINFORMATION,
						"......",
						"( *｀ω´)");
				}
			}
		}
		else
		{ // デバイス復旧失敗時
			MyMathUtil::GenerateMessageBox(MB_ICONERROR,
				"...Sorry.",
				"デバイスの復旧に失敗しました...\nごめんね...(下のはお詫び猫)\n\n  A_A___\n_(U U|||)_");

			int nID = 0;
			nID = MyMathUtil::GenerateMessageBox(MB_ICONINFORMATION | MB_YESNO,
				"Helper",
				"直前の配置情報を保存できます。保存しますか？");

			if (nID == IDYES)
			{ // 書き出し
				SaveEditFile();
			}

			PostQuitMessage(0);
		}
	}
}

//================================================================================================================
// --- デバイスの取得 ---
//================================================================================================================
LPDIRECT3DDEVICE9 GetDevice(void)
{
	g_mdDirect3DDevice.LockMultiData();
	return g_pD3DDevice;
}

//================================================================================================================
// --- デバイスの取得終了 ---
//================================================================================================================
void EndDevice(void)
{
	g_mdDirect3DDevice.UnlockMultiData();
}

//================================================================================================================
// --- メインスレッドの状態の取得 ---
//================================================================================================================
bool GetIsMainThread(void)
{
	return g_bMainThread;
}

//================================================================================================================
// -- 獲得済みウィンドウハンドルの取得 ---
//================================================================================================================
HWND GetHandleWindow(void)
{
	if (g_hWnd != NULL)
	{
		return g_hWnd;
	}
	else
	{
		MessageBox(g_hWnd, "ウィンドウハンドルが確保されていません!", "警告！", MB_ICONWARNING);
		return NULL;
	}
}

//===============================================================================================================
// --- ウィンドウフルスクリーン処理 ---
//===============================================================================================================
void ToggleFullscreen(HWND hWnd)
{
	// 現在のウィンドウスタイルを取得
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	if (g_isFullscreen)
	{
		// ウィンドウモードに切り替え
		SetWindowLong(hWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, g_windowRect.left, g_windowRect.top,
			g_windowRect.right - g_windowRect.left, g_windowRect.bottom - g_windowRect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_NORMAL);
	}
	else
	{
		// フルスクリーンモードに切り替え
		GetWindowRect(hWnd, &g_windowRect);
		SetWindowLong(hWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_MAXIMIZE);
	}

	g_isFullscreen = !g_isFullscreen;
}

CREATE_LOOPID(loopThread)
{

}

//===============================================================================================================
// --- 更新時間保存関数 ---
//===============================================================================================================
void SetNewSaveTime(void)
{
	time_t time1;		// 時刻取得
	tm* tmp = nullptr;	// ポインタ

	/* 日付情報の取得 */
	time(&time1);
	tmp = localtime(&time1);

	ZeroMemory(g_aLastSaveTime, sizeof(g_aLastSaveTime));

	/* YYYY/MM/DD形式で印字 */
	strftime(g_aLastSaveTime, sizeof(g_aLastSaveTime), "%m-%d %H:%M:%S", tmp);
}