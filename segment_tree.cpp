#include <iostream>
using namespace std;

/*
*   This is an implementation of the Segment Tree.
*   It supports point update, interval update, and interval query (of sum, can be modified to query min/max, etc.) all in O(logN) time.
*   It is a complete binary tree where a node represents a range, and its chilren represent half of that range each.
*   A leaf node represents a single point.
*/

class SegmentTree {
    int _left_bound, _right_bound; // Describes the range.
    
    // We represent the tree structure using a vector. 
    // This is convenient since a segment tree is a complete binary tree.
    // A node indexed i has its children at indices i * 2, i * 2 + 1.
    vector<int> _tree;
    
    // We maintain a lazy tag used in range update for efficiency.
    // ex. Update +3 to range (5,8). 
    // Once we update the node value, we update the lazy tag instead of pushing the update down to its children.
    // Lazy tag should be converted to lower node values only when necessary.
    vector<int> _tags;
    
    // Calculates the space needed for initialization.
    int _space_needed() {
        // It is always safe to allocate 4 * range space. TODO: optimize using more precise calculation (take log, ...)
        return 4 * (_right_bound - _left_bound + 1);
    }
    
    // Recursively builds the tree. Each call initializes the node indexed (and its children).
    void _build(int idx, int left, int right, const vector<int>& vec) {
        if (left == right) {
            _tree[idx] = vec[left];
            return;
        }
        
        int mid = left + (right - left) / 2;
        _build(idx*2, left, mid, vec);
        _build(idx*2+1, mid+1, right, vec);
        _tree[idx] = _tree[idx*2] + _tree[idx*2+1];
    }
    
    /* 
     *  Recursively add a value to each point of the range.
     *  @param value: the value to be updated.
     *  @param idx: the index of the current node.
     *  @param left, right: range of query.
     *  @param left_bound, right_bound: range of the current node.
    */
    void _add(int value, int idx, int left, int right, int left_bound, int right_bound) {
        
        // The range queried covers the range of the node.
        // We are supposed to update its children, but instead we update the lazy tag.
        if (left <= left_bound && right_bound <= right) {
            _tree[idx] += value * (right_bound - left_bound + 1);
            _tags[idx] += value;
            return;
        }
        
        int mid = left_bound + (right_bound - left_bound) / 2;
        
        // Push stacked updates (lazy tag) down.
        if (_tags[idx] != 0 && left_bound != right_bound) {
            _tags[idx*2] += _tags[idx]; 
            _tags[idx*2+1] += _tags[idx];
            
            _tree[idx*2] += _tags[idx] * (mid - left_bound + 1);
            _tree[idx*2+1] += _tags[idx] * (right_bound - mid);
                
            _tags[idx] = 0;
        }
        
        // Overlap at left child.
        if (left <= mid) {
            _add(value, idx*2, left, right, left_bound, mid);
        }
            
        // Overlap at right child.
        if (right >= mid+1) {
            _add(value, idx*2+1, left, right, mid+1, right_bound);
        }
        
        _tree[idx] = _tree[idx*2] + _tree[idx*2+1];
        
    }
    
    /* 
     *  Recursively gets the sum of the range.
     *  @param idx: the index of the current node.
     *  @param left, right: range of query.
     *  @param left_bound, right_bound: range of the current node.
    */
    int _get_sum(int idx, int left, int right, int left_bound, int right_bound) {
        // The range queried covers the range of the node. No need to look further down.
        if (left <= left_bound && right_bound <= right) {
            return _tree[idx];
        }
        
        int mid = left_bound + (right_bound - left_bound) / 2, sum = 0;
        
        // Push stacked updates (lazy tag) down.
        if (_tags[idx] != 0 && left_bound != right_bound) {
            _tags[idx*2] += _tags[idx]; 
            _tags[idx*2+1] += _tags[idx];
            
            _tree[idx*2] += _tags[idx] * (mid - left_bound + 1);
            _tree[idx*2+1] += _tags[idx] * (right_bound - mid);
                
            _tags[idx] = 0;
        }
        
        // Overlap at left child.
        if (left <= mid) {
            sum += _get_sum(idx*2, left, right, left_bound, mid);
        }
            
        // Overlap at right child.
        if (right >= mid+1) {
            sum += _get_sum(idx*2+1, left, right, mid+1, right_bound);
        }
        return sum;
    }
    
public:
    // Initializes as an empty tree with a range.
    SegmentTree(int left_bound, int right_bound) {
        _left_bound = left_bound; 
        _right_bound = right_bound;
        _tree = vector<int>(_space_needed(), 0); 
        _tags = vector<int>(_space_needed(), 0); 
    }
    
    // Initializes using a vector.
    SegmentTree(const vector<int>& vec) {
        _left_bound = 0; 
        _right_bound = vec.size()-1;
        _tree = vector<int>(_space_needed(), 0);
        _tags = vector<int>(_space_needed(), 0); 
        _build(1, _left_bound, _right_bound, vec);
    }
    
    // Gets the sum of range [left, right].
    int get_sum(int left, int right) {
        return _get_sum(1, left, right, _left_bound, _right_bound);
    }
    
    // Adds a valud to range [left, right].
    void add(int value, int left, int right) {
        _add(value, 1, left, right, _left_bound, _right_bound);
    }
};

int main() {
    // Let's create a segment tree and try some of its features.
    SegmentTree segtree(1, 50);
    cout<<"Sum of range [1,50] = "<<segtree.get_sum(2,5)<<endl;
    
    // Add 1 to position 1.
    segtree.add(1,1,1);
    cout<<"Sum of range [2,5] = "<<segtree.get_sum(2,5)<<endl;
    cout<<"Sum of range [1,2] = "<<segtree.get_sum(1,2)<<endl;
    
    segtree.add(-1,1,1);
    cout<<"Sum of range [2,5] = "<<segtree.get_sum(2,5)<<endl;
    cout<<"Sum of range [1,2] = "<<segtree.get_sum(1,2)<<endl;
    
    // Add i to index i. The original distribution would look like [1,2,3,4,...,50].
    for (int i=1; i<=50; i++) {
        segtree.add(i, i, i);
    }
    cout<<"Sum of range [2,5] = "<<segtree.get_sum(2,5)<<endl;
    cout<<"Sum of range [1,10] = "<<segtree.get_sum(1,10)<<endl;
    
    // Cancel out the update above.
    for (int i=1; i<=50; i++) {
        segtree.add(-i, i, i);
    }
    cout<<"Sum of range [2,5] = "<<segtree.get_sum(2,5)<<endl;
    cout<<"Sum of range [1,10] = "<<segtree.get_sum(1,10)<<endl;
    
    // Add 5 to range [10,23].
    segtree.add(5, 10, 23);
    cout<<"Sum of range [2,15] = "<<segtree.get_sum(2,15)<<endl;
    cout<<"Sum of range [10,26] = "<<segtree.get_sum(10,26)<<endl;
}