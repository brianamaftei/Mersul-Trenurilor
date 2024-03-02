class Client
{
private:
    int cl;
    time_t currentTime;
    int command;
    char info[200];

public:
    Client(int client, int c, const char *msg) : cl(client), command(c), currentTime(time(nullptr))
    {
        if (msg == nullptr)
        {
            msg = "";
        }
        strcpy(info, msg);
        info[strlen(info)] = '\0';
    }

    const char *getInfo() const
    {
        return info;
    }

    int getCommand()
    {
        return command;
    }

    int getCl()
    {
        return cl;
    }

    time_t getTime()
    {
        return currentTime;
    }
};
