
/* Chat message */
typedef struct _chatMessage {
    char *token;
    char *nickname; /* Sender's nickname */
    char *message;  /* Chat message contents */
} chatMessage;

/*
Structure of a message body is as follows:
:token:nickname:this is the message text
*/
typedef struct _Message {
    int token_offset;
    int token_length;
    int nick_offset;
    int nick_length;
    int message_offset;
    int message_length;
} Message;

char *format_message(chatMessage *message);