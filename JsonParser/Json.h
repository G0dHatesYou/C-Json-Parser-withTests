#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>
#include <exception>

	struct BadParse : public std::exception
	{
		const char * what() const throw ()
		{
			return "json parser Error!";
		}
	};

	enum JsonType { jsonObject, jsonArray, jsonString, jsonBoolean, jsonNumber, jsonNull, jsonUnknown };

	class JsonObject {
	public:
		JsonObject();
		JsonObject(JsonType);

		std::string toString();
		JsonType getType();
		int size();

		int asInt();
		double asDouble();
		bool asBool();
		void* asNull();
		std::string asString();

		void setType(JsonType);
		void addProperty(std::string key, JsonObject value);
		void addElement(JsonObject value);
		void setString(std::string string);
		
		JsonObject operator[](int i);
		JsonObject operator[](std::string s);
		friend bool operator== (const JsonObject &left, const JsonObject &right);

	private:
		std::string makeSpace(int spaceCount);
		std::string toStringWithSpace(int spaceCount);

	private:
		std::string mStringValue;
		JsonType mType;
		std::vector<std::pair<std::string, JsonObject> > mProperties;
		std::map<std::string, size_t> mMapIndex;
		std::vector<JsonObject> mArray;
	};

	namespace parser {
		enum tokenType { tokenString, tokenNumber, tokenBoolean, tokenBracesOpen, tokenBracesClose, tokenBracketOpen, tokenBracketClose, tokenComma, tokenColon, tokenNull, tokenUnknown };

		class Token {
		public:
			Token(std::string value = "", tokenType type = tokenUnknown) : mStringValue(value), mType(type)
			{
			}
			tokenType getType() const;
			std::string getValue() const;

		private:
			std::string mStringValue;
			tokenType mType;
		};

		JsonObject parse(const std::string& jsonString, bool multithreaded = false);
		JsonObject parseFile(const std::string& filePath);

		int nextWhitespace(const std::string& jsonString, int pos);
		int skipWhitespaces(const std::string& jsonString, int pos);

		std::vector<Token> tokenizeMultithreaded(std::string jsonString);
		std::vector<Token> tokenize(std::string jsonString);
		JsonObject jsonParse(std::vector<Token> v, int startPos, int& endPos);
	}
