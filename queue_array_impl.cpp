#include <iostream>
#include <memory>

// Queue structure using a free-node pool for storage.
struct Queue {
    int front{-1}; // Index of the front element.
    int rear{-1};  // Index of the rear element.
    
    // Free node pool holding node arrays and free list information.
    struct {
        std::unique_ptr<float[]> data{nullptr};    // Node values.
        std::unique_ptr<int[]> next{nullptr};        // Next pointers for linking nodes.
        std::unique_ptr<int[]> next_free{nullptr};   // Free list linking.
        std::unique_ptr<bool[]> allocated{nullptr};  // Allocation flags.
        size_t size{0};                              // Total number of nodes.
        int free_head{-1};                           // Head of the free list.
    } free_node_stack;
};

// Initialize the queue with N nodes.
// All nodes are initially free and linked as a free stack.
void Queue_init(Queue &queue, const size_t &N) {
    queue.front = -1;
    queue.rear = -1;
    queue.free_node_stack.size = N;
    queue.free_node_stack.data = std::make_unique<float[]>(N);
    queue.free_node_stack.next = std::make_unique<int[]>(N);
    queue.free_node_stack.next_free = std::make_unique<int[]>(N);
    queue.free_node_stack.allocated = std::make_unique<bool[]>(N);
    queue.free_node_stack.free_head = 0; // Free list starts at index 0.

    for (size_t i = 0; i < N; ++i) {
        queue.free_node_stack.data[i] = 0.0f;
        queue.free_node_stack.next[i] = -1;
        queue.free_node_stack.allocated[i] = false;
        queue.free_node_stack.next_free[i] = (i < N - 1) ? static_cast<int>(i + 1) : -1;
    }
}

// Allocate a node by "popping" from the free list.
// Sets the node's value and marks it as allocated.
// Returns the allocated node index via node_idx.
void Queue_allocateNode(Queue &queue, const float &value, int &node_idx) {
    node_idx = -1;
    if (queue.free_node_stack.free_head == -1) {
        std::cerr << "Error: No free node available." << std::endl;
        return;
    }
    // Pop the node from the free list.
    node_idx = queue.free_node_stack.free_head;
    queue.free_node_stack.free_head = queue.free_node_stack.next_free[node_idx];

    // Initialize the allocated node.
    queue.free_node_stack.data[node_idx] = value;
    queue.free_node_stack.next[node_idx] = -1;
    queue.free_node_stack.allocated[node_idx] = true;
}

// Deallocate a node by "pushing" it back onto the free list.
// Checks for double deallocation.
void Queue_deallocateNode(Queue &queue, const size_t &idx) {
    if (idx >= queue.free_node_stack.size) {
        std::cerr << "Error: Index out of bounds in deallocation." << std::endl;
        return;
    }
    if (!queue.free_node_stack.allocated[idx]) {
        std::cerr << "Error: Node " << idx << " is already deallocated." << std::endl;
        return;
    }
    
    // Reset node's value and next pointer.
    queue.free_node_stack.data[idx] = 0.0f;
    queue.free_node_stack.next[idx] = -1;
    queue.free_node_stack.allocated[idx] = false;
    
    // Push the node back to the free list.
    queue.free_node_stack.next_free[idx] = queue.free_node_stack.free_head;
    queue.free_node_stack.free_head = idx;
}

// Enqueue a value into the queue.
void Queue_enqueue(Queue &queue, const float &value) {
    int new_node = -1;
    Queue_allocateNode(queue, value, new_node);
    if (new_node == -1)
        return;
    
    // If the queue is empty, set front and rear to the new node.
    if (queue.front == -1) {
        queue.front = new_node;
        queue.rear = new_node;
    } else {
        // Link the new node at the end of the queue.
        queue.free_node_stack.next[queue.rear] = new_node;
        queue.rear = new_node;
    }
}

// Dequeue a value from the queue.
// Returns the value that was removed.
float Queue_dequeue(Queue &queue) {
    if (queue.front == -1) {
        std::cerr << "Error: Queue underflow." << std::endl;
        return 0.0f;
    }
    int node_idx = queue.front;
    float value = queue.free_node_stack.data[node_idx];
    
    // Move front to the next node.
    queue.front = queue.free_node_stack.next[node_idx];
    // If the queue becomes empty, reset the rear pointer.
    if (queue.front == -1)
        queue.rear = -1;
    
    // Deallocate the node.
    Queue_deallocateNode(queue, node_idx);
    return value;
}

// Peek at the value at the front of the queue without dequeuing.
float Queue_peek(const Queue &queue) {
    if (queue.front == -1) {
        std::cerr << "Error: Queue is empty." << std::endl;
        return 0.0f;
    }
    return queue.free_node_stack.data[queue.front];
}

// Print the contents of the queue (from front to rear).
void Queue_print(const Queue &queue) {
    int current = queue.front;
    std::cout << "Queue: ";
    while (current != -1) {
        std::cout << queue.free_node_stack.data[current] << " ";
        current = queue.free_node_stack.next[current];
    }
    std::cout << std::endl;
}

// Demonstration of queue operations.
int main() {
    constexpr size_t N = 10;
    Queue queue;
    
    // Initialize the queue with N nodes.
    Queue_init(queue, N);
    
    // Enqueue values.
    Queue_enqueue(queue, 1.1f);
    Queue_enqueue(queue, 2.2f);
    Queue_enqueue(queue, 3.3f);
    Queue_print(queue);  // Expected output: 1.1 2.2 3.3
    
    // Dequeue a value.
    float dequeued = Queue_dequeue(queue);
    std::cout << "Dequeued: " << dequeued << std::endl;
    Queue_print(queue);  // Expected output: 2.2 3.3
    
    // Peek at the front value.
    std::cout << "Peek: " << Queue_peek(queue) << std::endl;
    
    return 0;
}
