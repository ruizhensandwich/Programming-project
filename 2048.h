#ifndef _2048_H
#define _2048_H

#define SIZE 4

// 보드 / 게임 상태 관련
void begin_board();          // 보드 초기화 + 시작 타일 2개 생성
void play_board();           // 보드 출력 (컬러 포함)
void random_tile();          // 랜덤 타일 생성

// 이동 / 병합 관련
void shift_left(int line[SIZE]);   // 한 줄 압축
void merge_tiles(int line[SIZE]);  // 한 줄 병합 (점수 반영)
void move_left();                  // 전체 왼쪽 이동
void flip_axis();                  // 전치 (행<->열)
void flip_row();                   // 행 반전
void move(char dir);               // 방향에 따라 이동 (w/a/s/d)

// 게임 상태 판정
int is_dead();          // 더 이상 이동/병합 불가 여부 (1이면 게임오버)
int has_won();          // 2048 타일 생성 여부 (1이면 승리)
int get_score();        // 현재 점수

// 실행 취소 / 저장 / 불러오기
void undo();                          // 직전 상태로 되돌리기
int save_game(const char* filename);  // 현재 상태 저장
int load_game(const char* filename);  // 저장된 상태 불러오기

#endif