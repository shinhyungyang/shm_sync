#include"COstack.h"

template <typename T> 
void lock_free_stack<T>::push(T const &data) {
  node *const new_node = new node(data);
  new_node->next = head.load(std::memory_order_acquire);
  while (!head.compare_exchange_weak(new_node->next, new_node,
        std::memory_order_acq_rel,
        std::memory_order_relaxed));
}
template <typename T>
std::shared_ptr<T> lock_free_stack<T>::pop(int tid) {
  node *old_head = head.load(std::memory_order_relaxed);
  do {
    node *temp;
    do {
      temp = old_head;
      hazard_pointers[tid].pointer.store(old_head);
      old_head = head.load(std::memory_order_acquire);
    } while (old_head != temp);
  } while (old_head && !head.compare_exchange_strong(old_head, old_head->next,
        std::memory_order_acq_rel,
        std::memory_order_relaxed));

  // < Retire & Reclaim step >
  hazard_pointers[tid].pointer.store(nullptr, std::memory_order_relaxed);
  std::shared_ptr<T> res;
  if (old_head) {
    res.swap(old_head->data);
    Retire(old_head, std::memory_order_acq_rel);
    if (reclaim_cnt.load(std::memory_order_acquire) >= 100)  
      delete_nodes_with_no_hazards(std::memory_order_acquire); 
  }
  return res;
}

lock_free_stack<int> ls;

void sync_producer(int iter) {

  for (int i = 0; i < iter; i++) {
    ls.push(i);
  }
}

void sync_consumer(int iter, int tid) {

  for (int i = 0; i < iter; i++) {
    ls.pop(tid);
  }
  while (ls.pop(tid))
    ;
}
