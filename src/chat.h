
#define MAX_MSG 512

/* Chat session structure */
typedef struct chat_session {
    char *nick;  /* User's nickname */
    char *token; /* Session token used for simple authentication */
} chat_session;

/* Chat message */
typedef struct chat_message {
    char *sender;  /* Sender's nickname */
    char *message; /* Chat message contents */
} chat_message;
