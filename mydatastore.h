#ifndef MYDATASTORE_H
#define MYDATASTORE_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include "datastore.h"
#include "product.h"
#include "user.h"

class MyDataStore : public DataStore {
public:
    MyDataStore();
    ~MyDataStore();

    // Required DataStore functions
    void addProduct(Product* p) override;
    void addUser(User* u) override;
    std::vector<Product*> search(std::vector<std::string>& terms, int type) override;
    void dump(std::ostream& os) override;

    // Extra helper functions for cart/menu behavior
    bool addProductToUserCart(const std::string& username, Product* product);
    bool viewCart(const std::string& username) const;
    bool buyCart(const std::string& username);

private:
    std::vector<Product*> products_;
    std::map<std::string, User*> users_;
    std::map<std::string, std::set<Product*>> keywordIndex_;
    std::map<std::string, std::vector<Product*> > carts_;
};

#endif