#pragma once
#include "ofMain.h"


class ofxAppleMail : public ofThread
{
    public:

    ofxAppleMail();
    ~ofxAppleMail();
    
    // SETUP
    void setup(string from);
    void setup(string from, string signature);
    void setup(string from, int delaySecondsBetweenAttachments);
    void setup(string from, string signature, int delaySecondsBetweenAttachments);

    // MESSAGE
    void composeMessage(string subject, string content);
    void composeMessage(string subject, string content, vector<string> absoluteAttachsPath);
    
    // SEND
    void send(vector<string> to, vector<string> cc, vector<string> bcc);

    
//    void checkMailboxes();
//    vector <string> listMailboxes();
//    int getMailboxCount(const string &mailboxName);
    
    private:
    
    string theFrom;
    
    string theTos;
    string theCcs;
    string theBccs;
    
    string theSubject;
    string theContent;
    string theSignature;

    string theAttachments;
    string theDelay;    
    
    string combineToOneLine(string s);
    string combineToOneLine(vector<string> data);
    vector<string> parsePath(vector<string> path);
    
    bool sendMail = false;
    void threadedFunction();
    void send(string theFrom, string theTos, string theCcs, string theBccs, string theSubject, string theContent, string theSignature, string theAttachments, string theDelay);


//    void parseMailboxes();
//
//    ofAppGLFWWindow appWindow;
//    string clipboardMail;
//    vector <string> mailboxesName;
//    vector <int> mailboxesCount;

};
