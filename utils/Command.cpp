#include "../xml-handler/XmlHandle.cpp"
#include "Client.cpp"
#include <regex>

class BaseCommand
{
public:
    virtual char *execute() = 0;
};

class InformationCommand : public BaseCommand
{
private:
    Client client;
    XmlHandle xml;

public:
    InformationCommand(Client c, XmlHandle d) : client(c), xml(d) {}
    char *execute() override
    {
        const char *trainInfo = client.getInfo();
        char givenID[20] = "";

        regex words("\\w+");
        cregex_iterator start(trainInfo, trainInfo + strlen(trainInfo), words);
        cmatch match = *start;
        
        strcpy(givenID, match.str().c_str());
        givenID[strlen(givenID)] = '\0';
        
        Train train = xml.findTrainXML(givenID);
        return train.messageWritte();
    }
};

class ArrivalsCommand : public BaseCommand
{
private:
    Client client;
    XmlHandle xml;

public:
    ArrivalsCommand(Client c, XmlHandle d) : client(c), xml(d) {}
    char *execute() override
    {
        const char *trainInfo = client.getInfo();
        
        regex words("\\w+");
        cregex_iterator start(trainInfo, trainInfo + strlen(trainInfo), words);
        cmatch match = *start;
        
        char givenStation[20] = "";
        strcpy(givenStation, match.str().c_str());
        givenStation[strlen(givenStation)] = '\0';

        char hour[5];
        strcpy(hour, getNextHour(client.getTime()));
        // strcpy(hour, "12");
        hour[strlen(hour)] = '\0';
        
        Trains trains = xml.findStationsXML(givenStation, 1, hour);
        return trains.messageWritte();
    }
};

class DeparturesCommand : public BaseCommand
{
private:
    Client client;
    XmlHandle xml;

public:
    DeparturesCommand(Client c, XmlHandle d) : client(c), xml(d) {}
    char *execute() override
    {
        const char *trainInfo = client.getInfo();
        
        regex words("[\\w-]+");
        cregex_iterator start(trainInfo, trainInfo + strlen(trainInfo), words);
        cmatch match = *start;
        
        char givenStation[20] = "";
        strcpy(givenStation, match.str().c_str());
        givenStation[strlen(givenStation)] = '\0';
        
        char hour[5];
        strcpy(hour, getNextHour(client.getTime()));
        // strcpy(hour, "12");
        hour[strlen(hour)] = '\0';
        
        Trains trains = xml.findStationsXML(givenStation, 2, hour);
        return trains.messageWritte();
    }
};

class ModifyCommand : public BaseCommand
{
private:
    Client client;
    XmlHandle xml;

public:
    ModifyCommand(Client c, XmlHandle d) : client(c), xml(d) {}
    char *execute() override
    {
        char *message = new char[100];
        message[0] = '\0';
        
        const char *trainInfo = client.getInfo();
        
        regex words("[\\w-]+");
        cregex_iterator start(trainInfo, trainInfo + strlen(trainInfo), words);
        cmatch match = *start;
        
        char givenID[20] = "";
        strcpy(givenID, match.str().c_str());
        givenID[strlen(givenID)] = '\0';

        start++;
        match = *start;
        
        char givenStation[20] = "";
        strcpy(givenStation, match.str().c_str());
        givenStation[strlen(givenStation)] = '\0';

        start++;
        match = *start;
        char givenMinutes[20] = "";
        strcpy(givenMinutes, match.str().c_str());
        givenMinutes[strlen(givenMinutes)] = '\0';
        regex number("^\\d+$");
        if (regex_match(givenMinutes, number) == 0)
        {
            strcpy(message, "Nu ati introdus minute valide\n");
            message[strlen(message)] = '\0';
            return message;
        }

        int indicator = 0;
        start++;
        match = *start;
        char givenIndicator[20] = "";
        strcpy(givenIndicator, match.str().c_str());
        givenIndicator[strlen(givenIndicator)] = '\0';
        if (strcmp(givenIndicator, "inainte") == 0)
        {
            indicator = -1;
        }
        else if (strcmp(givenIndicator, "intarziere") == 0)
        {
            indicator = 1;
        }
        else
        {
            strcpy(message, "Nu ati precizat inainte/intarziere\n");
            message[strlen(message)] = '\0';
            return message;
        }

        char *result = xml.modifyXML(indicator, givenMinutes, givenID, givenStation);
        strcpy(message, result);
        return message;
    }
};

class QuitCommand : public BaseCommand
{
public:
    QuitCommand() {}
    char *execute() override
    {
        char *message = new char[50];
        message[0] = '\0';
        
        strcpy(message, "Inchidem aplicatia\n");
        message[strlen(message)] = '\0';
        
        return message;
    }
};

class Invoker
{
public:
    char *executeCommand(BaseCommand *command)
    {
        return command->execute();
    }
};
