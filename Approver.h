#ifndef APPROVER_H
#define APPROVER_H
 
#include "PolicyManager.h"

class Approver {
public:
    void login();
    void approveProposals(int userId);
    void managePolicies(int clientId);
};
 
#endif