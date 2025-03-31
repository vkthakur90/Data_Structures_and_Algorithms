#include <iostream>
#include <memory>

// Node structure for the doubly linked list.
struct Node {
    int data {0};
    std::unique_ptr<Node> next {nullptr}; // Unique ownership of the next node.
    Node* prev {nullptr};                 // Raw pointer to the previous node.
};

// Helper function: returns the raw pointer from a unique_ptr.
Node* ptr(std::unique_ptr<Node>& up) {
    return up.get();
}

// Overload for const unique_ptr.
const Node* ptr(const std::unique_ptr<Node>& up) {
    return up.get();
}

// Create a new node with the given value.
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
    
    Node* current = ptr(head);
    while (current->next) {
        current = ptr(current->next);
    }
    
    auto newNode = create_node(value);
    newNode->prev = current;  // Set the backward pointer.
    current->next = std::move(newNode);
}

// Prepend: Insert a new node with 'value' at the beginning of the list.
void prepend(std::unique_ptr<Node>& head, int value) {
    auto newNode = create_node(value);
    if (head) {
        head->prev = newNode.get();
        newNode->next = std::move(head);
    }
    head = std::move(newNode);
}

// Search: Return a raw pointer to the first node with the given value, or nullptr if not found.
Node* search(const std::unique_ptr<Node>& head, int value) {
    const Node* current = ptr(head);
    while (current) {
        if (current->data == value)
            return const_cast<Node*>(current); // Return non-const pointer.
        current = ptr(current->next);
    }
    return nullptr;
}

// Remove: Delete the first node whose data equals 'value'.
// Returns true if a node was removed, false otherwise.
bool remove(std::unique_ptr<Node>& head, int value) {
    if (!head)
        return false;
    
    Node* current = ptr(head);
    while (current) {
        if (current->data == value) {
            // If the node to be removed is the head.
            if (current == ptr(head)) {
                head = std::move(head->next);
                if (head)
                    head->prev = nullptr;
            } else {
                // Bypass the current node.
                Node* prevNode = current->prev;
                prevNode->next = std::move(current->next);
                if (prevNode->next)
                    prevNode->next->prev = prevNode;
            }
            return true;
        }
        current = ptr(current->next);
    }
    return false;
}

// Count: Return the number of nodes in the list.
int count(const std::unique_ptr<Node>& head) {
    int cnt = 0;
    const Node* current = ptr(head);
    while (current) {
        ++cnt;
        current = ptr(current->next);
    }
    return cnt;
}

// Reverse: Reverse the linked list in-place.
void reverse(std::unique_ptr<Node>& head) {
    std::unique_ptr<Node> newHead = nullptr;
    while (head) {
        // Detach the head node.
        auto node = std::move(head);
        head = std::move(node->next);
        node->prev = nullptr;
        
        // Prepend the detached node to newHead.
        if (newHead) {
            newHead->prev = node.get();
            node->next = std::move(newHead);
        } else {
            node->next = nullptr;
        }
        newHead = std::move(node);
    }
    head = std::move(newHead);
}

// Traverse: Print the list's elements.
void traverse(const std::unique_ptr<Node>& head) {
    const Node* current = ptr(head);
    while (current) {
        std::cout << current->data << " <-> ";
        current = ptr(current->next);
    }
    std::cout << "null\n";
}

int main() {
    std::unique_ptr<Node> head = nullptr;
    
    // Append nodes with values 1 through 5.
    for (int i = 1; i <= 5; ++i) {
        append(head, i);
    }
    
    std::cout << "List after appending 1 to 5:\n";
    traverse(head);
    std::cout << "Count: " << count(head) << "\n\n";
    
    // Prepend a node with value 0.
    prepend(head, 0);
    std::cout << "After prepending 0:\n";
    traverse(head);
    std::cout << "Count: " << count(head) << "\n\n";
    
    // Search for a node with value 3.
    Node* found = search(head, 3);
    if (found)
        std::cout << "Found node with value: " << found->data << "\n\n";
    else
        std::cout << "Node with value 3 not found\n\n";
    
    // Remove the node with value 3.
    bool removed = remove(head, 3);
    std::cout << "After removing 3 (removed = " << std::boolalpha << removed << "):\n";
    traverse(head);
    std::cout << "Count: " << count(head) << "\n\n";
    
    // Reverse the list.
    reverse(head);
    std::cout << "After reversing the list:\n";
    traverse(head);
    
    return 0;
}
