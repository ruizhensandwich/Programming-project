#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "2048.h"

int main() {
	srand(time(NULL));
	begin_board(); //보드 초기화 및 랜덤타일 2개 생성
	play_board(); 

	while (1) {
		char c = getchar();
		if (c == 'q') break; //quit 누르면 종료. 
		if (c == 'w' || c == 'a' || c == 's' || c == 'd') {
			move(c);
			play_board();
			if (is_dead()) {
				printf("Game Over! Final Score : %d\n", get_score());
				break;
			}
		}
	}
	return 0;
}