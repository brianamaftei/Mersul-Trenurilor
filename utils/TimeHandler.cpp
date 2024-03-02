#include <ctime>

time_t changeTime(const char givenClock[], int minutes, const char *actiune)
{
    char value[20];
    strcpy(value, givenClock);
    int minute = atoi(value + 3);
    value[2] = '\0';
    int ora = atoi(value);
    time_t t = time(nullptr);
    tm *local = localtime(&t);
    local->tm_hour = ora;
    local->tm_min = minute;
    if (strcmp(actiune, "minus") == 0)
    {
        local->tm_min -= minutes;
    }
    else if (strcmp(actiune, "plus") == 0)
    {
        local->tm_min += minutes;
    }
    t = mktime(local);
    return t;
}

char *clockDesign(time_t t)
{
    char *newTime = new char[6];
    tm *updated = localtime(&t);
    snprintf(newTime, sizeof(newTime), "%02d:%02d", updated->tm_hour, updated->tm_min);
    return newTime;
}

char *getNextHour(time_t givenTime)
{
    tm *local = localtime(&givenTime);
    local->tm_hour += 1;
    time_t update = mktime(local);
    local = localtime(&update);
    char *ora = new char[5];
    snprintf(ora, sizeof(ora), "%02d", local->tm_hour);
    return ora;
}
