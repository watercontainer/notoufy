//
// Created by Amrou on 02/05/2023.
//
#include "../include/notoufy.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace notoufy;

class INotifier {
public:
    virtual ~INotifier() = default;

    virtual void onEvent() = 0;

    virtual void onEvent1(int arg) = 0;
};

class Observer : public INotifier {
public:
    virtual void onEvent() override {
        notified = true;
    }

    virtual void onEvent1([[maybe_unused]] int arg) override {
        notified = true;
    }

    bool called() const {
        return notified;
    }

private:
    bool notified = false;
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

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}