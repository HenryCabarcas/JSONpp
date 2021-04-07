/**
 * @file jpp.h
 * @author NerdTronik (cabarcasortiz@gmail.com)
 * @brief a simple JSON handler library, including let and object variable declaration
 * @version 0.1
 * @date 2021-03-12
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once

#ifndef _JSONPP_
#define _JSONPP_

#include <iostream>
#include <vector>
#include <string>
 //#include <sstream>

#if defined(__clang__)

#define _POSIX_C_SOURCE
#define _POSIX_SOURCE
#include <cstring>
#define JSON(...) strdup(#__VA_ARGS__);

#elif defined(__GNUC__) || defined(__GNUG__)

#include <cstring>
#define JSON(...) strdup(#__VA_ARGS__);

#elif defined(_MSC_VER)

#include <cstring>
#define JSON(...) _strdup(#__VA_ARGS__);

#endif

#ifndef _FILE_READER_
#define _FILE_READER_
#include <fstream>

inline char* readFile(const char* fileName, long int* size = (long int*)0)
{
	std::ifstream ifs(fileName, std::ios::in | std::ios::binary | std::ios::ate);
	if (ifs.good() || ifs.is_open()) {
		std::ifstream::pos_type fileSize = ifs.tellg();
		char* fi = new char[fileSize];
		ifs.seekg(0, ifs.beg);
		ifs.read(fi, fileSize);
		ifs.close();
		*size = (long int)fileSize;
		return fi;
	}

	return 0;
}
#endif

#define arraySize(array) sizeof(array) / sizeof(array[0]) // Size of any array type
#define $(...) [&](__VA_ARGS__, ..., void)				  // Easy lambda name
#define func(...) [&](__VA_ARGS__)						  // Easy to implement a lambda function
#define VASize(args) sizeof...(args...)					  // Size of variadic arguments
#define bold(value) "\x1b[1m" << value << "\x1b[0m"		  // Console bold print value
#define null nullptr									  // Easy to remember null value

typedef std::nullptr_t Null; // Null pointer to void
typedef std::string String;	 // Standard std::string
typedef bool Bool;			 // Standard bool type

template <typename T>
String str(T value) { return std::to_string(value); } // Function that stringify almost any type of value

namespace json
{
	enum class Type;		   // Enum that determines the value type
	enum class jsonOperations; // Infile json possible operation
	class let;				   // Class that stores a value of "any" type
	template <typename ID, typename VAL>
	class Map;
	class obj;						// Class that acts like a JS object
	class JSON;						// Json file handler
	typedef obj Object;				// obj class with easy to remember name
	typedef std::vector<let> Array; // A std::vector that allocates lets
	int n_tab = 0;					// Number of tabs used to print on console
									// Const values used in Json file parsing

	std::ostream& operator<<(std::ostream& os, let& _let); // Operator << to print prettier the let values
	template <typename T>
	std::ostream& operator<<(std::ostream& os, std::vector<T> arr); // Operator << to print any printable std::vector
	std::ostream& operator<<(std::ostream& os, obj obj);			// Operator << to correctly print an object
	enum class Type													// Enum that determines the var type
	{
		None = 0 << 1,
		Number = 1 << 1,
		String = 2 << 1,
		Boolean = 3 << 1,
		Null = 4 << 1,
		Object = 5 << 1,
		Array = 6 << 1
	};

	enum class jsonOperations
	{
		none,
		objOpen,
		objClose,
		arrayOpen,
		arrayClose
	};

	/** Map class
	 * @brief Class replacement of std::map due to allocation issues
	 * @tparam ID type to identify the stored value, usually std::string or const char*
	 * @tparam VAL type of value stored, can be any supported by std::vector allocator
	 */
	template <typename ID, typename VAL>
	class Map
	{
	public:
		Map() = default;
		~Map()
		{
			values.~vector();
			ids.~vector();
		}
		// Returns the length of the map
		size_t Size() const { return values.size(); }
		/** insert()
		 * @brief Inserts value at the end of the map
		 * @param _id ID type identifier variable
		 * @param _value VAL type value variable
		 */
		void insert(const ID& _id, const VAL& _value) { ids.push_back(_id), values.push_back(_value); }
		/** insert()
		 * @brief Inserts value at the end of the map
		 * @param tp pair of type <ID, VAL> that contains the values
		 */
		void insert(std::pair<ID, VAL>& tp) { ids.push_back(tp.first), values.push_back(tp.second); }
		// Removes the last value on map
		void pop_back() { ids.pop_back(), values.pop_back(); }
		VAL& operator[](ID& idx);
		VAL& operator[](size_t idx) { return values.at(idx); }
		/** getId()
		 * @brief Get the id of actual value
		 * @param idx Index of searching value
		 * @return Identifier found
		 */
		ID getId(size_t& idx) const { return ids[idx]; }
		/** find()
		 * @brief Finds value by index position
		 * @param idx  Index of searching id
		 * @return Index of found id
		 */
		size_t find(ID& idx) const;
		/** hasValue()
		 * @brief Check if map stores specific value
		 * @param val Value to find out
		 * @return true if finds the value
		 */
		Bool hasValue(const VAL& val) const;
		/** hasId()
		 * @brief Check if map stores specific id
		 * @param Id Id to find out
		 * @return true if finds the id
		 */
		Bool hasId(const ID& Id) const;
		// Returns true if map is empty
		Bool isEmpty() const { return values.empty(); }

	private:
		std::vector<VAL> values; // Standart vector that stores the values
		std::vector<ID> ids;	 // Standart vector that stores the identifiers
	};

	/** obj class
	 * @brief Object class.
	 * This class acts like a json object, storing values with idenfiers and
	 * arrays.
	 */
	class obj
	{
	public:
		obj() = default;
		~obj() noexcept { values.~Map(); }
		let& operator[](const char* name) { return values.operator[](name); }
		// Returns true if obj is empty
		Bool isEmpty() { return values.isEmpty(); }
		friend std::ostream& operator<<(std::ostream& os, obj obj);

	private:
		/** addValue()
		 * @brief Adds value to obj variable
		 * @tparam T Any of admitted let types
		 * @param name Identifier of the new value
		 * @param val Value to be stored
		 */
		template <typename T>
		void addValue(String name, T val)
		{
			values.insert(name, val);
		}
		/** getId()
		 * @brief Get the Id of stored value
		 * @param val Index position of stored value
		 * @return Identifier value: String
		 */
		String getId(size_t val) const { return values.getId(val); }
		Map<const char*, let> values; // Map that stores the objects values
									   //String parent;			 // Parent identifier
	};

	/** let class
	 * @brief Class that stores a value of "any" type  */
	class let
	{
	public:
		let()
		{
			idx = -1;
			type = Type::None;
			_null = null;
		}
		~let() { clear(); }
		template <typename T>
		let(T value) { operator=(value); }
		// Conversion function
		template <typename T>
		operator T()
		{
			return getValue<T>();
		}
		template <typename T>
		let& operator=(T value) { return setValue(&value); }
		template <typename T>
		let& operator=(T* value) { return setValue(value); }
		let& operator[](const std::string& name) { return getObject()[name.c_str()]; }
		let& operator[](const char* name) { return getObject()[name]; }
		let& operator[](int idx) { return _array[idx]; }
		friend std::ostream& operator<<(std::ostream& os, let& _let);
		/** index()
		 * @brief Get actual value type
		 * @return Index of actual value type
		 */
		short int index() const { return idx; }
		/** getType()
		 * @brief Get the value type
		 * @return Type of actual value
		 */
		Type getType() const { return type; }

	protected:
		// Unnamed union that stores basic types:
		union
		{
			Null _null;			 // Null value storage
			Bool _bool;			 // Bool value storage
			int _int;			 // Num value storage
			float _float;		 // Num value storage
			long double _double; // Num value storage
			const char* _str;	 // String value storage
		};
		Array _array = {}; // Array value storage
		obj _obj;		   // Object value storage

	private:
		void clear(); // Deallocates let values
		/** getValue()
		 * @brief Get the actual Value
		 * @tparam T Any of the let admitted types
		 * @return Actual stored value
		 */
		template <typename T>
		T getValue();
		/** getObject()
		 * @brief Get the Object stored
		 * @return Reference of object stored
		 */
		obj& getObject()
		{
			idx = 6;
			return _obj;
		}
		/** setObject()
		 * @brief Set the Object object
		 * @param obj
		 * @return let&
		 */
		let& setObject(obj* obj);
		/** setValue()
		 * @brief Set a value to the storage
		 * @tparam T  Type of any of let admitted
		 * @param value  Value to be stored
		 * @return Reference to the stored value
		 */
		template <typename T>
		let& setValue(T* value);
		short int idx; // Index of actual type
		Type type;	   // Enum of actual type
	};

	// Typenames accepted by the let class
	const char* typenames[] = {
		typeid(String).name(), typeid(String*).name(), typeid(char*).name(),
		typeid(char).name(), typeid(const char*).name(), typeid(const char).name(),
		typeid(Bool).name(), typeid(Bool*).name(), typeid(int).name(),
		typeid(int*).name(), typeid(float).name(), typeid(float*).name(),
		typeid(long double).name(), typeid(long double*).name(),
		typeid(double).name(), typeid(double*).name(), typeid(Array).name(),
		typeid(Array*).name(), typeid(obj).name(), typeid(obj*).name(),
		typeid(nullptr).name(), typeid(let).name(), typeid(let*).name()

	};

	/** JSON class
	 * @brief Json file handler  */
	class JSON
	{
	public:
		//JSON() = default;
		Bool readJSON(String filename);
		/*let operator[](char* value)
		{
			Object templ;
			templ = find(value);
			return &templ[value];
		}*/
		let Parse(char* _string = (char*)0);
		let parseFile(const char* filePath);
		obj find(String index);
		const short int error = -1; // Default error value

	private:
		/**
		 * @brief Verify if filepath points to a .json file
		 * @param filepath Path to .json file
		 * @return true If file has .json extension
		 * @return false If file hasn't .json extension
		 */
		bool VerifyIsJson(const char* filepath);
		/** @brief Parses String value and skips it */
		void ParseString();
		/** @brief Parses Boolean value and skips it */
		int ParseBoolean();
		/** @brief Parses integer Number value and skips it */
		int ParseInt();
		/** @brief Parses integer Number value and skips it */
		double ParseDecimal();
		bool ParseNumber();
		/** @brief Parses Null value and skips it */
		bool ParseNull();
		/** @brief Parses Comments and skips them */
		bool parseComments();
		std::string errorDescription = "This is a valid JSON",					   // Description of error in case of one
			filename = "none";													   // Last file validated name
		std::vector<const char*> expectedValues = { "none" };					   // Values to be expected in the error instance
		long int idx = -1,														   // Buffer index and
			size = 0;															   // Buffer size
		int actualChar = 0, keyLevels = 0, levels = 0, limit = 0, actualParam = 0; // Temporal use variable
		int errorLine = 1;														   // Line number where the error is located
		int errorPos = 0;														   // Character position from error line start
		char actualH = ' ';														   // Determines the last syntax char
		char* Buffer = NULL;													   // Pointer to the JSON buffer
		jsonOperations lastOperation = jsonOperations::none;					   // Last Operation infile
		std::vector<int> arrayLevel;											   // Determines ho many objects are declared in an array to avoid the use of : in an array root
		bool fileIsValid = true;												   // Determines if file is a valid JSON
		let content;
		std::string strValue, numValue, lastValue;
		int actualvar = 0;
		//void syncdata(Assign assignate);
		Bool isRead = true;
		int act = 0, globallvl = -1, globalpos = 0;
		int isReserved(String character);
		Type check(String var);
		Bool checkdef(String line);
		bool parse(String buffer);
	};

	template <typename ID, typename VAL>
	inline Bool Map<ID, VAL>::hasId(const ID& val) const
	{
		for (auto& i : ids)
			if (i == val)
				return true;
		return false;
	}
	template <typename ID, typename VAL>
	inline Bool Map<ID, VAL>::hasValue(const VAL& val) const
	{
		for (auto& i : values)
			if (i == val)
				return true;
		return false;
	}
	template <typename ID, typename VAL>
	inline VAL& Map<ID, VAL>::operator[](ID& idx)
	{
		size_t i = 0;
		for (i = 0; i < values.size(); ++i)
			if (ids[i] == idx)
				return values[i];
		insert(idx, nullptr);
		return values.back();
	}

	template <typename ID, typename VAL>
	inline size_t Map<ID, VAL>::find(ID& idx) const
	{
		size_t i = 0;
		for (i = 0; i < values.size(); ++i)
			if (ids[i] == idx)
				return i;
		return -1;
	}
	template <typename T>
	inline T let::getValue()
	{
		//T out = {};
		const char* id = typeid(T).name();
		if ((id == typenames[0] or id == typenames[1] or id == typenames[2] or
			id == typenames[3] or id == typenames[4] or id == typenames[5]) and
			idx == 0)
			return *(T*)(char*)&_str;
		else if ((id == typenames[6] or id == typenames[7]) and idx == 1)
			return *(T*)&_bool;
		else if ((id == typenames[8] or id == typenames[9]) and idx == 2)
			return *(T*)&_int;
		else if ((id == typenames[10] or id == typenames[11]) and idx == 3)
			return *(T*)&_float;
		else if ((id == typenames[12] or id == typenames[13] or id == typenames[14] or id == typenames[15]) and idx == 4)
			return *(T*)&_double;
		else if ((id == typenames[16] or id == typenames[17]) and idx == 5)
			return *(T*)(&_array);
		else if ((id == typenames[18] or id == typenames[19]) and idx == 6)
			return *(T*)&_obj;
		else if (id == typenames[20] and idx == -1)
			return *(T*)&_null;
		return T();
		//return out;
	}
	template <typename T>
	inline let& let::setValue(T* value)
	{
		clear();
		auto id = typeid(T).name();
		if (id == typenames[0] or id == typenames[1] or id == typenames[2] or
			id == typenames[3] or id == typenames[4] or id == typenames[5])
		{
			_str = (const char*)value; //new (&_str) String((char*)value);
			type = Type::String, idx = 0;
		}
		else if (id == typenames[6] or id == typenames[7])
			_bool = *(Bool*)value, type = Type::Boolean, idx = 1;
		else if (id == typenames[8] or id == typenames[9])
			_int = *(int*)value, type = Type::Number, idx = 2;
		else if (id == typenames[10] or id == typenames[11])
			_float = *(float*)value, type = Type::Number, idx = 3;
		else if ((id == typenames[12] or id == typenames[13] or id == typenames[14] or id == typenames[15]))
			_double = *(double*)value, type = Type::Number, idx = 4;
		else if (id == typenames[16] or id == typenames[17])
			_array = *(Array*)value, type = Type::Array, idx = 5;
		else if (id == typenames[18] or id == typenames[19])
			_obj = *(obj*)value, type = Type::Object, idx = 6;
		else if (id == typenames[20])
			_null = nullptr, type = Type::Null, idx = -1;
		else if (id == typenames[21] or id == typenames[22])
			*this = *value;

		return *this;
	}
	inline let& let::setObject(obj* obj)
	{
		clear();
		_obj = *obj;
		type = Type::Object;
		idx = 6;
		return *this;
	}
	inline void let::clear()
	{
		//free((void *)_str);
		//_str = "";
		if (idx == 5)
			_array.~vector();
		else if (idx == 6)
			_obj.~obj();
		else
			_null = null;
		type = Type::None;
		//_int = 0;
		//_bool = 0;
		//_double = 0;
		//_float = 0;
		idx = -1;
	}
	inline std::ostream& operator<<(std::ostream& os, let& _let)
	{
		switch (_let.index())
		{
		case 0:
			os << "\"" << _let._str << "\"";
			break;
		case 1:
			os << (_let._bool ? "true" : "false");
			break;
		case 2:
			os << _let._int;
			break;
		case 3:
			os << _let._float;
			break;
		case 4:
			os << _let._double;
			break;
		case 6:
			os << _let._obj;
			break;
		case 5:
			os << _let._array;
			break;
		default:
			os << "null";
			break;
		}
		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, obj obj)
	{
		n_tab += 2;
		os << "{ \n";
		for (size_t i = 0; i < obj.values.Size(); ++i)
		{
			for (int i = 0; i < n_tab; i++)
				os << "  ";
			os << "\"" << bold(obj.values.getId(i)) << "\": " << obj.values[i];
			if (i < obj.values.Size() - 1)
				os << ", ";
			os << "\n";
		}
		n_tab -= 2;
		for (int i = 0; i < n_tab; i++)
			os << "  ";
		os << "}";
		return os;
	}

	template <typename T>
	inline std::ostream& operator<<(std::ostream& os, std::vector<T> arr)
	{
		os << "[ ";
		for (size_t i = 0; i < arr.size(); i++)
		{
			os << arr[i];
			if (i < arr.size() - 1)
				os << ", ";
		}
		os << " ]";
		return os;
	}

	inline String tolow(String value)
	{
		for (auto& i : value)
			i = tolower(i);
		return value;
	}

	inline String toup(String value)
	{
		for (auto& i : value)
			i = toupper(i);
		return value;
	}

	inline bool JSON::parseComments()
	{
		idx++;
		if (Buffer[idx] == '*')
		{
			idx++;
			while (Buffer[idx] != '*' || Buffer[idx + 1] != '/')
			{
				idx++;
				if (idx >= size)
					return fileIsValid = false;
				if (Buffer[idx] == '\n')
					actualChar = idx;
			}
			idx++;
			return true;
		}
		else if (Buffer[idx] == '/')
		{
			idx++;
			while (Buffer[idx] != '\n')
			{
				idx++;
				if (idx >= size)
					return fileIsValid = false;
			}
			actualChar = idx;
			idx++;
			return true;
		}
		fileIsValid = false;
		return false;
	}

	inline void JSON::ParseString()
	{
		lastValue = strValue;
		strValue = "";
		idx++;
		if (Buffer[idx] == '\"')
			return;
		while (Buffer[idx] != '\"')
		{
			strValue += Buffer[idx];
			if (Buffer[idx] == '\\' && Buffer[idx + 1] == '\"')
				idx++;
			idx++;
		}
	}

	inline int JSON::ParseBoolean()
	{
		switch (Buffer[idx])
		{
		case 't':
			idx++;
			if (idx > size - 1)
				return error;
			if (Buffer[idx] != 'r')
				return error;
			idx++;
			if (idx > size - 1)
				return error;
			if (Buffer[idx] != 'u')
				return error;
			idx++;
			if (idx > size - 1)
				return error;
			if (Buffer[idx] != 'e')
				return error;
			return 1;
		case 'f':
			idx++;
			if (idx > size - 1)
				return error;
			if (Buffer[idx] != 'a')
				return error;
			idx++;
			if (idx > size - 1)
				return error;
			if (Buffer[idx] != 'l')
				return error;
			idx++;
			if (idx > size - 1)
				return error;
			if (Buffer[idx] != 's')
				return error;
			idx++;
			if (idx > size - 1)
				return error;
			if (Buffer[idx] != 'e')
				return error;
			return 0;
		default:
			return error;
		}
		return error;
	}

	inline bool JSON::ParseNumber()
	{
		lastValue = numValue;
		numValue = "";
		bool keep = true, isInt = true;
		int num_dots = 0, num_minus = 0, num_nums = 0;
		std::string out = "";
		do
		{
			if (idx >= size)
				break;
			switch (Buffer[idx])
			{
			case 'e':
			case 'E':
				if (Buffer[idx - 1] == '-' || Buffer[idx - 1] == '+')
				{
					fileIsValid = false;
					return isInt;
				}
				isInt = false;
				numValue += 'e';
				num_nums++;
				break;
			case '0':
				numValue += '0';
				num_nums++;
				break;
			case '1':
				numValue += '1';
				num_nums++;
				break;
			case '2':
				numValue += '2';
				num_nums++;
				break;
			case '3':
				numValue += '3';
				num_nums++;
				break;
			case '4':
				numValue += '4';
				num_nums++;
				break;
			case '5':
				numValue += '5';
				num_nums++;
				break;
			case '6':
				numValue += '6';
				num_nums++;
				break;
			case '7':
				numValue += '7';
				num_nums++;
				break;
			case '8':
				numValue += '8';
				num_nums++;
				break;
			case '9':
				numValue += '9';
				num_nums++;
				break;
			case '.':
				if (num_dots >= 1)
				{
					fileIsValid = false;
					return isInt;
				}
				numValue += '.';
				isInt = false;
				num_dots++;
				break;
			case '+':
				if (Buffer[idx - 1] == 'e' || Buffer[idx - 1] == 'E')
				{
					numValue += '+';
					break;
				}
				fileIsValid = false;
				return isInt;
			case '-':
				if (Buffer[idx - 1] == '0' ||
					Buffer[idx - 1] == '1' ||
					Buffer[idx - 1] == '2' ||
					Buffer[idx - 1] == '3' ||
					Buffer[idx - 1] == '4' ||
					Buffer[idx - 1] == '5' ||
					Buffer[idx - 1] == '6' ||
					Buffer[idx - 1] == '7' ||
					Buffer[idx - 1] == '8' ||
					Buffer[idx - 1] == '9')
				{
					fileIsValid = false;
					return isInt;
				}
				if (num_minus == 1)
				{
					if (Buffer[idx - 1] != 'e' && Buffer[idx - 1] != 'E')
					{
						fileIsValid = false;
						return isInt;
					}
				}
				else if (num_minus > 1)
				{
					fileIsValid = false;
					return isInt;
				}
				numValue += '-';
				num_minus++;
				break;
			default:
				if ((num_minus > 0 && num_nums <= 0) || Buffer[idx - 1] == '+' || Buffer[idx - 1] == 'e' || Buffer[idx - 1] == 'E')
				{
					fileIsValid = false;
					return isInt;
				}
				keep = false;
				break;
			}
			idx++;
		} while (keep);
		idx -= 2;
		return isInt;
	}

	inline bool JSON::ParseNull()
	{
		if (Buffer[idx] == 'n')
		{
			idx++;
			if (idx > size - 1)
				return false;
			if (Buffer[idx] != 'u')
				return false;
			idx++;
			if (idx > size - 1)
				return false;
			if (Buffer[idx] != 'l')
				return false;
			idx++;
			if (idx > size - 1)
				return false;
			if (Buffer[idx] != 'l')
				return false;
			return true;
		}
		return false;
	}

	inline let JSON::Parse(char* _string)
	{
		strValue = "";
		lastValue = "";
		numValue = "";
		fileIsValid = true;
		actualH = ' ';
		errorLine = 1, actualChar = 0, keyLevels = 0, levels = 0, limit = 0, actualParam = 0, idx = -1;
		std::vector<Array> ArrayLevels = {};
		std::vector<let> objLevels = {};

		if (_string != 0)
			Buffer = _string;
		if (size == 0)
			size = strlen(Buffer) + 2;
		auto Log = [&]() {
			std::string val = "";
			switch (Buffer[idx])
			{
			case '\n':
				val = "Line end: \\n";
				break;
			case '\r':
				val = "\\r";
				break;
			case '\t':
				val = "Tabular: \\t";
				break;
			case EOF:
			case 0x03:
				val = "End of file";
				break;
			case '\0':
				val = "Null character";
				break;
			default:
				val = Buffer[idx];
				break;
			}
			errorPos = idx - actualChar;
			errorDescription = "Error at line " + std::to_string(errorLine) + ", in character -> '" + val + "' in position: " + std::to_string(errorPos) + '\n';
		};

		do
		{
			idx++;
			switch (Buffer[idx])
			{
			case '\"':
			{
				switch (actualH)
				{
				case ',':
					ParseString();
					if (arrayLevel.size() > 0)
						if (arrayLevel.back() == 0 && ArrayLevels.size() > 0)
							ArrayLevels.back().push_back(strValue.c_str());
					break;
				case '{':
					ParseString();
					break;
				case '[':
					ParseString();
					if (ArrayLevels.size() > 0)
						ArrayLevels.back().push_back(strValue.c_str());
					break;
				case ':':
					ParseString();
					if (objLevels.size() > 0)
						objLevels.back()[lastValue] = strValue.c_str();
					break;
				case '}':
				case ']':
				case '\"':
					Log();
					errorDescription += "Expected -> ',' , ':' , '}' , ']'\n";
					expectedValues = { ",", ":", "}", "]" };
					return fileIsValid = false;
				default:
					ParseString();
					break;
				}
				actualH = '\"';
				break;
			}
			case '{':
			{
				if (arrayLevel.size() > 0)
					if (arrayLevel.back() >= 0)
						arrayLevel.back()++;
				if (content.getType() == Type::None)
				{
					content = obj();
					objLevels.push_back(content);
				}
				lastOperation = jsonOperations::objOpen;
				switch (actualH)
				{
				case ':':
					objLevels.back()[strValue] = obj();
					//objLevels.push_back(objLevels.back()[strValue]);
					break;
				case ',':
				case '[':
					objLevels.push_back(obj());
					break;
				case '{':
				case '}':
				case ']':
				case '\"':
				{
					Log();
					errorDescription += "Expected -> 'Definition' , ',' , '}' , ']'\n";
					expectedValues = { "Definition", ",", "}", "]" };
					return fileIsValid = false;
				}
				default:
					break;
				}

				actualH = '{';
				keyLevels++;
				break;
			}
			case '}':
			{
				if (arrayLevel.size() > 0)
					if (arrayLevel.back() >= 0)
						arrayLevel.back()--;
				if (lastOperation == jsonOperations::arrayOpen)
				{
					Log();
					errorDescription += "Expected -> ']'\n";
					expectedValues = { "]" };
					return fileIsValid = false;
				}
				lastOperation = jsonOperations::objClose;
				switch (actualH)
				{
				case '}':
				case ']':
				case '{':
				case '\"':
					break;
				case ':':
				case '[':
				case ',':
					Log();
					errorDescription += "Expected -> 'Definition' , 'Value' , '{' , '['\n";
					expectedValues = { "Definition", "Value", "{", "[" };
					return fileIsValid = false;
				default:
					break;
				}
				actualH = '}';
				keyLevels--;
				break;
			}
			case '[':
			{
				arrayLevel.push_back(0);
				if (content.getType() == Type::None)
				{
					content = Array();
					objLevels.push_back(content);
				}
				lastOperation = jsonOperations::arrayOpen;
				switch (actualH)
				{
				case ',':
				case '{':
				case '[':
				case ':':
					break;
				case '}':
				case ']':
				case '\"':
					Log();
					errorDescription += "Expected -> ',' , '}' , ']'\n";
					expectedValues = { ",", "}", "]" };
					return fileIsValid = false;
				default:
					break;
				}
				actualH = '[';
				levels++;
				break;
			}
			case ']':
			{
				if (arrayLevel.size() > 0)
					arrayLevel.pop_back();
				if (lastOperation == jsonOperations::objOpen)
				{
					Log();
					errorDescription += "Expected-> '}' \n";
					expectedValues = { "]" };
					return fileIsValid = false;
				}
				lastOperation = jsonOperations::arrayClose;
				switch (actualH)
				{
				case '}':
				case ']':
				case '[':
				case '\"':
					break;
				case ':':
				case '{':
				case ',':
					Log();
					errorDescription += "Expected -> '[' , '{' , 'Value' , 'Definition'\n";
					expectedValues = { "[", "{", "Value", "Definition" };
					return fileIsValid = false;
				default:
					break;
				}
				actualH = ']';
				levels--;
				break;
			}
			case ',':
			{
				switch (actualH)
				{
				case ']':
				case '\"':
				case '}':
					break;
				case '[':
				case ':':
				case '{':
				case ',':
					Log();
					errorDescription += "Expected -> ']' , '[' , '{' , 'Value' , 'Definition'\n";
					expectedValues = { "[", "]", "{", "Value", "Definition" };
					return fileIsValid = false;
				default:
					break;
				}
				actualH = ',';
				break;
			}
			case ':':
			{
				if (arrayLevel.size() > 0)
					if (arrayLevel.back() == 0)
					{
						Log();
						errorDescription += "Illegal declaration, you cannot make a declaration into an array.\nExpected ',' , ']'\n";
						expectedValues = { ",", "]" };
						return fileIsValid = false;
					}
				switch (actualH)
				{
				case ']':
				case '}':
				case '[':
				case ':':
				case '{':
				case ',':
					Log();
					errorDescription += "Expected -> 'Definition' , '[' , 'Value'\n";
					expectedValues = { "Definition", "[", "Value" };
					return fileIsValid = false;
				case '\"':
				default:
					break;
				}
				actualH = ':';
				break;
			}
			case 'f':
			case 't':
			{
				switch (actualH)
				{
				case '[':
				case ',':
				case ':':
					break;
				case '\"':
				case ']':
				case '}':
				case '{':
					Log();
					errorDescription += "Expected -> ',' , ':' , '}' , ']' , 'Definition'\n";
					expectedValues = { ",", ":", "}", "]", "Definition" };
					return fileIsValid = false;
				default:
					break;
				}
				actualH = '\"';
				switch (ParseBoolean())
				{
				case 0:
					break;
				case 1:
					break;
				default:
					Log();
					errorDescription += "Expected -> 'true' , 'false'\n";
					expectedValues = { "true", "false" };
					return fileIsValid = false;
					break;
				}
				break;
			}
			case 'n':
			{
				switch (actualH)
				{
				case '[':
				case ',':
				case ':':
					break;
				case '\"':
				case ']':
				case '}':
				case '{':
					Log();
					errorDescription += "Expected -> ',' , ':' , '}' , ']' , 'Definition'\n";
					expectedValues = { ",", ":", "}", "]", "Definition" };
					return fileIsValid = false;
				default:
					break;
				}
				actualH = '\"';
				if (!ParseNull())
				{
					Log();
					errorDescription += "Expected -> 'null' , ',' , ':' , '}' , ']'\n";
					expectedValues = { "null", ",", ":", "}", "]" };
					return fileIsValid = false;
				}
				break;
			}
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-':
			{
				switch (actualH)
				{
				case '[':
				case ',':
				case ':':
					break;
				case '\"':
				case ']':
				case '}':
				case '{':
					Log();
					errorDescription += "Expected -> ',' , ':' , '}' , ']' , 'Definition'\n";
					expectedValues = { ",", ":", "}", "]", "Definition" };
					return fileIsValid = false;
				default:
					break;
				}
				actualH = '\"';
				ParseNumber();
				if (!fileIsValid)
				{
					Log();
					errorDescription += "Expected -> ',' , '}' , ']' , 'Number'\n";
					expectedValues = { ",", "}", "]", "Number" };
					return fileIsValid = false;
				}
				break;
			}
			case '\n':
				errorLine += 1;
				actualChar = idx;
				break;
			case '\r':
			case ' ':
			case '\t':
			case '\v':
			case '\b':
			case '\f':
			case '\a':
				break;
			case EOF:
			case '\0':
				idx = size;
				break;
			case '/':
				if (!parseComments())
				{
					Log();
					if (idx >= size)
					{
						errorDescription += "Comment has no end statement. Expected -> '*/' , '\\n'\n";
						expectedValues = { "*/", "\\n" };
					}
					else
					{
						errorDescription += "Invalid comment. Expected -> '/' , '*'\n";
						expectedValues = { "/", "*" };
					}
					return fileIsValid = false;
				}
				break;
			default:
				Log();
				errorDescription += "Invalid character\n";
				return fileIsValid = false;
			}

		} while (idx < size - 1);

		delete[] Buffer;
		if (_string != 0)
			delete[] _string;
		arrayLevel.clear();
		fileIsValid = levels == 0 && keyLevels == 0 ? true : false;
		if (keyLevels > 0)
		{
			Log();
			errorDescription += "Missing " + std::to_string(keyLevels) + " '}'\n";
			expectedValues = { "}" };
		}
		else if (keyLevels < 0)
		{
			Log();
			errorDescription += "Missing " + std::to_string(keyLevels) + " '{'\n";
			expectedValues = { "{" };
		}

		if (levels > 0)
		{
			Log();
			errorDescription += "Missing " + std::to_string(levels) + " ']'\n";
			expectedValues = { "]" };
		}
		else if (levels < 0)
		{
			Log();
			errorDescription += "Missing " + std::to_string(levels) + " '['\n";
			expectedValues = { "[" };
		}
		return content;
	}

} // namespace json

#endif