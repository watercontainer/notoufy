//
// Created by Amrou on 02/05/2023.
//
#include "notoufy.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace notoufy;

class INotifier {
public:
    virtual ~INotifier() = default;

    virtual void onEvent() = 0;
    virtual void onEventIncrement() = 0;
    virtual void onEvent1(int arg) = 0;
};

class Observer : public INotifier {
public:
    virtual void onEvent() override {
        notified = true;
    }

    virtual void onEventIncrement() override {
        ++counter;
    }

    virtual void onEvent1([[maybe_unused]] int arg) override {
        notified = true;
    }

    bool called() const {
        return notified;
    }

    const uint32_t& count() const {
        return counter;
    }

private:
    bool notified = false;
    uint32_t counter = 0;
};

TEST(Notifications, test_register) {
    Notifier<INotifier> notification_bus;

    auto observer = std::make_shared<Observer>();
    notification_bus.Notify(&INotifier::onEvent);
    ASSERT_FALSE(observer->called());

    notification_bus.Register(observer);
    notification_bus.Notify(&INotifier::onEvent);
    ASSERT_TRUE(observer->called());
}

TEST(Notifications, test_unregister) {
    Notifier<INotifier> notification_bus;
    auto observer = std::make_shared<Observer>();

    notification_bus.Register(observer);
    notification_bus.Unregister(observer);
    notification_bus.Notify(&INotifier::onEvent);
    ASSERT_FALSE(observer->called());
}

TEST(Notifications, test_notify_one_no_args) {
    Notifier<INotifier> notification_bus;

    auto observer = std::make_shared<Observer>();
    notification_bus.Register(observer);
    notification_bus.Notify(&INotifier::onEvent);

    ASSERT_TRUE(observer->called());
}

TEST(Notifications, test_notify_multiple_no_args) {
    Notifier<INotifier> notification_bus;

    std::vector<std::shared_ptr<Observer>> observers;
    for (auto i = 0; i < 10; ++i) {
        observers.emplace_back(std::make_shared<Observer>());
        notification_bus.Register(observers[i]);
    }

    notification_bus.Notify(&INotifier::onEvent);

    for (auto &obs: observers) {
        ASSERT_TRUE(obs->called());
    }
}

TEST(Notifications, test_notify_one_arg) {
    Notifier<INotifier> notification_bus;

    auto observer = std::make_shared<Observer>();
    notification_bus.Register(observer);
    notification_bus.Notify(&INotifier::onEvent1, 0xcafe);

    ASSERT_TRUE(observer->called());
}

TEST(Notifications, test_notify_multiple_arg) {
    Notifier<INotifier> notification_bus;

    std::vector<std::shared_ptr<Observer>> observers;
    for (auto i = 0; i < 10; ++i) {
        observers.emplace_back(std::make_shared<Observer>());
        notification_bus.Register(observers[i]);
    }

    notification_bus.Notify(&INotifier::onEvent1, 0xcafe);

    for (auto &obs: observers) {
        ASSERT_TRUE(obs->called());
    }
}

TEST(Notifications, test_notify_one_count_multithreaded) {
    Notifier<INotifier, Multithreaded> notification_bus;

    auto observer = std::make_shared<Observer>();
    notification_bus.Register(observer);

    // Spawn multiple thread to increment the same variable.
    std::vector<std::thread> threads;
    for (int i = 0 ; i < 10 ; ++i) {
        threads.emplace_back([&notification_bus]() {
            for (int j = 0; j < 10000 ; ++j) {
                notification_bus.Notify(&INotifier::onEventIncrement);
            }
        });
    }

    for (auto& thread:threads) {
        thread.join();
    }

    ASSERT_EQ(observer->count(), 100000);
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}