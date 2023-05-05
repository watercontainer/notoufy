#ifndef NOTOUF_NOTOUF_H
#define NOTOUF_NOTOUF_H

#include <vector>
#include <memory>
#include <concepts>
#include <mutex>

namespace notoufy {

    class NullMutex {
    public:
        void lock() {};
        void unlock() {};
    };

    using Multithreaded = std::mutex;

    template<typename Ifc, typename Mutex = NullMutex>
    class Notifier {
    private:
        std::vector<std::weak_ptr<Ifc>> observers;
        Mutex m;
    public:
        void Register(std::weak_ptr<Ifc> obs) {
            std::lock_guard lock(m);
            observers.push_back(obs);
        }

        void Unregister(std::weak_ptr<Ifc> obs) {
            const auto obsPtr = obs.lock();
            std::lock_guard lock(m);
            // Since c++20, std::erase_if replaces remove/erase idiom
            std::erase_if(observers, [obsPtr](const auto &ptr) {
                return (ptr.expired() || (obsPtr == ptr.lock()));
            });
        }

        template<typename... F, typename... Arg>
        // First arg is a Function M of signature void (Ifc::*)(F)
        // that is a pointer to a function member of class Ifc
        void Notify(void (Ifc::*M)(F...), Arg &&...args) {
            std::lock_guard lock(m);
            // take the opportunity to clean up any expired observers
            std::erase_if(observers, [&M, &args...](const auto &obs) {
                auto ptr = obs.lock();
                if (ptr) {
                    (ptr.get()->*M)(std::forward<Arg>(args)...);
                    return false;
                }
                return true;
            });
        }
    };
};


#endif //NOTOUF_NOTOUF_H
