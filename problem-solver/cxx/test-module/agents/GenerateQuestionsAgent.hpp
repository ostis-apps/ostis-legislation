#pragma once

#include <sc-memory/sc_agent.hpp>
#include <array>
#include "utility"

class GenerateQuestionsAgent : public ScActionInitiatedAgent {
private:
    int static const QUESTION_COUNT = 10;
    int static const QUESTION_MODEL_COUNT = 3;
    int currentModelNumber = 0;

public:
    ScAddr GetActionClass() const override;
    ScResult DoProgram(ScAction & action) override;
    std::string replacePlaceholder(const std::string &str, const std::string &placeholder, const std::string &replacement);
    int randomIntegerNumber(const int start, const int questionNumbers);
    ScAddr initResultStructure(ScStructure & structure);
    ScAddr findAnswerTemplate(ScAddr modelNode);
    ScAddr findQuestionTemplate(ScAddr modelNode);
    std::string findQuestionText(ScAddr modelNode);
    std::array<int, QUESTION_MODEL_COUNT> getModelQuestionCounts();
    void generateQuestion(ScAddr questionModelAddr, std::string questionText, std::set<std::string> & questions,
                          ScStructure & structure, ScAddr questionTuple, int questionNumber);
    void generateQuestions(ScAddr questionModelClassAddr, ScStructure & structure);
    std::array<ScAddr, QUESTION_MODEL_COUNT> findQuestionModel(ScAddr questionModelClass);
    std::array<int, QUESTION_MODEL_COUNT - 1> generateRandomIntegerNumberInRange(int randomCount, int finish, int dist);
};