#include <iostream>
#include <memory>

// Stack structure using a free-node pool for storage.
struct Stack {
    int top{-1}; // Index of the top element in the stack

    // The free node pool holding node arrays and free list information.
    struct {
        std::unique_ptr<float[]> data{nullptr};    // Node values
        std::unique_ptr<int[]> next{nullptr};        // Next pointers for linking nodes
        std::unique_ptr<int[]> next_free{nullptr};   // Free list linking (free stack)
        std::unique_ptr<bool[]> allocated{nullptr};  // Allocation flags
        size_t size{0};                              // Total number of nodes
        int free_head{-1};                           // Head of the free list (index of first free node)
    } free_node_stack;
};

// Initialize the stack with N nodes.
// All nodes are initially free and linked as a free stack.
void Stack_init(Stack &stack, const size_t &N) {
    stack.top = -1;
    stack.free_node_stack.size = N;
    stack.free_node_stack.data = std::make_unique<float[]>(N);
    stack.free_node_stack.next = std::make_unique<int[]>(N);
    stack.free_node_stack.next_free = std::make_unique<int[]>(N);
    stack.free_node_stack.allocated = std::make_unique<bool[]>(N);
    stack.free_node_stack.free_head = 0; // Free list starts at index 0

    for (size_t i = 0; i < N; ++i) {
        stack.free_node_stack.data[i] = 0.0f;
        stack.free_node_stack.next[i] = -1;
        stack.free_node_stack.allocated[i] = false;
        stack.free_node_stack.next_free[i] = (i < N - 1) ? static_cast<int>(i + 1) : -1;
    }
}

// Allocate a node by "popping" from the free stack.
// Sets the node's value and marks it as allocated.
// Returns the allocated node index via node_idx.
void Stack_allocateNode(Stack &stack, const float &value, int &node_idx) {
    node_idx = -1;
    if (stack.free_node_stack.free_head == -1) {
        std::cerr << "Error: No free node available." << std::endl;
        return;
    }
    // Pop: take the node at free_head.
    node_idx = stack.free_node_stack.free_head;
    // Update free_head to the next free node.
    stack.free_node_stack.free_head = stack.free_node_stack.next_free[node_idx];

    // Initialize the allocated node.
    stack.free_node_stack.data[node_idx] = value;
    stack.free_node_stack.next[node_idx] = -1;
    stack.free_node_stack.allocated[node_idx] = true;
}

// Deallocate a node by "pushing" it back onto the free stack.
// Checks for double deallocation.
void Stack_deallocateNode(Stack &stack, const size_t &idx) {
    if (idx >= stack.free_node_stack.size) {
        std::cerr << "Error: Index out of bounds in deallocation." << std::endl;
        return;
    }
    if (!stack.free_node_stack.allocated[idx]) {
        std::cerr << "Error: Node " << idx << " is already deallocated." << std::endl;
        return;
    }
    // Reset node's value and next pointer.
    stack.free_node_stack.data[idx] = 0.0f;
    stack.free_node_stack.next[idx] = -1;
    stack.free_node_stack.allocated[idx] = false;

    // Push: add this node back to the free stack.
    stack.free_node_stack.next_free[idx] = stack.free_node_stack.free_head;
    stack.free_node_stack.free_head = idx;
}

// Push a value onto the stack.
void Stack_push(Stack &stack, const float &value) {
    int new_node = -1;
    Stack_allocateNode(stack, value, new_node);
    if (new_node == -1)
        return;

    // Link the new node into the stack.
    // New node's next pointer points to the current top.
    stack.free_node_stack.next[new_node] = stack.top;
    // Update the top of the stack.
    stack.top = new_node;
}

// Pop a value from the stack.
// Returns the popped value.
float Stack_pop(Stack &stack) {
    if (stack.top == -1) {
        std::cerr << "Error: Stack underflow." << std::endl;
        return 0.0f; // Alternatively, throw an exception.
    }
    int node_idx = stack.top;
    float value = stack.free_node_stack.data[node_idx];
    // Update top to the next element in the stack.
    stack.top = stack.free_node_stack.next[node_idx];
    // Deallocate the node.
    Stack_deallocateNode(stack, node_idx);
    return value;
}

// Peek at the top value of the stack without popping it.
float Stack_peek(const Stack &stack) {
    if (stack.top == -1) {
        std::cerr << "Error: Stack is empty." << std::endl;
        return 0.0f;
    }
    return stack.free_node_stack.data[stack.top];
}

// Print the contents of the stack (from top to bottom).
void Stack_print(const Stack &stack) {
    int current = stack.top;
    std::cout << "Stack: ";
    while (current != -1) {
        std::cout << stack.free_node_stack.data[current] << " ";
        current = stack.free_node_stack.next[current];
    }
    std::cout << std::endl;
}

// Demonstration of stack operations.
int main() {
    constexpr size_t N = 10;
    Stack stack;

    // Initialize the stack with N nodes.
    Stack_init(stack, N);

    // Push values onto the stack.
    Stack_push(stack, 1.0f);
    Stack_push(stack, 2.0f);
    Stack_push(stack, 3.0f);
    Stack_print(stack);  // Expected: 3 2 1  (top to bottom)

    // Pop a value from the stack.
    float popped = Stack_pop(stack);
    std::cout << "Popped: " << popped << std::endl;
    Stack_print(stack);  // Expected: 2 1

    // Peek at the top value.
    std::cout << "Peek: " << Stack_peek(stack) << std::endl;

    return 0;
}
