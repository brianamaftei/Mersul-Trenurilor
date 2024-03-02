#include "TimeHandler.cpp"

class Station
{
private:
    char name[20];
    char sosire[20];
    char plecare[20];
    char inainte[20];
    char intarziere[20];

public:
    Station()
    {
        name[0] = '\0';
        sosire[0] = '\0';
        plecare[0] = '\0';
        inainte[0] = '\0';
        intarziere[0] = '\0';
    }

    void setName(const char *newName)
    {
        if (newName)
        {
            strcpy(name, newName);
            name[strlen(name)] = '\0';
        }
    }

    const char *getName() const
    {
        return name;
    }

    void setSosire(const char *newSosire)
    {
        if (newSosire)
        {
            strcpy(sosire, newSosire);
            sosire[strlen(sosire)] = '\0';
        }
    }

    const char *getSosire() const
    {
        return sosire;
    }

    void setPlecare(const char *newPlecare)
    {
        if (newPlecare)
        {
            strcpy(plecare, newPlecare);
            plecare[strlen(plecare)] = '\0';
        }
    }

    const char *getPlecare() const
    {
        return plecare;
    }

    void setInainte(const char *newInainte)
    {
        if (newInainte)
        {
            strcpy(inainte, newInainte);
            inainte[strlen(inainte)] = '\0';
        }
    }

    const char *getInainte() const
    {
        return inainte;
    }

    void setIntarziere(const char *newIntarziere)
    {
        if (newIntarziere)
        {
            strcpy(intarziere, newIntarziere);
            intarziere[strlen(intarziere)] = '\0';
        }
    }

    const char *getIntarziere() const
    {
        return intarziere;
    }

    void print() const
    {
        printf("Statie nume: %s\n", name);
        printf("Sosire: %s\n", sosire);
        printf("Plecare: %s\n", plecare);
        printf("Inainte: %s\n", inainte);
        printf("Intarziere: %s\n", intarziere);
    }

    char *messageWritte() const
    {
        int lg = strlen(name) + 3 * strlen(plecare) + strlen(inainte) + strlen(intarziere);
        char *result = new char[lg + 100];
        result[0] = '\0';
        
        strcat(result, "Statie: ");
        strcat(result, name);
        
        strcat(result, "\n Sosire: ");
        strcat(result, sosire);
        
        strcat(result, "\n Plecare: ");
        strcat(result, plecare);
        strcat(result, "\n");

        if (atoi(inainte))
        {
            strcat(result, " Ajunge inainte cu ");
            strcat(result, inainte);
            strcat(result, " minute\n Estimare sosire: ");
            
            time_t s = changeTime(sosire, atoi(inainte), "minus");
            tm *updated = localtime(&s);
            
            char *newTime = new char[6];
            snprintf(newTime, sizeof(newTime), "%02d:%02d", updated->tm_hour, updated->tm_min);
            
            strcat(result, newTime);
            strcat(result, "\n");
        }
        else if (atoi(intarziere))
        {
            strcat(result, " Ajunge in intarziere cu ");
            strcat(result, intarziere);
            strcat(result, " minute\n Estimare sosire: ");
            
            time_t s = changeTime(sosire, atoi(intarziere), "plus");
            tm *updated = localtime(&s);
            
            char *newTime = new char[6];
            snprintf(newTime, sizeof(newTime), "%02d:%02d", updated->tm_hour, updated->tm_min);
            
            strcat(result, newTime);
            strcat(result, "\n");
            
        }
        else
        {
            strcat(result, "Ajunge conform cu planul\n");
        }

        result[strlen(result)] = '\0';
        return result;
    }
};

class Train
{
private:
    char ID[20];
    Station stations[20];
    int ct;

public:
    Train() : ct(0)
    {
        ID[0] = '\0';
    }

    void setID(const char *newID)
    {
        if (newID)
        {
            strcpy(ID, newID);
            ID[strlen(ID)] = '\0';
        }
    }

    const char *getID() const
    {
        return ID;
    }

    void addStation(const Station &station)
    {
        if (ct < 20)
        {
            stations[ct++] = station;
        }
        else
            printf("%s", "max stations\n");
    }

    Station getStation(int index) const
    {
        if (index >= 0 && index < ct)
        {
            return stations[index];
        }
        return stations[ct - 1];
    }

    int getCount() const
    {
        return ct;
    }

    void setCount()
    {
        if (ct < 20)
        {
            ct++;
        }
        else
            printf("%s", "ati atins ct maxim\n");
    }

    void print() const
    {
        int i;
        printf("Train ID: %s\n", ID);
        printf("Number of stations: %d\n", ct);
        for (i = 0; i < ct; i++)
        {
            printf("Station %d:\n", i + 1);
            stations[i].print();
            printf("%s", "\n");
        }
    }
    char *messageWritte() const
    {

        int lg = strlen(ID);
        for (int i = 0; i < ct; i++)
            lg += strlen(stations[i].messageWritte());
        
        char *result = new char[lg + 100];
        result[0] = '\0';

        if (strlen(ID) == 0)
        {
            strcpy(result, "Trenul nu a fost gasit\n");
            return result;
        }

        strcat(result, "Tren ID: ");
        strcat(result, ID);
        strcat(result, "\n");
        
        for (int i = 0; i < ct; i++)
        {
            char *stationInfo = stations[i].messageWritte();
            strcat(result, stationInfo);
        }
        
        strcat(result, "\n");
        result[strlen(result)] = '\0';
        
        return result;
    }
};

class Trains
{
private:
    Train trains[20];
    int nr;

public:
    Trains() : nr(0) {}

    void addTrain(const Train &train)
    {
        if (nr < 20)
        {
            trains[nr++] = train;
        }
        else
            printf("%s", "nu mai e loc");
    }

    Train getTrain(int index) const
    {
        if (index >= 0 && index < nr)
        {
            return trains[index];
        }
        else
            printf("%s", "id tren incorect");
    }

    int getNr() const
    {
        return nr;
    }
    void setNr()
    {
        if (nr < 20)
        {
            nr++;
        }
    }
    void print() const
    {
        int i;
        printf("Number of trains: %d\n", nr);
        
        for (i = 0; i < nr; i++)
        {
            printf("Train %d:\n", i + 1);
            trains[i].print();
            printf("%s", "\n");
        }
    }

    char *messageWritte() const
    {
        int lg = nr * 10;
        
        for (int i = 0; i < nr; i++)
        {
            lg += strlen(trains[i].getID());
            lg += strlen(trains[i].getStation(i).messageWritte());
        }
        
        char *result = new char[lg + 100];
        result[0] = '\0';

        if (nr == 0)
        {
            strcpy(result, "Nu exista niciun tren in urmatoarea ora\n");
            return result;
        }

        for (int i = 0; i < nr; i++)
        {
            strcat(result, "Tren ID: ");
            strcat(result, trains[i].getID());
            
            strcat(result, "\n");
            strcat(result, trains[i].getStation(i).messageWritte());
        }
        result[strlen(result)] = '\0';
        return result;
    }
};
