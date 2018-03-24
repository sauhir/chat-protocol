
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
