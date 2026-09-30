#include <sstream>
#include "book.h"
#include "util.h"
// Book constructor
Book::Book(const std::string& name, double price, int qty, const std::string& isbn, const std::string& author)
    : Product("book", name, price, qty),
      isbn_(isbn),
      author_(author)
{
}
// Returns the appropriate keywords for Book products
std::set<std::string> Book::keywords() const
{
    std::set<std::string> result;

    // Add keywords from product name
    std::set<std::string> nameWords = parseStringToWords(name_);
    result.insert(nameWords.begin(), nameWords.end());

    // Add keywords from author
    std::set<std::string> authorWords = parseStringToWords(author_);
    result.insert(authorWords.begin(), authorWords.end());

    // Add ISBN exactly as a keyword
    result.insert(isbn_);

    return result;
}
// Returns a string to display the product info for hits of the search
std::string Book::displayString() const
{
    std::ostringstream oss;
    oss << name_ << "\n"
        << "Author: " << author_ << " ISBN: " << isbn_ << "\n"
        << price_ << " " << qty_ << " left.";
    return oss.str();
}
// Outputs the product info in the database format
void Book::dump(std::ostream& os) const
{
    os << "book\n"
       << name_ << "\n"
       << price_ << "\n"
       << qty_ << "\n"
       << isbn_ << "\n"
       << author_ << "\n";
}