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
    // All containers start empty
}
// Destructor
MyDataStore::~MyDataStore()
{
    // Delete every product owned by the data store
    for(size_t i = 0; i < products_.size(); ++i) {
        delete products_[i];
    }
    // Delete every user owned by the data store
    for(map<string, User*>::iterator it = users_.begin();
        it != users_.end(); ++it) {
        delete it->second;
    }
}
// Adds a product to the data store
void MyDataStore::addProduct(Product* p)
{
    // Check for a null product pointer
    if(p == NULL) {
        return;
    }
    // Store the product pointer
    products_.push_back(p);
    // Get all searchable keywords belonging to the product
    set<string> keywords = p->keywords();
    // Add the product to the index for each of its keywords
    for(set<string>::iterator it = keywords.begin();
        it != keywords.end(); ++it) {
        // Store every keyword in lowercase for case-insensitive searching
        string normalizedKeyword = convToLower(*it);
        // Associate this product with the normalized keyword
        keywordIndex_[normalizedKeyword].insert(p);
    }
}
// Adds a user to the data store
void MyDataStore::addUser(User* u)
{
    // Check for a null user pointer
    if(u == NULL) {
        return;
    }
    // Convert the username to lowercase for case-insensitive lookup
    string normalizedName = convToLower(u->getName());
    // Store the user using the normalized username as the key
    users_[normalizedName] = u;
}
// Performs a search for products matching the given terms
vector<Product*> MyDataStore::search(vector<string>& terms, int type)
{
    // Store the final search results
    vector<Product*> result;
    // Return an empty result if no search terms were provided
    if(terms.empty()) {
        return result;
    }
    // Store the products that match the processed search terms
    set<Product*> matches;
    // Track whether matches has been initialized with a keyword result
    bool initialized = false;
    // Process every search term
    for(size_t i = 0; i < terms.size(); ++i) {
        // Normalize the search term for case-insensitive lookup
        string term = convToLower(terms[i]);
        // Look for the search term in the keyword index
        map<string, set<Product*> >::iterator found =
            keywordIndex_.find(term);
        // Handle a keyword that does not exist in the index
        if(found == keywordIndex_.end()) {
            // An AND search fails if any search term has no matches
            if(type == 0) {
                return result;
            }
            // An OR search can ignore a term that has no matches
            continue;
        }
        // Copy the products associated with the current keyword
        set<Product*> currentMatches = found->second;

        // Initialize matches with the first keyword that was found
        if(!initialized) {
            matches = currentMatches;
            initialized = true;
        }
        // Intersect the results when performing an AND search
        else if(type == 0) {
            matches = setIntersection(matches, currentMatches);
        }
        // Combine the results when performing an OR search
        else {
            matches = setUnion(matches, currentMatches);
        }
    }
    // Convert the set of matching products into a vector
    for(set<Product*>::iterator it = matches.begin();
        it != matches.end(); ++it) {
        result.push_back(*it);
    }

    // Return the matching products
    return result;
}
// Reproduces the database using the current product and user values
void MyDataStore::dump(ostream& os)
{
    // Begin the products section
    os << "<products>" << endl;
    // Output every product in database format
    for(std::vector<Product*>::size_type i = 0; i < products_.size(); ++i) {
        products_[i]->dump(os);
    }
    // End the products section
    os << "</products>" << endl;
    // Begin the users section
    os << "<users>" << endl;
    // Output every user in database format
    for(map<string, User*>::iterator it = users_.begin();
        it != users_.end(); ++it) {
        it->second->dump(os);
    }
    // End the users section
    os << "</users>" << endl;
}
// Adds a product to a user's cart
bool MyDataStore::addProductToUserCart(
    const string& username,
    Product* product)
{
    // Convert the username to lowercase for case-insensitive lookup
    string normalizedName = convToLower(username);
    // Check whether the user exists and the product pointer is valid
    if(users_.find(normalizedName) == users_.end() ||
       product == NULL) {
        return false;
    }
    // Add one occurrence of the product to the end of the user's cart
    carts_[normalizedName].push_back(product);
    // Report that the product was successfully added
    return true;
}
// Displays the contents of a user's cart
bool MyDataStore::viewCart(const string& username) const
{
    // Convert the username to lowercase for case-insensitive lookup
    string normalizedName = convToLower(username);
    // Check whether the user exists
    if(users_.find(normalizedName) == users_.end()) {
        return false;
    }
    // Look for the user's cart
    map<string, vector<Product*> >::const_iterator cartIt =
        carts_.find(normalizedName);
    // A valid user may not have created a cart yet
    if(cartIt == carts_.end()) {
        return true;
    }
    // Get a reference to the user's cart
    const vector<Product*>& cart = cartIt->second;
    // Display the cart products in FIFO order
    for(std::vector<Product*>::size_type i = 0; i < cart.size(); ++i) {
        // Display the cart products in FIFO order
        cout << "Item " << i + 1 << endl;
        cout << cart[i]->displayString() << endl;
    }
    // Report that the username was valid
    return true;
}
// Attempts to purchase every product in a user's cart
bool MyDataStore::buyCart(const string& username)
{
    // Convert the username to lowercase for case-insensitive lookup
    string normalizedName = convToLower(username);
    // Look for the user
    map<string, User*>::iterator userIt =
        users_.find(normalizedName);
    // Report an invalid username
    if(userIt == users_.end()) {
        return false;
    }
    // Look for the user's cart
    map<string, vector<Product*> >::iterator cartIt =
        carts_.find(normalizedName);
    // A valid user with no cart has nothing to purchase
    if(cartIt == carts_.end()) {
        return true;
    }
    // Get the user who is purchasing the products
    User* user = userIt->second;
    // Get a reference to the user's current cart
    vector<Product*>& cart = cartIt->second;
    // Store products that cannot be purchased
    vector<Product*> remaining;
    // Process products in the order they were added
    for(size_t i = 0; i < cart.size(); ++i) {
        // Get the next product in the cart
        Product* product = cart[i];
        // Purchase the product if it is in stock and affordable
        if(product->getQty() > 0 &&
           user->getBalance() >= product->getPrice()) {
            // Reduce the product quantity by one
            product->subtractQty(1);
            // Deduct the product price from the user's balance
            user->deductAmount(product->getPrice());
        }
        else {
            // Keep products that could not be purchased in the cart
            remaining.push_back(product);
        }
    }
    // Replace the cart with the products that were not purchased
    cart = remaining;
    // Report that the username was valid
    return true;
}