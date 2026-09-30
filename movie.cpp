#include <sstream>
#include "movie.h"
#include "util.h"

Movie::Movie(const std::string& name, double price, int qty,
             const std::string& genre, const std::string& rating)
    : Product("movie", name, price, qty),
      genre_(genre),
      rating_(rating)
{
}

std::set<std::string> Movie::keywords() const
{
    std::set<std::string> result;

    // Add keywords from the movie name
    std::set<std::string> nameWords = parseStringToWords(name_);
    result.insert(nameWords.begin(), nameWords.end());

    // Add the genre as a keyword exactly as written
    result.insert(genre_);

    return result;
}

std::string Movie::displayString() const
{
    std::ostringstream oss;
    oss << name_ << "\n"
        << "Genre: " << genre_ << " Rating: " << rating_ << "\n"
        << price_ << " " << qty_ << " left.";
    return oss.str();
}

void Movie::dump(std::ostream& os) const
{
    os << "movie\n"
       << name_ << "\n"
       << price_ << "\n"
       << qty_ << "\n"
       << genre_ << "\n"
       << rating_ << "\n";
}