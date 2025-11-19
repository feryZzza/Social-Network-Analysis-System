#ifndef SEARCH_TREE_H
#define SEARCH_TREE_H

#include "data_structure/tree.h"
#include "data_structure/lin_list.h"

template <typename T>
class BinarySearchTree : public BinaryTree<T> {//二分搜索树类，继承自二叉树
public:
    BinarySearchTree() {}

    ~BinarySearchTree() {} // 基类的虚析构函数会处理内存释放

    // 覆盖基类的 insert，改为 BST 插入规则 (左小右大)
    void insert(const T& val) override {
        this->root = insertInternal(this->root, val);
    }

    // 查找是否存在某值
    bool search(const T& val) const {//调用递归实现
        return searchInternal(this->root, val) != nullptr;
    }

    // 获取指向某值的节点指针 (如果需要修改节点内容)
    TreeNode<T>* find(const T& val) const {
        return searchInternal(this->root, val);
    }

    // 删除节点
    virtual void remove(const T& val) {//调用递归实现
        this->root = removeInternal(this->root, val);
    }

protected:
    // 递归插入实现
    TreeNode<T>* insertInternal(TreeNode<T>* node, const T& val) {
        if (!node) return new TreeNode<T>(val);
        //涉及到比较，所以 T 类型必须重载比较运算符
        if (val < node->data) {
            node->left = insertInternal(node->left, val);
        } else if (val > node->data) {
            node->right = insertInternal(node->right, val);
        }
        // 如果相等，默认不重复插入，保持树中值唯一（这个树主要用于查找用户名，用户名是唯一的）
        return node;
    }

    // 递归查找实现
    TreeNode<T>* searchInternal(TreeNode<T>* node, const T& val) const {
        if (!node || node->data == val) return node;
        
        if (val < node->data) {
            return searchInternal(node->left, val);
        } else {
            return searchInternal(node->right, val);
        }
    }

    // 辅助：查找子树中的最小值节点 (用于删除时寻找后继)
    TreeNode<T>* findMin(TreeNode<T>* node) {//直接往左走到底，就是最小值
        while (node && node->left) node = node->left;
        return node;
    }

    // 递归删除实现
    TreeNode<T>* removeInternal(TreeNode<T>* node, const T& val) {
        if (!node) return nullptr;//找不到啥也不干

        if (val < node->data) {
            node->left = removeInternal(node->left, val);
        } else if (val > node->data) {
            node->right = removeInternal(node->right, val);
        } else {
            // 找到目标节点
            if (!node->left) {//没有左节点
                TreeNode<T>* temp = node->right;
                delete node;
                return temp; // 返回右孩子接替位置
            }else if (!node->right) {//
                TreeNode<T>* temp = node->left;
                delete node;
                return temp; // 返回左孩子接替位置
            }
            // 如果左右都有节点找到右子树中的最小值（中序后继）来替代当前节点
            TreeNode<T>* temp = findMin(node->right);
            // 将后继的值复制到当前节点
            node->data = temp->data;
            // 删除右子树中的那个后继节点
            node->right = removeInternal(node->right, temp->data);
        }
        return node;
    }
};

template <typename T>
class AVLTree : public BinarySearchTree<T> {//平衡搜索树，继承自二分搜索树
public:
    AVLTree() {}
    AVLTree(SeqList<T>& list) {//通过线性表构建 AVL 树
        for (int i = 0; i < list.size(); ++i) {
            this->insert(list[i]);
        }
    }
    ~AVLTree() {} // 基类的虚析构函数会处理内存释放
    // 比正常的二分搜索树多了一步检查高度进行平衡的操作
    void insert(const T& val) override {
        this->root = insertAndBalance(this->root, val);
    }
    // 删除元素并保持平衡
    void remove(const T& val) override {
        this->root = removeAndBalance(this->root, val);
    }
    // 获取树的高度 (辅助)
    int getHeight() {
        return height(this->root);
    }

private:

    int height(TreeNode<T>* node) {//获取节点高度
        return node ? node->height : 0;
    }

    int getBalance(TreeNode<T>* node) {//获取节点平衡因子
        return node ? height(node->left) - height(node->right) : 0;
    }

    void updateHeight(TreeNode<T>* node) {//更新节点高度
        if (node) {
            node->height = 1 + std::max(height(node->left), height(node->right));
        }
    }

    // 右旋
    TreeNode<T>* rotateRight(TreeNode<T>* y) {
        TreeNode<T>* x = y->left;
        TreeNode<T>* T2 = x->right;

        x->right = y;
        y->left = T2;

        updateHeight(y);
        updateHeight(x);

        return x; 
    }
    // 左旋
    TreeNode<T>* rotateLeft(TreeNode<T>* x) {
        TreeNode<T>* y = x->right;
        TreeNode<T>* T2 = y->left;

        y->left = x;
        x->right = T2;

        updateHeight(x);
        updateHeight(y);

        return y; 
    }

    // 平衡节点
    TreeNode<T>* balanceNode(TreeNode<T>* node) {
        updateHeight(node);
        int balance = getBalance(node);

        // LL
        if (balance > 1 && getBalance(node->left) >= 0) return rotateRight(node);
        
        // LR
        if (balance > 1 && getBalance(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // RR
        if (balance < -1 && getBalance(node->right) <= 0) return rotateLeft(node);
        
        // RL
        if (balance < -1 && getBalance(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    TreeNode<T>* insertAndBalance(TreeNode<T>* node, const T& val) {
        if (!node) return new TreeNode<T>(val);

        if (val < node->data) {
            node->left = insertAndBalance(node->left, val);
        } else if (val > node->data) {
            node->right = insertAndBalance(node->right, val);
        } else {
            return node; // 重复值
        }

        return balanceNode(node);
    }

    TreeNode<T>* removeAndBalance(TreeNode<T>* node, const T& val) {
        if (!node) return nullptr;

        if (val < node->data) {
            node->left = removeAndBalance(node->left, val);
        } else if (val > node->data) {
            node->right = removeAndBalance(node->right, val);
        } else {
            // 找到节点
            if (!node->left || !node->right) {
                TreeNode<T>* temp = node->left ? node->left : node->right;
                if (!temp) { // 无子节点
                    delete node;
                    return nullptr;
                } else { // 单子节点
                    TreeNode<T>* toDelete = node;
                    node = temp; 
                    delete toDelete;
                    // node 已经指向子节点，递归返回后上层会处理高度更新
                }
            } else {
                // 双子节点：使用基类的 findMin 辅助
                TreeNode<T>* temp = this->findMin(node->right);
                node->data = temp->data;
                node->right = removeAndBalance(node->right, temp->data);
            }
        }

        if (!node) return nullptr;
        // 重新平衡
        return balanceNode(node);
    }
};


#endif