#ifndef SIGNAL_H__
#define SIGNAL_H__

#include <functional>
#include <vector>
#include <mutex>
#include <memory>

namespace szabi
{
  // When a slot is overloaded an ugly cast should be used
  // static_cast<void(T::*)(<parameters>)>(&T::<slot>);
  // This helper simplifies this a bit
  // overload<parameters>::of(&T::slot);
  template <typename... Args>
  struct overload
  {
    template <typename F, typename R>
    static constexpr auto of(R(F::*ptr)(Args...)) -> decltype(ptr)
    {
      return ptr;
    }
  };

  namespace
  {
    class slot_base
    {
    public:
      virtual void disconnect() = 0;
    };

    template <typename... Args>
    class slot_impl : public slot_base
    {
    public:
      using slot_type = std::function<void(Args...)>;
      using disconnector_type = std::function<void()>;

      slot_impl(slot_type&& slot, disconnector_type&& disconnector) :
        slot(std::move(slot)),
        disconnector(std::move(disconnector))
      {}

      void disconnect()
      {
        this->disconnector();
      }

      void operator()(Args... args)
      {
        this->slot(std::forward<Args>(args)...);
      }

      private:
        slot_type slot;
        disconnector_type disconnector;
    };
  }

  // This class manages a connection between a signal and slot
  class connection
  {
  public:
    connection(const std::weak_ptr<slot_base>& slot) : slot(slot) {}

    ~connection() {}

    void disconnect() const
    {
      if(auto temp = this->slot.lock())
      {
        temp->disconnect();
      }
    }

    bool connected() const
    {
      return !this->slot.expired();
    }

  private:
    std::weak_ptr<slot_base> slot;
  };

  template <typename... Args>
  class signal;

  namespace signals
  {
    // This class is used to disconnect all slots when the object is destroyed
    class auto_disconnect
    {
      // A signal has access to add_connection function
      template <typename... Args> friend class szabi::signal;
    public:
      virtual ~auto_disconnect()
      {
        std::lock_guard<std::mutex> lock(this->mutex);
        for(const auto& conn : this->connections)
        {
          conn.disconnect();
        }
      }

    protected:
      void add_connection(const connection& conn)
      {
        std::lock_guard<std::mutex> lock(this->mutex);
        this->connections.push_back(conn);
      }

    private:
      std::vector<connection> connections;
      mutable std::mutex mutex;
    };
  }

  template <typename... Args>
  class signal
  {
  public:
    signal() {}
    virtual ~signal() {}

    using slot_type = slot_impl<Args...>;
    using slot_container_type = std::vector<std::shared_ptr<slot_type>>;
    using slot_iterator_type = typename slot_container_type::iterator;

    template <typename S>
    connection connect(S&& slot)
    {
      return this->connect_impl(slot);
    }

    template <typename S, typename T>
    connection connect(S&& slot, T* instance)
    {
      static_assert(std::is_base_of<signals::auto_disconnect, T>::value, "T must inherit auto_disconnect");
      connection conn = this->connect_impl(
        [&](Args... args)
        {
          // Using a lambda function to call a slot which is a member function
          (static_cast<T*>(instance)->*slot)(std::forward<Args>(args)...);
        });

      instance->add_connection(conn);

      return conn;
    }

    template <typename S, typename T>
    connection connect(S&& slot, T& instance)
    {
      return this->connect(slot, std::addressof(instance));
    }

    void operator()(Args const&... args)
    {
      std::lock_guard<std::mutex> lock(this->mutex);
      for(auto const& slot : this->slots)
      {
        if(slot)
        {
          (*slot)(args...);
        }
      }
    }
  private:
    slot_container_type slots;
    mutable std::mutex mutex;

    template<typename S>
    connection connect_impl(S&& slot)
    {
      std::lock_guard<std::mutex> lock(this->mutex);
      this->slots.push_back(std::shared_ptr<slot_type>(new slot_type(
        std::forward<S>(slot), [&]()
        {
          std::lock_guard<std::mutex> lock(this->mutex);
          slot_iterator_type it = std::prev(this->slots.end());
          this->slots.erase(it);
        })));

      return { this->slots.back() };
    }
  };
}

#endif /* SIGNAL_H__ */
