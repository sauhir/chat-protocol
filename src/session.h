
/* Chat session structure */
typedef struct _chatSession {
    char *nickname; /* User's nickname */
    char *token;    /* Session token used for simple authentication */
} chatSession;

char *create_token(char *token, size_t size);
chatSession *create_session();
