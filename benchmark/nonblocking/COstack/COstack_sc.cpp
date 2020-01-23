#include"COstack.h"

template <typename T>
void lock_free_stack<T>::push(T const &data) {
  node *const new_node = new node(data);
  new_node->next = head.load();
  while (!head.compare_exchange_weak(new_node->next, new_node))
    ;
}

template <typename T>
std::shared_ptr<T> lock_free_stack<T>::pop(int tid) {
  node *old_head = head.load();
  do {
    node *temp;
    do {
      temp = old_head;
      hazard_pointers[tid].pointer.store(old_head);
      old_head = head.load();
    } while (old_head != temp);
  } while (old_head && !head.compare_exchange_strong(old_head, old_head->next));

  // < Retire & Reclaim step >
  hazard_pointers[tid].pointer.store(nullptr);
  std::shared_ptr<T> res;
  if (old_head) {
    res.swap(old_head->data);
    Retire(old_head);
    if (reclaim_cnt.load() >= 100)  
      delete_nodes_with_no_hazards(); 
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
