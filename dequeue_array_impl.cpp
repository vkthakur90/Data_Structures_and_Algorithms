#include <iostream>
#include <memory>

// Deque structure using a free-node pool for storage.
struct Deque {
    int head{-1};  // Index of the first element.
    int tail{-1};  // Index of the last element.
    
    // Free-node pool holding node arrays and free list information.
    struct {
        std::unique_ptr<float[]> data{nullptr};     // Node values.
        std::unique_ptr<int[]> next{nullptr};         // Next pointers (indices).
        std::unique_ptr<int[]> prev{nullptr};         // Previous pointers (indices).
        std::unique_ptr<int[]> next_free{nullptr};    // Free list linking.
        std::unique_ptr<bool[]> allocated{nullptr};   // Allocation flags.
        size_t size{0};                               // Total number of nodes.
        int free_head{-1};                            // Head of the free list.
    } pool;
};

// Initialize the Deque with N nodes.
// All nodes are initially free and linked as a free list.
void Deque_init(Deque &deque, const size_t N) {
    deque.head = -1;
    deque.tail = -1;
    deque.pool.size = N;
    deque.pool.data = std::make_unique<float[]>(N);
    deque.pool.next = std::make_unique<int[]>(N);
    deque.pool.prev = std::make_unique<int[]>(N);
    deque.pool.next_free = std::make_unique<int[]>(N);
    deque.pool.allocated = std::make_unique<bool[]>(N);
    deque.pool.free_head = 0;  // Free list starts at index 0

    for (size_t i = 0; i < N; ++i) {
        deque.pool.data[i] = 0.0f;
        deque.pool.next[i] = -1;
        deque.pool.prev[i] = -1;
        deque.pool.allocated[i] = false;
        deque.pool.next_free[i] = (i < N - 1) ? static_cast<int>(i + 1) : -1;
    }
}

// Allocate a node from the free list.
// Initializes the node with the provided value and returns its index via node_idx.
void Deque_allocateNode(Deque &deque, const float value, int &node_idx) {
    node_idx = -1;
    if (deque.pool.free_head == -1) {
        std::cerr << "Error: No free node available." << std::endl;
        return;
    }
    // "Pop" a node from the free list.
    node_idx = deque.pool.free_head;
    deque.pool.free_head = deque.pool.next_free[node_idx];

    // Initialize the node.
    deque.pool.data[node_idx] = value;
    deque.pool.next[node_idx] = -1;
    deque.pool.prev[node_idx] = -1;
    deque.pool.allocated[node_idx] = true;
}

// Deallocate a node by pushing it back onto the free list.
// Checks for double deallocation.
void Deque_deallocateNode(Deque &deque, const int idx) {
    if (idx < 0 || static_cast<size_t>(idx) >= deque.pool.size) {
        std::cerr << "Error: Index out of bounds in deallocation." << std::endl;
        return;
    }
    if (!deque.pool.allocated[idx]) {
        std::cerr << "Error: Node " << idx << " is already deallocated." << std::endl;
        return;
    }
    // Reset node's data and pointers.
    deque.pool.data[idx] = 0.0f;
    deque.pool.next[idx] = -1;
    deque.pool.prev[idx] = -1;
    deque.pool.allocated[idx] = false;

    // "Push" this node back onto the free list.
    deque.pool.next_free[idx] = deque.pool.free_head;
    deque.pool.free_head = idx;
}

// Insert a value at the front of the deque.
void Deque_pushFront(Deque &deque, const float value) {
    int new_node = -1;
    Deque_allocateNode(deque, value, new_node);
    if (new_node == -1)
        return;

    if (deque.head == -1) {
        // If the deque is empty, new node becomes both head and tail.
        deque.head = new_node;
        deque.tail = new_node;
    } else {
        // Link new node in front of the current head.
        deque.pool.next[new_node] = deque.head;
        deque.pool.prev[deque.head] = new_node;
        deque.head = new_node;
    }
}

// Insert a value at the back of the deque.
void Deque_pushBack(Deque &deque, const float value) {
    int new_node = -1;
    Deque_allocateNode(deque, value, new_node);
    if (new_node == -1)
        return;

    if (deque.tail == -1) {
        // If the deque is empty, new node becomes both head and tail.
        deque.head = new_node;
        deque.tail = new_node;
    } else {
        // Link new node after the current tail.
        deque.pool.next[deque.tail] = new_node;
        deque.pool.prev[new_node] = deque.tail;
        deque.tail = new_node;
    }
}

// Remove and return the value at the front of the deque.
float Deque_popFront(Deque &deque) {
    if (deque.head == -1) {
        std::cerr << "Error: Deque is empty." << std::endl;
        return 0.0f;
    }
    int node_idx = deque.head;
    float value = deque.pool.data[node_idx];

    // Update head to the next node.
    deque.head = deque.pool.next[node_idx];
    if (deque.head != -1)
        deque.pool.prev[deque.head] = -1;
    else
        deque.tail = -1;  // Deque is now empty.

    Deque_deallocateNode(deque, node_idx);
    return value;
}

// Remove and return the value at the back of the deque.
float Deque_popBack(Deque &deque) {
    if (deque.tail == -1) {
        std::cerr << "Error: Deque is empty." << std::endl;
        return 0.0f;
    }
    int node_idx = deque.tail;
    float value = deque.pool.data[node_idx];

    // Update tail to the previous node.
    deque.tail = deque.pool.prev[node_idx];
    if (deque.tail != -1)
        deque.pool.next[deque.tail] = -1;
    else
        deque.head = -1;  // Deque is now empty.

    Deque_deallocateNode(deque, node_idx);
    return value;
}

// Peek at the front value of the deque without removing it.
float Deque_peekFront(const Deque &deque) {
    if (deque.head == -1) {
        std::cerr << "Error: Deque is empty." << std::endl;
        return 0.0f;
    }
    return deque.pool.data[deque.head];
}

// Peek at the back value of the deque without removing it.
float Deque_peekBack(const Deque &deque) {
    if (deque.tail == -1) {
        std::cerr << "Error: Deque is empty." << std::endl;
        return 0.0f;
    }
    return deque.pool.data[deque.tail];
}

// Print the contents of the deque (from front to back).
void Deque_print(const Deque &deque) {
    int current = deque.head;
    std::cout << "Deque: ";
    while (current != -1) {
        std::cout << deque.pool.data[current] << " ";
        current = deque.pool.next[current];
    }
    std::cout << std::endl;
}

// Demonstration of deque operations.
int main() {
    constexpr size_t N = 10;
    Deque deque;
    
    // Initialize the deque with N nodes.
    Deque_init(deque, N);
    
    // Insert elements at both ends.
    Deque_pushFront(deque, 1.1f);  // Deque: 1.1
    Deque_pushBack(deque, 2.2f);   // Deque: 1.1, 2.2
    Deque_pushFront(deque, 0.5f);  // Deque: 0.5, 1.1, 2.2
    Deque_pushBack(deque, 3.3f);   // Deque: 0.5, 1.1, 2.2, 3.3
    
    Deque_print(deque);  // Expected output: 0.5 1.1 2.2 3.3
    
    // Peek at both ends.
    std::cout << "Front element: " << Deque_peekFront(deque) << std::endl;
    std::cout << "Back element: " << Deque_peekBack(deque) << std::endl;
    
    // Remove elements from both ends.
    std::cout << "Popped from front: " << Deque_popFront(deque) << std::endl;
    Deque_print(deque);  // Expected: 1.1 2.2 3.3
    
    std::cout << "Popped from back: " << Deque_popBack(deque) << std::endl;
    Deque_print(deque);  // Expected: 1.1 2.2
    
    return 0;
}
