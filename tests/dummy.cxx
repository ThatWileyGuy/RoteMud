#define BOOST_TEST_MODULE example
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <sstream>
#include <boost/archive/tmpdir.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <coroutine>
#include <future>
#include <optional>

using namespace boost::serialization;

class SuperFun
{
    int m_x;
    int m_y;
    double m_wheee;

  public:
    SuperFun() : m_x(1), m_y(2), m_wheee(3.0)
    {
    }

    template <class Archive> void serialize(Archive& ar, const unsigned int)
    {
        ar& make_nvp("x", m_x);
        ar& make_nvp("y", m_y);
        ar& make_nvp("wheee", m_wheee);
    }
};

namespace rote
{

BOOST_AUTO_TEST_CASE(serialization_test)
{
    SuperFun s;

    std::stringstream ss;

    boost::archive::xml_oarchive out(ss);
    out << make_nvp("SuperFun", s);

    std::cout << ss.str() << std::endl;

    BOOST_TEST(true);
}

template <typename T> struct Awaitable;

template <typename T> struct Awaiter
{
    Awaitable<T>& m_awaitable;

    // if this is true, the coroutine does not suspend
    bool await_ready() const noexcept
    {
        return m_awaitable.hasValue();
    }

    // actually suspend the coroutine
    void await_suspend(std::coroutine_handle<> h)
    {
        m_awaitable.setHandle(h);
    }

    // returns the thing we were waiting for
    T await_resume()
    {
        return m_awaitable.getValue();
    }
};

template <typename T> struct Awaitable
{
  private:
    mutable std::recursive_mutex m_lock;
    std::thread::id m_owningThread = {};
    std::optional<T> m_item;
    std::coroutine_handle<> m_handle;

  public:
    // not movable or copyable
    Awaitable() = default;
    Awaitable(const Awaitable&) = delete;
    Awaitable& operator=(const Awaitable&) = delete;
    Awaitable(Awaitable&&) = delete;
    Awaitable& operator=(Awaitable&&) = delete;

    bool hasValue() const
    {
        std::lock_guard lock(m_lock);
        return m_item.has_value();
    }

    T getValue()
    {
        std::lock_guard lock(m_lock);
        // This should only ever be called from the coroutine we resumed in setValue - check the thread ID
        assert(m_owningThread == std::this_thread::get_id());
        assert(m_item.has_value());
        T result = m_item.value();
        m_item.reset();
        return result;
    }

    void setValue(T&& value)
    {
        std::lock_guard lock(m_lock);
        assert(!m_item.has_value());
        m_item.emplace(std::move(value));
        assert(m_handle.address() != nullptr);
        m_owningThread = std::this_thread::get_id();

        // when we come back, the same coroutine might be waiting on us again
        // clear out the handle so it can get repopulated if needed
        auto localHandle = std::move(m_handle);
        m_handle = {};

        localHandle.resume();

        m_owningThread = {};
        assert(!m_item.has_value());
    }

    Awaiter<T> getAwaiter()
    {
        std::lock_guard lock(m_lock);
        assert(m_handle.address() == nullptr);
        return Awaiter<T>(*this);
    }

    bool hasHandle() const
    {
        std::lock_guard lock(m_lock);
        return m_handle.address() != nullptr;
    }

    void setHandle(std::coroutine_handle<>& h)
    {
        std::lock_guard lock(m_lock);
        assert(m_handle.address() == nullptr);
        m_handle = h;
    }
};

Awaitable<std::string> g_awaiter;

void inputLoop() noexcept
{
    char buf[256];

    while (true)
    {
        std::cin.getline(buf, sizeof(buf));

        if (!g_awaiter.hasValue())
        {
            auto line = std::string(buf);

            g_awaiter.setValue(std::move(line));

            if (!g_awaiter.hasHandle())
            {
                std::cout << "no one waiting - input loop bailing out" << std::endl;
                return;
            }
        }
        else
        {
            std::cout << "no one was waiting for a string?" << std::endl;
            return;
        }
    }
}

struct VoidReturn
{
    struct promise_type
    {
        VoidReturn get_return_object()
        {
            return {};
        }
        void return_void()
        {
            return;
        }
        std::suspend_never initial_suspend()
        {
            return {};
        }
        std::suspend_never final_suspend() noexcept
        {
            return {};
        }
        void unhandled_exception()
        {
            auto e = std::current_exception();
            assert(0);
            std::terminate();
        }
    };
};

Awaiter<std::string> getString()
{
    return g_awaiter.getAwaiter();
}

VoidReturn doFunStuff()
{
    std::cout << "What's your name?" << std::endl;

    auto name = co_await getString();

    std::cout << "And how old are you?" << std::endl;

    auto age = co_await getString();

    std::cout << "Ah, so you're " << name << " and you're " << age << " years old!" << std::endl;
}

BOOST_AUTO_TEST_CASE(coroutine_test)
{
    std::thread inputThread(&inputLoop);

    doFunStuff();

    inputThread.join();
}

} // namespace rote
