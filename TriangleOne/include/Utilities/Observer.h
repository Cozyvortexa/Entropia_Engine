#pragma once
#include <functional>
#include <vector>
#include <unordered_map>


struct ScopedConnection {
    ~ScopedConnection() { if (disconnectAction) disconnectAction();}

    ScopedConnection() = default;
    ScopedConnection(std::function<void()> disconnectAction) {
        this->disconnectAction = disconnectAction;
    }

    std::function<void()> disconnectAction;

    // Move
    ScopedConnection(ScopedConnection&& other) noexcept
        : disconnectAction(std::move(other.disconnectAction)) {
        other.disconnectAction = nullptr;
    }

    ScopedConnection& operator=(ScopedConnection&& other) noexcept {
        if (this != &other) {
            if (disconnectAction) disconnectAction(); // We remove the old one
            disconnectAction = std::move(other.disconnectAction);
            other.disconnectAction = nullptr;
        }
        return *this;
    }

    // Copy probidden
    ScopedConnection(const ScopedConnection&) = delete;
    ScopedConnection& operator=(const ScopedConnection&) = delete;
};

template<typename T>
class Observer{
public:
    using Callback = std::function<void(const T&)>;
    Observer() = default;

    //Movement and copying are prohibited
    Observer(const Observer&) = delete;
    Observer& operator=(const Observer&) = delete;
    Observer(Observer&&) = delete;
    Observer& operator=(Observer&&) = delete;

    void Set(const T& value)
    {
        if (value == m_value)
            return;

        m_value = value;

        for (auto& cb : m_callbacks)
            cb.second(m_value);
    }

    const T& Get() const { return m_value; }

    [[nodiscard]] ScopedConnection Subscribe(std::function<void(const T&)> callback) {
        size_t id = ++m_nextId;
        m_callbacks[id] = callback;

        return ScopedConnection{ [this, id]() {
            this->m_callbacks.erase(id);
        } };
    }

    void Unsubscribe(int id)
    {
        auto it = m_callbacks.find(id);
        if (it == m_callbacks.end()) return;

        m_callbacks.erase(id);
    }

private:
    int m_nextId = 0;
    T m_value{};
    std::unordered_map<int, Callback> m_callbacks;
};