#ifndef __FOO_H__
#define __FOO_H__

#include <string>
#include <vector>

bool is_file_exist(const char* fname);
bool is_file_exist(const std::string& fname);

void test();

void dump_jsonfile(const std::string& fn);
std::vector<std::string> get_vector_from_jsonfile(const std::string& fn,
                                                  std::vector<std::string> keys);
double get_double_from_jsonfile(const std::string& fn,
                              std::vector<std::string> keys);
int get_int_from_jsonfile(const std::string& fn,
                          std::vector<std::string> keys);

template<class T>
bool get_value_from_jsonfile(const std::string& json_file,
                        std::vector<std::string> keys,
                        T& value);


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


#endif  // __FOO_H__
