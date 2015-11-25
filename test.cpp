#include <signals.h>

#include <iostream>
#include <string>
#include <cstdlib>

void slot(const std::string& message) {
    std::cout << message << std::endl << std::endl;
}

void slot_with_ref(std::string& message) {
    message = "The slot was executed";
}

void slot_should_be_disconnected() {
    std::cout << "This should not be seen" << std::endl << std::endl;
}

void overloaded_slot(const std::string& message) {
    slot(message);
}

void overloaded_slot(const std::string& message1, const std::string& message2) {
    std::cout << message1 << " " << message2 << std::endl << std::endl;
}

class slots : public szabi::signals::auto_disconnect {
public:
    slots() { }

    ~slots() { }

    void slot(const std::string& message) {
        std::cout << message << std::endl << std::endl;
    }

    void overloaded_slot(const std::string& message) {
        this->slot(message);
    }

    void overloaded_slot(const std::string& message1, const std::string& message2) {
        std::cout << message1 << " " << message2 << std::endl << std::endl;
    }

    void slot0() {
        std::cout << "Hello world" << std::endl << std::endl;
    }

    void slot1(const std::string& message) {
        this->slot(message);
    }

    void slot2(const std::string& message1, const std::string& message2) {
        std::cout << message1 << " " << message2 << std::endl << std::endl;
    }
};

class this_should_not_be_seen : public szabi::signals::auto_disconnect {
public:
    this_should_not_be_seen() {}
    ~this_should_not_be_seen() {}

    void slot_should_be_disconnected() {
        std::cout << "This should not be seen" << std::endl << std::endl;
    }
};

int main() {
    szabi::signal<> signal0;
    szabi::signal<const std::string&> signal1;
    szabi::signal<const std::string&, const std::string&> signal2;
    szabi::signal<std::string&> signal_ref;

    signal_ref.connect(&slot_with_ref);

    slots s;
    signal0.connect(&slots::slot0, s);

    signal1.connect([](const std::string& message) {
        std::cout << message << std::endl << std::endl;
    });

    // overloaded slots
    signal1.connect(szabi::overload<std::string>::of(&overloaded_slot));
    signal2.connect(szabi::overload<std::string, std::string>::of(&overloaded_slot));

    // using overload<...>::of(&foo::bar) helper
    signal1.connect(szabi::overload<std::string>::of(&slots::overloaded_slot), s);
    signal2.connect(szabi::overload<std::string, std::string>::of(&slots::overloaded_slot), s);

    signal1.connect(&slots::slot1, &s);
    signal2.connect(&slots::slot2, &s);

    auto conn0 = signal0.connect(&slot_should_be_disconnected);
    conn0.disconnect();

    if (!conn0.connected()) {
        std::cout << "Slot disconnected" << std::endl << std::endl;
    }

    {
        this_should_not_be_seen should_not_seen;
        szabi::signals::connection conn1 = signal0.connect(&this_should_not_be_seen::slot_should_be_disconnected, should_not_seen);
    }

    signal0.emit();
    signal1.emit("Message one");
    signal2.emit("Message one", "Message two");

    std::string message = "The slot wasn't executed";
    signal_ref.emit(message);

    std::cout << message << std::endl << std::endl;

    return EXIT_SUCCESS;
}
