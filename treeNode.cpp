#include "treeNode.h"
#include "rtree.h"
#include <iostream>
#include <vector>
#include <config.hpp>

using namespace std;

// Constructor
TreeNode::TreeNode(int32_t maxEntries, int32_t minEntries)
{

    // Initialize values
    nodeID_ = -1; // This would be the page where the node is stored
    maxEntries_ = maxEntries;
    minEntries_ = minEntries;

    currEntries_ = 0;

    // Initialzie MBRs
    currentMBR_ = Rectangle();

    MBR_ = new Rectangle[maxEntries_ + 1];

    // Initialize pointers
    childPointers_ = new int32_t[maxEntries_ + 1];
}

// Insert an entry into the subtree
// Returns -1 if entry successful, otherwise returns the ID of the new node that must be inserted into the parent
int32_t TreeNode::insert(Rectangle MBR, int32_t pointer, RTree *rTree)
{

    // Update the MBR
    currentMBR_ = Rectangle::combine(currentMBR_, MBR);

    // Calculate the index of the child to traverse to
    int32_t childIdx = -1;
    long double minAreaEnlargement = 0;
    long double childArea = 0;

    for (int32_t index = 0; index < currEntries_; index++)
    {

        long double areaEnlargment = MBR_[index].getAreaEnlargement(MBR);
        long double area = MBR_[index].getArea();

        if (childIdx == -1 || areaEnlargment < minAreaEnlargement || areaEnlargment == minAreaEnlargement && childArea > area)
        {
            childIdx = index;
            minAreaEnlargement = areaEnlargment;
            childArea = area;
        }
    }

    // Get that child node
    TreeNode *childNode = rTree->getNode(childPointers_[childIdx]);

    // Insert into that node
    int32_t newNodeID = childNode->insert(MBR, pointer, rTree);

    // Update that node's MBR
    MBR_[childIdx] = childNode->currentMBR_;

    // Perform addition if necessary
    if (newNodeID != -1)
    {

        // Get the new node
        TreeNode *newNode = rTree->getNode(newNodeID);

        // Add that node info
        addEntry(newNode->currentMBR_, newNodeID);

        // Free the memory
        delete newNode;
    }

    // Free memory
    delete childNode;

    // Perform splitting if necessary and return the new node formed
    if (currEntries_ > maxEntries_)
    {
        return split(rTree);
    }

    // Save changes to the disk
    rTree->saveNode(this);

    return -1;
}

// Add an entry in the node
void TreeNode::addEntry(Rectangle MBR, int32_t pointer)
{

    // Update MBRs
    MBR_[currEntries_] = MBR;

    if (currEntries_ == 0)
    {
        currentMBR_ = MBR_[currEntries_];
    }
    else
    {
        currentMBR_ = Rectangle::combine(currentMBR_, MBR_[currEntries_]);
    }

    childPointers_[currEntries_] = pointer;
    currEntries_++;
}

// Split the node and return the ID of the new node
int32_t TreeNode::split(RTree *rTree)
{

    // Using linear splitting;

    int32_t index1 = 0;
    int32_t index2 = 1;
    long double maxDistance = Rectangle::distance(MBR_[index1], MBR_[index2]);

    // Loop over all possilbe pairs and find the pair with the largest distance
    for (int32_t i = 0; i < currEntries_; i++)
    {
        for (int32_t j = i + 1; j < currEntries_; j++)
        {
            long double distance = Rectangle::distance(MBR_[i], MBR_[j]);
            if (distance > maxDistance)
            {
                index1 = i;
                index2 = j;
            }
        }
    }

    // Create 2 new nodes
    TreeNode *splitNode1 = createSplitNode();
    TreeNode *splitNode2 = createSplitNode();

    // Add Entries in index1 and index2
    addEntryToSplitNode(index1, splitNode1, rTree);
    addEntryToSplitNode(index2, splitNode2, rTree);

    int32_t remainingEntries = currEntries_ - 2;

    // Iterate over all remaining entries
    for (int32_t idx = 0; idx < currEntries_; idx++)
    {
        if (idx != index1 && idx != index2)
        {
            // Choose a node, and add the entry to that node
            addEntryToSplitNode(idx, chooseSplitNode(idx, splitNode1, splitNode2, remainingEntries), rTree);
            remainingEntries--;
        }
    }

    // Transfer the contents of one node to the current node
    copyNodeContent(splitNode1);

    // Save the changes to the disk
    rTree->saveNode(splitNode2);
    rTree->saveNode(this);

    // Return ID
    int32_t returnID = splitNode2->nodeID_;

    // Free the memory
    delete splitNode1;
    delete splitNode2;

    // Return the newly creater node's ID
    return returnID;
}

// Helper function to choose node to add entry to
TreeNode *TreeNode::chooseSplitNode(int32_t entryIdx, TreeNode *splitNode1, TreeNode *splitNode2, int32_t remainingEntries)
{
    // Check if minimum criteria needs to be satisfied
    if (splitNode1->currEntries_ == minEntries_ - remainingEntries)
    {
        return splitNode1;
    }
    else if (splitNode2->currEntries_ == minEntries_ - remainingEntries)
    {
        return splitNode2;
    }
    else
    {
        // Calculate area enlargment
        long double enlargementNode1 = splitNode1->currentMBR_.getAreaEnlargement(MBR_[entryIdx]);
        long double enlargementNode2 = splitNode2->currentMBR_.getAreaEnlargement(MBR_[entryIdx]);

        if (enlargementNode1 > enlargementNode2)
        {
            return splitNode2;
        }
        else if (enlargementNode2 > enlargementNode1)
        {
            return splitNode1;
        }
        else
        {
            // If they are equal then assign on the basis of area
            long double areaNode1 = splitNode1->currentMBR_.getArea();
            long double areaNode2 = splitNode2->currentMBR_.getArea();

            if (areaNode1 > areaNode2)
            {
                return splitNode2;
            }
            else if (areaNode2 > areaNode1)
            {
                return splitNode1;
            }
            else
            {
                // Assign on the basis of number of entries
                if (splitNode1->currEntries_ > splitNode2->currEntries_)
                {
                    return splitNode2;
                }
                else
                {
                    return splitNode1;
                }
            }
        }
    }
}

// Create a split node
TreeNode *TreeNode::createSplitNode()
{
    return new TreeNode(maxEntries_, minEntries_);
}

// Add entry to a split node
void TreeNode::addEntryToSplitNode(int32_t entryIdx, TreeNode *splitNode, RTree *rTree)
{
    splitNode->addEntry(MBR_[entryIdx], childPointers_[entryIdx]);
}

// Copy content from a node
void TreeNode::copyNodeContent(TreeNode *node)
{
    currEntries_ = node->currEntries_;

    // Copy MBR
    currentMBR_ = node->currentMBR_;

    for (int32_t idx = 0; idx < currEntries_; idx++)
    {
        MBR_[idx] = node->MBR_[idx];
        childPointers_[idx] = node->childPointers_[idx];
    }
}

// Destructor
TreeNode::~TreeNode()
{
    delete[] MBR_;
    delete[] childPointers_;
}

// Query
void TreeNode::query(Rectangle MBR, vector<int32_t> &a, RTree *rTree)
{

    Rectangle b = Rectangle::intersection(currentMBR_, MBR);

    if (b.isValid())
    {
        for (int i = 0; i < currEntries_; i++)
        {
            Rectangle x = Rectangle::intersection(MBR_[i], MBR);
            if (x.isValid())
            {
                TreeNode *node = rTree->getNode(childPointers_[i]);
                node->query(MBR, a, rTree);
                delete node;
            }
        }
    }
}
