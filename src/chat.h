
#define MAX_MSG 512

/* Chat session structure */
typedef struct _chatSession {
    char *nickname; /* User's nickname */
    char *token;    /* Session token used for simple authentication */
} chatSession;
