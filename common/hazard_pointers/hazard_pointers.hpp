#ifndef __HP_OWNER_H__
#define __HP_OWNER_H__

#include<globals.h>
#include<atomic>
#include<functional>
#include<thread>

alignas(CACHELINE_SIZE) unsigned const max_hazard_pointers = MAX_NR_THREADS ;

alignas(CACHELINE_SIZE) unsigned int start_cid = max_hazard_pointers / 2;

struct alignas(CACHELINE_SIZE) hazard_pointer {
    std::atomic<void *> pointer;
};

hazard_pointer hazard_pointers[max_hazard_pointers];

void Select_order(std::memory_order& Order, std::memory_order&& expected) {
  Order = (Order != std::memory_order_seq_cst ? expected : std::memory_order_seq_cst);
}

bool outstanding_hazard_pointers_for(void *p, std::memory_order Order = std::memory_order_seq_cst) {
  for (unsigned i = start_cid; i < max_hazard_pointers; ++i) {
    if (hazard_pointers[i].pointer.load(Order) == p) 
      return true;
  }
  return false;
}

alignas(CACHELINE_SIZE) std::atomic<int> reclaim_cnt(0);

template <typename T> void do_delete(void *p) { delete static_cast<T *>(p); }
struct alignas(CACHELINE_SIZE) data_to_reclaim {
  void *data;
  std::function<void(void *)> deleter;
  data_to_reclaim *next;
  template <typename T>
    data_to_reclaim(T *p) : data(p), deleter(&do_delete<T>), next(0) {}
  ~data_to_reclaim() { deleter(data); }
};

std::atomic<data_to_reclaim *> node_to_reclaim;

void add_to_reclaim_list(data_to_reclaim *node, std::memory_order Order = std::memory_order_seq_cst) {
  
  while (!node_to_reclaim.compare_exchange_weak(node->next, node, Order, 
        std::memory_order_relaxed))
    ;
}

template <typename T> void Retire(T *data, std::memory_order Order = std::memory_order_seq_cst) {
  add_to_reclaim_list(new data_to_reclaim(data), Order);
  Select_order(Order, std::memory_order_relaxed);
  reclaim_cnt.fetch_add(1, Order);
}

void delete_nodes_with_no_hazards(std::memory_order Order = std::memory_order_seq_cst) {
  data_to_reclaim *current = node_to_reclaim.exchange(nullptr, Order);

  while (current) {
    data_to_reclaim *const next = current->next;
    if (!outstanding_hazard_pointers_for(current->data, Order)) {
      delete current;
      Select_order(Order, std::memory_order_relaxed);
      reclaim_cnt.fetch_sub(1, Order);
    } else {
      Select_order(Order, std::memory_order_acq_rel);
      add_to_reclaim_list(current, Order); 
    }
    current = next;
  }
}
#endif
