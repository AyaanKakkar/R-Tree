#ifndef _LEAFNODE
#define _LEAFNODE
#include "treeNode.h"
#include <vector>

using namespace std;

class LeafNode : public TreeNode
{

    /*
        Used to declare a leaf node in the R-Tree
    */

private:
    // Override functions
    virtual TreeNode *createSplitNode();
    void addLeafEntry(Rectangle MBR, int32_t pointer);

    virtual void addEntryToSplitNode(int32_t entryIdx, TreeNode *splitNode, RTree *rTree);

    // Copy node
    virtual void copyNodeContent(TreeNode *node);

public:
    // Constructor
    LeafNode(int32_t maxEntries, int32_t minEntries);

    // Destructor
    ~LeafNode();

    // Override functions
    virtual int32_t insert(Rectangle MBR, int32_t pointer, RTree *rTree);

    // Query
    virtual void query(Rectangle MBR, vector<int32_t> &a, RTree *rTree);

    friend class RTree;
};

#endif