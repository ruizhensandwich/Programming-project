#include <stdio.h>
#include <stdlib.h>
#include "2048.h"

static int board[SIZE][SIZE];
static int prev_board[SIZE][SIZE];  // undo용
static int score = 0;
static int prev_score = 0;
static int moved = 0;
static int won = 0;

// ===== ANSI 컬러 코드 =====
#define ANSI_RESET      "\x1b[0m"
#define ANSI_BOLD       "\x1b[1m"
#define ANSI_DIM        "\x1b[2m"
#define ANSI_FG_BLACK   "\x1b[30m"
#define ANSI_FG_RED     "\x1b[31m"
#define ANSI_FG_GREEN   "\x1b[32m"
#define ANSI_FG_YELLOW  "\x1b[33m"
#define ANSI_FG_BLUE    "\x1b[34m"
#define ANSI_FG_MAGENTA "\x1b[35m"
#define ANSI_FG_CYAN    "\x1b[36m"
#define ANSI_FG_WHITE   "\x1b[37m"

// 값에 따라 색상 선택 (네가 지정한 팔레트 그대로 반영)
static const char* color_for_value(int v) {
    switch (v) {
    case 2:    // 파랑
        return ANSI_FG_BLUE;
    case 4:    // 빨강
        return ANSI_FG_RED;
    case 8:    // 노랑
        return ANSI_FG_YELLOW;
    case 16:   // 초록
        return ANSI_FG_GREEN;
    case 32:   // 핑크 = 마젠타
        return ANSI_FG_MAGENTA;
    case 64:   // 카키 느낌 = 조금 죽인 노랑
        return ANSI_DIM ANSI_FG_YELLOW;
    case 128:  // 연어색 느낌 = 조금 죽인 빨강
        return ANSI_DIM ANSI_FG_RED;
    case 256:  // 하늘색 = 시안
        return ANSI_FG_CYAN;
    case 512:  // 보라 = 진한 마젠타
        return ANSI_BOLD ANSI_FG_MAGENTA;
    case 1024: // 연한 노랑 = 밝은 노랑
        return ANSI_BOLD ANSI_FG_YELLOW;
    case 2048: // 연한 보라 = 톤 낮춘 보라 느낌
        return ANSI_DIM ANSI_FG_MAGENTA;
    default:   // 그 이상(4096 등)은 흰색 볼드
        return ANSI_BOLD ANSI_FG_WHITE;
    }
}

// 최대 타일 (승리 체크용)
static int max_tile() {
    int m = 0;
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            if (board[r][c] > m) m = board[r][c];
    return m;
}

// ===== 출력 =====
void play_board() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    int m = max_tile();
    printf("Score: %d   Max: %d\n", score, m);
    if (won) {
        printf(">>> You made 2048! (Win state reached)\n");
    }

    printf("+------+------+------+------+ \n");
    for (int r = 0; r < SIZE; r++) {
        printf("|");
        for (int c = 0; c < SIZE; c++) {
            if (board[r][c] == 0) {
                printf("      |");
            }
            else {
                const char* col = color_for_value(board[r][c]);
                printf(" %s%4d%s |", col, board[r][c], ANSI_RESET);
            }
        }
        printf("\n+------+------+------+------+ \n");
    }
    printf("W/A/S/D or 방향키: move, U: undo, P: save, L: load, Q: quit\n");
}

// ===== 랜덤 타일 생성 =====
void random_tile() {
    int empty[SIZE * SIZE][2];
    int count = 0;

    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (board[r][c] == 0) {
                empty[count][0] = r;
                empty[count][1] = c;
                count++;
            }
        }
    }

    if (count == 0) return;

    int k = rand() % count;
    int val = (rand() % 10 == 0) ? 4 : 2;
    board[empty[k][0]][empty[k][1]] = val;

    if (val == 2048) won = 1;
}

// ===== 보드 초기화 =====
void begin_board() {
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++) {
            board[r][c] = 0;
            prev_board[r][c] = 0;
        }
    score = 0;
    prev_score = 0;
    moved = 0;
    won = 0;

    random_tile();
    random_tile();
}

// ===== 한 줄 압축 =====
void shift_left(int line[SIZE]) {
    int temp[SIZE] = { 0 };
    int idx = 0;
    for (int i = 0; i < SIZE; i++) {
        if (line[i] != 0) {
            temp[idx++] = line[i];
        }
    }
    for (int i = 0; i < SIZE; i++) {
        line[i] = temp[i];
    }
}

// ===== 한 줄 병합 =====
void merge_tiles(int line[SIZE]) {
    shift_left(line);
    for (int i = 0; i < SIZE - 1; i++) {
        if (line[i] != 0 && line[i] == line[i + 1]) {
            line[i] *= 2;
            score += line[i];
            if (line[i] == 2048) won = 1;
            line[i + 1] = 0;
            moved = 1;
            i++;  // 한 번 합쳐진 타일은 건너뛰기
        }
    }
    shift_left(line);
}

// ===== 전체 왼쪽 이동 =====
void move_left() {
    moved = 0;
    for (int r = 0; r < SIZE; r++) {
        int before[SIZE];
        for (int c = 0; c < SIZE; c++) {
            before[c] = board[r][c];
        }

        merge_tiles(before);

        for (int c = 0; c < SIZE; c++) {
            if (board[r][c] != before[c]) moved = 1;
            board[r][c] = before[c];
        }
    }
}

// ===== 행 반전 =====
void flip_row() {
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE / 2; c++) {
            int t = board[r][c];
            board[r][c] = board[r][SIZE - 1 - c];
            board[r][SIZE - 1 - c] = t;
        }
    }
}

// ===== 전치 =====
void flip_axis() {
    for (int r = 0; r < SIZE; r++) {
        for (int c = r + 1; c < SIZE; c++) {
            int t = board[r][c];
            board[r][c] = board[c][r];
            board[c][r] = t;
        }
    }
}

// ===== 현재 상태를 prev_*에 백업 (Undo용) =====
static void backup_state() {
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            prev_board[r][c] = board[r][c];
    prev_score = score;
}

// ===== 실행 취소 =====
void undo() {
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            board[r][c] = prev_board[r][c];
    score = prev_score;
    won = (max_tile() >= 2048) ? 1 : 0;
}

// ===== 방향 이동 =====
void move(char dir) {
    backup_state(); // 이동 전에 상태 백업

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

    if (moved) {
        random_tile();
    }
}

// ===== 게임오버 판단 =====
int is_dead() {
    // 빈 칸이 있으면 게임오버 아님
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            if (board[r][c] == 0)
                return 0;

    // 인접한 같은 타일이 있으면 게임오버 아님
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (r + 1 < SIZE && board[r][c] == board[r + 1][c]) return 0;
            if (c + 1 < SIZE && board[r][c] == board[r][c + 1]) return 0;
        }
    }
    return 1; // 빈 칸도 없고, 합칠 것도 없음
}

// ===== 승리 여부 =====
int has_won() {
    return won;
}

// ===== 점수 반환 =====
int get_score() {
    return score;
}

// ===== 저장 =====
int save_game(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) return 0;

    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            fprintf(fp, "%d ", board[r][c]);
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "%d\n", score);
    fprintf(fp, "%d\n", won);

    fclose(fp);
    return 1;
}

// ===== 불러오기 =====
int load_game(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return 0;

    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (fscanf(fp, "%d", &board[r][c]) != 1) {
                fclose(fp);
                return 0;
            }
        }
    }
    if (fscanf(fp, "%d", &score) != 1) {
        fclose(fp);
        return 0;
    }
    if (fscanf(fp, "%d", &won) != 1) {
        fclose(fp);
        return 0;
    }

    fclose(fp);
    backup_state();  // 불러온 상태를 기준으로 undo 동기화
    return 1;
}