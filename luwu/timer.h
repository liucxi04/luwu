//
// Created by liucxi on 2022/4/26.
//

#ifndef LUWU_TIMER_H
#define LUWU_TIMER_H

#include <memory>
#include <set>
#include <vector>
#include "thread.h"

namespace liucxi {
    class TimerManager;

    class Timer : public std::enable_shared_from_this<Timer> {
        friend class TimerManager;

    public:
        typedef std::shared_ptr<Timer> ptr;

        bool cancel();

        bool refresh();

        bool reset(uint64_t ms, bool from_now);

    private:
        Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager *manager);

        explicit Timer(uint64_t next) : m_next(next) {}

    private:
        bool m_recurring = false;
        uint64_t m_ms = 0;
        uint64_t m_next = 0;
        std::function<void()> m_cb;
        TimerManager *m_manager = nullptr;
    private:
        struct Comparator {
            bool operator()(const Timer::ptr &lhs, const Timer::ptr &rhs) const;
        };
    };

    class TimerManager {
        friend class Timer;

    public:
        typedef RWMutex RWMutexType;

        TimerManager() = default;

        virtual ~TimerManager() = default;

        Timer::ptr addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false);

        Timer::ptr addConditionTimer(uint64_t ms, const std::function<void()> &cb,
                                     const std::weak_ptr<void> &weak_cond, bool recurring = false);

        uint64_t getNextTimer();

        void listExpiredCb(std::vector<std::function<void()>> &cbs);

    protected:
        virtual void onTimerInsertAtFront() = 0;

        void addTimer(const Timer::ptr &val, RWMutexType::WriteLock lock);

        bool hasTimer();

    private:
        RWMutexType m_mutex;
        std::set<Timer::ptr, Timer::Comparator> m_timers;
        bool m_tickled = false;
    };
}

#endif //LUWU_TIMER_H