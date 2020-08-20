#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>
#include "Json.h"
using namespace std;

int main() {
	JsonObject obj = parser::parse(R"({
    "json": [
        {
            "tag_name": "a",
            "attr": [{"key": "href", "value": "http"}, {"key": "target", "value": "_blank"}]
        },
        {
            "this_is": ["array", "of", "strings"],
            "number_array": [1, 2, 4, 8, 16],
            "double": 3.14,
            "boolean": true,
            "null": null,
            "mixed": [1, 2, {"test1": -10.2345, "test2": false}, null, 0.4, ["text", ["array", true]], "more text"]
        },
        {
            "done": true
        },
        {
            "control_chars": "\n\t\n"
        }
    ]
})", true);

	cout << obj.toString() << endl;
	system("pause");
	return 0;
}