#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "constants.h"
#include "prototypes.h"

typedef struct
{
    int id;
    int expectedDuration;
    int startTime;
    char description[MAX_DSCPRT_LENGTH];
    char user[MAX_USER_LENGTH];
    char activity[MAX_ACTV_LENGTH];

} Task;

char users[MAX_USERS][MAX_USER_LENGTH];                                        /*tracks all users*/
char activities[MAX_ACTIVITIES][MAX_ACTV_LENGTH] = {TO_DO, IN_PROGRESS, DONE}; /*tracks all activities*/
int time = START_TIME;
int totalTasks = 0;
int totalUsers = 0;
int totalActivities = 3;
int taskOrder[MAX_TASKS] = {0}; /*tracks order of tasks*/
Task allTasks[MAX_TASKS];

int main()
{
    int run = 1;
    int c;
    while (run == 1)
    {
        c = getchar();
        switch (c)
        {
        case 'q':
            run = 0;
            break;
        case 't':
            AddTask();
            break;
        case 'l':
            ListTasks();
            break;
        case 'n':
            AdvanceTime();
            break;
        case 'u':
            AddUser();
            break;
        case 'm':
            MoveTask();
            break;
        case 'd':
            ListTasksinActivity();
            break;
        case 'a':
            AddActivity();
            break;
        }
    }
    return 0;
}

char SanitizeArgument(char *string)
{
    int i = 0;
    int flag = 0; /* flags whether we've seen an alphabetic character or not*/
    char c;
    /* copies whatever it is between blank characters */
    while ((c = getchar()) != '\n' && (c != ' ' || flag == 0))
    {
        if (c != ' ' && c != '\t')
            flag = 1;
        if (flag == 1)
            string[i++] = c;
    }
    string[i] = '\0';
    return c; /* used to check if it's the end of the command */
}

void SanitizeString(char *string)
{
    int i = 0;
    int flag = 0; /* flags whether we've seen an alphabetic character or not*/
    char c;
    while ((c = getchar()) != '\n')
    {
        if (c != ' ' && c != '\t')
            flag = 1;
        if (flag == 1)
            string[i++] = c;
    }
    string[i] = '\0';
}

void AddTask()
{
    Task newTask;
    int i, first, last, middle;

    scanf("%d", &newTask.expectedDuration);
    SanitizeString(newTask.description);
    if (totalTasks == MAX_TASKS)
    {
        printf(TASK_LIMIT_ERROR);
        return;
    }
    for (i = 0; i < totalTasks; ++i)
        if (strcmp(newTask.description, allTasks[i].description) == 0)
        {
            printf(DESCRIPTION_EXISTS_ERROR);
            return;
        }
    if (newTask.expectedDuration < 1)
    {
        printf(DURATION_ERROR);
        return;
    }

    allTasks[totalTasks] = newTask;
    allTasks[totalTasks].id = totalTasks + 1;
    strcpy(allTasks[totalTasks].activity, TO_DO);
    allTasks[totalTasks].startTime = 0;

    if (totalTasks == 0)
        taskOrder[0] = 1;
    else
    {
        first = 0;                   /*first index*/
        last = totalTasks - 1;       /*last index*/
        middle = (first + last) / 2; /*middle index*/
        while (first < last)
        {
            if (strcmp(allTasks[taskOrder[middle] - 1].description, newTask.description) < 0)
                first = middle + 1;
            else
                last = middle - 1;
            middle = (first + last) / 2;
        }
        if (strcmp(allTasks[taskOrder[first] - 1].description, newTask.description) < 0)
            ++first;
        for (i = totalTasks; i > first; --i)
            taskOrder[i] = taskOrder[i - 1];
        taskOrder[first] = totalTasks + 1;
    }

    ++totalTasks;
    printf("%s %d\n", TASK, totalTasks);
}

void ListTasks()
{
    char id[6];
    char lastChar;
    int flag = 0;

    while (flag == 0)
    {
        lastChar = SanitizeArgument(id);
        if (id[0] == '\0')
        {
            ListAllTasks();
            return;
        }
        else if (atoi(id) > totalTasks || atoi(id) < 1)
            printf("%s: %s", id, NO_TASK_ERROR);
        else
            printf("%s %s #%d %s\n",
                   id,
                   allTasks[atoi(id) - 1].activity,
                   allTasks[atoi(id) - 1].expectedDuration,
                   allTasks[atoi(id) - 1].description);
        if (lastChar == '\n')
            flag = 1;
    }
}

void ListAllTasks()
{
    int i;
    for (i = 0; i < totalTasks; ++i)
        printf("%d %s #%d %s\n",
               taskOrder[i],
               allTasks[taskOrder[i] - 1].activity,
               allTasks[taskOrder[i] - 1].expectedDuration,
               allTasks[taskOrder[i] - 1].description);
}

void AdvanceTime()
{
    int timeAdded;
    scanf("%d", &timeAdded);
    if (timeAdded < 0)
    {
        printf(TIME_ERROR);
        return;
    }
    if (timeAdded == 0)
    {
        printf("%d\n", time);
        return;
    }
    time += (int)timeAdded;
    printf("%d\n", time);
}

void AddUser()
{
    char user[MAX_USER_LENGTH];
    int i;
    SanitizeArgument(user);
    if (user[0] == '\0')
    {
        for (i = 0; i < totalUsers; ++i)
            printf("%s\n", users[i]);
        return;
    }
    else if (totalUsers == 50)
    {
        printf(USER_LIMIT_ERROR);
        return;
    }
    for (i = 0; i < totalUsers; ++i)
        if (strcmp(users[i], user) == 0)
        {
            printf(USER_EXISTS_ERROR);
            return;
        }
    strcpy(users[totalUsers], user);
    ++totalUsers;
}

void MoveTask()
{
    int id, i;
    char user[MAX_USER_LENGTH];
    char activity[MAX_ACTV_LENGTH];
    int user_exists = 0;
    int activity_exists = 0;
    scanf("%d", &id);
    SanitizeArgument(user);
    SanitizeString(activity);
    if (id > totalTasks || id < 1)
    {
        printf(NO_TASK_ERROR);
        return;
    }
    if (strcmp(activity, TO_DO) == 0)
    {
        if (strcmp(allTasks[id - 1].activity, TO_DO) == 0)
            return;
        printf(TASK_STARTED_ERROR);
        return;
    }
    for (i = 0; i < totalUsers; ++i)
    {
        if (strcmp(user, users[i]) == 0)
            user_exists = 1;
    }
    if (user_exists == 0)
    {
        printf(NO_USER_ERROR);
        return;
    }
    for (i = 0; i < totalActivities; ++i)
    {
        if (strcmp(activity, activities[i]) == 0)
            activity_exists = 1;
    }
    if (activity_exists == 0)
    {
        printf(NO_ACTIVITY_ERROR);
        return;
    }
    strcpy(allTasks[id - 1].user, user);
    if (strcmp(allTasks[id - 1].activity, TO_DO) == 0)
        allTasks[id - 1].startTime = time;
    if (strcmp(activity, DONE) == 0)
    {
        if (strcmp(allTasks[id - 1].activity, DONE) == 0)
            return;
        printf("%s=%d %s=%d\n",
               DURATION,
               time - allTasks[id - 1].startTime,
               SLACK,
               (time - allTasks[id - 1].startTime) - allTasks[id - 1].expectedDuration);
    }
    strcpy(allTasks[id - 1].activity, activity);
}

void ListTasksinActivity()
{
    char activity[MAX_ACTV_LENGTH];
    int idOrder[MAX_TASKS];
    int i, temp2;
    int count = 0;
    int j = 0;
    int activity_exists = 0;
    SanitizeString(activity);
    for (i = 0; i < totalActivities; ++i)
    {
        if (strcmp(activity, activities[i]) == 0)
            activity_exists = 1;
    }
    if (activity_exists == 0)
    {
        printf(NO_ACTIVITY_ERROR);
        return;
    }
    for (i = 0; i < totalTasks; ++i)
    {
        if (strcmp(allTasks[taskOrder[i] - 1].activity, activity) == 0)
        {
            idOrder[j++] = taskOrder[i];
            ++count;
        }
    }
    for (i = 0; i < count; ++i)
    {
        for (j = i + 1; j < count; ++j)
        {
            if (allTasks[idOrder[i] - 1].startTime > allTasks[idOrder[j] - 1].startTime)
            {
                temp2 = idOrder[i];
                idOrder[i] = idOrder[j];
                idOrder[j] = temp2;
            }
            else if (allTasks[idOrder[i] - 1].startTime == allTasks[idOrder[j] - 1].startTime &&
                     strcmp(allTasks[idOrder[i] - 1].description, allTasks[idOrder[j] - 1].description) > 0)
            {
                temp2 = idOrder[i];
                idOrder[i] = idOrder[j];
                idOrder[j] = temp2;
            }
        }
    }

    for (i = 0; i < count; ++i)
        printf("%d %d %s\n",
               idOrder[i],
               allTasks[idOrder[i] - 1].startTime,
               allTasks[idOrder[i] - 1].description);
}

void AddActivity()
{
    char activity[MAX_ACTV_LENGTH];
    int i;
    SanitizeString(activity);
    if (activity[0] == '\0')
    {
        for (i = 0; i < totalActivities; ++i)
            printf("%s\n", activities[i]);
        return;
    }
    if (totalActivities == 10)
    {
        printf(ACTIVITY_LIMIT_ERROR);
        return;
    }
    for (i = 0; i < totalActivities; ++i)
    {
        if (strcmp(activity, activities[i]) == 0)
        {
            printf(ACTIVITY_EXISTS_ERROR);
            return;
        }
    }
    for (i = 0; i < (int)strlen(activity); ++i)
    {
        if (activity[i] == '\0')
            break;
        if (islower(activity[i]))
        {
            printf(DESCRIPTION_ERROR);
            return;
        }
    }
    strcpy(activities[totalActivities], activity);
    ++totalActivities;
}