#include <iostream>

#include "../lib/Scheduler.h"
#include <math.h>

using namespace Scheduler;

struct AddNumber {
    AddNumber(float n){
        number = n;
    }
    float add(float a) const {
        return a + number;
    }
    AddNumber(AddNumber&& other) : number(other.number){
        std::cout << "move\n";
    }
    AddNumber(const AddNumber& other) : number(other.number){
        std::cout << "copy\n";
    }
    AddNumber& operator=(AddNumber&& other) {
        if (this != &other) {
            number = other.number;
        }
        std::cout << "move =\n";
        return *this;
    }
    float number;
};

int main(int argc, char** argv) {
    float a = 1;
    float b = -2;
    float c = 0;
    AddNumber add(3);

    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](float a, float c) {return -4 * a * c;}, a, c);

    auto id2 = scheduler.add([](float b, float v) {return b * b + v;}, b, scheduler.getFutureResult<float>(id1));

    auto id3 = scheduler.add([](float b, float d) {return -b + std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

    auto id4 = scheduler.add([](float b, float d) {return -b - std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

    auto id5 = scheduler.add([](float a, float v) {return v / (2 * a);}, a, scheduler.getFutureResult<float>(id3));

    auto id6 = scheduler.add([](float a, float v) {return v / (2 * a);}, a, scheduler.getFutureResult<float>(id4));
    
    auto id8 = scheduler.add(&AddNumber::add, std::move(AddNumber(3)), scheduler.getFutureResult<float>(id6));
    scheduler.executeAll();

    return 0;
}