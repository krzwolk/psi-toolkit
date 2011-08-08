#include "morfologik.hpp"
#include <iostream>
#include <boost/algorithm/string.hpp>

std::string Morfologik::tagSeparator = "+";

Morfologik::Morfologik() {
	jenv = NULL;

	JavaVirtualMachine *jvm = JavaVirtualMachine::Instance();
	jenv = jvm->getENV();

	initializePolishStemmer();
	initializeList();
	initializeWordData();
	initializeString();
}

std::vector<std::string> Morfologik::stem(const std::string& word) {

	jstring jword = jenv->NewStringUTF(word.c_str());	
	jobject objList = (jobject)jenv->CallObjectMethod(objPolishStemmer, midPolishStemmerLookup, jword);
	jenv->DeleteLocalRef(jword);
	int stemsCount = (int)jenv->CallIntMethod(objList, midListGetSize, NULL);

	jobject objWordData = NULL;
	jobject objForString = NULL;
	jstring text = NULL;
	const char *pstem = NULL;
	const char *ptags = NULL;
	std::vector<std::string> stems;
	
	for (int i = 0; i < stemsCount; i++) {
		objWordData = (jobject)jenv->CallObjectMethod(objList, midListGetElement, (jint)i);

		// Get stem of word.
		objForString = jenv->CallObjectMethod(objWordData, midWordDataGetStem, NULL);
		text = (jstring)jenv->CallObjectMethod(objForString, midStringToString, NULL);
		pstem = jenv->GetStringUTFChars(text, NULL);
		if (pstem == NULL) {
			/* OutOfMemoryError already throw */
		}
		
		// Get tags of word.
		objForString = jenv->CallObjectMethod(objWordData, midWordDataGetTag, NULL);
		text = (jstring)jenv->CallObjectMethod(objForString, midStringToString, NULL);
		ptags = jenv->GetStringUTFChars(text, NULL);
		if (pstem == NULL) {
			/* OutOfMemoryError already throw */
		}

		// Split tags if it is neccessary.
		size_t shouldBeSplit = std::string(ptags).find(tagSeparator);
		if (shouldBeSplit != std::string::npos) {
			std::vector<std::string> tags;
			boost::split(tags, ptags, boost::is_any_of(tagSeparator));

			for (int j = 0; j < (int)tags.size(); j++) {
				stems.insert(stems.end(), pstem);
				stems.insert(stems.end(), tags[j]);
			}
		}
		else {
			stems.insert(stems.end(), pstem);
			stems.insert(stems.end(), ptags);
		}
		
		jenv->ReleaseStringUTFChars(text, pstem);
		jenv->ReleaseStringUTFChars(text, ptags);
	}
	
	jenv->DeleteLocalRef(objList);
	jenv->DeleteLocalRef(objWordData);
	jenv->DeleteLocalRef(objForString);
	jenv->DeleteLocalRef(text);
	return stems;
}

void Morfologik::initializePolishStemmer() {
	clsPolishStemmer = NULL;
	objPolishStemmer = NULL;
	midPolishStemmerConstructor = NULL;
	midPolishStemmerLookup = NULL;

	clsPolishStemmer = jenv->FindClass("morfologik/stemming/PolishStemmer");
	if (clsPolishStemmer != NULL) {
		midPolishStemmerConstructor = jenv->GetMethodID(clsPolishStemmer, "<init>", "()V");
		midPolishStemmerLookup = jenv->GetMethodID(clsPolishStemmer, "lookup", "(Ljava/lang/CharSequence;)Ljava/util/List;");
	}
	else {
		printf("clsPolishStemmer is NULL\n");	// FIXME: should be an exception
	}
	objPolishStemmer = jenv->NewObject(clsPolishStemmer, midPolishStemmerConstructor, NULL);
}

void Morfologik::initializeList() {
	clsList = NULL;
	midListGetSize = NULL;
	midListGetElement = NULL;

	clsList = jenv->FindClass("java/util/List");
	if (clsList != NULL) {
		midListGetSize = jenv->GetMethodID(clsList, "size", "()I");
		midListGetElement = jenv->GetMethodID(clsList, "get", "(I)Ljava/lang/Object;");
	}
}

void Morfologik::initializeWordData() {
	clsWordData = NULL;
	midWordDataGetStem = NULL;
	midWordDataGetTag = NULL;

	clsWordData = jenv->FindClass("morfologik/stemming/WordData");
	if (clsWordData != NULL) {
		midWordDataGetStem = jenv->GetMethodID(clsWordData, "getStem", "()Ljava/lang/CharSequence;");
		midWordDataGetTag = jenv->GetMethodID(clsWordData, "getTag", "()Ljava/lang/CharSequence;");
	}
}

void Morfologik::initializeString() {
	clsString = NULL;
	midStringToString = NULL;

	clsString = jenv->FindClass("java/lang/String");
	if (clsString != NULL) {
		midStringToString = jenv->GetMethodID(clsString, "toString", "()Ljava/lang/String;");
	}
}

