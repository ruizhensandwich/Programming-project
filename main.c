#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>   // getch()
#include "2048.h"

#define SAVE_FILE "save_2048.txt"

// 방향키 + WASD 공통 처리
char read_key() {
    int ch = getch();

    // q / WASD / U / P / L 그대로 반환
    if (ch == 'q' || ch == 'Q' ||
        ch == 'w' || ch == 'W' ||
        ch == 'a' || ch == 'A' ||
        ch == 's' || ch == 'S' ||
        ch == 'd' || ch == 'D' ||
        ch == 'u' || ch == 'U' ||
        ch == 'p' || ch == 'P' ||
        ch == 'l' || ch == 'L') {
        return (char)ch;
    }

    // 방향키: 0 또는 224 + 코드
    if (ch == 0 || ch == 224) {
        int ch2 = getch();
        switch (ch2) {
        case 72: return 'w'; // ↑
        case 80: return 's'; // ↓
        case 75: return 'a'; // ←
        case 77: return 'd'; // →
        default: return 0;   // 사용하지 않는 키
        }
    }

    return 0; // 인식 안 하는 키
}

int main() {
    srand((unsigned int)time(NULL));
    begin_board();
    play_board();

    while (1) {
        char c = read_key();
        if (c == 0) continue; // 의미 없는 키는 무시

        // 대문자 들어오면 소문자로 통일
        if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';

        if (c == 'q') {
            break;
        }
        else if (c == 'u') {
            undo();
            play_board();
        }
        else if (c == 'p') {
            if (save_game(SAVE_FILE)) {
                printf("Game saved to %s\n", SAVE_FILE);
            }
            else {
                printf("Save failed.\n");
            }
        }
        else if (c == 'l') {
            if (load_game(SAVE_FILE)) {
                play_board();
                printf("Game loaded from %s\n", SAVE_FILE);
            }
            else {
                printf("Load failed.\n");
            }
        }
        else if (c == 'w' || c == 'a' || c == 's' || c == 'd') {
            move(c);
            play_board();

            if (has_won()) {
                printf("You reached 2048! You win!\n");
                // 여기서 계속 할지, 바로 끝낼지 선택 가능
                break;
            }

            if (is_dead()) {
                printf("Game Over! Final Score: %d\n", get_score());
                break;
            }
        }
    }

    return 0;
}