# szabi::signal
### Signal pattern implemented in C++11

Declaring a signal with an int parameter:
```cpp
szabi::signal<int> signal_int_changed;
```
Connecting slots to signal:
```cpp
// Global slot
void slot_int_changed(int);

// All classes which have slots must derive from szabi::auto_disconnect
class foo : public szabi::auto_disconnect
{
  public:
  void slot_int_changed(int);
  void overloaded_slot(int);
  void overloaded_slot(int, int);
};

// Connect global slot
signal_int_changed.connect(&slot_int_changed);

// Connect lambda slot
signal_int_changed.connect([](int)
{
  //...
});

//Connect member function slot
foo f;
signal_int_changed.connect(&foo::slot_int_changed, f);
```

Connecting overloaded slots:
```cpp
szabi::signal<int> signal_1;
szabi::signal<int, int> signal_2;

foo f;

signal_1.connect(szabi::overload<int>::of(&foo::overloaded_signal), f);
signal_2.connect(szabi::overload<int, int>::of(&foo::overloaded_signal), f);
```
When connecting overloaded slots the compiler can't deduce the parameter list so you need to be explicit on that by using *szabi::overload*
```cpp
// Use with global slots
szabi::overload</* parameter list */>::of(&slot);

// Use with slots which are member functions
szabi::overload</* parameter list */>::of(&class_name::slot);
```

Using *szabi::overload* is equal to the following code:
```cpp
// szabi::overload</* parameter list */>::of(&class_name::slot);
static_cast<void(class_name::*)(/* parameter list */)>(&class_name::slot);
```

Emitting a signal calls all of the connected slots:
```cpp
signal_int_changed.emit(1);
```
Disconnecting a signal:
```cpp
// Connecting a slot return a szabi::signals::connection instance which can be
// to check if the slot is connected or disconnect it
szabi::signals::connection conn = signal.connect(&slot);

conn.disconnect();

// Checking if a slot is connected
if(conn.connected()) {
  //...
}
```

When an object goes out of scope all of it's slots will be automatically disconnected thanks to szabi::auto_disconnect:
```cpp
#include <iostream>

class should_not_seen
{
public:
  void slot() {
    std::cout << "This should not be seen" << std::endl;
  }
}

int main() {
  szabi::signal<> test_signal;

  std::cout << "Execution started" << std::endl;

  {
    should_not_seen instance;
    test_signal.connect(&should_not_seen::slot, instance);

    // The *instance* goes out of scope and the slot will be disconnected before emitting the signal
  }

  test_signal.emit();

  std::cout << "Execution done" << std::endl;

  return 0;
}
```
