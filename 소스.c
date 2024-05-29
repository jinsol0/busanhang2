#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <stdbool.h>

// �Ƕ����
#define LEN_MIN 10   //���� ����
#define LEN_MAX 50
#define STM_MIN 0   // ������ ü��
#define STM_MAX 5
#define PROB_MIN 10   // Ȯ��
#define PROB_MAX 90
#define AGGRO_MIN 0   // ��׷� ����
#define AGGRO_MAX 5

// ������ �̵� ����
#define MOVE_LEFT 1
#define MOVE_STAY 0

// ������ ���� ���
#define ATK_NONE 0
#define ATK_CITIZEN 1
#define ATK_DONGSEOK 2

// ������ �ൿ
#define ACTION_REST 0
#define ACTION_PROVOKE 1
#define ACTION_PULL 2

// loca 0,1,2 / aggro 0, 1 �ϸ� �򰥸��ϱ� ���߿� ��� ����°� ������ bb

int train[LEN_MAX];
int loca[3] = { 6,3,2 }; // C, Z, M ����
int prev_loca[3];
int aggro[2] = { 1,1 }; // C, M ����
int prev_aggro[2];
int M_stm[2]; // ���� stamina, ���� stamina

// ���߿� len-loca �̰� �� ��ȿ�����̴� loca�� ��ġ�� �տ������� ���ɷ� ����
// ����... ���߿� loca �� aggro �� Ȯ���ؼ� prev ����Ұ� �Ⱦ��κ��� �����ϱ�... 
// �� ������ ���� �迭 �� ���� �س����͵� ��ȿ������ �Ͱ����� �ϴ� �װ� ���߿� ����

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
bool print_C_act(int);
bool print_Z_act();
bool print_M_act(int);
void act_rest();
void act_provoke();
void act_pull(int);

// ������ ������� �ʴ� �ߴ��� ���� �߻�! ���� ����


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
	bool result = false;

	while (1) {
		printf("\n");

		// �̵� ������
		Cit_move(p);

		if (round % 2 == 1) Zom_move(act_pull); // ���� ������ '�ٵ��' �� ���� �ɼ� �߰�

		set_train(t_len, round);
		print_train(t_len);
		print_C_status(t_len);
		print_Z_status(round, t_len);
		printf("\n");

		scan_M_move();

		set_train(t_len, round);
		print_train(t_len);
		print_M_status(t_len);

		// �ൿ ������
		print_train(t_len);
		result = print_C_act(t_len);
		if (result == true) break;

		result = print_Z_act();
		if (result == true) break;

		act_pull = print_M_act(p);
		if (result == true) break;

		round++;
	}

	return 0;
}

void intro() {
	printf("[20220817 ������]\n\n");
	printf("===== �λ��� =====\n\n");
	Sleep(200);
	printf("������ Ÿ�� ������ ���� ������.\n");
	Sleep(200);
	printf("��ſ� ������ ���۵� �͸� ���Ҵ�\n");
	Sleep(200);
	printf("�� ");
	Sleep(300);
	printf("�� ");
	Sleep(300);
	printf("�� ");
	Sleep(300);
	printf("!!\n\n");
	Sleep(300);
	printf("\"�׾���...\"\n\n");
	printf("���� ���°� �߻��ϰ� ����..\n");
	Sleep(200);
	printf("�������� ���� �ùε��� ��ų �� ���� ���ΰ�?\n\n");
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
		else if (i == len - loca[2]) {
			train[i] = 'M';
		}
		else if (i == len - loca[1]) {
			train[i] = 'Z';
		}
		else if (i == len - loca[0]) {
			if (!(round % 2 == 1 && loca[0] - loca[1] == 0)) {
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
	prev_loca[0] = loca[0];
	prev_aggro[0] = aggro[0];

	if (r <= 100 - per) {
		loca[0]++;
		if (aggro[0] < AGGRO_MAX) aggro[0]++;
	}
	else {
		if (aggro[0] > AGGRO_MIN) aggro[0]--;
	}
}

void Zom_move(bool pull) {
	prev_loca[1] = loca[1];

	if (pull == false) {
		if (aggro[0] >= aggro[1]) {
			if (loca[0] - loca[1] != 1)
			{
				loca[1]++; // ���� ���� �̵�
			}
		}
		else {
			if (loca[1] - loca[2] != 1)
			{
				loca[1]++; // ���� ������ �̵�
			}
		}
	}
}

void print_C_status(int len) {
	if (prev_loca[0] == loca[0]) {
		if (prev_aggro[0] == aggro[0]) {
			printf("citizen: stay %d (aggro: %d)\n", \
				len - loca[0], aggro[0]);
		}
		else {
			printf("citizen: stay %d (aggro: %d -> %d)\n", \
				len - loca[0], prev_aggro[0], aggro[0]);
		}
	}
	else {
		if (prev_aggro[0] == aggro[0]) {
			printf("citizen: %d -> %d (aggro: %d)\n", \
				len - prev_loca[0], len - loca[0], aggro[0]);
		}
		else {
			printf("citizen: %d -> %d (aggro: %d -> %d)\n", \
				len - prev_loca[0], len - loca[0], prev_aggro[0], aggro[0]);
		}
	}
}

void print_Z_status(int round, int len) {
	if (round % 2 == 1) {
		if (prev_loca[1] == loca[1]) {
			printf("zombie: stay %d\n", len - loca[1]);
		}
		else {
			printf("zombie: %d -> %d\n", len - prev_loca[1], len - loca[1]);
		}
	}
	else {
		printf("zombie: stay %d (cannot move)\n", len - loca[1]);
	}
}

void scan_M_move() {
	int M_move;
	prev_loca[2] = loca[2];
	prev_aggro[1] = aggro[1];

	if (loca[1] - loca[2] == 1) { // ���� ������ �ٷ� ���� ���� ��
		do
		{
			printf("madongseok move(0:stay)>> ");
			scanf_s("%d", &M_move);
		} while (!(M_move == MOVE_STAY));
	}
	else {
		do
		{
			printf("madongseok move(0:stay, 1:left)>> ");
			scanf_s("%d", &M_move);
		} while (!(M_move == MOVE_STAY || M_move == MOVE_LEFT));
	}

	switch (M_move)
	{
	case 1:
		loca[2]++;
		if (aggro[1] < AGGRO_MAX) aggro[1]++;
		break;
	case 0:
		if (aggro[1] > AGGRO_MIN) aggro[1]--;
		break;
	}
}

void print_M_status(int len) {
	if (prev_loca[2] == loca[2]) {
		if (prev_aggro[1] == aggro[1]) {
			printf("madongseok: stay %d(aggro: %d, stamina: %d)\n", \
				len - loca[2], aggro[1], M_stm[1]);
		}
		else {
			printf("madongseok: stay %d(aggro: %d -> %d, stamina: %d)\n", \
				len - loca[2], prev_aggro[1], aggro[1], M_stm[1]);
		}
	}
	else {
		if (prev_aggro[1] == aggro[1]) {
			printf("madongseok: %d -> %d (aggro: %d, stamina: %d)\n", \
				len - prev_loca[2], len - loca[2], aggro[1], M_stm[1]);
		}
		else {
			printf("madongseok: %d -> %d (aggro: %d -> %d, stamina: %d)\n", \
				len - prev_loca[2], len - loca[2], prev_aggro[1], aggro[1], M_stm[1]);
		}
	}
}

bool print_C_act(int len) {
	printf("citizen does nothing.\n");
	if (loca[0] == len - 1) {
		printf("YOU WIN!\n");
		return true;
	}

	return false;
}

bool print_Z_act() {
	M_stm[0] = M_stm[1];
	int atk_target = ATK_NONE;
	if (loca[0] - loca[1] == 0 && loca[1] - loca[2] == 0) {
		if (aggro[0] > aggro[1]) {
			atk_target = ATK_CITIZEN;
		}
		else {
			atk_target = ATK_DONGSEOK;
		}
	}
	else if (loca[0] - loca[1] == 1) atk_target = ATK_CITIZEN;
	else if (loca[1] - loca[2] == 1) atk_target = ATK_DONGSEOK;

	switch (atk_target)
	{
	case ATK_NONE:
		printf("zombie attacked nobody.\n");
		return false;
	case ATK_CITIZEN:
		printf("GAME OVER! citizen dead...\n");
		return true;
	case ATK_DONGSEOK:
		M_stm[1]--;
		printf("zombie attacked madongseok (aggro: %d vs. %d, madongseok stamina: %d -> %d)\n", \
			aggro[0], aggro[1], M_stm[0], M_stm[1]);
		if (M_stm[1] == 0) {
			printf("GAME OVER! madongseok dead...\n");
			return true;
		}
		return false;
	}
}

bool print_M_act(int per) { // �굵 �� �׷��� 2���� �ൿ�� �����ϰ� ����(����Ʈ&�Ǻ�)
	int act;

	if (loca[1] - loca[2] == 1) { // ����� ������ ���
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
	M_stm[1]++;
	if (aggro[0] > AGGRO_MIN) aggro[1]--;
	printf("\nmadongseok rests...\n");
	printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n", \
		loca[2], aggro[1] + 1, aggro[1], M_stm[0], M_stm[1]);
}

void act_provoke() {
	aggro[1] = AGGRO_MAX;
	printf("\nmadongseok provoked zombie...\n");
	printf("madongseok: %d (aggro: %d -> %d, stamina: %d)\n", \
		loca[2], aggro[1] + 1, aggro[1], M_stm[1]); // ���� ��׷� 6���� ��µȴ�. �ذ���
}

void act_pull(int per) {
	int r;
	prev_aggro[1] = aggro[1];
	M_stm[1]--;
	if (aggro[0] + 2 <= AGGRO_MAX - 2) {
		aggro[1] += 2;
	}
	else if (aggro[0] + 1 == AGGRO_MAX - 1) {
		aggro[1]++;
	}

	r = rand() % 101;
	if (r <= 100 - per) {
		printf("\nmadongseok pulled zombie... Next turn, it can't move\n");
	}
	else {
		printf("\nmadongseok failed to pull zombie\n");
	}
	printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n", \
		loca[2], prev_aggro[1], aggro[1], M_stm[0], M_stm[1]);
	if (M_stm[1] == 0) return // ���⼭ ���� ���� �������
}

// pull ���� ���� �Ǻ� ���� & ���� ���� ���� �Ǻ� ����.. �������� ����ʹ�..
// �� ���� �ٸ����� �˾Ҵµ� �������� ������ ���� ������&�ϰ����� �ʹ� �����Ƽ� 
// ��ĥ �κ��� ����̳�... ���� ū ������ �Ǻ� ���� & �迭 ���� ���(�ʹ� ��ȿ����)