#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
#include <deque>

struct Node {
    std::string name;
    std::deque<Node> child;
};

#endif //_CONFIG_Hy