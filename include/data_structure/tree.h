#ifndef TREE_H
#define TREE_H

#include <iostream>
#include "data_structure/queue.h" // 用于层序遍历

template <typename T>
class TreeNode {//树节点类,里面的data必须重载比较运算符
public:
    T data;             // 数据
    TreeNode<T>* left;  // 左孩子
    TreeNode<T>* right; // 右孩子
    int height;         // 节点高度(用于AVL平衡计算)

    TreeNode(T val = T()) : data(val), left(nullptr), right(nullptr), height(1) {}
    //重载比较
    bool operator==(const TreeNode<T>& other) const {
        return this->data == other.data;
    }
    bool operator<(const TreeNode<T>& other) const {
        return this->data < other.data;
    }
    bool operator>(const TreeNode<T>& other) const {
        return this->data > other.data;
    }

};

template <typename T>
class BinaryTree {
public:
    BinaryTree() : root(nullptr) {}
    virtual ~BinaryTree() { destroy(root); }


    virtual void insert(const T& val) {//插入，可以被子类重载，优先插入左边
        if (!root) {
            root = new TreeNode<T>(val);
            return;
        }
        LinkQueue<TreeNode<T>*> q;
        q.enqueue(root);
        while (!q.empty()) {
            TreeNode<T>* current = q.dequeue();
            if (!current->left) {
                current->left = new TreeNode<T>(val);
                return;
            } else {
                q.enqueue(current->left);
            }
            if (!current->right) {
                current->right = new TreeNode<T>(val);
                return;
            } else {
                q.enqueue(current->right);
            }
        }
    }

    // 递归遍历接口，可以输入一个回调函数，但是回调函数必须接受一个 T 类型参数
    template <typename Func>
    void preOrder(Func visit) const { preOrderInternal(root, visit); }

    template <typename Func>
    void inOrder(Func visit) const { inOrderInternal(root, visit); }

    template <typename Func>
    void postOrder(Func visit) const { postOrderInternal(root, visit); }
    
    // 层序遍历
    template <typename Func>
    void levelOrder(Func visit) const {
        if (!root) return;
        LinkQueue<TreeNode<T>*> q;
        q.enqueue(root);
        while (!q.empty()) {
            TreeNode<T>* current = q.dequeue();
            visit(current->data); // 执行回调
            if (current->left) q.enqueue(current->left);
            if (current->right) q.enqueue(current->right);
        }
    }

    // 获取根节点（供子类或特殊用途使用）
    TreeNode<T>* Root() const { return root; }

protected:
    TreeNode<T>* root;

    // 递归销毁
    void destroy(TreeNode<T>* node) {
        if (node) {
            destroy(node->left);
            destroy(node->right);
            delete node;
        }
    }
    // 递归遍历实现
    template <typename Func>
    void preOrderInternal(TreeNode<T>* node, Func visit) const {
        if (node) {
            visit(node->data);       // 根
            preOrderInternal(node->left, visit);  // 左
            preOrderInternal(node->right, visit); // 右
        }
    }
    
    template <typename Func>
    void inOrderInternal(TreeNode<T>* node, Func visit) const {
        if (node) {
            inOrderInternal(node->left, visit);   // 左
            visit(node->data);       // 根
            inOrderInternal(node->right, visit);  // 右
        }
    }

    template <typename Func>
    void postOrderInternal(TreeNode<T>* node, Func visit) const {
        if (node) {
            postOrderInternal(node->left, visit); // 左
            postOrderInternal(node->right, visit);// 右
            visit(node->data);       // 根
        }
    }
};


#endif