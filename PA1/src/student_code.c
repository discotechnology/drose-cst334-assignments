
#include "student_code.h"
#include <string.h>
#include <stdlib.h>

String String__init(char* input_c_str) {
  String new_str;

  // Get size of the input string since we use it a few times throughout
  size_t length_of_string = strlen(input_c_str);

  // Set metadata associated with the string
  new_str.length = length_of_string;
  new_str.capacity = length_of_string + 1;

  // Copy over the data itself
  new_str.data = malloc(new_str.capacity);
  strncpy(new_str.data, input_c_str, new_str.capacity);

  // Return the string
  return new_str;
}

void String__delete(String* str) {
  free(str->data);
}

size_t String__length(const String* str) {
	return str->length;
}

void String__reserve(String* str, size_t new_capacity) {
	if (new_capacity < str->capacity) {
		return;
	}
	else {
		str->capacity = new_capacity;
		for (int i = (int) str->length; i < new_capacity; i++) {
			str->data[str->length + i] = '\0';
		}
	}
}

void String__resize(String* str, size_t new_size, const char c) {
	if (new_size < str->length) {
		str->length = new_size;
		for (int i = (int) str->length; i < str->capacity; i++) {
			str->data[i] = '\0';
		}
		str->capacity = new_size + 1;
	}
	else {
		for (int i = (int) str->length; i < new_size; i++) {
			str->data[i] = c;
		}
		str->length = new_size;
		str->capacity = new_size + 1;
		str->data[str->length] = '\0';
	}
}

void String__clear(String* str) {
	for (int i = 0; i < str->capacity; i++) {
		str->data[i] = '\0';
	}
	str->length = 0;
	str->capacity = 0;
}

bool String__empty(String* str) {
	return (str->length == 0);
}

char String__at(String* str, size_t index) {
	if (index > str->length - 1) {
		return '\0';
	}
	else {
		return str->data[index];
	}
}

char String__back(String* str) {
	if (str->length > 0) {
		return str->data[str->length - 1];
	}
	else {
		return '\0';
	}
}

char String__front(String* str) {
	if (str->length > 0) {
		return *(str->data);
	}
	else {
		return '\0';
	}
}

void String__append(String* str, const String* str_to_add) {
	/*
	* Note, I tried the below first and I could not tell for the life of me why it didn't work.
	* for(int i = 0; i < str_to_add->length; i++)
	*	String__push_back(&str, str_to_add->data[i]);
	*/
	
	size_t newlength = str->length;
	for (int i = 0; i < str_to_add->length; i++) {
		str->data[str->length + i] = str_to_add->data[i];
		newlength++;
	}
	str->length = newlength;
	str->data[newlength] = '\0';
	str->capacity = newlength + 1;
}

void String__push_back(String* str, const char char_to_add) {
	str->data[str->length] = char_to_add;
	str->data[str->capacity] = '\0';
	str->length = str->length + 1;
	str->capacity = str->capacity + 1;
}

void String__pop_back(String* str) {
  String__resize(str, (str->length - 1), '\0');
}

void String__insert(String* str, const String* str_to_insert, size_t index) {
	if (index >= str->length) {
		return;
	}

	//Result string
	String temp = String__init("");

	//Get the initial substring
	for (int i = 0; i < index; i++) {
		String__push_back(&temp, str->data[i]);
	}

	//Append the inserted string
	String__append(&temp, str_to_insert);

	//Append the ending substring
	for (int i = (int) index; i < str->length; i++) {
		String__push_back(&temp, str->data[i]);
	}

	//Swap with result string
	String__swap(str, &temp);
}

void String__erase(String* str, size_t pos, size_t len) {
	if (pos >= str->length) {
		return;
	}

	//Result string
	String temp = String__init("");

	//Get the initial substring
	for (int i = 0; i < pos; i++) {
		String__push_back(&temp, str->data[i]);
	}

	//Append the ending substring
	for (int i = (int) (pos + len); i < str->length; i++) {
		String__push_back(&temp, str->data[i]);
	}

	//Swap with result string
	String__swap(str, &temp);
}

void String__replace(String* str, size_t pos, size_t len, const String* str_to_replace_with) {
	if (pos >= str->length) {
		return;
	}

	String__erase(str, pos, len);
	String__insert(str, str_to_replace_with, pos);
}

void String__swap(String* str1, String* str2) {
	
	String temp = *str1;
	*str1 = *str2;
	*str2 = temp;

}


