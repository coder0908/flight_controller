/*
 * msgbox.c
 *
 *  Created on: 2025. 12. 4.
 *      Author: coder0908
 */


#include <string.h>
#include <assert.h>
#include "msgbox.h"
#include "utils/utils.h"

#define MSGBOX_CNT_MAX 50
#define MSGBOX_SUBCNT_MAX 20
#define MSGBOX_NAMELEN_MAX 20
#define MSGBOX_LEN_IDS ((MSGBOX_CNT_MAX/8) + (MSGBOX_CNT_MAX%8)?1:0)
#define MSGBOX_ID_INVALID -1

struct msgbox {
	int id;
	char name[MSGBOX_NAMELEN_MAX];
	uint8_t name_len;

	uint8_t subs_cnt;	// sub가 빠져나간다면, 앞에서부터 꽉차게 재정렬 필요
	msgbox_callback_fp callbacks[MSGBOX_SUBCNT_MAX];

};


static struct msgbox s_msgboxes[MSGBOX_CNT_MAX];
static int s_msgboxes_len = 0;


static int msgbox_hash(int hash, char val)
{
	return (hash*31 + val) % MSGBOX_CNT_MAX;
}

static int msgbox_hash_name(const char msgbox_name[], uint8_t msgbox_name_len)
{

	assert(msgbox_name);
	assert(msgbox_name_len <= MSGBOX_NAMELEN_MAX);

	int msgbox_id = 0xD2;
	for (uint8_t i=0; i<msgbox_name_len; i++) {
		msgbox_id = msgbox_hash(msgbox_id, msgbox_name[i]);
	}

	return msgbox_id;
}

static struct msgbox* msgbox_find_addr(const char msgbox_name[], uint8_t msgbox_name_len)
{
	int msgbox_id = msgbox_hash_name(msgbox_name, msgbox_name_len);

	struct msgbox *mb = NULL;
	for (int i=0; i<MSGBOX_CNT_MAX; i++) {
		mb = s_msgboxes + msgbox_id;
		if (mb->id == msgbox_id ||
			mb->name_len == msgbox_name_len ||
			strncmp(msgbox_name, mb->name, msgbox_name_len) == 0) {
			return s_msgboxes + msgbox_id;
		}
		msgbox_id = (msgbox_id+1) % MSGBOX_CNT_MAX;
	}
	return NULL;
}

static int msgbox_alloc_id(const char msgbox_name[], uint8_t msgbox_name_len)
{
	assert(msgbox_name);
	assert(msgbox_name_len <= MSGBOX_NAMELEN_MAX);

	if (s_msgboxes_len == MSGBOX_CNT_MAX) {
		return MSGBOX_ID_INVALID;
	}

	int msgbox_id = msgbox_hash_name(msgbox_name, msgbox_name_len);

	for (int i=0; i<MSGBOX_CNT_MAX; i++) {
		if (s_msgboxes[msgbox_id].id == MSGBOX_ID_INVALID) {
			return msgbox_id;
		}
		msgbox_id = (msgbox_id+1) % MSGBOX_CNT_MAX;
	}
	return MSGBOX_ID_INVALID;

}

static void msgbox_free_id(int msgbox_id)
{
	assert(msgbox_id >= 0);
	assert(msgbox_id < MSGBOX_CNT_MAX);

	struct msgbox *mb = s_msgboxes + msgbox_id;
	if (mb->id != msgbox_id) {
		return;
	}
	mb->id = MSGBOX_ID_INVALID;
}

bool msgbox_create(const char msgbox_name[], uint8_t msgbox_name_len, uint32_t *msgbox_id)
{
	assert(msgbox_name_len<=MSGBOX_NAMELEN_MAX);
	assert(msgbox_id);

	*msgbox_id = msgbox_alloc_id(msgbox_name, msgbox_name_len);
	if (*msgbox_id == MSGBOX_ID_INVALID) {
		return false;
	}

	struct msgbox *mb = s_msgboxes + *msgbox_id;

	mb->id = *msgbox_id;
	s_msgboxes_len += 1;
	strcpy_n(mb->name, msgbox_name, msgbox_name_len);
	mb->name_len = msgbox_name_len;
	mb->subs_cnt = 0;

	return true;
}

void msgbox_destroy(int msgbox_id)
{
	struct msgbox *mb = s_msgboxes + msgbox_id;
	for (uint8_t i=0; i<mb->subs_cnt; i++) {
		mb->callbacks[i] = NULL;
	}
	mb->subs_cnt = 0;

	char dumy_name[MSGBOX_NAMELEN_MAX] = {0,};
	strcpy_n(mb->name, dumy_name, mb->name_len);
	mb->name_len = 0;
	msgbox_free_id(msgbox_id);
}

bool msgbox_publish(int msgbox_id, uint8_t msg[], uint8_t msg_len)
{
	msgbox_callback_fp *callbacks = s_msgboxes[msgbox_id].callbacks;
	uint8_t subs_cnt = s_msgboxes[msgbox_id].subs_cnt;

	for (uint8_t i=0; i<subs_cnt; i++) {
		callbacks[i](msg, msg_len);
	}

	return true;
}

bool msgbox_subscribe(char msgbox_name[], uint8_t msgbox_name_len, msgbox_callback_fp callback)
{
	assert(msgbox_name);
	assert(callback);

	struct msgbox *mb = msgbox_find_addr(msgbox_name, msgbox_name_len);

	if (mb->subs_cnt == MSGBOX_SUBCNT_MAX) {
		return false;
	}

	mb->callbacks[mb->subs_cnt] = callback;
	mb->subs_cnt++;

	return true;
}

void msgbox_dispath(int msgbox_id)
{

}



