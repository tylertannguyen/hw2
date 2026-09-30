#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include "mydatastore.h"
#include "util.h"

using namespace std;

// Constructor
MyDataStore::MyDataStore()
{
    // starts empty
}

// Destructor
MyDataStore::~MyDataStore()
{
    for(size_t i = 0; i < products_.size(); i++) {
        delete products_[i];
    }

    for(map<string, User*>::iterator it = users_.begin(); it != users_.end(); ++it) {
        delete it->second;
    }
}

// Adds a product to the data store
void MyDataStore::addProduct(Product* p)
{
    // Check for null pointer
    if(p == NULL) {
        return;
    }
    // Add product to products_ vector
    products_.push_back(p);
    // Add product to keywordIndex_ map
    std::set<std::string> keywords = p->keywords();
    // For each keyword, add the product to the set of products 
    for(std::set<std::string>::iterator it = keywords.begin(); it != keywords.end(); ++it) {
        keywordIndex_[*it].insert(p);
    }
}

// Adds a user to the data store
void MyDataStore::addUser(User* u)
{
    // Check for null pointer
    if(u == NULL) {
        return;
    }
    // Add user to users_ map, using lowercase name as key
    std::string normalizedName = convToLower(u->getName());
    users_[normalizedName] = u;
}

// Performs a search of products whose keywords match the given "terms"
std::vector<Product*> MyDataStore::search(std::vector<std::string>& terms, int type)
{
    std::vector<Product*> result;
    // If no search terms, return empty result
    if(terms.empty()) {
        return result;
    }
    // Use a set to hold the matching products
    std::set<Product*> matches;
    // For each search term, find the matching products and update the matches set
    for(size_t i = 0; i < terms.size(); i++) {
        string term = convToLower(terms[i]);
        // Find the set of products for this term
        map<string, set<Product*>>::iterator found = keywordIndex_.find(term);
        // If the term is not found, return empty result for AND search, or continue for OR search
        if(found == keywordIndex_.end()) {
            // If AND search, return empty result
            if(type == 0) {
                return result;
            }
            // If OR search, continue to next term
            else {
                continue;
            }
        }
        // Get the set of products for this term
        std::set<Product*> currentMatches = found->second;
        // If this is the first term, initialize matches to currentMatches
        if(i == 0) {
            matches = currentMatches;
        }
        // If this is not the first term, update matches based on the search type
        else if(type == 0) {
            matches = setIntersection(matches, currentMatches);
        }
        // If this is not the first term and type is OR, update matches to be the union of matches and currentMatches
        else {
            matches = setUnion(matches, currentMatches);
        }
    }
    // Convert the set of matches to a vector for the result
    for(std::set<Product*>::iterator it = matches.begin(); it != matches.end(); ++it) {
        result.push_back(*it);
    }
    // Store the last search results 
    lastSearchResults_ = result;
    return result;
}

// Reproduce the database file from the current Products and User values
void MyDataStore::dump(std::ostream& os)
{
    // Dump products
    for(size_t i = 0; i < products_.size(); i++) {
        products_[i]->dump(os);
    }
    // Dump users
    for(map<string, User*>::iterator it = users_.begin(); it != users_.end(); ++it) {
        it->second->dump(os);
    }
}
// Adds a product to the user's cart
void MyDataStore::addProductToUserCart(const std::string& username, int hitIndex)
{
    // Normalize the username to lowercase for consistent lookup
    std::string normalizedName = convToLower(username);
    // Find the user
    std::map<std::string, User*>::iterator userIt = users_.find(normalizedName);
    // If the user is not found, return
    if(userIt == users_.end()) {
        return; 
    }
    // make sure the hit index is valid
    if(hitIndex < 0 || hitIndex >= (int)lastSearchResults_.size()) {
        return; // invalid request; menu layer can print message
    }
    // add one product pointer to this user's cart
    carts_[normalizedName].push_back(lastSearchResults_[hitIndex]);
}
// Displays the contents of the user's cart
void MyDataStore::viewCart(const std::string& username) const
{
    // Normalize the username to lowercase for consistent lookup
    std::string normalizedName = convToLower(username);
    // Find the user
    std::map<std::string, User*>::const_iterator userIt = users_.find(normalizedName);
    // If the user is not found, return
    if(userIt == users_.end()) {
        return; 
    }
    // Find the user's cart
    std::map<std::string, std::vector<Product*> >::const_iterator cartIt = carts_.find(normalizedName);
    // If the cart is not found, return
    if(cartIt == carts_.end()) {
        return; // no cart yet, so nothing to print
    }
    // Get the user's cart
    const std::vector<Product*>& cart = cartIt->second;
    // Print the contents of the cart
    for(size_t i = 0; i < cart.size(); i++) {
        std::cout << i + 1 << ": " << cart[i]->displayString() << std::endl;
    }
}
// Processes the purchase of all items in the user's cart
void MyDataStore::buyCart(const std::string& username)
{
    // Normalize the username to lowercase for consistent lookup
    std::string normalizedName = convToLower(username);
    // Find the user
    std::map<std::string, User*>::iterator userIt = users_.find(normalizedName);
    // If the user is not found, return
    if(userIt == users_.end()) {
        return; // invalid username; menu layer can print message
    }
    // Get the user object
    User* user = userIt->second;
    // Find the user's cart
    std::map<std::string, std::vector<Product*> >::iterator cartIt = carts_.find(normalizedName);
    // If the cart is not found, return
    if(cartIt == carts_.end()) {
        return;
    }
    // Get the user's cart
    std::vector<Product*> cart = cartIt->second;
    std::vector<Product*> remaining;
    // Process each product in the cart
    for(size_t i = 0; i < cart.size(); i++) {
        Product* p = cart[i];
        // Check if the product is in stock and if the user has enough balance
        if(p->getQty() > 0 && user->getBalance() >= p->getPrice()) {
            p->subtractQty(1);
            user->deductAmount(p->getPrice());
        }
        // If the product cannot be purchased, add it to the remaining vector
        else {
            remaining.push_back(p);
        }
    }
    // Update the user's cart to only contain the remaining products
    carts_[normalizedName] = remaining;
}