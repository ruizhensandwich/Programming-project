#include <stdio.h>
#include <stdlib.h>
#include "2048.h"

static int board[SIZE][SIZE];
static int score=0;
static int moved=0;

void play_board() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    printf("Score : %d\n", score);
    printf("+------+------+------+------+ \n");
    for (int r = 0; r < SIZE; r++) {
        printf("|");
        for (int c = 0; c < SIZE; c++) {
            if (board[r][c] == 0)printf("      |");
            else printf(" %4d |", board[r][c]);
        }
        printf("\n+------+------+------+------+ \n");
    }
    printf("W/A/S/D to move, Q to quit\n");
}
void random_tile() {
    int empty[SIZE * SIZE][2];
    int count = 0;
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            if (board[r][c] == 0) {
                empty[count][0] = r;
                empty[count][1] = c;
                count++;
            }
    if (count == 0) return;

    int k = rand() % count;
    int val = (rand() % 10 == 0) ? 4 : 2;
    board[empty[k][0]][empty[k][1]] = val;
}

// ----- 보드 초기화 -----
void begin_board() {
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            board[r][c] = 0;
    score = 0;
    random_tile();
    random_tile();
}

// ----- 한 줄 압축 -----
void shift_left(int line[SIZE]) {
    int temp[SIZE] = { 0 }, idx = 0;
    for (int i = 0; i < SIZE; i++)
        if (line[i] != 0)
            temp[idx++] = line[i];
    for (int i = 0; i < SIZE; i++)
        line[i] = temp[i];
}

// ----- 한 줄 병합 -----
void merge_tiles(int line[SIZE]) {
    shift_left(line);
    for (int i = 0; i < SIZE - 1; i++) {
        if (line[i] != 0 && line[i] == line[i + 1]) {
            line[i] *= 2;
            score += line[i];
            line[i + 1] = 0;
            moved = 1;
            i++; // 한 번 합쳐진 타일은 건너뜀
        }
    }
    shift_left(line);
}

// ----- 전체 왼쪽 이동 -----
void move_left() {
    moved = 0;
    for (int r = 0; r < SIZE; r++) {
        int before[SIZE];
        for (int c = 0; c < SIZE; c++) before[c] = board[r][c];

        merge_tiles(before);

        for (int c = 0; c < SIZE; c++) {
            if (board[r][c] != before[c]) moved = 1;
            board[r][c] = before[c];
        }
    }
}

// ----- 행 반전 -----
void flip_row() {
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE / 2; c++) {
            int t = board[r][c];
            board[r][c] = board[r][SIZE - 1 - c];
            board[r][SIZE - 1 - c] = t;
        }
    }
}

// ----- 전치 -----
void flip_axis() {
    for (int r = 0; r < SIZE; r++) {
        for (int c = r + 1; c < SIZE; c++) {
            int t = board[r][c];
            board[r][c] = board[c][r];
            board[c][r] = t;
        }
    }
}

// ----- 방향 이동 -----
void move(char dir) {
    if (dir == 'a') {
        move_left();
    }
    else if (dir == 'd') {
        flip_row();
        move_left();
        flip_row();
    }
    else if (dir == 'w') {
        flip_axis();
        move_left();
        flip_axis();
    }
    else if (dir == 's') {
        flip_axis();
        flip_row();
        move_left();
        flip_row();
        flip_axis();
    }
    if (moved) random_tile();
}

// ----- 게임오버 판단 -----
int is_dead() {
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            if (board[r][c] == 0)
                return 0;

    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++) {
            if (r + 1 < SIZE && board[r][c] == board[r + 1][c]) return 0;
            if (c + 1 < SIZE && board[r][c] == board[r][c + 1]) return 0;
        }
    return 1;
}

// ----- 점수 반환 -----
int get_score() {
    return score;
}