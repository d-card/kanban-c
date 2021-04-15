/*
 * File:  prototypes.h
 * Author:  Diogo Cardoso 99209
 * Description: File where all functions' prototypes + Task struct are declared.
*/

typedef struct{
    int id;
    int expectedDuration;
    int startTime;
    char description[MAX_DSCPRT_LENGTH];
    char user[MAX_USER_LENGTH];
    char activity[MAX_ACTV_LENGTH];

} Task;

void SanitizeString(char *string);
char SanitizeArgument(char *string);
void AddTask();
int CheckAddTaskErrors(Task task);
void OrderNewTask(char *description);
void ListTasks();
void ListAllTasks();
void AdvanceTime();
void AddUser();
void MoveTask();
int CheckMoveTaskErrors(int id, char *user, char *activity);
void ListTasksInActivity();
void SortTasksByTime(int *idOrder, int count);
void AddActivity();
int CheckActivityErrors(char *activity);