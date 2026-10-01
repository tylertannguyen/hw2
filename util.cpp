#include <iostream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include "util.h"

using namespace std;
std::string convToLower(std::string src)
{
    std::transform(src.begin(), src.end(), src.begin(), ::tolower);
    return src;
}

/** Complete the code to convert a string containing a rawWord
    to a set of words based on the criteria given in the assignment **/
std::set<std::string> parseStringToWords(string rawWords)
{
    set<string> words;
    string word;
    // Adds a separator so the last word get processed by the loop.
    rawWords += ' '; 

    for(std::string::size_type i = 0; i < rawWords.size(); ++i) {
        // Get the current character
        char c = rawWords[i];
        // A space or punctuation character indicates the end of a word. 
        if(isspace(static_cast<unsigned char>(c)) || ispunct(static_cast<unsigned char>(c))) {
            // If the word is at least 2 characters long, it is added to the set of words.
            if(word.length() >= 2) {
                words.insert(convToLower(word));
            }
            // Start fresh for the next word
            word.clear();
        }
        else {
            // Add character to word being built
            word += c;
        }
    }
    // Give back all key words found
    return words;
}

/**************************************************
 * COMPLETED - You may use the following functions
 **************************************************/

// Used from http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from start
std::string &ltrim(std::string &s) {
    s.erase(s.begin(), 
	    std::find_if(s.begin(), 
			 s.end(), 
			 std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
std::string &rtrim(std::string &s) {
    s.erase(
	    std::find_if(s.rbegin(), 
			 s.rend(), 
			 std::not1(std::ptr_fun<int, int>(std::isspace))).base(), 
	    s.end());
    return s;
}

// trim from both ends
std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}
