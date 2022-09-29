
#include <iostream>
#include <unordered_set>
#include <vector>
#include <queue>
#include <unordered_map>
#include "wikiscraper.h"


using std::cout;            using std::endl;
using std::string;          using std::vector;
using std::priority_queue;  using std::unordered_map;
using std::unordered_set;




/*
 * This is the function you will be implementing. It takes
 * two string representing the names of a start_page and
 * end_page and is supposed to return a ladder, represented
 * as a vector<string>, of links that can be followed from
 * start_page to get to the end_page.
 *
 * For the purposes of this algorithm, the "name" of a Wikipedia
 * page is what shows at the end of the URL when you visit that page
 * in your web browser. For ex. the name of the Stanford University
 * Wikipedia page is "Stanford_University" since the URL that shows
 * in your browser when you visit this page is:
 *
 *       https://en.wikipedia.org/wiki/Stanford_University
 */
struct element {
    int common;
    vector<string> ladder;
};

int getCommon(unordered_set<string>& mid, unordered_set<string>& end) {
    int ret = 0;
    for (auto &ele : end) {
        if (mid.count(ele)) {
            ret++;
        }
    }
    return ret;
}

vector<string> findWikiLadder(const string& start_page, const string& end_page) {
    WikiScraper wiki;
    auto end_set = wiki.getLinkSet(end_page);
    auto start_set = wiki.getLinkSet(start_page);
    unordered_set<string> page_cache;
    // compare method
    auto comp = [] (element &a, element &b) {
      return a.common < b.common;
    };
    priority_queue<element, vector<element>, decltype(comp)> queue(comp);
    // enqueue the start element
    element start{0, {start_page}};
    start.common = getCommon(start_set, end_set);
    queue.push(start);
    page_cache.insert(start_page);
    while (!queue.empty()) {
        auto tmp = queue.top();
        queue.pop();
        auto tmp_set = wiki.getLinkSet(tmp.ladder.back());
        if (tmp_set.count(end_page)) {
            tmp.ladder.push_back(end_page);
            return tmp.ladder;
        }
        for (auto & page : tmp_set) {
            if (page_cache.count(page)) continue;
            element new_page{0, tmp.ladder};
            new_page.ladder.push_back(page);
            auto new_set = wiki.getLinkSet(page);
            new_page.common = getCommon(new_set, end_set);
            queue.push(new_page);
        }
    }


    return {};
}



int main() {
    auto ladder = findWikiLadder("Milkshake", "Gene");
    cout << endl;

    if(ladder.empty()) {
        cout << "No ladder found!" << endl;
    } else {
        cout << "Ladder found:" << endl;
        cout << "\t";

        // Print the ladder here!
        cout << "start" ;
        for (auto &website : ladder) {
            cout << " >> " << website ;
        }
        cout << endl;
    }

    return 0;
}




