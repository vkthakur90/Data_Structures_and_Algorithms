#include <iostream>
#include <memory>

// Structure representing a BinarySearchTree using a free-node pool.
struct BinarySearchTree {
    int root{-1}; // Index of the root node.

    // Free-node pool holding node arrays and free list information.
    struct {
        std::unique_ptr<float[]> key{nullptr};    // Node key values.
        std::unique_ptr<int[]> left{nullptr};       // Left child indices.
        std::unique_ptr<int[]> right{nullptr};      // Right child indices.
        std::unique_ptr<int[]> next_free{nullptr};  // Free list linking.
        std::unique_ptr<bool[]> allocated{nullptr}; // Allocation flags.
        size_t size{0};                             // Total number of nodes.
        int free_head{-1};                          // Head of the free list.
    } pool;
};

// Initialize the BinarySearchTree with N nodes.
// All nodes are initially free and linked into the free list.
void BinarySearchTree_init(BinarySearchTree &tree, const size_t &N) {
    tree.root = -1;
    tree.pool.size = N;
    tree.pool.key = std::make_unique<float[]>(N);
    tree.pool.left = std::make_unique<int[]>(N);
    tree.pool.right = std::make_unique<int[]>(N);
    tree.pool.next_free = std::make_unique<int[]>(N);
    tree.pool.allocated = std::make_unique<bool[]>(N);
    tree.pool.free_head = 0;  // Free list starts at index 0.

    for (size_t i = 0; i < N; ++i) {
        tree.pool.key[i] = 0.0f;
        tree.pool.left[i] = -1;
        tree.pool.right[i] = -1;
        tree.pool.allocated[i] = false;
        tree.pool.next_free[i] = (i < N - 1) ? static_cast<int>(i + 1) : -1;
    }
}

// Allocate a node from the free list.
// Initializes the node with the provided key and returns its index via node_idx.
void BinarySearchTree_allocateNode(BinarySearchTree &tree, const float &key, int &node_idx) {
    node_idx = -1;
    if (tree.pool.free_head == -1) {
        std::cerr << "Error: No free node available." << std::endl;
        return;
    }
    // "Pop" a node from the free list.
    node_idx = tree.pool.free_head;
    tree.pool.free_head = tree.pool.next_free[node_idx];

    // Initialize the node.
    tree.pool.key[node_idx] = key;
    tree.pool.left[node_idx] = -1;
    tree.pool.right[node_idx] = -1;
    tree.pool.allocated[node_idx] = true;
}

// Deallocate a node by pushing it back onto the free list.
void BinarySearchTree_deallocateNode(BinarySearchTree &tree, const size_t &idx) {
    if (idx >= tree.pool.size) {
        std::cerr << "Error: Index out of bounds in deallocation." << std::endl;
        return;
    }
    if (!tree.pool.allocated[idx]) {
        std::cerr << "Error: Node " << idx << " is already deallocated." << std::endl;
        return;
    }
    // Reset node's content.
    tree.pool.key[idx] = 0.0f;
    tree.pool.left[idx] = -1;
    tree.pool.right[idx] = -1;
    tree.pool.allocated[idx] = false;

    // Push node back into the free list.
    tree.pool.next_free[idx] = tree.pool.free_head;
    tree.pool.free_head = idx;
}

// Insert a key into the BinarySearchTree.
void BinarySearchTree_insert(BinarySearchTree &tree, const float &key) {
    int new_node = -1;
    BinarySearchTree_allocateNode(tree, key, new_node);
    if (new_node == -1)
        return;

    // If the tree is empty, set the new node as the root.
    if (tree.root == -1) {
        tree.root = new_node;
        return;
    }

    // Otherwise, find the correct spot for insertion.
    int current = tree.root;
    while (true) {
        if (key < tree.pool.key[current]) {
            // Go left.
            if (tree.pool.left[current] == -1) {
                tree.pool.left[current] = new_node;
                break;
            } else {
                current = tree.pool.left[current];
            }
        } else {
            // Go right.
            if (tree.pool.right[current] == -1) {
                tree.pool.right[current] = new_node;
                break;
            } else {
                current = tree.pool.right[current];
            }
        }
    }
}

// Search for a key in the BinarySearchTree.
// Returns the node index via result if found; otherwise, result is set to -1.
void BinarySearchTree_search(BinarySearchTree &tree, const float &key, int &result) {
    int current = tree.root;
    while (current != -1) {
        if (tree.pool.key[current] == key) {
            result = current;
            return;
        }
        if (key < tree.pool.key[current])
            current = tree.pool.left[current];
        else
            current = tree.pool.right[current];
    }
    result = -1;
}

// Helper function: find the minimum node in the subtree rooted at node_idx.
int BinarySearchTree_findMin(BinarySearchTree &tree, int node_idx) {
    while (tree.pool.left[node_idx] != -1)
        node_idx = tree.pool.left[node_idx];
    return node_idx;
}

// Delete a node with the specified key from the BinarySearchTree.
void BinarySearchTree_delete(BinarySearchTree &tree, const float &key) {
    int parent = -1;
    int current = tree.root;
    bool isLeftChild = false;
    
    // Locate the node to delete and its parent.
    while (current != -1 && tree.pool.key[current] != key) {
        parent = current;
        if (key < tree.pool.key[current]) {
            isLeftChild = true;
            current = tree.pool.left[current];
        } else {
            isLeftChild = false;
            current = tree.pool.right[current];
        }
    }
    if (current == -1) {
        std::cerr << "Error: Key " << key << " not found." << std::endl;
        return;
    }
    
    // Case 1: Node is a leaf.
    if (tree.pool.left[current] == -1 && tree.pool.right[current] == -1) {
        if (current == tree.root)
            tree.root = -1;
        else if (isLeftChild)
            tree.pool.left[parent] = -1;
        else
            tree.pool.right[parent] = -1;
        BinarySearchTree_deallocateNode(tree, current);
    }
    // Case 2: Node has one child.
    else if (tree.pool.left[current] == -1 || tree.pool.right[current] == -1) {
        int child = (tree.pool.left[current] != -1) ? tree.pool.left[current] : tree.pool.right[current];
        if (current == tree.root)
            tree.root = child;
        else if (isLeftChild)
            tree.pool.left[parent] = child;
        else
            tree.pool.right[parent] = child;
        BinarySearchTree_deallocateNode(tree, current);
    }
    // Case 3: Node has two children.
    else {
        // Find the in-order successor (minimum node in right subtree).
        int successorParent = current;
        int successor = tree.pool.right[current];
        while (tree.pool.left[successor] != -1) {
            successorParent = successor;
            successor = tree.pool.left[successor];
        }
        // Copy the successor's key into the current node.
        tree.pool.key[current] = tree.pool.key[successor];
        // Remove the successor node.
        if (tree.pool.left[successor] == -1 && tree.pool.right[successor] == -1) {
            if (tree.pool.left[successorParent] == successor)
                tree.pool.left[successorParent] = -1;
            else
                tree.pool.right[successorParent] = -1;
        } else {
            int child = (tree.pool.left[successor] != -1) ? tree.pool.left[successor] : tree.pool.right[successor];
            if (tree.pool.left[successorParent] == successor)
                tree.pool.left[successorParent] = child;
            else
                tree.pool.right[successorParent] = child;
        }
        BinarySearchTree_deallocateNode(tree, successor);
    }
}

// In-order traversal helper for the BinarySearchTree.
void BinarySearchTree_inOrder(const BinarySearchTree &tree, int node_idx) {
    if (node_idx == -1)
        return;
    BinarySearchTree_inOrder(tree, tree.pool.left[node_idx]);
    std::cout << tree.pool.key[node_idx] << " ";
    BinarySearchTree_inOrder(tree, tree.pool.right[node_idx]);
}

// Print the BinarySearchTree using in-order traversal.
void BinarySearchTree_printInOrder(const BinarySearchTree &tree) {
    std::cout << "BinarySearchTree In-Order: ";
    BinarySearchTree_inOrder(tree, tree.root);
    std::cout << std::endl;
}

// Demonstration of BinarySearchTree operations.
int main() {
    constexpr size_t N = 20;
    BinarySearchTree tree;
    
    // Initialize the BinarySearchTree with a pool of N nodes.
    BinarySearchTree_init(tree, N);
    
    // Insert keys.
    BinarySearchTree_insert(tree, 50.0f);
    BinarySearchTree_insert(tree, 30.0f);
    BinarySearchTree_insert(tree, 70.0f);
    BinarySearchTree_insert(tree, 20.0f);
    BinarySearchTree_insert(tree, 40.0f);
    BinarySearchTree_insert(tree, 60.0f);
    BinarySearchTree_insert(tree, 80.0f);
    
    BinarySearchTree_printInOrder(tree);  // Expected output (sorted): 20 30 40 50 60 70 80
    
    // Search for a key.
    int result;
    BinarySearchTree_search(tree, 60.0f, result);
    if (result != -1)
        std::cout << "Key 60 found at node index: " << result << std::endl;
    else
        std::cout << "Key 60 not found." << std::endl;
    
    // Delete a key.
    BinarySearchTree_delete(tree, 30.0f);
    BinarySearchTree_printInOrder(tree);  // Expected output: 20 40 50 60 70 80
    
    return 0;
}
