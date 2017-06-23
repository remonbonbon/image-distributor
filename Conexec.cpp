#include <sstream>
#include <windows.h>

/***********************************************************************************
	�R���\�[�����_�C���N�g
***********************************************************************************/
#define MAX_CONSOLE_BUF		0xFFFFF		//�p�C�v�o�b�t�@�T�C�Y
#define READ_SIZE			0xFFFF		//�ǂݍ��݃o�b�t�@�T�C�Y
void Conexec(std::stringstream &stream, char *filename, char *CurrentDir)
{
	//���O�Ȃ��p�C�v�̍쐬
	SECURITY_ATTRIBUTES sa;		// �Z�L�����e�B����
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;		//�f�t�H
		sa.bInheritHandle = TRUE;			//�n���h���p��
	HANDLE hRead;		// �ǂݎ�葤�̃n���h��
	HANDLE hWrite;		// �������ݑ��̃n���h��
	CreatePipe(&hRead, &hWrite, &sa, MAX_CONSOLE_BUF);

	//�q�v���Z�X���쐬
	PROCESS_INFORMATION ProcInfo;
	STARTUPINFO StartInfo = {0};
		StartInfo.cb = sizeof(STARTUPINFO);
		StartInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		StartInfo.hStdOutput = hWrite;
	CreateProcessA(	NULL,			//PCTSTR pszApplicationName,       // ���s�t�@�C����					NULL	NULL�̎����̍ŏ��̃g�[�N�������s�t�@�C����
					filename,		//PTSTR  pszCommandLine,           // �R�}���h���C���p�����[�^			filename
					NULL,			//PSECURITY_ATTRIBUTES psaProcess, // �v���Z�X�̕ی쑮��				NULL
					NULL,			//PSECURITY_ATTRIBUTES psaThread,  // �X���b�h�̕ی쑮��				NULL
					TRUE,			//BOOL   bInheritHandles,          // �I�u�W�F�N�g�n���h���p���̃t���O	TRUE	�n���h���p��
					NULL,			//DWORD  fdwCreate,                // �����t���O						NULL
					NULL,			//PVOID  pvEnvironment,            // ���ϐ����ւ̃|�C���^			NULL
					CurrentDir,		//PCTSTR pszCurDir,                // �N�����J�����g�f�B���N�g��		NULL
					&StartInfo,	//LPSTARTUPINFO  psiStartInfo,     // �E�B���h�E�\���ݒ�				STARTUPINFO �\���̂̃A�h���X
					&ProcInfo	//PPROCESS_INFORMATION ppiProcInfo // �v���Z�X�E�X���b�h�̏��			PROCESS_INFORMATION �\���̂̃A�h���X
				);

	//�R���\�[���A�v���̃v���Z�X�����݂��Ă���ԑ҂�
	WaitForSingleObject(ProcInfo.hProcess, INFINITE);	//�I�u�W�F�N�g���V�O�i����ԂɂȂ�܂őҋ@��������
	CloseHandle(hWrite);

	//�W���o�͂���ǂݎ��
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
