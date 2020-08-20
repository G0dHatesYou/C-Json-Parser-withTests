#include "Json.h"
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>
#include <queue>
#include <future>
using namespace std;

string deserialize(const string& inputString)
{
	string outputString = "";
	for (size_t i = 0; i < inputString.length(); i++)
	{
		if (inputString[i] == '\\' && i + 1 < inputString.length())
		{
			if (inputString[i + 1] == '\"')
			{
				outputString += '"';
			}
			else if (inputString[i + 1] == '\\')
			{
				outputString += '\\';
			}
			else if (inputString[i + 1] == '/')
			{
				outputString += '/';
			}
			else if (inputString[i + 1] == 'b')
			{
				outputString += '\b';
			}
			else if (inputString[i + 1] == 'f')
			{
				outputString += '\f';
			}
			else if (inputString[i + 1] == 'n')
			{
				outputString += '\n';
			}
			else if (inputString[i + 1] == 'r')
			{
				outputString += '\r';
			}
			else if (inputString[i + 1] == 't')
			{
				outputString += '\t';
			}
			i++;
			continue;
		}
		outputString += inputString[i];
	}
	return outputString;
}

string JsonObject::makeSpace(int spaceCount)
{
	string s = "";
	while (spaceCount--)
	{
		s += "  ";
	}
	return s;
}
string JsonObject::toStringWithSpace(int spaceCount)
{
	switch (mType)
	{
	case jsonString:
		return string("\"") + mStringValue + string("\"");
		break;
	
	case jsonBoolean:
		return mStringValue;
		break;
	
	case jsonNumber:
		return mStringValue;
		break;
	
	case jsonNull:
		return "null";
		break;
	
	case jsonUnknown:
		break;
		return "unknown";
	
	case jsonObject:
	{
		string s = string("{\n");
		for (size_t i = 0; i < mProperties.size(); i++)
		{
			s += makeSpace(spaceCount) + string("\"") + mProperties[i].first + string("\": ") + mProperties[i].second.toStringWithSpace(spaceCount + 1) + string(i == mProperties.size() - 1 ? "" : ",") + string("\n");
		}
		s += makeSpace(spaceCount - 1) + string("}");
		return s;
	}
	break;
	
	case jsonArray:
	{
		string s = "[";
		for (size_t i = 0; i < mArray.size(); i++)
		{
			if (i)
			{
				s += ", ";
			}
			s += mArray[i].toStringWithSpace(spaceCount + 1);
		}
		s += "]";
		return s;
	}
	break;
	
	default:
		break;
	}
	return "error";
}

JsonObject::JsonObject() :mType(jsonUnknown)
{
}

JsonObject::JsonObject(JsonType type) : mType(type)
{
}

string JsonObject::toString()
{
	return toStringWithSpace(1);
}

JsonType JsonObject::getType()
{
	return mType;
}

void JsonObject::setType(JsonType type)
{
	mType = type;
}

void JsonObject::addProperty(string key, JsonObject v)
{
	mMapIndex[key] = mProperties.size();
	mProperties.push_back(make_pair(key, v));
}

void JsonObject::addElement(JsonObject v)
{
	mArray.push_back(v);
}

void JsonObject::setString(string s)
{
	mStringValue = s;
}

int JsonObject::asInt()
{
	stringstream ss;
	ss << mStringValue;
	int k;
	ss >> k;
	return k;
}

double JsonObject::asDouble()
{
	stringstream ss;
	ss << mStringValue;
	double k;
	ss >> k;
	return k;
}

bool JsonObject::asBool()
{
	if (mStringValue == "true")
	{
		return true;
	}
	return false;
}

void* JsonObject::asNull()
{
	return NULL;
}

string JsonObject::asString()
{
	return deserialize(mStringValue);
}

int JsonObject::size()
{
	if (mType == jsonArray)
	{
		return (int)mArray.size();
	}
	if (mType == jsonObject)
	{
		return (int)mProperties.size();;
	}
	return 0;
}

JsonObject JsonObject::operator[](int i)
{
	if (mType == jsonArray)
	{
		return mArray[i];
	}
	if (mType == jsonObject)
	{
		return mProperties[i].second;
	}
	return JsonObject();
}

inline bool operator== (const JsonObject &left, const JsonObject &right)
{
	return (left.mStringValue == right.mStringValue && left.mType == right.mType &&
		left.mProperties == right.mProperties && left.mMapIndex == right.mMapIndex && left.mArray == right.mArray);
}

JsonObject JsonObject::operator[](string s)
{
	if (mMapIndex.find(s) == mMapIndex.end())
	{
		return JsonObject();
	}
	return mProperties[mMapIndex[s]].second;
}

int parser::nextWhitespace(const string& source, int i)
{
	while (i < (int)source.length())
	{
		if (source[i] == '"')
		{
			i++;
			while (i < (int)source.length() && (source[i] != '"' || source[i - 1] == '\\'))
			{
				i++;
			}
		}

		if (source[i] == '\'')
		{
			i++;
			while (i < (int)source.length() && (source[i] != '\'' || source[i - 1] == '\\'))
			{
				i++;
			}
		}

		if (isspace(source[i]))
		{
			return i;
		}

		i++;
	}
	return (int)source.length();
}

int parser::skipWhitespaces(const string& source, int i)
{
	while (i < (int)source.length())
	{
		if (!isspace(source[i]))
		{
			return i;
		}
		i++;
	}
	return -1;
}


namespace parser
{
	std::vector<Token> tokenizeMultithreaded(std::string jsonString)
	{
		vector<Token> tokens;

		vector<string> strings;
		istringstream f(jsonString);
		string s;
		while (getline(f, s, '\n')) {
			strings.push_back(s);
		}

		try
		{
			std::queue<std::future<std::vector<Token>>> results;
			
			for (auto jsonRow : strings)
			{
				results.emplace(std::async([jsonRow]() { return tokenize(jsonRow); }));
			}

			while (!results.empty())
			{
				auto tempTokens = results.front().get();
				std::cout << "Done" << endl;
				tokens.insert(tokens.end(), tempTokens.begin(), tempTokens.end());
				results.pop();
			}
		}
		catch (const std::exception& exception)
		{
			std::cout << "error occured while processing " << exception.what() << std::endl;
		}

		return tokens;
	}

	vector<parser::Token> parser::tokenize(string source)
	{
		std::cout << "Thread" << endl;
		source += " ";
		vector<Token> tokens;
		int index = skipWhitespaces(source, 0);
		while (index >= 0)
		{
			int next = nextWhitespace(source, index);
			string tokenizeString = source.substr(index, next - index);

			size_t pos = 0;
			while (pos < tokenizeString.length())
			{
				if (tokenizeString[pos] == '"')
				{
					size_t nestedPos = pos + 1;
					while (nestedPos < tokenizeString.length() && (tokenizeString[nestedPos] != '"' || tokenizeString[nestedPos - 1] == '\\'))
					{
						nestedPos++;
					}
					tokens.push_back(Token(tokenizeString.substr(pos + 1, nestedPos - pos - 1), tokenString));
					pos = nestedPos + 1;
					continue;
				}

				if (tokenizeString[pos] == '\'')
				{
					size_t nestedPos = pos + 1;
					while (nestedPos < tokenizeString.length() && (tokenizeString[nestedPos] != '\'' || tokenizeString[nestedPos - 1] == '\\'))
					{
						nestedPos++;
					}
					tokens.push_back(Token(tokenizeString.substr(pos + 1, nestedPos - pos - 1), tokenString));
					pos = nestedPos + 1;
					continue;
				}

				if (tokenizeString[pos] == ',')
				{
					tokens.push_back(Token(",", tokenComma));
					pos++;
					continue;
				}

				if (tokenizeString.substr(pos, 4) == "true")
				{
					tokens.push_back(Token("true", tokenBoolean));
					pos += 4;
					continue;
				}

				if (tokenizeString.substr(pos, 5) == "false")
				{
					tokens.push_back(Token("false", tokenBoolean));
					pos += 5;
					continue;
				}

				if (tokenizeString.substr(pos, 4) == "null")
				{
					tokens.push_back(Token("null", tokenNull));
					pos += 4;
					continue;
				}

				if (tokenizeString[pos] == '}')
				{
					tokens.push_back(Token("}", tokenBracesClose));
					pos++;
					continue;
				}

				if (tokenizeString[pos] == '{')
				{
					tokens.push_back(Token("{", tokenBracesOpen));
					pos++;
					continue;
				}

				if (tokenizeString[pos] == ']')
				{
					tokens.push_back(Token("]", tokenBracketClose));
					pos++;
					continue;
				}

				if (tokenizeString[pos] == '[')
				{
					tokens.push_back(Token("[", tokenBracketOpen));
					pos++;
					continue;
				}

				if (tokenizeString[pos] == ':')
				{
					tokens.push_back(Token(":", tokenColon));
					pos++;
					continue;
				}

				if (tokenizeString[pos] == '-' || isdigit(tokenizeString[pos]))
				{
					size_t nestedPos = pos;

					if (tokenizeString[nestedPos] == '-')
					{
						nestedPos++;
					}

					while (nestedPos < tokenizeString.size() && (isdigit(tokenizeString[nestedPos]) || tokenizeString[nestedPos] == '.'))
					{
						nestedPos++;
					}

					tokens.push_back(Token(tokenizeString.substr(pos, nestedPos - pos), tokenNumber));
					pos = nestedPos;
					continue;
				}

				throw BadParse();
			}

			index = skipWhitespaces(source, next);
		}
		return tokens;
	}

	JsonObject parser::jsonParse(vector<Token> tokens, int startPos, int& endPos)
	{
		JsonObject current;
		if (tokens[startPos].getType() == tokenBracesOpen)
		{
			current.setType(jsonObject);
			int parsedStartPos = startPos + 1;

			while (tokens[parsedStartPos].getType() != tokenBracesClose)
			{
				if (tokens[parsedStartPos].getType() == tokenComma)
				{
					parsedStartPos++;
				}
				else 
				{
					if (tokens[parsedStartPos - 1].getType() != tokenBracesOpen)
					{
						throw BadParse();
					}
				}

				string key = tokens[parsedStartPos].getValue();
				parsedStartPos += 2;
				int parsedEndPos = parsedStartPos;
				JsonObject parsedObj = jsonParse(tokens, parsedStartPos, parsedEndPos);
				current.addProperty(key, parsedObj);
				parsedStartPos = parsedEndPos;
			}

			endPos = parsedStartPos + 1;
			return current;
		}
		if (tokens[startPos].getType() == tokenBracketOpen)
		{
			current.setType(jsonArray);
			int parsedStartPos = startPos + 1;

			while (tokens[parsedStartPos].getType() != tokenBracketClose)
			{
				if (tokens[parsedStartPos].getType() == tokenComma)
				{
					parsedStartPos++;
				}
				else
				{
					if (tokens[parsedStartPos - 1].getType() != tokenBracketOpen)
					{
						throw BadParse();
					}
				}


				int parsedEndPos = parsedStartPos;
				JsonObject parsedObj = jsonParse(tokens, parsedStartPos, parsedEndPos);
				current.addElement(parsedObj);
				parsedStartPos = parsedEndPos;
			}

			endPos = parsedStartPos + 1;
			return current;
		}

		if (tokens[startPos].getType() == tokenNumber)
		{
			current.setType(jsonNumber);
			current.setString(tokens[startPos].getValue());
			endPos = startPos + 1;
			return current;
		}

		if (tokens[startPos].getType() == tokenString)
		{
			current.setType(jsonString);
			current.setString(tokens[startPos].getValue());
			endPos = startPos + 1;
			return current;
		}

		if (tokens[startPos].getType() == tokenBoolean)
		{
			current.setType(jsonBoolean);
			current.setString(tokens[startPos].getValue());
			endPos = startPos + 1;
			return current;
		}

		if (tokens[startPos].getType() == tokenNull)
		{
			current.setType(jsonNull);
			current.setString("null");
			endPos = startPos + 1;
			return current;
		}

		if (current == JsonObject())
		{
			throw BadParse();
		}
		return current;
	}

	JsonObject parser::parse(const string& jsonString, bool multithreaded)
	{
		if (jsonString.empty())
		{
			return JsonObject();
		}

		int endPos = 0;
		if (multithreaded)
		{
			return jsonParse(tokenizeMultithreaded(jsonString), 0, endPos);
		}
		return jsonParse(tokenize(jsonString), 0, endPos);
	}

	JsonObject parser::parseFile(const string& filename)
	{
		ifstream in(filename.c_str());
		string str = "";
		string tmp;
		while (getline(in, tmp))
		{
			str += tmp;
		}
		in.close();
		return parser::parse(str);
	}
	tokenType Token::getType() const
	{
		return mType;
	}
	string Token::getValue() const
	{
		return mStringValue;
	}
}