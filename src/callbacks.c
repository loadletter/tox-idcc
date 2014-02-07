#include <string.h>
#include <tox/tox.h>

#include "misc.h"
#include "fileop.h"

static int group_chat_number = -1;
static Tox *group_messenger;

int group_chat_init(Tox *m)
{
	group_messenger = m;
	group_chat_number = tox_add_groupchat(m);
	if(group_chat_number == -1)
		yerr("Failed to initialize groupchat");
	return group_chat_number;
}

void on_request(uint8_t *key, uint8_t *data, uint16_t length, void *m)
{
	char *keystr;
	data[length - 1] = '\0'; /* make sure the message is null terminated */
	
	keystr = human_readable_id(key, TOX_CLIENT_ID_SIZE);
	yinfo("Friend request from %s (%s)", keystr, data);
	int friendnum = tox_add_friend_norequest(m, key);
	
	if(friendnum == -1)
		yerr("Failed to add %s", keystr);
	else
		yinfo("Added %s as friend n° %i", keystr, friendnum);
		
	free(keystr);
}

void on_message(Tox *m, int friendnum, uint8_t *string, uint16_t length, void *userdata)
{
	int rc;
	uint8_t fnick[TOX_MAX_NAME_LENGTH];
	rc = tox_get_name(m, friendnum, fnick);
	if(rc == -1)
		strcpy((char *) fnick, "Unknown");
	
	ydebug("Message from friend n°%i [%s]: %s", friendnum, fnick, string);
	
	/* TODO: add function to parse commands */
	if(strcmp((char *) string, "invite") == 0 && group_chat_number != -1)
	{
		rc = tox_invite_friend(m, friendnum, group_chat_number);
		if(rc == -1)
			yerr("Failed to invite friend n°%i [%s] to groupchat n°%i", friendnum, fnick, group_chat_number);
	}
	
}

void on_new_file(FileNode *fn)
{
	char *msg;
	int rc;
	if(group_chat_number == -1)
		goto errormsg;
	
	//msg = gnu_basename(fn->file); TODO: fix
	msg = fn->file;
	
	rc = tox_group_message_send(group_messenger, group_chat_number, (uint8_t *) msg, strlen(msg));
	if(rc == -1)
		goto errormsg;

	errormsg:
		yerr("Failed to notify new file %s on groupchat", fn->file);
}