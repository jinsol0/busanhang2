#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <stdbool.h>

// 피라미터
#define LEN_MIN 10   //기차 길이
#define LEN_MAX 50
#define STM_MIN 0   // 마동석 체력
#define STM_MAX 5
#define PROB_MIN 10   // 확률
#define PROB_MAX 90
#define AGGRO_MIN 0   // 어그로 범위
#define AGGRO_MAX 5

// 마동석 이동 방향
#define MOVE_LEFT 1
#define MOVE_STAY 0

// 좀비의 공격 대상
#define ATK_NONE 0
#define ATK_CITIZEN 1
#define ATK_DONGSEOK 2

// 마동석 행동
#define ACTION_REST 0
#define ACTION_PROVOKE 1
#define ACTION_PULL 2

// 배열의 인덱스(loca, aggro)
#define Citizen 0
#define Madong 1
#define Zombie 2

int train[LEN_MAX];
int loca[3] = { 6, 2, 3 }; // C, M, Z 순서
int prev_loca[3];
int aggro[2] = { 1,1 }; // C, M 순서
int prev_aggro[2];
int M_stm[2]; // 이전 stamina, 현재 stamina

void intro();
int scan_len();
int scan_stamina();
int scan_p();
void set_train(int, int);
void print_train(int);
void Cit_move(int);
void Zom_move(bool);
void print_C_status(int);
void print_Z_status(int, int);
void scan_M_move();
void print_M_status(int);
bool check_C_act(int);
bool check_Z_act();
bool check_M_act(int);
void act_rest();
void act_provoke();
bool act_pull(int);
bool check_M_die();

int main(void) {
	srand((unsigned int)time(NULL));
	intro();

	int t_len, p;
	t_len = scan_len();
	M_stm[1] = scan_stamina();
	p = scan_p();

	int round = 1;
	set_train(t_len, round);
	print_train(t_len);

	bool act_pull = false;
	bool exit_game = false;

	while (1) {
		printf("\n");

		// 이동 페이즈
		Cit_move(p);

		if (round % 2 == 1) Zom_move(act_pull);

		set_train(t_len, round);
		print_train(t_len);
		print_C_status(t_len);
		print_Z_status(round, t_len);
		printf("\n");

		scan_M_move();

		set_train(t_len, round);
		print_train(t_len);
		print_M_status(t_len);

		// 행동 페이즈
		print_train(t_len);
		exit_game = check_C_act(t_len);
		if (exit_game == true) break;

		exit_game = check_Z_act();
		if (exit_game == true) break;

		exit_game = check_M_die();
		if (exit_game == true) break;
		act_pull = check_M_act(p);

		round++;
	}

	return 0;
}

void intro() {
	printf("[20220817 김진솔]\n\n");
	printf("===== 부산헹 =====\n\n");
	Sleep(200);
	printf("기차를 타고 여행을 가던 마동석.\n");
	Sleep(200);
	printf("즐거운 여행이 시작될 것만 같았던\n");
	Sleep(200);
	printf("그 ");
	Sleep(300);
	printf("순 ");
	Sleep(300);
	printf("간 ");
	Sleep(300);
	printf("!!\n\n");
	Sleep(300);
	printf("\"그어어억...\"\n\n");
	printf("좀비 사태가 발생하고 만다..\n");
	Sleep(200);
	printf("마동석은 과연 시민들을 지킬 수 있을 것인가?\n\n");
	Sleep(300);
	printf("=== Game Start! ===\n\n");
	Sleep(300);
}

int scan_len() {
	int len;
	do
	{
		printf("train length(10~50)>> ");
		scanf_s("%d", &len);
	} while (len <LEN_MIN || len > LEN_MAX);

	return len;
}

int scan_stamina() {
	int stamina;
	do
	{
		printf("madongesok stamina(0~5)>> ");
		scanf_s("%d", &stamina);
	} while (stamina<STM_MIN || stamina>STM_MAX);

	return stamina;
}

int scan_p() {
	int p;
	do
	{
		printf("percentile probability 'p'(10~90)>> ");
		scanf_s("%d", &p);
	} while (p<PROB_MIN || p > PROB_MAX);

	return p;
}

void set_train(int len, int round) {
	for (int i = 0; i < len; i++) {
		if (i == 0 || i == len - 1) {
			train[i] = '#';
		}
		else if (i == len - loca[Madong]) {
			train[i] = 'M';
		}
		else if (i == len - loca[Zombie]) {
			train[i] = 'Z';
		}
		else if (i == len - loca[Citizen]) {
			if (!(round % 2 == 1 && loca[Citizen] - loca[Madong] == 0)) {
				train[i] = 'C';
			}
		}
		else {
			train[i] = ' ';
		}
	}
}

void print_train(int len) {
	for (int i = 0; i < len; i++) {
		printf("#");
	}
	printf("\n");
	for (int i = 0; i < len; i++) {
		printf("%c", train[i]);
	}
	printf("\n");
	for (int i = 0; i < len; i++) {
		printf("#");
	}
	printf("\n\n");
}

void Cit_move(int per) {
	int r = rand() % 101;
	prev_loca[Citizen] = loca[Citizen];
	prev_aggro[Citizen] = aggro[Citizen];

	if (r <= 100 - per) {
		loca[Citizen]++;
		if (aggro[Citizen] < AGGRO_MAX) aggro[Citizen]++;
	}
	else {
		if (aggro[Citizen] > AGGRO_MIN) aggro[Citizen]--;
	}
}

void Zom_move(bool pull) {
	prev_loca[Zombie] = loca[Zombie];

	if (pull == false) {
		if (aggro[Citizen] >= aggro[Madong]) {
			if ((loca[Citizen] - loca[Zombie]) != 1) 
			{
				loca[Zombie]++; // 좀비 왼쪽 이동
			}
		}
		else {
			if ((loca[Zombie] - loca[Madong]) != 1) 
			{
				loca[Zombie]++; // 좀비 오른쪽 이동
			}
		}
	}
}

void print_C_status(int len) {
	if (prev_loca[Citizen] == loca[Citizen]) {
		if (prev_aggro[Citizen] == aggro[Citizen]) {
			printf("citizen: stay %d (aggro: %d)\n", \
				len - loca[Citizen], aggro[Citizen]);
		}
		else {
			printf("citizen: stay %d (aggro: %d -> %d)\n", \
				len - loca[Citizen], prev_aggro[Citizen], aggro[Citizen]);
		}
	}
	else {
		if (prev_aggro[Citizen] == aggro[Citizen]) {
			printf("citizen: %d -> %d (aggro: %d)\n", \
				len - prev_loca[Citizen], len - loca[Citizen], aggro[Citizen]);
		}
		else {
			printf("citizen: %d -> %d (aggro: %d -> %d)\n", \
				len - prev_loca[Citizen], len - loca[Citizen], prev_aggro[Citizen], aggro[Citizen]);
		}
	}
}

void print_Z_status(int round, int len) {
	if (round % 2 == 1) {
		if (prev_loca[Zombie] == loca[Zombie]) {
			printf("zombie: stay %d\n", len - loca[Zombie]);
		}
		else {
			printf("zombie: %d -> %d\n", len - prev_loca[Zombie], len - loca[Zombie]);
		}
	}
	else {
		printf("zombie: stay %d (cannot move)\n", len - loca[Zombie]);
	}
}

void scan_M_move() {
	int M_move;
	prev_loca[Madong] = loca[Madong];
	prev_aggro[Madong] = aggro[Madong];

	if (loca[Zombie] - loca[Madong] == 1) { // 좀비가 마동석 바로 옆에 있을 때
		do
		{
			printf("madongseok move(0:stay)>> ");
			scanf_s("%d", &M_move);
		} while (M_move != MOVE_STAY);
	}
	else {
		do
		{
			printf("madongseok move(0:stay, 1:left)>> ");
			scanf_s("%d", &M_move);
		} while (M_move < MOVE_STAY || M_move > MOVE_LEFT);
	}

	switch (M_move)
	{
	case MOVE_LEFT:
		loca[Madong]++;
		if (aggro[Madong] < AGGRO_MAX) aggro[Madong]++;
		break;
	case MOVE_STAY:
		if (aggro[Madong] > AGGRO_MIN) aggro[Madong]--;
		break;
	}
}

void print_M_status(int len) {
	if (prev_loca[Madong] == loca[Madong]) {
		if (prev_aggro[Madong] == aggro[Madong]) {
			printf("madongseok: stay %d(aggro: %d, stamina: %d)\n\n", \
				len - loca[Madong], aggro[Madong], M_stm[1]);
		}
		else {
			printf("madongseok: stay %d(aggro: %d -> %d, stamina: %d)\n\n", \
				len - loca[Madong], prev_aggro[Madong], aggro[Madong], M_stm[1]);
		}
	}
	else {
		if (prev_aggro[Madong] == aggro[Madong]) {
			printf("madongseok: %d -> %d (aggro: %d, stamina: %d)\n\n", \
				len - prev_loca[Madong], len - loca[Madong], aggro[Madong], M_stm[1]);
		}
		else {
			printf("madongseok: %d -> %d (aggro: %d -> %d, stamina: %d)\n\n", \
				len - prev_loca[Madong], len - loca[Madong], prev_aggro[Madong], aggro[Madong], M_stm[1]);
		}
	}
}


bool check_C_act(int len) {
	if (loca[Citizen] == len - 1) {
		printf("YOU WIN!\n");
		return true;
	}
	else {
		printf("citizen does nothing.\n");
	}

	return false;
}

bool check_Z_act() {
	M_stm[0] = M_stm[1];
	int atk_target = ATK_NONE;
	if (loca[Citizen] - loca[Zombie] == 1 && loca[Zombie] - loca[Madong] == 1) { // 둘다 인접한 경우
		if (aggro[Citizen] > aggro[Madong]) {
			atk_target = ATK_CITIZEN;
		}
		else {
			atk_target = ATK_DONGSEOK;
		}
	}
	else if (loca[Citizen] - loca[Zombie] == 1) atk_target = ATK_CITIZEN; // 시민과 인접
	else if (loca[Zombie] - loca[Madong] == 1) atk_target = ATK_DONGSEOK; // 마동석과 인접

	switch (atk_target)
	{
	case ATK_NONE:
		printf("zombie attacked nobody.\n");
		return false;
	case ATK_CITIZEN:
		printf("GAME OVER! citizen dead...\n");
		return true;
	case ATK_DONGSEOK:
		M_stm[0] = M_stm[1];
		if (M_stm[1] > STM_MIN) {
			M_stm[1]--;
			printf("zombie attacked madongseok (aggro: %d vs. %d, madongseok stamina: %d -> %d)\n", \
				aggro[Citizen], aggro[Madong], M_stm[0], M_stm[1]);
			return false;
		}
	}

	return false;
}

bool check_M_act(int per) { 
	int act;

	if (loca[Zombie] - loca[Madong] == 1) { // 좀비와 인접한 경우
		do {
			printf("madongseok action(0.rest, 1.provoke, 2. pull)>> ");
			scanf_s("%d", &act);
		} while (act <ACTION_REST || act > ACTION_PULL);

		switch (act)
		{
		case ACTION_REST:
			act_rest();
			break;
		case ACTION_PROVOKE:
			act_provoke();
			break;
		case ACTION_PULL:
			act_pull(per);
			return true;
		}
	}
	else {
		do {
			printf("madongseok action(0.rest, 1.provoke)>> ");
			scanf_s("%d", &act);
		} while (act<ACTION_REST || act > ACTION_PROVOKE);

		switch (act)
		{
		case ACTION_REST:
			act_rest();
			break;
		case ACTION_PROVOKE:
			act_provoke();
			break;
		}
	}

	return false;
}

void act_rest() {
	M_stm[0] = M_stm[1];
	prev_aggro[Madong] = aggro[Madong];
	if(M_stm[1] <AGGRO_MAX) M_stm[1]++;
	if (aggro[Madong] > AGGRO_MIN) aggro[Madong]--;
	printf("\nmadongseok rests...\n");
	printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n", \
		loca[Madong], prev_aggro[Madong], aggro[Madong], M_stm[0], M_stm[1]);
}

void act_provoke() {
	prev_aggro[Madong] = aggro[Madong];
	aggro[Madong] = AGGRO_MAX;
	printf("\nmadongseok provoked zombie...\n");
	printf("madongseok: %d (aggro: %d -> %d, stamina: %d)\n", \
		loca[Madong], prev_aggro[Madong], aggro[Madong], M_stm[1]); 
}

bool act_pull(int per) {
	int r;
	M_stm[0] = M_stm[1];
	prev_aggro[Madong] = aggro[Madong];
	if(M_stm[1]>STM_MIN) M_stm[1]--;
	if (aggro[Madong] <= AGGRO_MAX - 2) {
		aggro[Madong] += 2;
	}
	else if (aggro[Madong] == AGGRO_MAX - 1) {
		aggro[Madong]++;
	}

	r = rand() % 101;
	if (r <= 100 - per) {
		printf("\nmadongseok pulled zombie... Next turn, it can't move\n");
		printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n", \
			loca[Madong], prev_aggro[Madong], aggro[Madong], M_stm[0], M_stm[1]);
		return true;
	}
	else {
		printf("\nmadongseok failed to pull zombie\n");
		printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n", \
			loca[Madong], prev_aggro[Madong], aggro[Madong], M_stm[0], M_stm[1]);
		return false;
	}
}

bool check_M_die() {
	if (M_stm[1] == 0) { // 마동석 사망 판별
		printf("GAME OVER! madongseok dead...\n");
		return true;
	}
	else {
		return false;
	}
}
