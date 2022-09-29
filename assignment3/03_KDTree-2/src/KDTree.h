/**
 * File: KDTree.h
 * Author: (your name here)
 * ------------------------
 * TODO: fill in this file comment!
 */

#ifndef KDTREE_INCLUDED
#define KDTREE_INCLUDED

#include "Point.h"
#include "BoundedPQueue.h"
#include <stdexcept>
#include <cmath>

// "using namespace" in a header file is conventionally frowned upon, but I'm
// including it here so that you may use things like size_t without having to
// type std::size_t every time.
using namespace std;

template <size_t N, typename ElemType>
class Node {
public:
    Point<N> point;
    ElemType value;
    std::unique_ptr<Node<N, ElemType>> left;
    std::unique_ptr<Node<N, ElemType>> right;
};

template <size_t N, typename ElemType>
class KDTree {
public:
    // Constructor: KDTree();
    // Usage: KDTree<3, int> myTree;
    // ----------------------------------------------------
    // Constructs an empty KDTree.
    KDTree() : _size(0) {};
    
    // Destructor: ~KDTree()
    // Usage: (implicit)
    // ----------------------------------------------------
    // Cleans up all resources used by the KDTree.
    ~KDTree() {};
    
    // KDTree(const KDTree& rhs);
    // KDTree& operator=(const KDTree& rhs);
    // Usage: KDTree<3, int> one = two;
    // Usage: one = two;
    // -----------------------------------------------------
    // Deep-copies the contents of another KDTree into this one.
    KDTree(const KDTree& other) {
        deep_copy(&_root, &other._root);
        _size = other._size;
    }

    KDTree& operator=(const KDTree& rhs) {
        Node<N, ElemType> *p_i = _root.release();
        delete p_i;
        deep_copy(&_root, &rhs._root);
        _size = rhs._size;
        return *this;
    }

    void deep_copy(std::unique_ptr<Node<N, ElemType>> *root, const std::unique_ptr<Node<N, ElemType>> *other) {
        if (*other == nullptr) {
            return;
        }
        *root = std::make_unique<Node<N, ElemType>>();
        for (int i = 0; i < N; i++) {
            (**root).point[i] = (**other).point[i];
            (**root).value = (**other).value;
        }
        deep_copy(&((**root).left), &((**other).left));
        deep_copy(&((**root).right), &((**other).right));
    }

    // KDTree(KDTree&& rhs);
    // KDTree& operator=(KDTree&& rhs);
    // Usage: KDTree<3, int> one = std::move(two);
    // Usage: one = std::move(two);
    // -----------------------------------------------------
    // Moves the contents of another KDTree into this one,
    // leaving the other KDTree in a valid but undefined state.
    KDTree(KDTree&& other) : _size(other._size), _root(std::move(other._root)) {other._size = 0;}
    KDTree& operator=(KDTree&& rhs) {
        _size = rhs._size;
        _root = std::move(rhs._root);
        rhs._size = 0;
        return *this;
    }
    
    // size_t dimension() const;
    // Usage: size_t dim = kd.dimension();
    // ----------------------------------------------------
    // Returns the dimension of the points stored in this KDTree.
    size_t dimension() const {
        return N;
    }
    
    // size_t size() const;
    // bool empty() const;
    // Usage: if (kd.empty())
    // ----------------------------------------------------
    // Returns the number of elements in the kd-tree and whether the tree is
    // empty.
    size_t size() const {return _size;}
    bool empty() const {return _size == 0;}
    
    // bool contains(const Point<N>& pt) const;
    // Usage: if (kd.contains(pt))
    // ----------------------------------------------------
    // Returns whether the specified point is contained in the KDTree.
    bool contains(const Point<N>& pt) const {
        ElemType &value = getElem(*_root, pt, 0);
        if (&value == nullptr) {
            return false;
        }
        return true;
    }
    
    // void insert(const Point<N>& pt, const ElemType& value);
    // Usage: kd.insert(v, "This value is associated with v.");
    // ----------------------------------------------------
    // Inserts the point pt into the KDTree, associating it with the specified
    // value. If the element already existed in the tree, the new value will
    // overwrite the existing one.
    void insert(const Point<N>& pt, const ElemType& value) {
        ElemType &val = getElem(*_root, pt, 0);
        if (&val == nullptr) {
            insert_helper(pt, value, &_root, 0, _size);
            return;
        }
        val = value;
        return;
    }

    void insert_helper(const Point<N>& pt, const ElemType& value, std::unique_ptr<Node<N, ElemType>> *root, size_t dimension, size_t &size) {
        if (*root == nullptr) {
            *root = std::make_unique<Node<N, ElemType>>();
            for (long long unsigned int i = 0; i < N; i++) {
                (**root).point[i] = pt[i];
            }
            (**root).value = value;
            size++;
            return;
        }
        if (pt[dimension] < (**root).point[dimension]) {
            insert_helper(pt, value, &((**root).left), (dimension + 1) % N, size);
        } else {
            insert_helper(pt, value, &((**root).right), (dimension + 1) % N, size);
        }
    }
    
    // ElemType& operator[](const Point<N>& pt);
    // Usage: kd[v] = "Some Value";
    // ----------------------------------------------------
    // Returns a reference to the value associated with point pt in the KDTree.
    // If the point does not exist, then it is added to the KDTree using the
    // default value of ElemType as its key.
    ElemType& operator[](const Point<N>& pt) {
        ElemType &value = getElem(*_root, pt, 0);
        if (&value == nullptr) {
            this->insert(pt, ElemType());
            ElemType &new_value = getElem(*_root, pt, 0);
            return new_value;
        }
        return value;
    }
    
    // ElemType& at(const Point<N>& pt);
    // const ElemType& at(const Point<N>& pt) const;
    // Usage: cout << kd.at(v) << endl;
    // ----------------------------------------------------
    // Returns a reference to the key associated with the point pt. If the point
    // is not in the tree, this function throws an out_of_range exception.
    ElemType& at(const Point<N>& pt) {
        ElemType &value = getElem(*_root, pt, 0);
        if (&value == nullptr) {
            throw out_of_range("");
        }
        return value;
    }
    const ElemType& at(const Point<N>& pt) const {
        ElemType &value = getElem(*_root, pt, 0);
        if (&value == nullptr) {
            throw out_of_range("");
        }
        return value;
    }

    static ElemType& getElem(Node<N, ElemType> &root, const Point<N>& pt, size_t dimension) {
        if (&root == nullptr) {
            return *static_cast<ElemType *>(nullptr);
        }
        if (root.point == pt) {
            return root.value;
        }
        if (pt[dimension] < root.point[dimension]) {
            return getElem(*(root.left), pt, (dimension + 1) % N);
        } else {
            return getElem(*(root.right), pt, (dimension + 1) % N);
        }
    }
    
    // ElemType kNNValue(const Point<N>& key, size_t k) const
    // Usage: cout << kd.kNNValue(v, 3) << endl;
    // ----------------------------------------------------
    // Given a point v and an integer k, finds the k points in the KDTree
    // nearest to v and returns the most common value associated with those
    // points. In the event of a tie, one of the most frequent value will be
    // chosen.
    ElemType kNNValue(const Point<N>& key, size_t k) const {
        BoundedPQueue<ElemType> queue(k);
        KNN_helper(key, queue, 0, *_root);
        unordered_map<ElemType, int> record;
        ElemType common;
        int      common_num = 0;
        while (!queue.empty()) {
            record[queue.dequeueMin()]++;
        }
        for (auto & elem : record) {
            if (elem.second > common_num) {
                common_num = elem.second;
                common = elem.first;
            }
        }
        return common;
    }

    static void KNN_helper(const Point<N>& key, BoundedPQueue<ElemType> &queue, size_t dimension, Node<N, ElemType> &root) {
        if (&root == nullptr) {
            return;
        }
        double distance = Distance(key, root.point);
        queue.enqueue(root.value, distance);
        if (key[dimension] < root.point[dimension]) {
            KNN_helper(key, queue, (dimension + 1) % N, *root.left);
        } else {
            KNN_helper(key, queue, (dimension + 1) % N, *root.right);
        }

        if (queue.size() < N || queue.worst() > std::abs(key[dimension] - root.point[dimension])) {
            if (key[dimension] >= root.point[dimension]) {
                KNN_helper(key, queue, (dimension + 1) % N, *root.left);
            } else {
                KNN_helper(key, queue, (dimension + 1) % N, *root.right);
            }
        }
    }

private:
    // TODO: Add implementation details here.
    std::unique_ptr<Node<N, ElemType>> _root;
    size_t _size;
};


#endif // KDTREE_INCLUDED
