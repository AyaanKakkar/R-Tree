#include "leafNode.h"
#include "rtree.h"
#include <iostream>
#include <config.hpp>

using namespace std;

LeafNode::LeafNode(int32_t maxEntries, int32_t minEntries) : TreeNode(maxEntries, minEntries)
{
}

// Insert an entry into the subtree
// Returns -1 if entry successful, otherwise returns the ID of the new node that must be inserted into the parent
int32_t LeafNode::insert(Rectangle MBR, int32_t pointer, RTree *rTree)
{

    addLeafEntry(MBR, pointer);

    // Perform splitting if necessary
    if (currEntries_ > maxEntries_)
    {
        return split(rTree);
    }

    // Save changes to the disk
    rTree->saveNode(this);

    return -1;
}

// Adds an entry to the leaf
void LeafNode::addLeafEntry(Rectangle MBR, int32_t pointer)
{
    // Call add Entry
    addEntry(MBR, pointer);
}

void LeafNode::addEntryToSplitNode(int32_t entryIdx, TreeNode *splitNode, RTree *rTree)
{
    ((LeafNode *)splitNode)->addEntry(MBR_[entryIdx], childPointers_[entryIdx]);
}

void LeafNode::copyNodeContent(TreeNode *node)
{
    LeafNode *temp = (LeafNode *)node;
    currEntries_ = temp->currEntries_;

    // Copy MBR
    currentMBR_ = temp->currentMBR_;

    for (int32_t idx = 0; idx < currEntries_; idx++)
    {
        MBR_[idx] = temp->MBR_[idx];
        childPointers_[idx] = temp->childPointers_[idx];
    }
}

// Create a new split node
TreeNode *LeafNode::createSplitNode()
{
    return new LeafNode(maxEntries_, minEntries_);
}

// Query
void LeafNode::query(Rectangle MBR, vector<int32_t> &a, RTree *rTree)
{

    Rectangle b = Rectangle::intersection(currentMBR_, MBR);

    if (b.isValid())
    {
        for (int i = 0; i < currEntries_; i++)
        {

            Rectangle inte = Rectangle::intersection(MBR, MBR_[i]);

            if (inte.isValid())
            {
                a.push_back(i);
            }

            break;
        }
    }
}

LeafNode::~LeafNode()
{
}