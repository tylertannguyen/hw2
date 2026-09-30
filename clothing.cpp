#include <sstream>
#include "clothing.h"
#include "util.h"
// Clothing constructor
Clothing::Clothing(const std::string& name, double price, int qty, const std::string& size, const std::string& brand)
    : Product("clothing", name, price, qty),
      size_(size),
      brand_(brand)
{
}
// Returns the appropriate keywords for Clothing products
std::set<std::string> Clothing::keywords() const
{
    std::set<std::string> result;

    // Add keywords from product name
    std::set<std::string> nameWords = parseStringToWords(name_);
    result.insert(nameWords.begin(), nameWords.end());

    // Add keywords from brand
    std::set<std::string> brandWords = parseStringToWords(brand_);
    result.insert(brandWords.begin(), brandWords.end());

    return result;
}
// Returns a string to display the product info for hits of the search
std::string Clothing::displayString() const
{
    std::ostringstream oss;
    oss << name_ << "\n"
        << "Size: " << size_ << " Brand: " << brand_ << "\n"
        << price_ << " " << qty_ << " left.";
    return oss.str();
}
// Outputs the product info in the database format
void Clothing::dump(std::ostream& os) const
{
    os << "clothing\n"
       << name_ << "\n"
       << price_ << "\n"
       << qty_ << "\n"
       << size_ << "\n"
       << brand_ << "\n";
}