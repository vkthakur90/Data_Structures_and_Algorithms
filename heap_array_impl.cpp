#include <iostream>
#include <memory>
#include <stdexcept>

// Heap structure implemented as a min-heap.
struct Heap {
    size_t capacity{0};                  // Total capacity of the heap.
    size_t size{0};                      // Current number of elements in the heap.
    std::unique_ptr<float[]> data{nullptr}; // Array to store heap elements.
};

// Initialize the Heap with a given capacity.
void Heap_init(Heap &heap, const size_t capacity) {
    heap.capacity = capacity;
    heap.size = 0;
    heap.data = std::make_unique<float[]>(capacity);
    // Optionally, initialize array values to 0.0f.
    for (size_t i = 0; i < capacity; ++i) {
        heap.data[i] = 0.0f;
    }
}

// Swap two elements in the heap.
void Heap_swap(Heap &heap, size_t i, size_t j) {
    float temp = heap.data[i];
    heap.data[i] = heap.data[j];
    heap.data[j] = temp;
}

// "Bubble up" the element at index i to restore the heap property.
void Heap_bubbleUp(Heap &heap, size_t i) {
    while (i > 0) {
        size_t parent = (i - 1) / 2;
        if (heap.data[i] < heap.data[parent]) {
            Heap_swap(heap, i, parent);
            i = parent;
        } else {
            break;
        }
    }
}

// "Bubble down" the element at index i to restore the heap property.
void Heap_bubbleDown(Heap &heap, size_t i) {
    while (true) {
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;
        size_t smallest = i;

        if (left < heap.size && heap.data[left] < heap.data[smallest])
            smallest = left;
        if (right < heap.size && heap.data[right] < heap.data[smallest])
            smallest = right;
        
        if (smallest != i) {
            Heap_swap(heap, i, smallest);
            i = smallest;
        } else {
            break;
        }
    }
}

// Insert a new key into the heap.
void Heap_insert(Heap &heap, const float key) {
    if (heap.size >= heap.capacity) {
        std::cerr << "Error: Heap is full." << std::endl;
        return;
    }
    // Place the new key at the end and bubble up.
    heap.data[heap.size] = key;
    Heap_bubbleUp(heap, heap.size);
    ++heap.size;
}

// Remove and return the minimum element (root) from the heap.
float Heap_removeMin(Heap &heap) {
    if (heap.size == 0) {
        std::cerr << "Error: Heap is empty." << std::endl;
        return 0.0f; // Alternatively, throw an exception.
    }
    float minValue = heap.data[0];
    // Replace root with the last element.
    heap.data[0] = heap.data[heap.size - 1];
    --heap.size;
    Heap_bubbleDown(heap, 0);
    return minValue;
}

// Peek at the minimum element in the heap.
float Heap_peek(const Heap &heap) {
    if (heap.size == 0) {
        std::cerr << "Error: Heap is empty." << std::endl;
        return 0.0f;
    }
    return heap.data[0];
}

// Print the heap elements (not in sorted order, but in array order).
void Heap_print(const Heap &heap) {
    std::cout << "Heap: ";
    for (size_t i = 0; i < heap.size; ++i) {
        std::cout << heap.data[i] << " ";
    }
    std::cout << std::endl;
}

// Demonstration of heap operations.
int main() {
    constexpr size_t CAPACITY = 10;
    Heap heap;
    
    Heap_init(heap, CAPACITY);
    
    // Insert elements into the heap.
    Heap_insert(heap, 5.0f);
    Heap_insert(heap, 3.0f);
    Heap_insert(heap, 8.0f);
    Heap_insert(heap, 1.0f);
    Heap_insert(heap, 4.0f);
    
    Heap_print(heap);  // Expected: root should be the minimum value.
    
    // Peek at the minimum element.
    std::cout << "Min element: " << Heap_peek(heap) << std::endl;
    
    // Remove elements one by one.
    std::cout << "Removing elements: ";
    while (heap.size > 0) {
        std::cout << Heap_removeMin(heap) << " ";
    }
    std::cout << std::endl;
    
    return 0;
}
