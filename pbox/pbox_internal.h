#ifndef __INTERNAL_PBOX_H__
#define __INTERNAL_PBOX_H__

// internal header file for pbox toolbox functions, not used for
// outside world

#include <string>

namespace pbox {


class Person
{
public:
    void setName(const std::string& s) {
        name = s;
    }
    std::string getName() const {
        return name;
    }
    void setAddress(const std::string& s) {
        address = s;
    }
    std::string getAddress() const {
        return address;
    }
    void setAge(int i) {
        age = i;
    }
    int getAge() const {
        return age;
    }
    bool operator==(const Person& rhs) {
        return (name == rhs.name) && (address == rhs.address) && (age == rhs.age);
    }
private:
    std::string name;
    std::string address;
    int age;
};

}   // namespace pbox

#endif  // __INTERNAL_PBOX_H__
