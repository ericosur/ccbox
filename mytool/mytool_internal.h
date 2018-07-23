#ifndef __INTERNAL_MY_TOOL_H__
#define __INTERNAL_MY_TOOL_H__

// internal header file for toolbox functions, not used for
// outside world

#include <string>

namespace mytool {


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

}   // namespace mytool

#endif  // __INTERNAL_MY_TOOL_H__
