#include <iostream>
#include "lin_list.h"
using namespace std;
int main() {

    SeqList<int> list(10);
    LinkList<int> linklist;
    linklist.add(1);
    linklist.insert(0,2);
    linklist[0]=3;
    cout<<linklist[0]<<endl;
    cout<<linklist.size()<<endl;

    
    return 0;
}