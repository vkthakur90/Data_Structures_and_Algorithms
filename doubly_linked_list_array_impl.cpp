#include <iostream>
#include <memory>

// Doubly linked list structure.
struct DoublyLinkedList {
    int head{-1}; // Index of the first node.
    int tail{-1}; // Index of the last node.

    // Free node stack and node arrays.
    struct {
        struct {
            std::unique_ptr<float[]> data;   // Node values.
            std::unique_ptr<int[]> next;       // Next pointers (indices).
            std::unique_ptr<int[]> prev;       // Previous pointers (indices).
        } nodes;
        std::unique_ptr<int[]> next_free;      // Free list linking (free stack).
        std::unique_ptr<bool[]> allocated;     // Allocation flags.
        size_t size{0};                        // Total number of nodes.
        int free_head{-1};                     // Head index for free list.
    } free_node_stack;
};

// Initialize the doubly linked list with N nodes.
// All nodes are initially free.
void DoublyLinkedList_init(DoublyLinkedList &list, const size_t N) {
    list.head = -1;
    list.tail = -1;
    list.free_node_stack.size = N;
    
    list.free_node_stack.nodes.data = std::make_unique<float[]>(N);
    list.free_node_stack.nodes.next = std::make_unique<int[]>(N);
    list.free_node_stack.nodes.prev = std::make_unique<int[]>(N);
    list.free_node_stack.next_free = std::make_unique<int[]>(N);
    list.free_node_stack.allocated = std::make_unique<bool[]>(N);
    list.free_node_stack.free_head = 0; // Free list starts at index 0

    for (size_t i = 0; i < N; ++i) {
        list.free_node_stack.nodes.data[i] = 0.0f;
        list.free_node_stack.nodes.next[i] = -1;
        list.free_node_stack.nodes.prev[i] = -1;
        list.free_node_stack.allocated[i] = false;
        list.free_node_stack.next_free[i] = (i < N - 1) ? static_cast<int>(i + 1) : -1;
    }
}

// Allocate a node from the free node stack, setting its value.
// Returns the allocated node index in node_idx.
void DoublyLinkedList_allocateNode(DoublyLinkedList &list, const float &value, int &node_idx) {
    node_idx = -1;
    if (list.free_node_stack.free_head == -1) {
        std::cerr << "Error: No free node available." << std::endl;
        return;
    }
    // "Pop" a node from the free stack.
    node_idx = list.free_node_stack.free_head;
    list.free_node_stack.free_head = list.free_node_stack.next_free[node_idx];

    // Initialize the node.
    list.free_node_stack.nodes.data[node_idx] = value;
    list.free_node_stack.nodes.next[node_idx] = -1;
    list.free_node_stack.nodes.prev[node_idx] = -1;
    list.free_node_stack.allocated[node_idx] = true;
}

// Deallocate a node by pushing it back onto the free node stack.
void DoublyLinkedList_deallocateNode(DoublyLinkedList &list, const size_t idx) {
    if (idx >= list.free_node_stack.size) {
        std::cerr << "Error: Index out of bounds in deallocation." << std::endl;
        return;
    }
    if (!list.free_node_stack.allocated[idx]) {
        std::cerr << "Error: Node " << idx << " is already deallocated." << std::endl;
        return;
    }
    // Reset the node.
    list.free_node_stack.nodes.data[idx] = 0.0f;
    list.free_node_stack.nodes.next[idx] = -1;
    list.free_node_stack.nodes.prev[idx] = -1;
    list.free_node_stack.allocated[idx] = false;
    
    // "Push" the node back onto the free stack.
    list.free_node_stack.next_free[idx] = list.free_node_stack.free_head;
    list.free_node_stack.free_head = idx;
}

// Append a value to the end of the doubly linked list.
void DoublyLinkedList_append(DoublyLinkedList &list, const float &value) {
    int new_node = -1;
    DoublyLinkedList_allocateNode(list, value, new_node);
    if (new_node == -1)
        return;

    // If the list is empty, set head and tail.
    if (list.head == -1) {
        list.head = new_node;
        list.tail = new_node;
    } else {
        // Link the new node after the current tail.
        list.free_node_stack.nodes.prev[new_node] = list.tail;
        list.free_node_stack.nodes.next[list.tail] = new_node;
        list.tail = new_node;
    }
}

// Prepend a value to the beginning of the doubly linked list.
void DoublyLinkedList_prepend(DoublyLinkedList &list, const float &value) {
    int new_node = -1;
    DoublyLinkedList_allocateNode(list, value, new_node);
    if (new_node == -1)
        return;

    if (list.head == -1) {
        // List is empty.
        list.head = new_node;
        list.tail = new_node;
    } else {
        // Link the new node before the current head.
        list.free_node_stack.nodes.next[new_node] = list.head;
        list.free_node_stack.nodes.prev[list.head] = new_node;
        list.head = new_node;
    }
}

// Insert a value after the node at a specified index.
void DoublyLinkedList_insertAfter(DoublyLinkedList &list, int node_idx, const float &value) {
    if (node_idx < 0 || static_cast<size_t>(node_idx) >= list.free_node_stack.size ||
        !list.free_node_stack.allocated[node_idx]) {
        std::cerr << "Error: Invalid node index for insertion." << std::endl;
        return;
    }
    int new_node = -1;
    DoublyLinkedList_allocateNode(list, value, new_node);
    if (new_node == -1)
        return;

    int next_node = list.free_node_stack.nodes.next[node_idx];
    
    // Set new node's pointers.
    list.free_node_stack.nodes.prev[new_node] = node_idx;
    list.free_node_stack.nodes.next[new_node] = next_node;
    
    // Link the new node into the list.
    list.free_node_stack.nodes.next[node_idx] = new_node;
    if (next_node != -1) {
        list.free_node_stack.nodes.prev[next_node] = new_node;
    } else {
        // New node is now the tail.
        list.tail = new_node;
    }
}

// Search for the first node containing the specified value.
// Returns the node index in result if found, otherwise returns -1.
void DoublyLinkedList_search(DoublyLinkedList &list, const float &value, int &result) {
    int current = list.head;
    while (current != -1) {
        if (list.free_node_stack.nodes.data[current] == value) {
            result = current;
            return;
        }
        current = list.free_node_stack.nodes.next[current];
    }
    result = -1;
}

// Delete the first node found that contains the specified value.
void DoublyLinkedList_delete(DoublyLinkedList &list, const float &value) {
    int current = list.head;
    while (current != -1) {
        if (list.free_node_stack.nodes.data[current] == value)
            break;
        current = list.free_node_stack.nodes.next[current];
    }
    if (current == -1) {
        std::cerr << "Value " << value << " not found." << std::endl;
        return;
    }

    int prev_node = list.free_node_stack.nodes.prev[current];
    int next_node = list.free_node_stack.nodes.next[current];

    // Update head and tail if necessary.
    if (prev_node != -1)
        list.free_node_stack.nodes.next[prev_node] = next_node;
    else
        list.head = next_node;

    if (next_node != -1)
        list.free_node_stack.nodes.prev[next_node] = prev_node;
    else
        list.tail = prev_node;

    // Deallocate the node.
    DoublyLinkedList_deallocateNode(list, current);
}

// Print the list from head to tail.
void DoublyLinkedList_print(const DoublyLinkedList &list) {
    int current = list.head;
    std::cout << "DoublyLinkedList: ";
    while (current != -1) {
        std::cout << list.free_node_stack.nodes.data[current] << " ";
        current = list.free_node_stack.nodes.next[current];
    }
    std::cout << std::endl;
}

// Demonstration of doubly linked list operations.
int main() {
    constexpr size_t N = 10;
    DoublyLinkedList list;
    
    // Initialize the doubly linked list with N nodes.
    DoublyLinkedList_init(list, N);
    
    // Append values.
    DoublyLinkedList_append(list, 1.1f);
    DoublyLinkedList_append(list, 2.2f);
    DoublyLinkedList_append(list, 3.3f);
    DoublyLinkedList_print(list);  // Expected: 1.1 2.2 3.3
    
    // Prepend a value.
    DoublyLinkedList_prepend(list, 0.0f);
    DoublyLinkedList_print(list);  // Expected: 0.0 1.1 2.2 3.3
    
    // Insert a value after the node with value 1.1.
    int pos;
    DoublyLinkedList_search(list, 1.1f, pos);
    if (pos != -1)
        DoublyLinkedList_insertAfter(list, pos, 1.5f);
    DoublyLinkedList_print(list);  // Expected: 0.0 1.1 1.5 2.2 3.3
    
    // Delete the first occurrence of 2.2.
    DoublyLinkedList_delete(list, 2.2f);
    DoublyLinkedList_print(list);  // Expected: 0.0 1.1 1.5 3.3
    
    return 0;
}
