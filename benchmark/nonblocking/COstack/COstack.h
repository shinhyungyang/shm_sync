#ifndef __CO_STACK_H__
#define __CO_STACK_H__ // prevent against double-inclusion
#include<globals.h>
#include<hazard_pointers.hpp>
#include<atomic>

template <typename T>
class lock_free_stack {
private:
  struct alignas(CACHELINE_SIZE) node {
    std::shared_ptr<T> data;
    node *next;
    node(T const &data_) : data(std::make_shared<T>(data_)) {}
  };
  std::atomic<node *> head;
public:
  void push(T const &data);
  std::shared_ptr<T> pop(int tid);
};
#endif
