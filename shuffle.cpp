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

	//初期化・表示
	for(i=0; i<ALL; i++)
	{
		array[i] = (double)(rand() % 1000) / 10.0;
		printf("%-3d : %g\n", i, array[i]);
	}

	//シャッフル
	Suffle(array, Start, End);

	//シャッフル後を表示
	printf("----------------------------------------------\n");
	for(i=0; i<ALL; i++)
		printf("%-3d : %g\n", i, array[i]);

 	return 0;
}

/******************************************************************************
配列をシャッフル
	TEMP *array		：シャッフル対象の配列
	int Start		：シャッフルの開始インデックス
	int End			：シャッフルの終了インデックス
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
