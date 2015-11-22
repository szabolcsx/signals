#pragma once

#include <functional>
#include <vector>

namespace szabi
{
  template <typename... Args>
  struct overload
  {
    template <typename F, typename R>
    static constexpr auto of(R(F::*ptr)(Args...)) -> decltype(ptr)
    {
      return ptr;
    }
  };

  class connection
  {
  public:
    connection(std::function<void()>&& disconnector) : disconnector(std::move(disconnector)) {}

    ~connection() {}

    void disconnect() const
    {
      this->disconnector();
    }

  private:
    std::function<void()> disconnector;
  };

  class auto_disconnect
  {

    template<typename... Args> friend class signal;
  public:
    virtual ~auto_disconnect()
    {
      for(const auto& conn : this->connections)
      {
        conn.disconnect();
      }
    }

  protected:
    void add_connection(const connection& conn)
    {
      this->connections.push_back(conn);
    }

  private:
    std::vector<connection> connections;
  };

  template <typename... Args>
  class signal
  {
  public:
    signal() {}
    virtual ~signal() {}

    using slot_type = std::function<void(Args...)>;
    using container_type = std::vector<slot_type>;
    using iterator_type = typename container_type::iterator;

    template <typename S>
    connection connect(S&& slot)
    {
      this->slots.push_back(std::forward<S>(slot));

      connection conn =
      {
        [&]()
        {
          iterator_type it = std::prev(this->slots.end());
          this->slots.erase(it);
        }
      };

      return conn;
    }

    template <typename S, typename T>
    connection connect(S&& slot, T* instance)
    {
      static_assert(std::is_base_of<auto_disconnect, T>::value, "T must inherit auto_disconnect");
      this->slots.emplace_back([&](Args... args)
      {
        (static_cast<T*>(instance)->*slot)(std::forward<Args>(args)...);
      });

      connection conn =
      {
        [&]()
        {
          iterator_type it = std::prev(this->slots.end());
          this->slots.erase(it);
        }
      };

      instance->add_connection(conn);

      return conn;
    }

    template <typename S, typename T>
    connection connect(S&& slot, T& instance)
    {
      static_assert(std::is_base_of<auto_disconnect, T>::value, "T must inherit auto_disconnect");
      this->slots.emplace_back([&](Args... args)
      {
        (static_cast<T*>(std::addressof(instance))->*slot)(std::forward<Args>(args)...);
      });

      connection conn =
      {
        [&]()
        {
          iterator_type it = std::prev(this->slots.end());
          this->slots.erase(it);
        }
      };

      instance.add_connection(conn);

      return conn;
    }

    void operator()(Args const&... args)
    {
      for(auto const& slot : this->slots)
      {
        if(slot)
        {
          slot(args...);
        }
      }
    }
  private:
    container_type slots;
  };
}
