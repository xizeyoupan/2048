#include <stdio.h>
#include <graphics.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ege/sys_edit.h>

#define DEFAULT_COLOR EGERGB(0xCD, 0xC1, 0xB4)
#define BACKGROUND_COLOR EGERGB(0xB7, 0xAD, 0xA0)
#define BLOCK_FONT_COLOR EGERGB(0x77, 0x6E, 0x65)

#define BOOL int
#define TRUE 1
#define FALSE 0

#define SPACING 5        //格子间距
#define LG_LENGTH 150   //大格子长度
#define SM_LENGTH 72    //小格子长度
#define FPS 144

BOOL moved = FALSE;
int score = 0;
int game_mode = 1;
int out_layer[4][4];
int inside_layer[4][4];
int cheat_count = 0;
int manage_count = 0;
int is_fresh = 0;
FILE *fp;

typedef struct Player {
    char username[100];
    int record;
} Player;

color_t block_colors[17] = {EGERGB(0XF3, 0XE9, 0XDF),
                            EGERGB(0XEE, 0XE4, 0XD9), EGERGB(0XEC, 0XDF, 0XC7), EGERGB(0XF2, 0XAD, 0X70),
                            EGERGB(0XEC, 0X87, 0X4B),
                            EGERGB(0XF6, 0X75, 0X56), EGERGB(0XEA, 0X52, 0X2F), EGERGB(0XF3, 0XD6, 0X60),
                            EGERGB(0XF1, 0XCE, 0X41),
                            EGERGB(0XE4, 0XBF, 0X23), EGERGB(0XE3, 0XB7, 0X0F), EGERGB(0XC5, 0X9E, 0X09),
                            EGERGB(0XFA, 0X5A, 0X6C),
                            EGERGB(0XF3, 0X42, 0X54), EGERGB(0XEB, 0X39, 0X36), EGERGB(0X69, 0XB1, 0XDD),
                            EGERGB(0X53, 0X9C, 0XE5)
                           };


int _random(int start, int end) {
    srand((int) time(NULL));
    return (rand() % (end - start)) + start;
}

void send_text(char a[]) {
    PIMAGE p = newimage(320, 60);
    setbkcolor(BACKGROUND_COLOR, p);
    setcolor(EGERGB(0x29, 0x29, 0x35), p);
    setfont(25, 0, "黑体", p);
    settextjustify(CENTER_TEXT, CENTER_TEXT, p);
    outtextxy(160, 30, a, p);
    putimage(LG_LENGTH + 5 * SPACING, LG_LENGTH * 4 + 10 * SPACING, p);
    delimage(p);
}

int get_random_empty_block() {
    int empty_block[28];
    int number = 0;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (0 < i && i < 3 && 0 < j && j < 3) {
                continue;
            }
            if (out_layer[i][j] == 0) {
                empty_block[number] = 100 + i * 10 + j;
                number++;
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (inside_layer[i][j] == 0) {
                empty_block[number] = 200 + i * 10 + j;
                number++;
            }
        }
    }
    if (number == 0) return -1;
    return empty_block[_random(0, number)];
}




PIMAGE get_new_block(int length, int i, int j) {
    char str[10];
    PIMAGE p = newimage(length, length);
    if (length == LG_LENGTH) {
        if (out_layer[i][j] == 0) {
            str[0] = '\0';
            setbkcolor(DEFAULT_COLOR, p);
        } else {
            itoa(out_layer[i][j], str, 10);
            setbkcolor(block_colors[(int) log2(out_layer[i][j])], p);
        }
    } else {
        if (inside_layer[i][j] == 0) {
            str[0] = '\0';
            setbkcolor(DEFAULT_COLOR, p);
        } else {
            itoa(inside_layer[i][j], str, 10);
            setbkcolor(block_colors[(int) log2(inside_layer[i][j])], p);
        }
    }
    setcolor(BLOCK_FONT_COLOR, p);
    setfont(40, 0, "Arial", p);
    settextjustify(CENTER_TEXT, CENTER_TEXT, p);
    outtextxy(length / 2, length / 2, str, p);
    return p;
}

void _gen_anime(int i, int j, int is_lg) {
    int x, y;
    if (is_lg) {
        x = j * (LG_LENGTH + SPACING) + 2 * SPACING;
        y = i * (LG_LENGTH + SPACING) + 2 * SPACING;
        PIMAGE p = get_new_block(LG_LENGTH, i, j);
        for (int i = LG_LENGTH - 15; i <= LG_LENGTH; i++) {
            putimage(x, y, i, i, p, 0, 0, LG_LENGTH, LG_LENGTH);
            delay_ms(2);
        }
        delimage(p);
    } else {
        x = LG_LENGTH + 3 * SPACING + j * (SM_LENGTH + SPACING);
        y = LG_LENGTH + 3 * SPACING + i * (SPACING + SM_LENGTH);
        PIMAGE p = get_new_block(SM_LENGTH, i, j);
        for (int i = SM_LENGTH - 15; i <= SM_LENGTH; i++) {
            putimage(x, y, i, i, p, 0, 0, SM_LENGTH, SM_LENGTH);
            delay_ms(2);
        }
        delimage(p);
    }
}

void add_random() {
    int probability = 10;
    int target = 4;
    if (cheat_count > 7) {
        probability = 100;
        target = 128;
    }
    int pool[100];
    for (int i = 0; i < probability; i++) {
        pool[i] = target;
    }
    for (int i = probability; i < 100; i++) {
        pool[i] = 2;
    }

    int add_number = pool[_random(0, 100)];
    int chosen_block = get_random_empty_block();

    int i = chosen_block / 10 % 10;
    int j = chosen_block % 10;
    if (chosen_block / 100 == 1) {
        out_layer[i][j] = add_number;
        _gen_anime(i, j, 1);
    } else {
        inside_layer[i][j] = add_number;
        _gen_anime(i, j, 0);
    }
}

BOOL is_over() {
    BOOL over = TRUE;

    if (get_random_empty_block() != -1) {
        return FALSE;
    }

    for (int i = 0; i < 4; i = i + 3) {
        for (int j = 1; j < 4; j++) {
            if (out_layer[i][j] == out_layer[i][j - 1] || out_layer[j][i] == out_layer[j - 1][i] || inside_layer[i][j] == inside_layer[i][j - 1] || inside_layer[j][i] == inside_layer[j - 1][i]) {
                over = FALSE;
            }
        }
    }

    for (int i = 1; i < 3; i++) {
        for (int j = 1; j < 3; j++) {
            if (
                (inside_layer[i][j] == inside_layer[i - 1][j]) ||
                (inside_layer[i][j] == inside_layer[i + 1][j]) ||
                (inside_layer[i][j] == inside_layer[i][j - 1]) ||
                (inside_layer[i][j] == inside_layer[i][j + 1])
            ) {
                over = FALSE;
            }
        }
    }

    return over;
}

void update() {

    PIMAGE rect;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (0 < i && i < 3 && 0 < j && j < 3) {
                continue;
            }
            rect = get_new_block(LG_LENGTH, i, j);
            putimage(j * (LG_LENGTH + SPACING) + 2 * SPACING, i * (LG_LENGTH + SPACING) + 2 * SPACING, rect);
            delimage(rect);
        }
    }


    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            rect = get_new_block(SM_LENGTH, i, j);
            putimage(LG_LENGTH + 3 * SPACING + j * (SM_LENGTH + SPACING),
                     LG_LENGTH + 3 * SPACING + i * (SPACING + SM_LENGTH), rect);
            delimage(rect);
        }
    }
}

void move_aside(int direction, int layer[4][4], int t) {
    switch (direction) {
        case 1:
            for (int cur = 1; cur < 4; cur++) {
                if (layer[cur][t] == 0) {
                    continue;
                }

                for (int i = cur; i > 0; i--) {
                    if (layer[i - 1][t] == 0) {
                        layer[i - 1][t] = layer[i][t];
                        layer[i][t] = 0;
                        moved = TRUE;
                    }
                }
            }
            break;
        case 2:
            for (int cur = 2; cur >= 0; cur--) {
                if (layer[cur][t] == 0) {
                    continue;
                }
                for (int i = cur; i < 3; i++) {
                    if (layer[i + 1][t] == 0) {
                        layer[i + 1][t] = layer[i][t];
                        layer[i][t] = 0;
                        moved = TRUE;
                    }
                }
            }
            break;
        case 3:
            for (int cur = 1; cur < 4; cur++) {
                if (layer[t][cur] == 0) {
                    continue;
                }
                for (int i = cur; i > 0; i--) {
                    if (layer[t][i - 1] == 0) {
                        layer[t][i - 1] = layer[t][i];
                        layer[t][i] = 0;
                        moved = TRUE;
                    }
                }
            }
            break;
        case 4:
            for (int cur = 2; cur >= 0; cur--) {
                if (layer[t][cur] == 0) {
                    continue;
                }
                for (int i = cur; i < 3; i++) {
                    if (layer[t][i + 1] == 0) {
                        layer[t][i + 1] = layer[t][i];
                        layer[t][i] = 0;
                        moved = TRUE;
                    }
                }
            }
            break;
    }
}


void move_up(int layer[4][4], int col) {
    for (int cur = 3; cur > 0; cur--) {
        if (layer[cur][col] == 0) {
            continue;
        }

        for (int i = cur; i > 0; i--) {
            if (layer[i - 1][col] == 0) {
                //冒泡
                layer[i - 1][col] = layer[i][col];
                layer[i][col] = 0;
                moved = TRUE;
            } else if (layer[i][col] == layer[i - 1][col]) {
                layer[i - 1][col] *= 2;
                layer[i][col] = 0;
                score += layer[i - 1][col];
                moved = TRUE;
                break;
            } else if (layer[i][col] != layer[i - 1][col]) {
                break;
            }
        }
    }

    move_aside(1, layer, col);
}

void move_down(int layer[4][4], int col) {
    for (int cur = 0; cur < 3; cur++) {
        if (layer[cur][col] == 0) {
            continue;
        }

        for (int i = cur; i < 3; i++) {
            if (layer[i + 1][col] == 0) {
                layer[i + 1][col] = layer[i][col];
                layer[i][col] = 0;
                moved = TRUE;
            } else if (layer[i][col] == layer[i + 1][col]) {
                layer[i + 1][col] *= 2;
                layer[i][col] = 0;
                score += layer[i + 1][col];
                moved = TRUE;
                break;
            } else if (layer[i][col] != layer[i + 1][col]) {
                break;
            }
        }
    }
    move_aside(2, layer, col);
}

void move_left(int layer[4][4], int row) {
    for (int cur = 3; cur > 0; cur--) {
        if (layer[row][cur] == 0) {
            continue;
        }

        for (int i = cur; i > 0; i--) {
            if (layer[row][i - 1] == 0) {
                layer[row][i - 1] = layer[row][i];
                layer[row][i] = 0;
                moved = TRUE;
            } else if (layer[row][i] == layer[row][i - 1]) {
                layer[row][i - 1] *= 2;
                layer[row][i] = 0;
                score += layer[row][i - 1];
                moved = TRUE;
                break;
            } else if (layer[row][i] != layer[row][i - 1]) {
                break;
            }
        }
    }
    move_aside(3, layer, row);

}

void move_right(int layer[4][4], int row) {
    for (int cur = 0; cur < 3; cur++) {
        if (layer[row][cur] == 0) {
            continue;
        }

        for (int i = cur; i < 3; i++) {
            if (layer[row][i + 1] == 0) {
                //冒泡
                layer[row][i + 1] = layer[row][i];
                layer[row][i] = 0;
                moved = TRUE;
            } else if (layer[row][i] == layer[row][i + 1]) {
                layer[row][i + 1] *= 2;
                layer[row][i] = 0;
                score += layer[row][i + 1];
                moved = TRUE;
                break;
            } else if (layer[row][i] != layer[row][i + 1]) {
                break;
            }
        }
    }
    move_aside(4, layer, row);
}

void merge_or_split(int i1, int j1, int i2, int j2, int i3, int j3, int is_merge) {
    if (is_merge) {
        if (inside_layer[i1][j1] == inside_layer[i2][j2] && inside_layer[i1][j1] &&
                (
                    (out_layer[i3][j3] == 2 * inside_layer[i1][j1]) ||
                    (out_layer[i3][j3] == 0)
                )) {

            out_layer[i3][j3] += 2 * inside_layer[i1][j1];
            inside_layer[i1][j1] = 0;
            inside_layer[i2][j2] = 0;
            moved = TRUE;
        }
    } else {
        if (out_layer[i3][j3] &&
                (
                    (out_layer[i3][j3] == 2 * inside_layer[i1][j1] && inside_layer[i2][j2] == inside_layer[i1][j1]) ||
                    (0 == inside_layer[i1][j1] && inside_layer[i2][j2] == inside_layer[i1][j1]) ||
                    (out_layer[i3][j3] == 2 * inside_layer[i1][j1] && inside_layer[i2][j2] == 0) ||
                    (out_layer[i3][j3] == 2 * inside_layer[i2][j2] && inside_layer[i1][j1] == 0)
                )) {
            inside_layer[i1][j1] += out_layer[i3][j3] / 2;
            inside_layer[i2][j2] += out_layer[i3][j3] / 2;
            out_layer[i3][j3] = 0;
            moved = TRUE;
        }
    }

}


void move(int direction) {
    moved = FALSE;
    // 1:up, 2:down, 3:left, 4:right
    switch (direction) {
        case 1:
            move_up(out_layer, 0);
            move_up(out_layer, 3);

            merge_or_split(0, 0, 0, 1, 0, 1, 1);
            merge_or_split(0, 2, 0, 3, 0, 2, 1);
            for (int i = 0; i < 4; i++) {
                move_aside(direction, inside_layer, i);
            }
            merge_or_split(3, 0, 3, 1, 3, 1, 0);
            merge_or_split(3, 2, 3, 3, 3, 2, 0);
            for (int i = 0; i < 4; i++) {
                move_up(inside_layer, i);
            }

            break;
        case 2:
            move_down(out_layer, 0);
            move_down(out_layer, 3);

            merge_or_split(3, 0, 3, 1, 3, 1, 1);
            merge_or_split(3, 2, 3, 3, 3, 2, 1);
            for (int i = 0; i < 4; i++) {
                move_aside(direction, inside_layer, i);
            }
            merge_or_split(0, 0, 0, 1, 0, 1, 0);
            merge_or_split(0, 2, 0, 3, 0, 2, 0);
            for (int i = 0; i < 4; i++) {
                move_down(inside_layer, i);
            }

            break;
        case 3:
            move_left(out_layer, 0);
            move_left(out_layer, 3);

            merge_or_split(0, 0, 1, 0, 1, 0, 1);
            merge_or_split(2, 0, 3, 0, 2, 0, 1);
            for (int i = 0; i < 4; i++) {
                move_aside(direction, inside_layer, i);
            }
            merge_or_split(0, 3, 1, 3, 1, 3, 0);
            merge_or_split(2, 3, 3, 3, 2, 3, 0);
            for (int i = 0; i < 4; i++) {
                move_left(inside_layer, i);
            }

            break;
        case 4:
            move_right(out_layer, 0);
            move_right(out_layer, 3);

            merge_or_split(0, 3, 1, 3, 1, 3, 1);
            merge_or_split(2, 3, 3, 3, 2, 3, 1);
            for (int i = 0; i < 4; i++) {
                move_aside(direction, inside_layer, i);
            }
            merge_or_split(0, 0, 1, 0, 1, 0, 0);
            merge_or_split(2, 0, 3, 0, 2, 0, 0);
            for (int i = 0; i < 4; i++) {
                move_right(inside_layer, i);
            }
            break;
    }
}

int put_score() {
    char _score[100];
    itoa(score, _score, 10);
    if (is_over()) {
        char s[100] = "游戏结束！总分：";
        strcat(s, _score);
        send_text(s);
        return 0;
    } else {
        char s[100] = "总分：";
        strcat(s, _score);
        send_text(s);
        return 1;
    }
}

void init_game() {
    memset(out_layer, 0, sizeof(out_layer));
    memset(inside_layer, 0, sizeof(inside_layer));
    PIMAGE p = newimage(130, 50);
    setbkcolor(BACKGROUND_COLOR, p);
    setcolor(EGERGB(0x29, 0x29, 0x35), p);
    setfont(25, 0, "黑体", p);
    settextjustify(CENTER_TEXT, CENTER_TEXT, p);
    outtextxy(65, 25, "重新开始", p);
    putimage(270, 720, p);
    delimage(p);

    PIMAGE Azusa = newimage();
    getimage_pngfile(Azusa, "./assets/anime_girl_PNG73.png");
    putimage_withalpha(nullptr, Azusa, 0, (LG_LENGTH + SPACING) * 4 + SPACING * 4);
    delimage(Azusa);
    score = 0;
    is_fresh = 0;
    update();
    add_random();
}

void write_to_file() {
    char name[50];
    scanf("%s", name);
    Player newplayer;
    newplayer.record = score;
    strcpy(newplayer.username, name);
    fwrite(&newplayer, sizeof(struct Player), 1, fp);
    fflush(fp);
}

void gameMode() {
    if (cheat_count == 8) {
        MessageBox(NULL, "作弊模式已开启", "呃呃", NULL);
        cheat_count++;
    }
    if (manage_count == 5) {
        game_mode = 0;
        manage_count++;
    }

    mouse_msg msg = {0};

    while (mousemsg()) {
        msg = getmouse();
        if (msg.is_left() && msg.is_down()) {
            if (270 < msg.x && msg.x < 270 + 130 && 720 < msg.y && msg.y < 720 + 50) {
                init_game();
            } else if (40 < msg.x && msg.x < 170 && 650 < msg.y && msg.y < 780) {
                cheat_count++;
            } else if (40 < msg.x && msg.x < 170 && 780 < msg.y && msg.y < 960) {
                manage_count++;
            }
        }
    }

    if (put_score() == 0) {
        setcolor(EGERGB(0x29, 0x29, 0x35));
        setfont(25, 0, "仿宋");
        xyprintf(230, 800, "在控制台中输入姓名：");
        if (is_fresh == 0) {
            is_fresh++;
            return;
        }
        write_to_file();
        return;
    }

    //按键检测
    int direction = -1;

    while (kbmsg()) {
        key_msg keyMsg = getkey();
        if (keyMsg.msg == key_msg_down) {
            switch (keyMsg.key) {
                case 'A':
                case key_left:
                    direction = 3;
                    break;
                case 'W':
                case key_up:
                    direction = 1;
                    break;
                case 'D':
                case key_right:
                    direction = 4;
                    break;
                case 'S':
                case key_down:
                    direction = 2;
                    break;
            }
        }
    }

    if (direction != -1) {
        move(direction);
        if (moved) {
            update();
            add_random();
        }
    }
}

void manageMode() {
    cleardevice();
}
int main() {

    fp = fopen("record.txt", "a+b");
    if (fp == NULL) {
        printf("open file error");
    }



    initgraph(640, 960);
    setcaption("2048");
    setbkcolor(BACKGROUND_COLOR);

    init_game();



    for (; is_run(); delay_fps(FPS)) {
        if (game_mode) {
            gameMode();
        } else {
            manageMode();
        }
    }

    fclose(fp);
    closegraph();
}
