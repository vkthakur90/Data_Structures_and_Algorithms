#include <iostream>
#include <memory>

struct LinkedList {
    int head{-1};
    struct {
        std::unique_ptr<float[]> data{nullptr};
        std::unique_ptr<int[]> next{nullptr};
        std::unique_ptr<bool[]> is_free{nullptr};
        size_t size{0};
    } nodes;
};

void LinkedList_init(LinkedList & list, const size_t & N){
    list.nodes.data = std::make_unique<float[]>(N);
    list.nodes.next = std::make_unique<int[]>(N);
    list.nodes.is_free = std::make_unique<bool[]>(N);
    list.nodes.size = N;
    
    for(size_t idx = 0; idx < N; ++idx){
        list.nodes.data[idx] = 0.0;
    }
    
    for(size_t idx = 0; idx < N; ++idx){
        list.nodes.next[idx] = -1;
    }

    for(size_t idx = 0; idx < N; ++idx){
        list.nodes.is_free[idx] = true;
    }    
}

void LinkedList_allocateNode(LinkedList & list, const float & value, int & node_idx){
    node_idx = -1;
    auto & N = list.nodes.size;
    
    for(size_t idx = 0; idx < N; ++idx){
        if(list.nodes.is_free[idx]){
            node_idx = idx;
            list.nodes.data[idx] = value;
            list.nodes.next[idx] = -1;
            list.nodes.is_free[idx] = false;
            break;
        }
    }
}

void LinkedList_deallocateNode(LinkedList & list, const size_t & idx){
    auto & N = list.nodes.size;
    
    if(idx >= 0 && idx < N){  
        if(!list.nodes.is_free[idx]){
            list.nodes.data[idx] = 0;
            list.nodes.next[idx] = -1;
            list.nodes.is_free[idx] = true;
        }
    }
}

void LinkedList_append(LinkedList & list, const float & value){
    int new_node = -1;
    
    LinkedList_allocateNode(list, value, new_node);
    
    if(list.head == -1){
        list.head = new_node;
        return;
    }
    
    int curr_node = list.head;
    
    while(list.nodes.next[curr_node] != -1){
        curr_node = list.nodes.next[curr_node];
    }
    
    list.nodes.next[curr_node] = new_node;   
}