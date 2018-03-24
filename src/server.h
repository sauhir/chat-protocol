
enum msg_type {
    MSG_ERROR = -1,
    MSG_NORMAL = 1,
    MSG_COMMAND = 2,
    MSG_HANDSHAKE = 4,
};

enum cmd_type {
    CMD_READ = 1,
    CMD_WRITE = 2,
    CMD_CLEAR = 4,
};

/*
Structure of a message body is as follows:
:token:nickname:this is the message text
*/
struct Message {
    int token_offset;
    int token_length;
    int nick_offset;
    int nick_length;
    int message_offset;
    int message_length;
};
