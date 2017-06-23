#include <sstream>
#include <windows.h>

/***********************************************************************************
	コンソールリダイレクト
***********************************************************************************/
#define MAX_CONSOLE_BUF		0xFFFFF		//パイプバッファサイズ
#define READ_SIZE			0xFFFF		//読み込みバッファサイズ
void Conexec(std::stringstream &stream, char *filename, char *CurrentDir)
{
	//名前なしパイプの作成
	SECURITY_ATTRIBUTES sa;		// セキュリティ属性
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;		//デフォ
		sa.bInheritHandle = TRUE;			//ハンドル継承
	HANDLE hRead;		// 読み取り側のハンドル
	HANDLE hWrite;		// 書き込み側のハンドル
	CreatePipe(&hRead, &hWrite, &sa, MAX_CONSOLE_BUF);

	//子プロセスを作成
	PROCESS_INFORMATION ProcInfo;
	STARTUPINFO StartInfo = {0};
		StartInfo.cb = sizeof(STARTUPINFO);
		StartInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		StartInfo.hStdOutput = hWrite;
	CreateProcessA(	NULL,			//PCTSTR pszApplicationName,       // 実行ファイル名					NULL	NULLの時↓の最初のトークンが実行ファイル名
					filename,		//PTSTR  pszCommandLine,           // コマンドラインパラメータ			filename
					NULL,			//PSECURITY_ATTRIBUTES psaProcess, // プロセスの保護属性				NULL
					NULL,			//PSECURITY_ATTRIBUTES psaThread,  // スレッドの保護属性				NULL
					TRUE,			//BOOL   bInheritHandles,          // オブジェクトハンドル継承のフラグ	TRUE	ハンドル継承
					NULL,			//DWORD  fdwCreate,                // 属性フラグ						NULL
					NULL,			//PVOID  pvEnvironment,            // 環境変数情報へのポインタ			NULL
					CurrentDir,		//PCTSTR pszCurDir,                // 起動時カレントディレクトリ		NULL
					&StartInfo,	//LPSTARTUPINFO  psiStartInfo,     // ウィンドウ表示設定				STARTUPINFO 構造体のアドレス
					&ProcInfo	//PPROCESS_INFORMATION ppiProcInfo // プロセス・スレッドの情報			PROCESS_INFORMATION 構造体のアドレス
				);

	//コンソールアプリのプロセスが存在している間待つ
	WaitForSingleObject(ProcInfo.hProcess, INFINITE);	//オブジェクトがシグナル状態になるまで待機し続ける
	CloseHandle(hWrite);

	//標準出力から読み取り
	char *rcvbuf = new char[READ_SIZE +1];
	DWORD dwRead;
	for(;;)
	{
		ReadFile(hRead, rcvbuf, READ_SIZE, &dwRead, NULL);
		rcvbuf[dwRead] = '\0';
		if(dwRead < READ_SIZE)
		{
			if(dwRead != 0) stream << rcvbuf;
			break;
		}
		stream << rcvbuf;
	}

	CloseHandle(hRead);
	delete []rcvbuf;

	return;
}
