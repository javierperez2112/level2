/**
 * @brief Lequel? language identification based on trigrams
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2022-2023
 *
 * @cite https://towardsdatascience.com/understanding-cosine-similarity-and-its-application-fd42f585296a
 */

#include <cmath>
#include <codecvt>
#include <locale>
#include <iostream>

#include "Lequel.h"

using namespace std;

/**
 * @brief Builds a trigram profile from a given text.
 *
 * @param text Vector of lines (Text)
 * @return TrigramProfile The trigram profile
 */
TrigramProfile buildTrigramProfile(const Text &text)
{
    wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    /* Delete end of line */
    for (auto line : text)
    {
        if ((line.length() > 0) && (line[line.length() - 1] == '\r'))
        {
            line = line.substr(0, line.length() - 1);
        }
    }

    /* Count frequencies */
    TrigramProfile trigramProfile;
    for (auto textLine : text){
        if(textLine.length() < 3)
            continue;
        wstring unicodeString = converter.from_bytes(textLine);
        for(int i = 0; i < unicodeString.length() - 3; i++)
        {
            wstring unicodeTrigram = unicodeString.substr(i, 3);
            string trigram = converter.to_bytes(unicodeTrigram);
            trigramProfile[trigram] = trigramProfile[trigram] + 1;
        }
    }
    // Tip: converts UTF-8 string to wstring
    // wstring unicodeString = converter.from_bytes(textLine);

    // Tip: convert wstring to UTF-8 string
    // string trigram = converter.to_bytes(unicodeTrigram);

    return trigramProfile; // Fill-in result here
}

/**
 * @brief Normalizes a trigram profile.
 *
 * @param trigramProfile The trigram profile.
 */
void normalizeTrigramProfile(TrigramProfile &trigramProfile)
{
    /* Calculate sqrt(sum of freq^2) */
    TrigramProfile::iterator iter;
    float norm = 0;
    for(iter = trigramProfile.begin(); iter != trigramProfile.end(); iter++)
    {
        float frequency = iter->second * iter->second;
        norm += frequency;
    }
    norm = fsqrt(norm);

    /* Normalize frequencies */
    for(iter = trigramProfile.begin(); iter != trigramProfile.end(); iter++)
    {
        iter->second = iter->second / norm;
    }

    return;
}

/**
 * @brief Calculates the cosine similarity between two trigram profiles
 *
 * @param textProfile The text trigram profile
 * @param languageProfile The language trigram profile
 * @return float The cosine similarity score
 */
float getCosineSimilarity(TrigramProfile &textProfile, TrigramProfile &languageProfile)
{
    TrigramProfile::iterator iter;
    float cosineSimilarity = 0;
    for(iter = textProfile.begin(); iter != textProfile.end(); iter++)
    {
        float languageFrequency = languageProfile[iter->first];
        if(languageFrequency != 0)
        {
            cosineSimilarity += languageFrequency * iter->second;
        } 
    }

    return cosineSimilarity;
}

/**
 * @brief Identifies the language of a text.
 *
 * @param text A Text (vector of lines)
 * @param languages A list of Language objects
 * @return string The language code of the most likely language
 */
string identifyLanguage(const Text &text, LanguageProfiles &languages)
{
    TrigramProfile textProfile = buildTrigramProfile(text);
    normalizeTrigramProfile(textProfile);
    float greatestSimilarity = 0;
    string languageCode = "";

    for(auto &language : languages)
    {
        float cosineSimilarity = getCosineSimilarity(textProfile, language.trigramProfile);
        if(cosineSimilarity > greatestSimilarity)
        {
            greatestSimilarity = cosineSimilarity;
            languageCode = language.languageCode;
        }
    }

    return languageCode;
}
