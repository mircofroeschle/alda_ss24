#ifndef LIST_HPP
#define LIST_HPP

#include <cassert>
#include <iostream>
#include <memory>

class List {
public:
  using Value = int;

  struct Item {
    std::unique_ptr<Item> next;

    Item() : Item(0) {}
    Item(Value v) : value{v} {}
    const Value &get_value() { return value; }

  private:
    Value value;
  };

  // Erzeugt eine leere Liste
  List() : last(&dummy) {}

  // Wir loeschen den Copy-Konstruktor. Damit ist es nicht mehr
  // moeglich ausversehen eine teure Kopie der Liste zu erstellen.
  List(List &) = delete;

  ~List() {
    // while(!empty())
    //     pop_front();
  }

  // Gibt genau dann `true` zurueck, wenn die Liste leer ist.
  //
  // # Example
  // ```c++
  // List lst;
  // std::cout << lst.empty() << "\n"; // gibt "true" aus.
  // lst.push_front(1234);
  // std::cout << lst.empty() << "\n"; // gibt "false" aus.
  // ```
  bool empty() const { return !dummy.next; }

  // Gibt die Anzahl der Elemente in der Liste zurueck.
  //
  // # Example
  // ```c++
  // List lst;
  // lst.push_front(1234);
  // std::cout << lst.size() << "\n"; // gibt "1" aus.
  // lst.push_front(1337);
  // std::cout << lst.size() << "\n"; // gibt "2" aus.
  // ```
  size_t size() const { return num_items; }

  // Haengt ein Element mit Wert `val` vorn an die Liste an.
  //
  // # Example
  // ```c++
  // List lst;
  // lst.push_front(1);
  // lst.push_front(2);
  // std::cout << lst << "\n"; // gibt "[2, 1]" aus.
  // ```
  Item *push_front(Value val) {
    auto new_item = std::make_unique<Item>(val);
    new_item->next = std::move(dummy.next);
    dummy.next = std::move(new_item);

    if (++num_items == 0) {
      last = dummy.next.get();
    }

    return dummy.next.get();
  }

  // Entfernt das erste Element der Liste und gibt das Item als unique_ptr
  // zurueck. Wenn die Liste leer ist, wird ein "non-owning" smart pointer
  // zurueckgegeben.
  //
  // # Example
  // ```c++
  // List lst;
  // lst.push_front(1);
  // {
  //    auto item = lst.pop_front();
  //    std::cout << item->get_value() << "\n"; // gibt "1" aus.
  // } // item wird hier freigegeben.
  // {
  //    auto item = lst.pop_front();
  //    assert(!item); // item ist "non-owning" nullptr, da die Liste leer ist.
  // }
  // ```
  std::unique_ptr<Item> pop_front() { return extract_after(dummy); }

  // Hilfsfunktion, die eine Callback-Funktion fuer jedes Element in der Liste
  // aufruft. Das Callback sollte eine Funktion sein, die ein const Value& als
  // Argument nimmt.
  //
  // # Example
  // ```c++
  // List lst;
  // lst.push_front(1);
  // lst.push_front(2);
  // lst.foreach([] (const List::Value& val) { std::cout << val << " "; }); //
  // gibt "2 1 " aus.
  // ```
  template <typename Callback> void foreach (Callback &&cb) const {
    Item *current = dummy.next.get();
    while (current != nullptr) {
      cb(current->get_value());
      current = current->next.get();
    }
  }

  // Diese Funktion erlaubt es eine Liste direkt auf einem ostream auszugeben,
  // z.B. mittels std::cout.
  //
  // # Example
  // ```c++
  // List lst;
  // lst.push_front(2);
  // lst.push_front(1);
  // std::cout << lst << "\n";
  // ```
  // Erzeugt die Ausgabe `[1, 2]`
  friend std::ostream &operator<<(std::ostream &stream, const List &list) {
    stream << '[';
    bool first_element = true;
    list.foreach ([&](const Value &value) {
      if (!first_element)
        stream << ", ";
      first_element = false;
      stream << value;
    });
    stream << ']';
    return stream;
  }

  ///////////////////////////////////////////

  // Haengt ein Element mit Wert `val` hinten an die Liste an.
  //
  // # Example
  // ```c++
  // List lst;
  // lst.push_back(1);
  // lst.push_back(2);
  // std::cout << lst << "\n"; // gibt "[1, 2]" aus.
  // ```
  Item *push_back(Value val) {
    assert(!last->next);
    return push_back_item(std::make_unique<Item>(val));
  }

  // Empfaengt ein (owned) unique_ptr<Item> und haengt das Item hinten an die
  // Liste an.
  //
  // # Example
  // ```c++
  // List lst;
  // lst.push_back_item(std::make_unique<List::Item>(1));
  // std::cout << lst << "\n"; // gibt "[1]" aus.
  // ```
  Item *push_back_item(std::unique_ptr<Item> &&item) {
    assert(!!item);
    last->next = std::move(item);
    last = last->next.get();
    num_items++;
    return last;
  }

  // Iteriert durch die Liste und ruft `predicate` fuer jedes Element auf.
  // Wenn `predicate` `true` zurueckgibt, wird das Element an das Ende der Liste
  // `append_to` verschoben.
  //
  // # Example
  // ```c++
  // List lst;
  // for(int i=0; i<10; ++i) lst.push_back(i);
  // List lst_even;
  // lst.move_into_if(lst_even, [] (const List::Value& val) { return val % 2 ==
  // 0; }); std::cout << lst << std::endl;      // gibt "[1, 3, 5, 7, 9]" aus.
  // std::cout << lst_even << std::endl; // gibt "[0, 2, 4, 6, 8]" aus.
  // ```
  template <typename Predicate>
  void move_into_if(List &append_to, Predicate &&predicate) {
    const size_t initial_size = size() + append_to.size();

    Item *previous = &dummy;
    Item *current = dummy.next.get();

    while (current) {
      if (predicate(current->get_value())) {
        std::cout << current->get_value() << std::endl;
        auto popped = extract_after(*previous);
        append_to.push_back_item(std::move(popped));
      }

      previous = previous->next.get();
      current = previous->next.get();
    }

    assert(size() + append_to.size() == initial_size);
  }

  // Gibt genau dann `true` zurueck, wenn die Liste sortiert ist.
  //
  // # Example
  // ```c++
  // List lst;
  // lst.push_back(2);
  // assert(lst.is_sorted());
  // lst.push_back(3);
  // assert(lst.is_sorted());
  // lst.push_back(1);
  // assert(!lst.is_sorted());
  // ```
  bool is_sorted() const {
    Item *current = dummy.next.get();
    while (current->next) {
      if (current->get_value() > current->next->get_value()) {
        return false;
      }
      current = current->next.get();
    }
    return true;
  }

private:
  Item dummy;
  Item *last;

  size_t num_items{0};

  std::unique_ptr<Item> extract_after(Item &before) {
    assert(before.next);

    auto popped = std::move(before.next);
    before.next = std::move(popped->next);

    assert(!popped->next);

    if (!before.next) {
      last = &before;
    }

    num_items--;
    return popped;
  }
};

#endif // LIST_HPP
#ifndef TESTING_HPP
#define TESTING_HPP

#define fail_if(condition)                                                     \
  if (condition) {                                                             \
    std::cerr << "Test failed (" __FILE__ << ":" << __LINE__                   \
              << "):\n - Condition: " #condition << "\n"                       \
              << std::endl;                                                    \
    return false;                                                              \
  }

#define fail_if_msg(condition, msg)                                            \
  if (condition) {                                                             \
    std::cerr << "Test failed (" __FILE__ << ":" << __LINE__                   \
              << "):\n - Condition: " #condition << "\n - Message:   " << msg  \
              << "\n"                                                          \
              << std::endl;                                                    \
    return false;                                                              \
  }

#define fail_unless(condition) fail_if(!(condition))
#define fail_unless_msg(condition, msg) fail_if_msg(!(condition), msg)

#define fail_unless_eq(a, b)                                                   \
  do {                                                                         \
    auto a_ = a;                                                               \
    auto b_ = b;                                                               \
    fail_unless_msg(a_ == b_, "expected " #a " ("                              \
                                  << a_ << ") to be equal to " #b " (" << b_   \
                                  << ")");                                     \
  } while (0)

#define fail_if_eq(a, b)                                                       \
  do {                                                                         \
    auto a_ = a;                                                               \
    auto b_ = b;                                                               \
    fail_if_msg(a_ == b_, "expected " #a " ("                                  \
                              << a_ << ") to be different from " #b " (" << b_ \
                              << ")");                                         \
  } while (0)

#define run_test(func)                                                         \
  do {                                                                         \
    std::cout << "Running test: " #func << std::endl;                          \
    if (func()) {                                                              \
      std::cout << "\033[0;32mTest succeeded: " #func << "\033[0m"             \
                << std::endl;                                                  \
    } else {                                                                   \
      std::cout << "\033[0;31mTest failed: " #func << "\033[0m" << std::endl;  \
    }                                                                          \
  } while (0)

#endif // TESTING_HPP
#include "list.hpp"
#include "testing.hpp"
#include <sstream>

bool test_push_front() {
  List lst;

  fail_unless(lst.empty());

  for (int i = 0; i < 10; ++i) {
    fail_unless_eq(lst.size(), i);
    lst.push_front(i);
  }

  fail_if(lst.empty());

  for (int i = 10; i; --i) {
    auto popped = lst.pop_front();
    fail_unless_eq(popped->get_value(), i - 1);
  }

  fail_unless(lst.empty());

  return true;
}

bool test_foreach() {
  List lst;

  fail_unless(lst.empty());

  int ref_sum = 0;

  for (int i = 0; i < 10; ++i) {
    fail_unless_eq(lst.size(), i);
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

  for (int i = 0; i < 1000; ++i) {
    lst.push_front(i);
  }

  return true;
}

int main() {
  run_test(test_push_front);
  run_test(test_foreach);
  run_test(test_ostream);
  run_test(test_destruct);

  return 0;
}
