
#include <gtest/gtest.h>
#include "Json.h"
using namespace std;

TEST(JsonParserTests, EmptyJsonTest) {
	string jsonText = "";
	JsonObject testObject = parser::parse(jsonText);
	JsonObject emptyObj;
	EXPECT_EQ(testObject, emptyObj);
}

TEST(JsonParserTests, IndentPlacementTest) {
	string jsonText = "{\"employee\":{\"name\":\"sonoo\",\"salary\":56000,\"married\":true}}";
	JsonObject testObject = parser::parse(jsonText);
	string parsedJson = "{\n  \"employee\": {\n    \"name\": \"sonoo\",\n    \"salary\": 56000,\n    \"married\": true\n  }\n}";
	EXPECT_EQ(testObject.toString(), parsedJson);
}

TEST(JsonParserTests, FileParseJson) {

	JsonObject fileParsedObject = parser::parseFile("data.json");
	string jsonString = R"({
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
})";
	JsonObject stringParsedObject = parser::parse(jsonString);
	EXPECT_EQ(fileParsedObject, stringParsedObject);
}

TEST(JsonParserTests, MapAccessTest) {
	string jsonText = R"({  
    "employee": { 
        "name":       "sonoo",  
        "salary":      56000,   
        "married":    true  
    }  
}  )";
	JsonObject testObject = parser::parse(jsonText);
	JsonObject internalObject = testObject["employee"];

	JsonObject internalObjectParsed = parser::parse(R"({ 
        "name":       "sonoo",  
        "salary":      56000,   
        "married":    true  
    })");
	EXPECT_EQ(internalObjectParsed, internalObject);
}

TEST(JsonParserTests, VectorAccessTest) {
	string jsonText = R"({  
    "employee": { 
        "name":       "sonoo",  
        "salary":      56000,   
        "married":    true  
    }  
}  )";
	JsonObject testObject = parser::parse(jsonText);
	string testString = testObject["employee"][0].toString();

	string nameString = R"("sonoo")";
	EXPECT_EQ(nameString, testString);
}

TEST(JsonParserTests, BracesMissTest) {
	string jsonTextWithMistake = R"({  
    "employee":  
        "name":       "sonoo",   
        "salary":      56000,   
        "married":    true  
    }  
}  )";
	EXPECT_THROW(parser::parse(jsonTextWithMistake), BadParse);
}

TEST(JsonParserTests, CommaMissTest) {
	string jsonTextWithMistake = R"({  
    "employee": { 
        "name":       "sonoo"   
        "salary":      56000   
        "married":    true  
    }  
}  )";

	EXPECT_THROW(parser::parse(jsonTextWithMistake), BadParse);
}

int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}