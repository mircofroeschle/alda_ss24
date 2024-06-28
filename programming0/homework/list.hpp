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

  /// Erzeugt eine leere Liste
  List() {last = &dummy;}

  /// Wir loeschen den Copy-Konstruktor. Damit ist es nicht mehr
  /// moeglich aus versehen eine teure Kopie der Liste zu erstellen.
  List(List &) = delete;

  /// AUFGABE 1: Destruktor für List:
  ///
  /// Lösen Sie das Problem wie folgt: Implementieren Sie unterhalb 
  /// der Zeile (List(List&) = delete;) einen Destruktor für List. 
  /// Dieser Destruktor soll solange in einer Schleife pop_front() aufrufen,
  /// bis die Liste leer ist. Nutzen Sie dabei aus, dass –wie oben beschrieben– pop_front() einen UP zurück
  /// liefert, der automatisch gelöscht wir
  ~List() {
    while( !empty()) {
      pop_front();
    }
    // std::cout << "Destruktor ausgeführt: Speicher freigegeben." << std::endl;
  }

  /// Gibt genau dann `true` zurueck, wenn die Liste leer ist.
  ///
  /// # Example
  /// ```c++
  /// List lst;
  /// std::cout << lst.empty() << "\n"; // gibt "true" aus.
  /// lst.push_front(1234);
  /// std::cout << lst.empty() << "\n"; // gibt "false" aus.
  /// ```
  bool empty() const { return !dummy.next; }

  /// Gibt die Anzahl der Elemente in der Liste zurueck.
  ///
  /// # Example
  /// ```c++
  /// List lst;
  /// lst.push_front(1234);
  /// std::cout << lst.size() << "\n"; // gibt "1" aus.
  /// lst.push_front(1337);
  /// std::cout << lst.size() << "\n"; // gibt "2" aus.
  /// ```
  size_t size() const { return num_items; }

  /// Haengt ein Element mit Wert `val` vorn an die Liste an.
  ///
  /// # Example
  /// ```c++
  /// List lst;
  /// lst.push_front(1);
  /// lst.push_front(2);
  /// std::cout << lst << "\n"; // gibt "[2, 1]" aus.
  /// ```
  Item *push_front(Value val) {
    auto new_item = std::make_unique<Item>(val);
    new_item->next = std::move(dummy.next);
    dummy.next = std::move(new_item);

    // update des letzten Elements, falls die Liste leer ist.
    if (num_items == 0) {last = dummy.next.get();}

    ++num_items;
    return dummy.next.get();
  }

  /// Entfernt das erste Element der Liste und gibt das Item als unique_ptr
  /// zurueck. Wenn die Liste leer ist, wird ein "non-owning" smart pointer
  /// zurueckgegeben.
  ///
  /// # Example
  /// ```c++
  /// List lst;
  /// lst.push_front(1);
  /// {
  ///    auto item = lst.pop_front();
  ///    assert(item->get_value() == 1);
  /// } // item wird hier freigegeben.
  /// {
  ///    auto item = lst.pop_front();
  ///    assert(!item); // item ist "non-owning" nullptr, da die Liste leer ist.
  /// }
  /// ```
  std::unique_ptr<Item> pop_front() { return extract_after(dummy); }

  /// Hilfsfunktion, die eine Callback-Funktion fuer jedes Element in der Liste
  /// aufruft. Das Callback sollte eine Funktion sein, die ein const Value& als
  /// Argument nimmt.
  ///
  /// # Example
  /// ```c++
  /// List lst;
  /// lst.push_front(1);
  /// lst.push_front(2);
  /// lst.foreach([] (const List::Value& val) { std::cout << val << " "; }); //
  /// gibt "2 1 " aus.
  /// ```
  template <typename Callback> void foreach (Callback &&cb) const {
    Item *current = dummy.next.get();
    while (current != nullptr) {
      cb(current->get_value());
      current = current->next.get();
    }
  }

  /// Diese Funktion erlaubt es eine Liste direkt auf einem ostream auszugeben,
  /// z.B. mittels std::cout.
  ///
  /// # Example
  /// ```c++
  /// List lst;
  /// lst.push_front(2);
  /// lst.push_front(1);
  /// std::cout << lst << "\n";
  /// ```
  /// Erzeugt die Ausgabe `[1, 2]`
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

  /// Haengt ein Element mit Wert `val` hinten an die Liste an.
  ///
  /// # Example
  /// ```c++
  /// List lst;
  /// lst.push_back(1);
  /// lst.push_back(2);
  /// std::cout << lst << "\n"; // gibt "[1, 2]" aus.
  /// ```
  Item *push_back(Value val) { //
    return push_back_item(std::make_unique<Item>(val));
  }

  /// Empfaengt ein (owned) unique_ptr<Item> und haengt das Item hinten an die
  /// Liste an.
  ///
  /// # Example
  /// ```c++
  /// List lst;
  /// lst.push_back_item(std::make_unique<List::Item>(1));
  /// std::cout << lst << "\n"; // gibt "[1]" aus.
  /// ```
  Item *push_back_item(std::unique_ptr<Item> &&item) {
    assert(!!item); // Tipp: `!!item` ist ein short-cut für
                    // `static_cast<bool>(item)`
    if (num_items == 0) {
      dummy.next = std::move(item);
      last = dummy.next.get();
    } else {
      last->next = std::move(item);  
      last = last->next.get();
    }
    num_items++;
    assert(!last->next);
    return nullptr;
  }

  /// Iteriert durch die Liste und ruft `predicate` fuer jedes Element auf.
  /// Wenn `predicate` `true` zurueckgibt, wird das Element an das Ende der
  /// Liste `append_to` verschoben.
  ///
  /// # Example
  /// ```c++
  /// List lst;
  /// for(int i=0; i<10; ++i) lst.push_back(i);
  /// List lst_even;
  /// lst.move_into_if(lst_even,
  ///   [] (const List::Value& val) { return val % 2 == 0; });
  /// std::cout << lst << std::endl;      // gibt "[1, 3, 5, 7, 9]" aus.
  /// std::cout << lst_even << std::endl; // gibt "[0, 2, 4, 6, 8]" aus.
  /// ```
  template <typename Predicate>
  void move_into_if(List &append_to_if_true, Predicate &&predicate) {
    const size_t initial_size = size() + append_to_if_true.size();
    (void)initial_size; // verhindert eine Warnung, falls assert wegoptimiert
    // wurde
    assert(!this->empty());

    Item* before = &dummy;
    Item* current = dummy.next.get();

    while (current) {
      if (predicate(current->get_value())) { 
        auto extracted_item = extract_after(*before);
        append_to_if_true.push_back_item(std::move(extracted_item));
        current = before->next.get();
      } else {
      before = current;
      current = current->next.get();
      }
      // std::cout << append_to_if_true <<std::endl;
    }

    assert(size() + append_to_if_true.size() == initial_size);
  }

  /// Hängt die übergebene Liste an die aktuelle Liste an; die übergebene Liste
  /// wird dabei geleert.
  ///
  /// # Example
  /// ```c++
  /// List lst1;
  /// lst1.push_back(1);
  /// List lst2;
  /// lst2.push_back(2);
  /// lst1.concat(lst2);
  /// std::cout << lst1 << std::endl; // gibt "[1, 2]" aus.
  /// std::cout << lst2 << std::endl; // gibt "[]" aus.
  /// ```
  // corrected concat
  void concat(List &other) {
    // (void)other; // verhindert Warnung; kann entfernt werden, sobald die
    // auskommentierte Implementierung genutzt wird.
    
    // Change in Aufgabe 3: Updating the last value of the new concated list
    auto last_2 = std::move(other.last);

    last->next = std::move(other.dummy.next); 
    num_items += other.num_items;
    other.num_items = 0;
    other.last = &other.dummy;

    // Change in Aufgabe 3: Updating the last value of the new concated list
    last = std::move(last_2);   
  }

  
  //Ergänzung, um den Test für Aufg. 3 durchführen zu können
  Item* get_last() const {
    return last;
  }

  /// Gibt genau dann `true` zurueck, wenn die Liste sortiert ist.
  ///
  /// # Example
  /// ```c++
  /// List lst;
  /// lst.push_back(2);
  /// assert(lst.is_sorted());
  /// lst.push_back(3);
  /// assert(lst.is_sorted());
  /// lst.push_back(1);
  /// assert(!lst.is_sorted());
  /// ```
  bool is_sorted() const {
    if (empty())
      return true;

    Item *current = dummy.next.get();
    while (current->next) {
      if (current->get_value() > current->next->get_value()) {
        return false;
      }
      current = current->next.get();
    }
    return true;
  }

  /// Sortiert die Liste mittels QuickSort-Algorithmus und gibt
  /// die Anzahl der Vergleiche zurück. Als Pivotelement wird immer das erste
  /// Element der Liste verwendet.
  ///
  /// # Example
  /// ```c++
  /// List lst;
  /// lst.push_back(3);
  /// lst.push_back(1);
  /// lst.push_back(2);
  /// lst.push_back(4);
  /// lst.sort();
  /// std::cout << lst << std::endl; // gibt "[1, 2, 3, 4]" aus.
  /// ```
  uint64_t sort(uint16_t num_of_comparisons = 0) {

    if (this->size() <=1 ) {return num_of_comparisons;}

    List greater_or_equal;

    assert(greater_or_equal.empty());
    auto pivot = this->pop_front();

    auto predicate = [&pivot, &num_of_comparisons] (const List::Value& val) { 
      num_of_comparisons++; 
      return val >= pivot->get_value(); 
    };

    this->move_into_if(greater_or_equal, predicate);
    num_of_comparisons = greater_or_equal.sort(num_of_comparisons);
    num_of_comparisons = this->sort(num_of_comparisons);
    
    // Anmerkung: Zusammenführen funktioniert nicht richtig...

    this->push_back_item(std::move(pivot));
    this->concat(greater_or_equal);
    
    return num_of_comparisons;
  }
  

private:
  Item dummy;
  /// Erweitern Sie die Klasse List um ein privates Datenelement last vom Typ Item*. Es handelt
  /// sich also um einen klassichen Pointer und keinen smart pointer! Dieser soll folgende Datenstruk-
  /// turinvariante erfüllen: last zeigt immer auf den letzten Eintrag der Liste oder auf &dummy, falls
  /// die Liste leer ist.
  Item* last;

  size_t num_items{0};

  std::unique_ptr<Item> extract_after(Item &before) {
    assert(before.next);

    auto popped = std::move(before.next);
    before.next = std::move(popped->next);

    assert(!popped->next);

    // Falls das entferte Element das letzte ist
    if (before.next.get() == &dummy) {
      last = &before;
    }
    num_items--;

    return popped;
  }
};

#endif // LIST_HPP
