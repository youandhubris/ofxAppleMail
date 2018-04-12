#include "ofxAppleMail.h"

//--------------------------------------------------------------
ofxAppleMail::ofxAppleMail()
{
}

//--------------------------------------------------------------
ofxAppleMail::~ofxAppleMail()
{
    stopThread();
}

//--------------------------------------------------------------
void ofxAppleMail::setup(string from)
{
    setup(from, "", 1);
}

//--------------------------------------------------------------
void ofxAppleMail::setup(string from, string signature)
{
    setup(from, signature, 1);
}

//--------------------------------------------------------------
void ofxAppleMail::setup(string from, int delaySecondsBetweenAttachments)
{
    setup(from, "", delaySecondsBetweenAttachments);
}

//--------------------------------------------------------------
void ofxAppleMail::setup(string from, string signature, int delaySecondsBetweenAttachments)
{
    theFrom = combineToOneLine(from);
    
    if (signature != "") ofLogWarning("ofxAppleMail", "Signature is disabled on macOS 10.12+ because there's a know bug with AppleScript. Please setup default signature on Mail. More info: https://stackoverflow.com/questions/39860859/setting-message-signature-of-outgoing-message-using-applescript");
    theSignature = combineToOneLine(signature);
    
    theDelay = ofToString(delaySecondsBetweenAttachments);
    
    startThread();
}

//--------------------------------------------------------------
void ofxAppleMail::composeMessage(string subject, string content)
{
    vector <string> attachments = {};
    composeMessage(subject, content, attachments);
}

//--------------------------------------------------------------
void ofxAppleMail::composeMessage(string subject, string content, vector<string> absoluteAttachsPath)
{
    theSubject = combineToOneLine(subject);
    theContent = combineToOneLine(content);

    vector<string> attachmentsParsed = parsePath(absoluteAttachsPath);
    theAttachments = combineToOneLine(attachmentsParsed);
}

//--------------------------------------------------------------
void ofxAppleMail::send(vector<string> to, vector<string> cc, vector<string> bcc)
{
    theTos = combineToOneLine(to);
    theCcs = combineToOneLine(cc);
    theBccs = combineToOneLine(bcc);
    
    sendMail = true;
}

//--------------------------------------------------------------
string ofxAppleMail::combineToOneLine(string s)
{
    vector<string> data = {s};
    return combineToOneLine(data);
}

//--------------------------------------------------------------
string ofxAppleMail::combineToOneLine(vector<string> data)
{
    string combined = "";
    
    for (int i = 0; i < data.size(); i++)
    {
        combined += "\"";
        combined += data[i];
        combined += "\"";
        if (i < data.size() - 1) combined += ", ";
    }
    
    return combined;
}

//--------------------------------------------------------------
vector<string> ofxAppleMail::parsePath(vector<string> path)
{
    vector<string> parsed = {};
    
    for (int i = 0; i < path.size(); i++)
    {
        string p = path[i];
        ofStringReplace(p, "/", ":");
        if (p[0] == ':') p.erase(p.begin());
        parsed.push_back(p);
    }
    
    return parsed;
}

//--------------------------------------------------------------
void ofxAppleMail::threadedFunction()
{
    while(isThreadRunning())
    {
        if(lock())
        {
            if (sendMail)
            {
                send(theFrom, theTos, theCcs, theBccs, theSubject, theContent, theSignature, theAttachments, theDelay);
                
                sendMail = false;
            }
            
            unlock();
            
            
        }
        else ofLogWarning("ofxAppleMail") << "Unable to lock mutex.";
    }
}

//--------------------------------------------------------------
void ofxAppleMail::send(string theFrom, string theTos, string theCcs, string theBccs, string theSubject, string theContent, string theSignature, string theAttachments, string theDelay)
{
    string scriptString = "";

    scriptString = "osascript";
    scriptString += " -e 'tell application \"Mail\"'";

    scriptString += " -e     'set theFrom to " + theFrom + "'";
    scriptString += " -e     'set theTos to {" + theTos + "}'";
    scriptString += " -e     'set theCcs to {" + theCcs + "}'";
    scriptString += " -e     'set theBccs to {" + theBccs + "}'";

    scriptString += " -e     'set theSubject to " + theSubject + "'";
    scriptString += " -e     'set theContent to " + theContent + "'";
    scriptString += " -e     'set theSignature to " + theSignature + "'";
    scriptString += " -e     'set theAttachments to {" + theAttachments + "}'";
    scriptString += " -e     'set theDelay to " + theDelay + "'";

    scriptString += " -e     'set theMessage to make new outgoing message with properties {sender:theFrom, subject:theSubject, content:theContent, visible:false}'";
    scriptString += " -e     'tell theMessage'";
    scriptString += " -e         'repeat with theTo in theTos'";
    scriptString += " -e             'make new recipient at end of to recipients with properties {address:theTo}'";
    scriptString += " -e         'end repeat'";
    scriptString += " -e         'repeat with theCc in theCcs'";
    scriptString += " -e             'make new cc recipient at end of cc recipients with properties {address:theCc}'";
    scriptString += " -e         'end repeat'";
    scriptString += " -e         'repeat with theBcc in theBccs'";
    scriptString += " -e             'make new bcc recipient at end of bcc recipients with properties {address:theBcc}'";
    scriptString += " -e         'end repeat'";
    scriptString += " -e         'repeat with theAttachment in theAttachments'";
    scriptString += " -e             'make new attachment with properties {file name:theAttachment as alias} at after last paragraph'";
    scriptString += " -e             'delay theDelay'";
    scriptString += " -e         'end repeat'";
    scriptString += " -e     'end tell'";
    
    // BUG
    // scriptString += " -e     'set message signature of theMessage to signature theSignature'";
    
    scriptString += " -e     'send theMessage'";

    scriptString += " -e 'end tell'";

    ofSystem(scriptString);
}

////--------------------------------------------------------------
//void ofxAppleMail::checkMailboxes()
//{
//    string scriptString = "";
//
//    scriptString += "osascript -e 'tell application \"Mail\"\n";
//
//    // Local Mailboxes
//    scriptString += "set localMailboxes to every mailbox\n";
//    scriptString += "if (count of localMailboxes) is greater than 0 then\n";
//
//        // For each mailbox get Name & Count
//        scriptString += "set messageCountDisplay to \"\"\n";
//        scriptString += "repeat with eachMailbox in localMailboxes\n";
//            scriptString += "set mailboxName to name of eachMailbox\n";
//            scriptString += "set messageCount to (count of (messages of eachMailbox)) as string\n";
//            scriptString += "set messageCountDisplay to messageCountDisplay & mailboxName & \" \" & messageCount & \" \"\n";
//        scriptString += "end repeat\n";
//    scriptString += "else\n";
//        scriptString += "set messageCountDisplay to \"Empty\"\n";
//    scriptString += "end if\n";
//
//    // To get the names of the accounts, do:
//    // get every account
//    scriptString += "set accountInUse to account \"edpventosender\"\n";
//    scriptString += "set accountMailboxes to every mailbox of accountInUse\n";
//    scriptString += "if (count of accountMailboxes) is greater than 0 then\n";
//    // For each mailbox get Name & Count
//        scriptString += "repeat with eachMailbox in accountMailboxes\n";
//            scriptString += "set mailboxName to name of eachMailbox\n";
//            scriptString += "set messageCount to (count of (messages of eachMailbox)) as string\n";
//            scriptString += "set messageCountDisplay to messageCountDisplay & mailboxName & \" \" & messageCount & \" \"\n";
//        scriptString += "end repeat\n";
//    scriptString += "end if\n";
//
//    // Return info to clipboard
//    scriptString += "set the clipboard to messageCountDisplay\n";
//    scriptString += "end tell'\n";
//
//    const char *scriptChar = scriptString.c_str();
//    system(scriptChar);
//
//    clipboardMail = appWindow.getClipboardString();
//    if (clipboardMail != "") parseMailboxes();
//}
//
////--------------------------------------------------------------
//vector <string> ofxAppleMail::listMailboxes()
//{
//    ofLogNotice("ofxAppleMail", "Mailboxes list:");
//    for (int i = 0; i < mailboxesName.size(); i++) {
//        ofLogNotice("ofxAppleMail", mailboxesName[i] + ": " + ofToString(mailboxesCount[i]));
//    }
//    return mailboxesName;
//}
//
////--------------------------------------------------------------
//int ofxAppleMail::getMailboxCount(const string &mailboxName)
//{
//    for (int i = 0; i < mailboxesCount.size(); i++) {
//        if (mailboxesName[i] == mailboxName) return mailboxesCount[i];
//    }
//}
//



////--------------------------------------------------------------
//void ofxAppleMail::parseMailboxes()
//{
//    mailboxesName.clear();
//    mailboxesCount.clear();
//
//    string mailboxName = "";
//    for (int i = 0; i < clipboardMail.length(); i++) {
//
//        if (isdigit(clipboardMail[i])) {
//
//            string digitsString = "";
//            digitsString += clipboardMail[i];
//
//            for (int j = i + 1; j < clipboardMail.length(); j++) {
//                if (isdigit(clipboardMail[j])) {
//                    digitsString += clipboardMail[j];
//                } else {
//                    i = j;
//                    break;
//                }
//            }
//
//            mailboxesCount.push_back(ofToInt(digitsString));
//            mailboxesName.push_back(mailboxName);
//            mailboxName = "";
//        }
//        else if (clipboardMail[i] != ' ') mailboxName += clipboardMail[i];
//    }
//}

