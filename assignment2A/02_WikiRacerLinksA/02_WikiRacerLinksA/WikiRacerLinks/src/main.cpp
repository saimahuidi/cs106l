#include <iostream>
#include <fstream>
#include <unordered_set>
#include <algorithm>

using std::cout;            using std::endl;
using std::string;          using std::unordered_set;

unordered_set<string> findWikiLinks(const string& page_html);

string fileToStr(std::fstream &file) {
    string ret = "";
    string line;
    while (std::getline(file, line)) {
        ret += line;
    }
    return ret;
}
int main() {
	
    /* TODO: Write code here! */

    /* Note if your file reading isn't working, please go to the
     * projects tab on the panel on the left, and in the run section,
     * uncheck the "Run in terminal" box and re-check it. This
     * should fix things.
     */
    std::fstream file("simple-invalid.txt");
    string f_str = fileToStr(file);
    unordered_set<string> get = findWikiLinks(f_str);
    for (auto &key : get) {
        cout << key << endl;
    }
    return 0;
}

unordered_set<string> findWikiLinks(const string& page_html) {
    static string wiki("\"/wiki/");
    auto first = page_html.begin(), last = page_html.end();
    unordered_set<string> ret;
    auto predicate = [] (char c) {return c != '#' && c != ':';};
    while (1) {
        first = std::search(first, last, wiki.begin(), wiki.end());
        // reach the end
        if (first == last) {
            break;
        }
        // common case
        first += wiki.size();
        auto end = std::find(first + 1, last, '\"');
        if (std::all_of(first, end, predicate)) {
            string key(first, end);
            ret.insert(key);
        }
        first = end + 1;
    }
    return ret;
}

