#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include "../utils/TrainClasses.cpp"

using namespace std;

class XmlHandle
{
private:
    xmlDocPtr information;
    const char *path = "../database/MersulTrenurilor.xml";
    const char *pathout = "../database/MersulTrenurilor.xml";

public:
    XmlHandle()
    {
        information = xmlParseFile(path);
        if (information == NULL)
        {
            perror("Probleme la parsare fisier XML\n");
            exit(23);
        }
    }

    void endXml()
    {
        if (information != NULL)
        {
            xmlFreeDoc(information);
        }
        xmlCleanupParser();
    }

    Train findTrainXML(char IDTrain[])
    {
        Train train;

        //<panou>
        xmlNodePtr firstNode = xmlDocGetRootElement(information);
        if (firstNode == NULL)
        {
            printf("%s", "nu avem noduri\n");
            xmlFreeDoc(information);
            perror("nu avem noduri\n");
            exit(24);
        }
        else
        {
            // tagul <tren >
            xmlNodePtr nodeTrain = firstNode->xmlChildrenNode;
            while (nodeTrain != NULL)
            {
                char tag[20];
                snprintf(tag, sizeof(tag), "%s", nodeTrain->name);
                if (strcmp(tag, "tren") == 0)
                {
                    char ID[20];
                    snprintf(ID, sizeof(ID), "%s", xmlGetProp(nodeTrain, (const xmlChar *)"tren_Id"));

                    if (strcmp(ID, IDTrain) == 0)
                    {
                        train.setID(ID);

                        // tagul <statie >
                        xmlNodePtr nodeStation = nodeTrain->xmlChildrenNode;
                        while (nodeStation != NULL)
                        {
                            char tag2[20];
                            snprintf(tag2, sizeof(tag2), "%s", nodeStation->name);
                            if (strcmp(tag2, "statie") == 0)
                            {
                                Station station;

                                // nume_statie
                                char name[20];
                                snprintf(name, sizeof(name), "%s", xmlGetProp(nodeStation, (const xmlChar *)"nume_statie"));
                                station.setName(name);

                                // <detaliu>
                                xmlNodePtr detailNode = nodeStation->xmlChildrenNode;
                                while (detailNode != NULL)
                                {
                                    char tag3[20];
                                    snprintf(tag3, sizeof(tag3), "%s", detailNode->name);

                                    char detailValue[20];
                                    snprintf(detailValue, sizeof(detailValue), "%s", xmlNodeGetContent(detailNode));
                                    if (strcmp(tag3, "sosire") == 0)
                                    {
                                        station.setSosire(detailValue);
                                    }
                                    else if (strcmp(tag3, "plecare") == 0)
                                    {
                                        station.setPlecare(detailValue);
                                    }
                                    else if (strcmp(tag3, "inainte") == 0)
                                    {
                                        station.setInainte(detailValue);
                                    }
                                    else if (strcmp(tag3, "intarziere") == 0)
                                    {
                                        station.setIntarziere(detailValue);
                                    }
                                    detailNode = detailNode->next;
                                }

                                train.addStation(station);
                            }
                            nodeStation = nodeStation->next;
                        }
                    }
                }
                nodeTrain = nodeTrain->next;
            }
        }

        return train;
    }

    Trains findStationsXML(char givenStation[], int indicator, char ora[])
    {
        Trains t;

        //<panou>
        xmlNodePtr firstNode = xmlDocGetRootElement(information);
        if (firstNode == NULL)
        {
            printf("%s", "nu avem noduri\n");
            xmlFreeDoc(information);
            perror("nu avem noduri\n");
            exit(24);
        }
        else
        {
            // tagul <tren >
            xmlNodePtr nodeTrain = firstNode->xmlChildrenNode;
            while (nodeTrain != NULL)
            {
                char tag[20];
                snprintf(tag, sizeof(tag), "%s", nodeTrain->name);
                if (strcmp(tag, "tren") == 0)
                {
                    Train train;
                    char ID[20];
                    snprintf(ID, sizeof(ID), "%s", xmlGetProp(nodeTrain, (const xmlChar *)"tren_Id"));
                    train.setID(ID);

                    // tagul <statie >
                    xmlNodePtr nodeStation = nodeTrain->xmlChildrenNode;
                    while (nodeStation != NULL)
                    {
                        char tag2[20];
                        snprintf(tag2, sizeof(tag2), "%s", nodeStation->name);
                        if (strcmp(tag2, "statie") == 0)
                        {
                            Station station;
                            char nameStation[20];
                            snprintf(nameStation, sizeof(nameStation), "%s", xmlGetProp(nodeStation, (const xmlChar *)"nume_statie"));
                            station.setName(nameStation);

                            if (strcmp(nameStation, givenStation) == 0)
                            {
                                // <detaliu>
                                char oraSosireEstimata[3];
                                char oraPlecareEstimata[3];

                                char valueSosire[10];
                                char valuePlecare[10];

                                char inainte_[10];
                                char intarziere_[10];

                                xmlNodePtr detailNode = nodeStation->xmlChildrenNode;
                                while (detailNode != NULL)
                                {
                                    char tag3[20];
                                    snprintf(tag3, sizeof(tag3), "%s", detailNode->name);

                                    char detailValue[20];
                                    snprintf(detailValue, sizeof(detailValue), "%s", xmlNodeGetContent(detailNode));
                                    if (strcmp(tag3, "sosire") == 0)
                                    {
                                        station.setSosire(detailValue);
                                        snprintf(valueSosire, sizeof(valueSosire), "%s", xmlNodeGetContent(detailNode)); 
                                    }
                                    else if (strcmp(tag3, "plecare") == 0)
                                    {
                                        station.setPlecare(detailValue);
                                        snprintf(valuePlecare, sizeof(valuePlecare), "%s", xmlNodeGetContent(detailNode));
                                    }
                                    else if (strcmp(tag3, "inainte") == 0)
                                    {
                                        station.setInainte(detailValue);
                                        strcpy(inainte_, detailValue);
                                    }
                                    else if (strcmp(tag3, "intarziere") == 0)
                                    {
                                        station.setIntarziere(detailValue);
                                        strcpy(intarziere_, detailValue);
                                    }
                                    detailNode = detailNode->next;
                                }

                                train.addStation(station);

                                int modification=0;
                                if (atoi(intarziere_) == 0)
                                {
                                    modification = -1;
                                }
                                else
                                {
                                    modification = 1;
                                }

                                time_t s = time(nullptr);
                                if(indicator == 1)
                                {
                                    int minute = atoi(valueSosire + 3);
                                    valueSosire[2] = '\0';
                                    int ora_ = atoi(valueSosire);
                                    time_t t = time(nullptr);
                                    tm *local = localtime(&t);
                                    if(atoi(intarziere_) == 0)
                                        local->tm_min = minute - atoi(inainte_);
                                    else
                                        local->tm_min = minute + atoi(intarziere_);
                                    local->tm_hour = ora_;
                                    t = mktime(local);
                                    s = t;
                                    local = localtime(&t);
                                    snprintf(oraSosireEstimata, sizeof(oraSosireEstimata), "%02d", local->tm_hour);
                                    printf("Sosire: ora: %d, minutele: %d\n", local->tm_hour, local->tm_min);

                                }
                                else
                                {
                                    int minute = atoi(valuePlecare + 3);
                                    valuePlecare[2] = '\0';
                                    int ora = atoi(valuePlecare);
                                    time_t t2 = time(nullptr);
                                    tm *local = localtime(&t2);
                                    local->tm_min = minute;
                                    local->tm_hour = ora;
                                   

                                    t2 = mktime(local); 
                                    if (s>=t2)
                                        local->tm_min = minute + atoi(intarziere_);
                                        
                                    local = localtime(&t2);
                                    snprintf(oraPlecareEstimata, sizeof(oraPlecareEstimata), "%02d", local->tm_hour);
                                    printf("Plecare: ora: %d, minutele: %d\n", local->tm_hour, local->tm_min);

                                }


                                if ((indicator == 1 && ora[0] == oraSosireEstimata[0] && ora[1] == oraSosireEstimata[1]) ||
                                    (indicator == 2 && ora[0] == oraPlecareEstimata[0] && ora[1] == oraPlecareEstimata[1]))
                                {
                                    t.addTrain(train);
                                }
                            }
                        }
                        nodeStation = nodeStation->next;
                    }
                }
                nodeTrain = nodeTrain->next;
            }
        }

        return t;
    }

    char *modifyXML(int indicator, char modification[], char trainId[], char givenStation[])
    {

        char *message = new char[100];
        strcpy(message, "Nu s-a facut nicio modificare\n");
        char childNodeName[20];
        if (indicator == -1)
        {
            strcpy(childNodeName, "inainte");
        }
        else if (indicator == 1)
        {
            strcpy(childNodeName, "intarziere");
        }
        char expr[100] = "//tren[@tren_Id='";
        strcat(expr, trainId);
        strcat(expr, "']");

        // char expr[100] = "//tren[@tren_Id='";
        // strcat(expr, trainId);
        // strcat(expr, "']/statie[@nume_statie='");
        // strcat(expr, givenStation);
        // strcat(expr, "']/");
        // strcat(expr, childNodeName);
        // sprintf(expr, "//tren[@tren_Id='%s']/statie[@nume_statie='%s']/%s", trainId, givenStation, childNodeName);

        xmlChar *xpath = BAD_CAST expr;
        xmlXPathContextPtr context = xmlXPathNewContext(information);
        if (context == NULL)
        {
            perror("Eroare la crearea contextului\n");
            exit(20);
        }
        xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath, context);
        xmlXPathFreeContext(context);

        if (result == NULL)
        {
            perror("Eroare la evaluarea expresiei\n");
            exit(2);
        }
        else
        {
            if (xmlXPathNodeSetIsEmpty(result->nodesetval))
            {
                xmlXPathFreeObject(result);
                strcpy(message, "ID-ul introdus este gresit\n");
                return message;
            }
            else
            {
                xmlNodePtr node = result->nodesetval->nodeTab[0];
                int gasit = 0;
                int ok = 0;
                for (xmlNodePtr nodeStation = node->children; nodeStation; nodeStation = nodeStation->next)
                {
                    char tag2[20];
                    snprintf(tag2, sizeof(tag2), "%s", nodeStation->name);
                    if (strcmp(tag2, "statie") == 0)
                    {
                        char nameStation[20];
                        snprintf(nameStation, sizeof(nameStation), "%s", xmlGetProp(nodeStation, (const xmlChar *)"nume_statie"));
                        if (strcmp(nameStation, givenStation) == 0)
                        {
                            gasit = 1;
                        }
                        xmlNodePtr detailNode = nodeStation->xmlChildrenNode;
                        time_t timeModel = time(nullptr);
                        while (detailNode != NULL)
                        {
                            char tag3[20];
                            snprintf(tag3, sizeof(tag3), "%s", detailNode->name);
                            time_t anteriorPlecare;
                            char detailValue[20];
                            snprintf(detailValue, sizeof(detailValue), "%s", xmlNodeGetContent(detailNode));
                            if (gasit == 1)
                            {
                                if (indicator == -1)
                                {
                                    if (strcmp(tag3, "inainte") == 0 && strcmp(givenStation, nameStation) == 0)
                                    {
                                        xmlNodeSetContent(detailNode, BAD_CAST modification);
                                    }
                                    else if (strcmp(tag3, "intarziere") == 0)
                                    {
                                        xmlNodeSetContent(detailNode, BAD_CAST "0");
                                    }
                                    else if (strcmp(tag3, "sosire") == 0 && strcmp(givenStation, nameStation) == 0 && ok == 1)
                                    {
                                        char value[20];
                                        snprintf(value, sizeof(value), "%s", xmlNodeGetContent(detailNode));
                                        int minute = atoi(value + 3);
                                        value[2] = '\0';
                                        int ora = atoi(value);
                                        time_t t = timeModel;
                                        tm *local = localtime(&t);
                                        local->tm_min = minute - atoi(modification);
                                        local->tm_hour = ora;
                                        t = mktime(local);
                                        if (t < anteriorPlecare)
                                        {
                                            strcpy(message, "Nu puteti face modificari decat in limitele reale\n");
                                            return message;
                                        }
                                        else
                                        {
                                            local = localtime(&t);
                                        }
                                    }
                                }
                                else if (indicator == 1)
                                {
                                    if (strcmp(tag3, "inainte") == 0)
                                    {
                                        xmlNodeSetContent(detailNode, BAD_CAST "0");
                                    }
                                    else if (strcmp(tag3, "intarziere") == 0)
                                    {
                                        xmlNodeSetContent(detailNode, BAD_CAST modification);
                                    }
                                }
                            }
                            else if (strcmp(tag3, "plecare") == 0)
                            {
                                ok = 1;
                                char value[20];
                                snprintf(value, sizeof(value), "%s", xmlNodeGetContent(detailNode));
                                int minute = atoi(value + 3);
                                value[2] = '\0';
                                int ora = atoi(value);
                                printf("Ora plecare: %d, minutele: %d\n", ora, minute);
                                time_t t2 = timeModel;
                                tm *local = localtime(&t2);
                                local->tm_min = minute;
                                local->tm_hour = ora;
                                anteriorPlecare = mktime(local);
                                t2 = anteriorPlecare;
                                local = localtime(&t2);
                            }

                            detailNode = detailNode->next;
                        }
                    }
                }
                xmlXPathFreeObject(result);
                if (!gasit)
                {
                    strcpy(message, "Nu exista statia introdusa\n");
                    return message;
                }
            }
        }

        // xmlSaveFormatFileEnc("-", information, "UTF-8", 1);
        if (xmlSaveFormatFileEnc(pathout, information, "UTF-8", 1) == -1)
        {
            perror("Nu s-a putut salva fisierul xml\n");
            exit(5);
        }
        else
        {
            strcpy(message, "Modificarea a fost facuta cu succes!\n");
        }

        return message;
    }
};
