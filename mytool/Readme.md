# readme

mytool is library for re-used functions.

## json.hpp

json.hpp is copied from https://github.com/nlohmann/json.git

## c++11 raw string literal

At "foo.cpp", function test uses **Raw string literal** to store a
json in a string. All escape sequence will not be translated.

```
    char text[] = R"(
    {
        "Image": {
            "Thumbnail": {
                "Url":    "http://www.example.com/image/481989943",
                "Height": 125,
                "Width":  100
            },
            "Animated" : false,
            "IDs": [
                "apple",
                "ball",
                "cat"
            ]
        },
        "floating": {
            "pi": 3.141592653589
        }
    }
    )";

```

Or you should write this.
```
    char foobar[] =
    "{"
        "\"name\": \"alice\""
    "}";

```
