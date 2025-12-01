#ifndef _2048_H

#define _2048_H

#define SIZE 4

void begin_board(); //보드 초기화
void play_board(); //현재 진행중인 보드 출력
void random_tile(); //랜덤 타일생성

void shift_left(int line[SIZE]); //한 줄 압축(왼쪽으로)
void merge_tiles(int line[SIZE]); //한 줄 병합
void move_left(); //전체 왼쪽 이동

void flip_axis(); //전치
void flip_row();
void move(char dir);

int is_dead(); // 이동불가 판정
int get_score(); //현재 점수 반환


#endif