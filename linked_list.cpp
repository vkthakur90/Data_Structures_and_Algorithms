#include <iostream>
#include <memory>

// Node structure with an integer data and a unique pointer to the next node.
struct Node {
    int data{0};
    std::unique_ptr<Node> next{nullptr};
};

// Helper function: returns the raw pointer from a unique_ptr.
Node* ptr(const std::unique_ptr<Node>& up) {
    return up.get();
}

std::unique_ptr<Node> create_node(int value) {
    auto node = std::make_unique<Node>();
    node->data = value;
    return node;
}

// Append: Insert a new node with 'value' at the end of the list.
void append(std::unique_ptr<Node>& head, int value) {
    if (!head) {
        head = create_node(value);
        return;
    }
    Node* curr = ptr(head);
    while (curr->next) {
        curr = ptr(curr->next);
    }
    curr->next = create_node(value);
}

// Prepend: Insert a new node with 'value' at the beginning of the list.
void prepend(std::unique_ptr<Node>& head, int value) {
    auto newNode = create_node(value);
    newNode->next = std::move(head);
    head = std::move(newNode);
}

// Search: Return a raw pointer to the first node with the given value, or nullptr if not found.
Node* search(const std::unique_ptr<Node>& head, int value) {
    Node* curr = ptr(head);
    while (curr) {
        if (curr->data == value)
            return curr;
        curr = ptr(curr->next);
    }
    return nullptr;
}

// Remove: Delete the first node whose data equals 'value'.
// Returns true if a node was removed, false otherwise.
bool remove(std::unique_ptr<Node>& head, int value) {
    if (!head)
        return false;
    
    if (head->data == value) {
        head = std::move(head->next);
        return true;
    }
    
    Node* curr = ptr(head);
    while (curr->next) {
        if (curr->next->data == value) {
            curr->next = std::move(curr->next->next);
            return true;
        }
        curr = ptr(curr->next);
    }
    return false;
}

// Count: Return the number of nodes in the list.
int count(const std::unique_ptr<Node>& head) {
    int cnt = 0;
    Node* curr = ptr(head);
    while (curr) {
        ++cnt;
        curr = ptr(curr->next);
    }
    return cnt;
}

// Reverse: Reverse the linked list in-place.
void reverse(std::unique_ptr<Node>& head) {
    std::unique_ptr<Node> prev = nullptr;
    while (head) {
        auto next = std::move(head->next);
        head->next = std::move(prev);
        prev = std::move(head);
        head = std::move(next);
    }
    head = std::move(prev);
}

// Traverse: Print the list's elements.
void traverse(const std::unique_ptr<Node>& head) {
    Node* curr = ptr(head);
    while (curr) {
        std::cout << curr->data << " -> ";
        curr = ptr(curr->next);
    }
    std::cout << "null\n";
}

// Cleanup: Iteratively clean up the list by moving head along the chain.
void cleanup(std::unique_ptr<Node>& head) {
    while (head) {
        head = std::move(head->next);
    }
}

int main() {
    std::unique_ptr<Node> head = nullptr;
    
    for (int idx = 1; idx <= 10; ++idx) {
        append(head, idx);
    }
    
    std::cout << "Initial list (append):\n";
    traverse(head);
    std::cout << "Count: " << count(head) << "\n\n";
    
    prepend(head, 0);
    std::cout << "After prepending 0:\n";
    traverse(head);
    std::cout << "Count: " << count(head) << "\n\n";
    
    Node* found = search(head, 5);
    if (found)
        std::cout << "Found value: " << found->data << "\n\n";
    else
        std::cout << "Value 5 not found.\n\n";
    
    bool removed = remove(head, 5);
    std::cout << "After removing 5 (removed = " << std::boolalpha << removed << "):\n";
    traverse(head);
    std::cout << "Count: " << count(head) << "\n\n";
    
    reverse(head);
    std::cout << "After reversing the list:\n";
    traverse(head);
    std::cout << "Count: " << count(head) << "\n\n";
    
    cleanup(head);
    if (!head)
        std::cout << "Cleanup complete: head is nullptr.\n";
    else
        std::cout << "Cleanup failed: list still exists.\n";
    
    return 0;
}
