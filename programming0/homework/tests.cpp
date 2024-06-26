#include "list.hpp"
#include "testing.hpp"
#include <sstream>

bool test_push_front() {
  List lst;

  fail_unless(lst.empty());

  for (int i = 0; i < 10; ++i) {
    fail_unless_eq(lst.size(), static_cast<size_t>(i));
    lst.push_front(i);
  }

  fail_if(lst.empty());

  for (int i = 10; i; --i) {
    fail_unless_eq(lst.size(), static_cast<size_t>(i));
    auto popped = lst.pop_front();
    fail_unless_eq(popped->get_value(), i - 1);
  }

  fail_unless(lst.empty());

  return true;
}

bool test_push_back() {
  List lst;

  fail_unless(lst.empty());

  for (int i = 0; i < 10; ++i) {
    fail_unless_eq(lst.size(), static_cast<size_t>(i));
    
    lst.push_back(i);
  }

  fail_if(lst.empty());

  for (int i = 10; i; --i) {
    fail_unless_eq(lst.size(), static_cast<size_t>(i));
    auto popped = lst.pop_front();
    fail_unless_eq(popped->get_value(), 10 - i);
  }

  fail_unless(lst.empty());


  return true;
}

bool test_2_4() {
  List lst;
  fail_unless(lst.empty());

  for (int i = 0; i < 3; ++i) {
    fail_unless_eq(lst.size(), static_cast<size_t>(i));
    lst.push_front(i);
  }
  
  
  lst.push_back(3);
  
  
  for (int i = 3; i; --i) {
    fail_unless_eq(lst.size(), static_cast<size_t>(i) + 1);
    auto popped = lst.pop_front();
    fail_unless_eq(popped->get_value(), i - 1);
  }

  lst.push_back(4);

  for (int i = 2; i; --i) {
    fail_unless_eq(lst.size(), static_cast<size_t>(i));
    auto popped = lst.pop_front();
  }

  fail_unless(lst.empty());
  return true;
}

bool test_foreach() {
  List lst;

  fail_unless(lst.empty());

  int ref_sum = 0;

  for (int i = 0; i < 10; ++i) {
    fail_unless_eq(lst.size(), static_cast<size_t>(i));
    lst.push_front(i);
    ref_sum += i * i;
  }

  int foreach_sum = 0;
  lst.foreach ([&foreach_sum](int i) { foreach_sum += i * i; });

  fail_unless_eq(foreach_sum, ref_sum);

  return true;
}

bool test_ostream() {
  List lst;
  lst.push_front(1);
  lst.push_front(2);

  std::stringstream ss;
  ss << lst;

  fail_unless_eq(ss.str(), "[2, 1]");
  return true;
}

bool test_destruct() {
  List lst;

  for (int i = 0; i < 100; ++i) {
    lst.push_front(i);
  }

  return true;
}

bool test_concat() {
  List lst_1;
  List lst_2;

  fail_unless(lst_1.empty());
  fail_unless(lst_2.empty());

  for (int i = 0; i < 10; ++i) {
    fail_unless_eq(lst_1.size(), static_cast<size_t>(i));
    // fail_unless_eq(lst_2.size(), static_cast<size_t>(i));
    lst_1.push_front(i);
    // lst_2.push_front(i);
  }

  lst_2.concat(lst_1);
  
  return true;
}

bool test_moveinto() {
  List lst;
  List append_if_true;

  fail_unless(lst.empty());

  for (int i = 0; i < 10; ++i) {
    fail_unless_eq(lst.size(), static_cast<size_t>(i));
    lst.push_front(i);
  }
  size_t init_size = lst.size();
  auto predicate = [] (const List::Value& val) { return val % 2 == 0; };

  lst.move_into_if(append_if_true, predicate);
  fail_unless_eq(lst.size() + append_if_true.size(), init_size);
  fail_if(lst.size() == init_size);

  return true;
}

int main() {
  run_test(test_push_front);
  run_test(test_foreach);
  run_test(test_ostream);
  run_test(test_destruct);
  run_test(test_push_back);
  run_test(test_2_4);
  run_test(test_concat);
  run_test(test_moveinto);

  return 0;
}



