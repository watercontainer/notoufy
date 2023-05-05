#ifndef NOTOUF_NOTOUF_H
#define NOTOUF_NOTOUF_H

#include <vector>
#include <memory>
#include <concepts>
//TODO concept/constraint
//TODO thread_safe

namespace notoufy {
    // Concept to constraint the template to classes that have
    // an onEvent method accepting 0 or more args
    template<typename T, typename ... Arg>
    concept handlesEvents = requires(T t, Arg... args) {
        { t.onEvent(args...) } -> std::same_as<void>;
    };

    template<typename Ifc> requires handlesEvents<Ifc>
    class Notifier {
    private:
        std::vector<std::weak_ptr<Ifc>> observers;
    public:
        void Register(std::weak_ptr<Ifc> obs) {
            observers.push_back(obs);
        }

        void Unregister(std::weak_ptr<Ifc> obs) {
            const auto obsPtr = obs.lock();
            // Since c++20, std::erase_if replaces remove/erase idiom
            std::erase_if(observers, [obsPtr](const auto &ptr) {
                return (ptr.expired() || (obsPtr == ptr.lock()));
            });
        }

        template<typename... F, typename... Arg>
        // First arg is a Function M of signature void (Ifc::*)(F)
        // that is a pointer to a function member of class Ifc
        void Notify(void (Ifc::*M)(F...), Arg &&...args) {
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
