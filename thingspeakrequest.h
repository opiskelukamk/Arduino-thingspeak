#ifndef THINGSPEAKUPLOADREQUEST_H
#define THINGSPEAKUPLOADREQUEST_H

#ifndef __AVR__
#include <string.h>
#include <stdio.h>
#else
#include <Arduino.h>
#endif
#define MAX_FIELDS          8

#define FIELD_ID_PREFIX     "field"
#define FIELD_SEPARATOR     "&"


class ThingSpeakUploadRequest
{
public:
    //Constructor
    ThingSpeakUploadRequest();
    //Destructor
    ~ThingSpeakUploadRequest();

    //Sets the Api key for the request
    bool SetApiKey(const char * szApiKey);

    //Gets the Api key of the request
    const char * GetApiKey();

    //Sets the field data for the upload request
    bool SetFieldData(unsigned int iField, const char * szData);
    //Sets the field data for the upload request
    bool SetFieldData(unsigned int iField, const long int iData);
        //Sets the field data for the upload request
    bool SetFieldData(unsigned int iField, const int iData);
    //Sets the field data for the upload request
    bool SetFieldData(unsigned int iField, const float fData);

    //Gets the field data of the upload request
    const char * GetFieldData(unsigned int iField);

    //Converts the request object into a string
    void ToString(char * buf);
    //Constructs the request object from a string
    bool FromString(const char * buf);

    //Resets all the data
    void ResetData();



private:

    char m_szApiKey[17];
    char * m_szFieldData[MAX_FIELDS];

};

#endif // THINGSPEAKUPLOADREQUEST_H
