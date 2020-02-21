#include<globals.h>
#include<memory>
#include<mutex>
#include<stack>



template<typename T>
class threadsafe_stack
{
private:
  struct node {
    std::shared_ptr<T> data;
    node *next;
    node(T const &data_) : data(std::make_shared<T>(data_)) {}
  };
  mutable std::mutex m;

  node * head;

public:
  threadsafe_stack() {
    node * const init_head = new node(0);
    init_head -> next = nullptr;
    this->head = init_head;
  }
  ~threadsafe_stack() {
    if (head == nullptr) return;
    for (auto n = head; n != nullptr; n=n->next)
      delete n;
  }

  void push(T new_value) {
    std::lock_guard<std::mutex> lock(m);
    node * const new_node = new node(new_value);
    new_node -> next = head;
    head = new_node;
  }

  void pop() {
    do{
      m.lock();
      if(head == nullptr) {
        m.unlock();
        continue;
      }
      else{
        node * old_head = head;
        head = old_head -> next;
        delete old_head;
        break;
      }
    } while(1);
    m.unlock();
  }
};

threadsafe_stack <int> ts;

void sync_producer(int iter) {

  for (int i = 0; i < iter; i++) {
    ts.push(i);
  }
}

void sync_consumer(int iter) {

  for (int i = 0; i < iter; i++) {
    ts.pop();
  }
}
