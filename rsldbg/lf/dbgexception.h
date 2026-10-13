#ifndef DBGEXCEPTION_H
#define DBGEXCEPTION_H

#include <string>

class dbgexception
{
public:
    ~dbgexception ();
    virtual const char* what () const;
};

class bdgnotsupported : public dbgexception
{
public:
    const char* what () const;
};

// ----------------------------------------------------------

class dbgverexception : public dbgexception
{
private:
    int reqMJver;
    int reqMIver;

    std::string errStr;
public:
    dbgverexception (int reqMJver_, int reqMIver_);

    const char* what () const;

    int GetReqMJVer () const;
    int GetReqMIVer () const;
};

// ----------------------------------------------------------

class dbglogicerror : public dbgexception
{
private:
    std::string errString;
public:
    dbglogicerror (const std::string& err);
    const char* what () const;
};

#endif // DBGEXCEPTION_H
