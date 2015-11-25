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
