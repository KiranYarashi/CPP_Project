#ifndef CLIENT_H
#define CLIENT_H
 
class Client {
public:
    void showClientMenu();
    void createClient();
    void listClients();
    int selectClient();
    void clientDashboard(int clientId);  
    void makePayment(int clientId);
};
 
#endif