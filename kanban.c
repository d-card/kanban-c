/*
 * File:  kanban.c
 * Author:  Diogo Cardoso 99209
 * Description: A program which recreates the Kanban scheduling system. 
 * Main file where all functions are defined.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "constants.h"
#include "prototypes.h"

/*global variables*/
char users[MAX_USERS][MAX_USER_LENGTH];                                        /*tracks all users*/
char activities[MAX_ACTIVITIES][MAX_ACTV_LENGTH] = {TO_DO, IN_PROGRESS, DONE}; /*tracks all activities*/
int time = START_TIME;                                                         /* tracks time */
int totalTasks = 0;                                                            /* tracks total number of tasks */
int totalUsers = 0;                                                            /* tracks total number of users */
int totalActivities = 3;                                                       /* tracks total number of activities */
int taskOrder[MAX_TASKS];                                                      /*tracks alphabetical order of tasks' descriptions*/
Task allTasks[MAX_TASKS];                                                      /*tracks all tasks*/

/* reads command letter and runs appropriate function */
int main(){
    int c, run = 1;

    while (run == 1){
        c = getchar();
        switch (c){
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
            ListTasksInActivity();
            break;
        case 'a':
            AddActivity();
            break;
        }
    }
    return 0;
}

/* reads 1 argument from input, saves what it reads into string and returns the last character read */
char SanitizeArgument(char *string){
    int c, i = 0, flag = 0; /* flags whether a character other than space/tab has been read */

    while ((c = getchar()) != '\n' && (!isspace(c) || flag == 0)){
        if (!isspace(c))
            flag = 1;
        if (flag == 1)
            string[i++] = c;
    }
    string[i] = '\0';
    return c; /* used to check if it's the end of the command */
}

/* reads a string from input and saves what it reads into string */
void SanitizeString(char *string){
    int c, i = 0, flag = 0; /* flags whether a character other than space/tab has been read */

    while ((c = getchar()) != '\n'){
        if (!isspace(c))
            flag = 1;
        if (flag == 1)
            string[i++] = c;
    }
    string[i] = '\0';
}

/* ran when "t" command is invoked. adds task to program */
void AddTask(){
    Task newTask;

    scanf("%d", &newTask.expectedDuration);
    SanitizeString(newTask.description);

    if (CheckAddTaskErrors(newTask) == 1)
        return;

    /* update task struct*/
    allTasks[totalTasks] = newTask;
    allTasks[totalTasks].id = totalTasks + 1;
    allTasks[totalTasks].startTime = 0;
    strcpy(allTasks[totalTasks].activity, TO_DO);

    OrderNewTask(newTask.description);

    ++totalTasks;
    printf("%s %d\n", TASK, totalTasks);
}

/* auxiliar function of AddTask(). checks for input errors */
int CheckAddTaskErrors(Task task){
    int i;

    if (totalTasks == MAX_TASKS){
        printf(TASK_LIMIT_ERROR);
        return 1;
    }
    for (i = 0; i < totalTasks; ++i)
        if (strcmp(task.description, allTasks[i].description) == 0){
            printf(DESCRIPTION_EXISTS_ERROR);
            return 1;
        }
    if (task.expectedDuration < 1){
        printf(DURATION_ERROR);
        return 1;
    }

    return 0;
}

/* auxiliar function of AddTask(). performs binary search and inserts newTask into correct place in allTasks vector*/
void OrderNewTask(char *description){
    int i, first, middle, last, index;
    
    /* binary search */
    if (totalTasks == 0)
        index = 0;
    else{
        first = 0;
        last = totalTasks - 1;
        middle = (first + last) / 2;
        while (first < last){ /* when first == last, task should either be in that index or 1 higher */
            if (strcmp(allTasks[taskOrder[middle] - 1].description, description) < 0)
                first = middle + 1;
            else
                last = middle - 1;
            middle = (first + last) / 2;
        }

        /* check where task should be put (same index or 1 higher) */
        index = first;
        if (strcmp(allTasks[taskOrder[first] - 1].description, description) < 0)
            index += 1;
    }

    /* moves appropriate tasks 1 index higher */
    for (i = totalTasks; i > index; --i)
        taskOrder[i] = taskOrder[i - 1];
    taskOrder[index] = totalTasks + 1; /* places new task in correct index */
}

/* ran when "l" command is invoked. lists all tasks or specified tasks */
void ListTasks(){
    char lastChar, id[MAX_TASKS_STRING_SIZE];
    int flag = 0;

    while (flag == 0){
        lastChar = SanitizeArgument(id);
        if (id[0] == '\0'){ /* check if command has no arguments */
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
        if (lastChar == '\n') /* check if it's the end of the command */
            flag = 1;
    }
}

/* auxiliar function of ListTasks(). lists all tasks */
void ListAllTasks(){
/* no need to sort tasks as they're already ordered in taskOrder vector */
    int i;

    for (i = 0; i < totalTasks; ++i)
        printf("%d %s #%d %s\n",
               taskOrder[i],
               allTasks[taskOrder[i] - 1].activity,
               allTasks[taskOrder[i] - 1].expectedDuration,
               allTasks[taskOrder[i] - 1].description);
}

/* ran when "n" command is invoked. advances program time */
void AdvanceTime(){
    int timeAdded;

    scanf("%d", &timeAdded);
    if (timeAdded < 0){
        printf(TIME_ERROR);
        return;
    }
    if (timeAdded == 0){
        printf("%d\n", time);
        return;
    }

    time += timeAdded;
    printf("%d\n", time);
}

/* ran when "u" command is invoked. adds user or lists all users */
void AddUser(){
    char user[MAX_USER_LENGTH];
    int i;

    SanitizeArgument(user);

    if (user[0] == '\0'){ /* check if command has no arguments */
        for (i = 0; i < totalUsers; ++i)
            printf("%s\n", users[i]);
        return;
    }
    else if (totalUsers == 50){
        printf(USER_LIMIT_ERROR);
        return;
    }
    for (i = 0; i < totalUsers; ++i)
        if (strcmp(users[i], user) == 0){
            printf(USER_EXISTS_ERROR);
            return;
        }

    strcpy(users[totalUsers], user);
    ++totalUsers;
}

/* ran when "m" command is invoked. moves task between activities */
void MoveTask(){
    int id;
    char user[MAX_USER_LENGTH], activity[MAX_ACTV_LENGTH];

    scanf("%d", &id);
    SanitizeArgument(user);
    SanitizeString(activity);

    if (CheckMoveTaskErrors(id, user, activity) == 1)
        return;

    if (strcmp(allTasks[id - 1].activity, TO_DO) == 0) /*if task is being moved from TO DO set start time*/
        allTasks[id - 1].startTime = time;
    if (strcmp(activity, DONE) == 0){ /*if task is being moved to DONE print required info*/
        if (strcmp(allTasks[id - 1].activity, DONE) == 0) /*if being moved from DONE to DONE do nothing*/
            return;
        printf("%s=%d %s=%d\n",
               DURATION,
               time - allTasks[id - 1].startTime,
               SLACK,
               (time - allTasks[id - 1].startTime) - allTasks[id - 1].expectedDuration);
    }

    /* update task struct */
    strcpy(allTasks[id - 1].activity, activity);
    strcpy(allTasks[id - 1].user, user);
}

/* auxiliar function of AddActivity(). checks for input errors */
int CheckMoveTaskErrors(int id, char *user, char *activity){
    int i, user_exists = 0, activity_exists = 0;

    if (id > totalTasks || id < 1){
        printf(NO_TASK_ERROR);
        return 1;
    }
    if (strcmp(activity, TO_DO) == 0){
        if (strcmp(allTasks[id - 1].activity, TO_DO) == 0)
            return 1; /*if moving from TO DO to TO DO do nothing*/

        printf(TASK_STARTED_ERROR);
        return 1;
    }
    for (i = 0; i < totalUsers; ++i)
        if (strcmp(user, users[i]) == 0)
            user_exists = 1;
    if (user_exists == 0){
        printf(NO_USER_ERROR);
        return 1;
    }
    for (i = 0; i < totalActivities; ++i)
        if (strcmp(activity, activities[i]) == 0)
            activity_exists = 1;
    if (activity_exists == 0){
        printf(NO_ACTIVITY_ERROR);
        return 1;
    }
    return 0;
}

/* ran when "d" command is invoked. lists tasks in specified activity */
void ListTasksInActivity(){
    char activity[MAX_ACTV_LENGTH];
    int i, j, idOrder[MAX_TASKS], count = 0, activity_exists = 0;

    SanitizeString(activity);

    for (i = 0; i < totalActivities; ++i)
        if (strcmp(activity, activities[i]) == 0)
            activity_exists = 1;
    if (activity_exists == 0){
        printf(NO_ACTIVITY_ERROR);
        return;
    }

    j = 0;
    /*copy id of tasks in activity to idOrder vector and count number of tasks*/
    for (i = 0; i < totalTasks; ++i)
        if (strcmp(allTasks[taskOrder[i] - 1].activity, activity) == 0){
            idOrder[j++] = taskOrder[i];
            ++count;
        }

    SortTasksByTime(idOrder, count);

    for (i = 0; i < count; ++i)
        printf("%d %d %s\n", 
               idOrder[i],
               allTasks[idOrder[i] - 1].startTime,
               allTasks[idOrder[i] - 1].description);
}

/* auxiliar function of ListTasksInActivity(). sorts tasks by start time */
void SortTasksByTime(int *idOrder, int count){
    int i, j, temp;

    /*sort tasks by startTime*/
    for (i = 0; i < count; ++i)
        for (j = i + 1; j < count; ++j){
            if (allTasks[idOrder[i] - 1].startTime > allTasks[idOrder[j] - 1].startTime){
                temp = idOrder[i];
                idOrder[i] = idOrder[j];
                idOrder[j] = temp;
            }
            /*if startTime is the same, sort them alphabetically by description*/
            else if (allTasks[idOrder[i] - 1].startTime == allTasks[idOrder[j] - 1].startTime &&
                     strcmp(allTasks[idOrder[i] - 1].description,
                            allTasks[idOrder[j] - 1].description) > 0){
                temp = idOrder[i];
                idOrder[i] = idOrder[j];
                idOrder[j] = temp;
            }
        }
}

/* ran when "a" command is invoked. adds activity to program */
void AddActivity(){
    char activity[MAX_ACTV_LENGTH];
    int i;

    SanitizeString(activity);

    if (activity[0] == '\0'){ /* check if command has no arguments */
        for (i = 0; i < totalActivities; ++i)
            printf("%s\n", activities[i]);
        return;
    }
    if (CheckActivityErrors(activity) == 1)
        return;

    strcpy(activities[totalActivities], activity);
    ++totalActivities;
}

/* auxiliar function of AddActivity(). checks for input errors */
int CheckActivityErrors(char *activity){
    int i;

    if (totalActivities == MAX_ACTIVITIES){
        printf(ACTIVITY_LIMIT_ERROR);
        return 1;
    }
    for (i = 0; i < totalActivities; ++i)
        if (strcmp(activity, activities[i]) == 0){
            printf(ACTIVITY_EXISTS_ERROR);
            return 1;
        }
    /* check if activity has lower-case letters */
    for (i = 0; i < (int)strlen(activity); ++i){
        if (activity[i] == '\0')
            break;
        if (islower(activity[i])){
            printf(DESCRIPTION_ERROR);
            return 1;
        }
    }
    return 0;
}