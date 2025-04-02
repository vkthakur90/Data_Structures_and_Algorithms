#include <iostream>
#include <memory>

// Struct definition with a nested free_node_stack holding node arrays and free list information.
struct LinkedList {
    int head{-1}; // Head of the linked list (index of first node)
    struct {
        struct {
            std::unique_ptr<float[]> data{nullptr};    // Node values
            std::unique_ptr<int[]> next{nullptr};        // Next pointers (indices)
        } nodes;
        std::unique_ptr<int[]> next_free{nullptr};       // Free list linking (free stack)
        std::unique_ptr<bool[]> allocated{nullptr};      // Allocation flags
        size_t size{0};                                  // Total number of nodes
        int free_head{-1};                               // Head of the free list (index of first free node)
    } free_node_stack;
};

// Initialize the linked list with N nodes.
// All nodes are initially free and linked as a free stack.
void LinkedList_init(LinkedList &list, const size_t &N) {
    list.head = -1;
    list.free_node_stack.size = N;
    list.free_node_stack.nodes.data = std::make_unique<float[]>(N);
    list.free_node_stack.nodes.next = std::make_unique<int[]>(N);
    list.free_node_stack.next_free = std::make_unique<int[]>(N);
    list.free_node_stack.allocated = std::make_unique<bool[]>(N);
    list.free_node_stack.free_head = 0; // Free list starts at index 0

    // Assuming N is the total number of nodes.
    for (size_t i = 0; i < N; ++i) 
        list.free_node_stack.nodes.data[i] = 0.0f;

    for (size_t i = 0; i < N; ++i)
        list.free_node_stack.nodes.next[i] = -1;

    for (size_t i = 0; i < N; ++i)
        list.free_node_stack.allocated[i] = false;

    for (size_t i = 0; i < N; ++i)
        list.free_node_stack.next_free[i] = (i < N - 1) ? static_cast<int>(i + 1) : -1;

}

// Allocate a node by "popping" from the free stack.
// Sets the node's value and marks it as allocated.
// Returns the allocated node index via node_idx.
void LinkedList_allocateNode(LinkedList &list, const float &value, int &node_idx) {
    node_idx = -1;
    if (list.free_node_stack.free_head == -1) {
        std::cerr << "Error: No free node available." << std::endl;
        return;
    }
    // Pop: take the node at free_head.
    node_idx = list.free_node_stack.free_head;
    // Update free_head to the next free node.
    list.free_node_stack.free_head = list.free_node_stack.next_free[node_idx];

    // Initialize the allocated node.
    list.free_node_stack.nodes.data[node_idx] = value;
    list.free_node_stack.nodes.next[node_idx] = -1;
    list.free_node_stack.allocated[node_idx] = true;
}

// Deallocate a node by "pushing" it back onto the free stack.
// Checks for double deallocation.
void LinkedList_deallocateNode(LinkedList &list, const size_t &idx) {
    if (idx >= list.free_node_stack.size) {
        std::cerr << "Error: Index out of bounds in deallocation." << std::endl;
        return;
    }
    
    if (!list.free_node_stack.allocated[idx]) {
        std::cerr << "Error: Node " << idx << " is already deallocated." << std::endl;
        return;
    }
    
    // Reset node's value and next pointer.
    list.free_node_stack.nodes.data[idx] = 0.0f;
    list.free_node_stack.nodes.next[idx] = -1;
    list.free_node_stack.allocated[idx] = false;

    // Push: add this node back to the free stack.
    list.free_node_stack.next_free[idx] = list.free_node_stack.free_head;
    list.free_node_stack.free_head = idx;
}

// Append a value to the end of the linked list.
void LinkedList_append(LinkedList &list, const float &value) {
    int new_node = -1;
    LinkedList_allocateNode(list, value, new_node);
    if (new_node == -1)
        return;

    if (list.head == -1) {
        // If the list is empty, the new node becomes the head.
        list.head = new_node;
    } else {
        int current = list.head;
        // Traverse until the end of the list.
        while (list.free_node_stack.nodes.next[current] != -1)
            current = list.free_node_stack.nodes.next[current];
        list.free_node_stack.nodes.next[current] = new_node;
    }
}

// Prepend a value to the beginning of the linked list.
void LinkedList_prepend(LinkedList &list, const float &value) {
    int new_node = -1;
    LinkedList_allocateNode(list, value, new_node);
    if (new_node == -1)
        return;

    // The new node points to the current head.
    list.free_node_stack.nodes.next[new_node] = list.head;
    list.head = new_node;
}

// Insert a value after the node at the specified index.
void LinkedList_insertAfter(LinkedList &list, int node_idx, const float &value) {
    if (node_idx < 0 || static_cast<size_t>(node_idx) >= list.free_node_stack.size ||
        !list.free_node_stack.allocated[node_idx]) {
        std::cerr << "Error: Invalid node index for insertion." << std::endl;
        return;
    }
    int new_node = -1;
    LinkedList_allocateNode(list, value, new_node);
    if (new_node == -1)
        return;

    // Insert the new node after node_idx.
    list.free_node_stack.nodes.next[new_node] = list.free_node_stack.nodes.next[node_idx];
    list.free_node_stack.nodes.next[node_idx] = new_node;
}

// Search for the first node containing the specified value.
// Returns the node index if found, otherwise returns -1.
void LinkedList_search(LinkedList &list, const float &value, int &result) {
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
void LinkedList_delete(LinkedList &list, const float &value) {
    int current = list.head;
    int prev = -1;
    // Traverse the list to locate the node with the given value.
    while (current != -1) {
        if (list.free_node_stack.nodes.data[current] == value)
            break;
        prev = current;
        current = list.free_node_stack.nodes.next[current];
    }
    if (current == -1) {
        std::cerr << "Value " << value << " not found." << std::endl;
        return;
    }
    // Remove the node from the list.
    if (prev == -1) {
        // Deleting the head.
        list.head = list.free_node_stack.nodes.next[current];
    } else {
        list.free_node_stack.nodes.next[prev] = list.free_node_stack.nodes.next[current];
    }
    // Deallocate the node (push it back onto the free stack).
    LinkedList_deallocateNode(list, current);
}

// Print the values in the linked list.
void LinkedList_print(const LinkedList &list) {
    int current = list.head;
    std::cout << "LinkedList: ";
    while (current != -1) {
        std::cout << list.free_node_stack.nodes.data[current] << " ";
        current = list.free_node_stack.nodes.next[current];
    }
    std::cout << std::endl;
}

// Demonstration of linked list operations.
int main() {
    constexpr size_t N = 10;
    LinkedList list;
    
    // Initialize the linked list with N nodes.
    LinkedList_init(list, N);
    
    // Append values.
    LinkedList_append(list, 1.1f);
    LinkedList_append(list, 2.2f);
    LinkedList_append(list, 3.3f);
    LinkedList_print(list);  // Expected: 1.1 2.2 3.3
    
    // Prepend a value.
    LinkedList_prepend(list, 0.0f);
    LinkedList_print(list);  // Expected: 0.0 1.1 2.2 3.3
    
    // Insert a value after the node with value 1.1.
    int pos;
    LinkedList_search(list, 1.1f, pos);
    if (pos != -1)
        LinkedList_insertAfter(list, pos, 1.5f);
    LinkedList_print(list);  // Expected: 0.0 1.1 1.5 2.2 3.3
    
    // Delete the first occurrence of 2.2.
    LinkedList_delete(list, 2.2f);
    LinkedList_print(list);  // Expected: 0.0 1.1 1.5 3.3
    
    return 0;
}
