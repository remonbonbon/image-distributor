#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ALL		16

template <class TEMP> void Suffle(TEMP *array, int Start, int End);

int main(void)
{
	int i;
	double array[ALL];
	int Start = 0, End = ALL-1;

	srand(time(NULL));

	//�������E�\��
	for(i=0; i<ALL; i++)
	{
		array[i] = (double)(rand() % 1000) / 10.0;
		printf("%-3d : %g\n", i, array[i]);
	}

	//�V���b�t��
	Suffle(array, Start, End);

	//�V���b�t�����\��
	printf("----------------------------------------------\n");
	for(i=0; i<ALL; i++)
		printf("%-3d : %g\n", i, array[i]);

 	return 0;
}

/******************************************************************************
�z����V���b�t��
	TEMP *array		�F�V���b�t���Ώۂ̔z��
	int Start		�F�V���b�t���̊J�n�C���f�b�N�X
	int End			�F�V���b�t���̏I���C���f�b�N�X
******************************************************************************/
template <class TEMP> void Suffle(TEMP *array, int Start, int End)
{
	int i, index;
	TEMP temp;
	for(i=Start; i<=End; i++)
	{
		index = rand() % (i - Start + 1) + Start;
		temp = array[i];
		array[i] = array[index];
		array[index] = temp;
	}
	return;
}
