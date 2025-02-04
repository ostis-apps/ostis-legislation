#include <random>
#include "set"
#include "utility"
#include "algorithm"
#include "random"

#include "GenerateQuestionsAgent.hpp"

#include "keynodes/GenerateQuestionsKeynodes.hpp"

std::string GenerateQuestionsAgent::replacePlaceholder(const std::string &str, const std::string &placeholder, const std::string &replacement) {
    std::string result = str;
    size_t pos = result.find(placeholder);

    while (pos != std::string::npos) {
        result.replace(pos, placeholder.length(), replacement);
        pos = result.find(placeholder, pos + replacement.length());
    }

    return result;
}

int GenerateQuestionsAgent::randomIntegerNumber(const int start, const int questionNumbers) {
    std::random_device randomDevice;
    std::mt19937 gen(randomDevice());
    std::uniform_int_distribution<> distribution(start, questionNumbers - 1);

    int randomIntegerNumber = distribution(gen);

    return randomIntegerNumber;
}

ScAddr GenerateQuestionsAgent::initResultStructure(ScStructure & structure) {
    ScAddr questionsTuple = m_context.GenerateNode(ScType::ConstNodeTuple);
    ScAddr questionsTupleConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, GenerateQuestionsKeynodes::test, questionsTuple);
    structure.Append(questionsTuple);
    structure.Append(questionsTupleConnector);
    structure.Append(GenerateQuestionsKeynodes::test);

    return questionsTuple;
}

ScAddr GenerateQuestionsAgent::findAnswerTemplate(ScAddr modelNode) {
    ScIterator5Ptr answerStructIterator = m_context.CreateIterator5(
            modelNode,
            ScType::ConstCommonArc,
            ScType::ConstNodeStructure,
            ScType::ConstPermPosArc,
            GenerateQuestionsKeynodes::nrel_answer_struct
    );
    ScAddr answerTemplateAddr;
    while (answerStructIterator->Next()) {
        answerTemplateAddr = answerStructIterator->Get(2);
    }

    return answerTemplateAddr;
}

ScAddr GenerateQuestionsAgent::findQuestionTemplate(ScAddr modelNode) {
    ScIterator5Ptr questionStructIterator = m_context.CreateIterator5(
            modelNode,
            ScType::ConstCommonArc,
            ScType::ConstNodeStructure,
            ScType::ConstPermPosArc,
            GenerateQuestionsKeynodes::nrel_question_struct
    );

    ScAddr questionTemplateAddr;
    while (questionStructIterator->Next()) {
        questionTemplateAddr = questionStructIterator->Get(2);
    }

    return questionTemplateAddr;
}

std::string GenerateQuestionsAgent::findQuestionText(ScAddr modelNode) {
    ScIterator5Ptr const questionTextIterator = m_context.CreateIterator5(
            modelNode,
            ScType::ConstCommonArc,
            ScType::ConstNodeLink,
            ScType::ConstPermPosArc,
            GenerateQuestionsKeynodes::nrel_question_text
    );

    ScAddr questionTextAddr;
    while (questionTextIterator->Next()) {
        questionTextAddr = questionTextIterator->Get(2);
    }

    std::string questionText;
    m_context.GetLinkContent(questionTextAddr, questionText);
    return questionText;
}

std::array<int, GenerateQuestionsAgent::QUESTION_MODEL_COUNT> GenerateQuestionsAgent::getModelQuestionCounts() {
    std::array<int, GenerateQuestionsAgent::QUESTION_MODEL_COUNT> resultArray{};
    std::array<int, GenerateQuestionsAgent::QUESTION_MODEL_COUNT - 1> randomDists =
            generateRandomIntegerNumberInRange(QUESTION_MODEL_COUNT - 1, QUESTION_COUNT, 2);
    resultArray[0] = 0;

    for (int i = 0; i < randomDists.size(); i++) {
        resultArray[i + 1] = randomDists[i];
    }
    std::sort(resultArray.begin(), resultArray.end());

    return resultArray;
}

void GenerateQuestionsAgent::generateQuestions(ScAddr questionModelClassAddr, ScStructure & structure) {
    ScAddr questionTuple = initResultStructure(structure);
    std::set<std::string> questions;
    std::array<int, QUESTION_MODEL_COUNT> questionModelsCount = getModelQuestionCounts();
    std::array<ScAddr, QUESTION_MODEL_COUNT> questionModels = findQuestionModel(questionModelClassAddr);

    ScIterator3Ptr const questionModelsIterator = m_context.CreateIterator3(
            questionModelClassAddr,
            ScType::ConstPermPosArc,
            ScType::ConstNode
    );

    ScAddr questionModel = questionModels[0];
    std::vector<int> indices(QUESTION_COUNT);
    std::iota(indices.begin(), indices.end(), 0);
    std::random_device randomDevice;
    std::mt19937 gen(randomDevice());
    std::shuffle(indices.begin(), indices.end(), gen);

    int questionIndex = 0;
    for (int i : indices) {
        for (int k = questionModelsCount.size() - 1; k >= 0; k--) {
            if (i >= questionModelsCount[k]) {
                questionModel = questionModels[k];
                break;
            }
        }
        generateQuestion(questionModel, findQuestionText(questionModel), questions, structure, questionTuple, questionIndex);
        questionIndex++;
    }
}

std::array<ScAddr, GenerateQuestionsAgent::QUESTION_MODEL_COUNT> GenerateQuestionsAgent::findQuestionModel(ScAddr questionModelClass) {
    std::array<ScAddr, GenerateQuestionsAgent::QUESTION_MODEL_COUNT> models;
    ScIterator3Ptr const questionModelsIterator = m_context.CreateIterator3(
            questionModelClass,
            ScType::ConstPermPosArc,
            ScType::ConstNode
    );
    int i = 0;
    while (questionModelsIterator->Next()) {
        models[i] = questionModelsIterator->Get(2);
        i++;
    }

    return models;
}

std::array<int, GenerateQuestionsAgent::QUESTION_MODEL_COUNT - 1> GenerateQuestionsAgent::generateRandomIntegerNumberInRange(int randomCount, int finish, int distance) {
    std::array<int, QUESTION_MODEL_COUNT - 1> result{};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, finish);

    int attempts = 0;
    int currentIndex = 0;

    while (currentIndex < randomCount && attempts < 1000) {
        int newNumber = dist(gen);
        bool isValid = true;

        for (int i = 0; i < currentIndex; ++i) {
            int diff = std::abs(newNumber - result[i]);
            if (diff < distance || diff > distance + 5) {
                isValid = false;
                break;
            }
        }

        if (isValid) {
            result[currentIndex] = newNumber;
            currentIndex++;
            attempts = 0;
        } else {
            attempts++;
        }
    }

    for (int i = currentIndex; i < QUESTION_MODEL_COUNT - 1; ++i) {
        result[i] = finish;
    }

    return result;
}

void GenerateQuestionsAgent::generateQuestion(const ScAddr questionModelAddr, std::string questionText,
                                              std::set<std::string> & questions, ScStructure & structure, ScAddr questionTuple, int questionNumber) {
    ScAddr questionTemplateAddr = GenerateQuestionsAgent::findQuestionTemplate(questionModelAddr);
    ScAddr answerTemplateAddr = GenerateQuestionsAgent::findAnswerTemplate(questionModelAddr);
    bool isNewQuestion = true;
    while (isNewQuestion) {
        ScTemplate questionTemplate;
        ScTemplateSearchResult questionSearchResult;
        ScTemplateResultItem questionResultItem;
        ScAddr questionPlaceholderTextAddr;
        std::string questionPlaceholderText;

        m_context.BuildTemplate(questionTemplate, questionTemplateAddr);
        m_context.SearchByTemplate(questionTemplate, questionSearchResult);
        questionSearchResult.Get(randomIntegerNumber(0, questionSearchResult.Size()), questionResultItem);
        questionResultItem.Get("_question_placeholder", questionPlaceholderTextAddr);
        m_context.GetLinkContent(questionPlaceholderTextAddr, questionPlaceholderText);

        ScTemplate answerTemplate;
        ScTemplateParams answerWithConcreteTermParams;
        ScAddr questionTerm;
        ScTemplateSearchResult answerSearchResult;
        ScTemplateResultItem answerResultItem;
        ScAddr correctAnswerAddr;
        std::string correctAnswerText;

        questionResultItem.Get("_term", questionTerm);
        answerWithConcreteTermParams.Add("_term", questionTerm);
        m_context.BuildTemplate(answerTemplate, answerTemplateAddr, answerWithConcreteTermParams);
        m_context.SearchByTemplate(answerTemplate, answerSearchResult);
        answerSearchResult.Get(0, answerResultItem);
        answerResultItem.Get("_correct_answer", correctAnswerAddr);
        m_context.GetLinkContent(correctAnswerAddr, correctAnswerText);

        std::string newQuestion = replacePlaceholder(questionText, "{}", questionPlaceholderText);
        if (questions.count(newQuestion) == 0) {
            questions.insert(newQuestion);
            ScAddr questionTextLink = m_context.GenerateLink(ScType::ConstNodeLink);
            m_context.SetLinkContent(questionTextLink, replacePlaceholder(questionText, "{}", questionPlaceholderText));
            ScAddr generatedQuestionNode = m_context.GenerateNode(ScType::ConstNode);
            ScAddr generateQuestionNodeClassConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, GenerateQuestionsKeynodes::generated_question, generatedQuestionNode);
            ScAddr answerTermConnector = m_context.GenerateConnector(ScType::ConstCommonArc, generatedQuestionNode, correctAnswerAddr);
            ScAddr questionTextLinkConnector = m_context.GenerateConnector(ScType::ConstCommonArc, generatedQuestionNode, questionTextLink);
            ScAddr generatedQuestionCorrectAnswerNonRelationConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, GenerateQuestionsKeynodes::nrel_generated_question_correct_answer, answerTermConnector);
            ScAddr generatedQuestionTextNonRelationConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, GenerateQuestionsKeynodes::nrel_generated_question_text, questionTextLinkConnector);
            ScAddr questionOrderNode = m_context.SearchElementBySystemIdentifier("rrel_" + std::to_string(questionNumber + 1));
            ScAddr tupleQuestionConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, questionTuple, generatedQuestionNode);
            ScAddr questionOrderNodeNonRelationConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, questionOrderNode, tupleQuestionConnector);
            ScAddr generatedQuestionModelConnector = m_context.GenerateConnector(ScType::ConstCommonArc, generatedQuestionNode, questionModelAddr);
            ScAddr generatedQuestionModelConnectorNonRelationConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, GenerateQuestionsKeynodes::nrel_generated_question_model, generatedQuestionModelConnector);
            ScAddr relatedTermConnector = m_context.GenerateConnector(ScType::ConstCommonArc, generatedQuestionNode, questionTerm);
            ScAddr relatedTermRelationConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, GenerateQuestionsKeynodes::nrel_related_term, relatedTermConnector);

            structure.Append(GenerateQuestionsKeynodes::nrel_related_term);
            structure.Append(relatedTermConnector);
            structure.Append(questionTerm);
            structure.Append(relatedTermRelationConnector);
            structure.Append(questionTextLink);
            structure.Append(answerTermConnector);
            structure.Append(questionTextLinkConnector);
            structure.Append(generatedQuestionCorrectAnswerNonRelationConnector);
            structure.Append(generatedQuestionTextNonRelationConnector);
            structure.Append(correctAnswerAddr);
            structure.Append(GenerateQuestionsKeynodes::generated_question);
            structure.Append(GenerateQuestionsKeynodes::nrel_generated_question_correct_answer);
            structure.Append(GenerateQuestionsKeynodes::nrel_generated_question_text);
            structure.Append(questionOrderNode);
            structure.Append(questionOrderNodeNonRelationConnector);
            structure.Append(tupleQuestionConnector);
            structure.Append(generateQuestionNodeClassConnector);
            structure.Append(generatedQuestionNode);
            structure.Append(generatedQuestionModelConnectorNonRelationConnector);
            structure.Append(generatedQuestionModelConnector);
            isNewQuestion = false;
        }
    }
}

ScAddr GenerateQuestionsAgent::GetActionClass() const {
    return GenerateQuestionsKeynodes::action_generate_questions;
}

ScResult GenerateQuestionsAgent::DoProgram(ScAction &action) {
    auto const & [questionModelAddr] = action.GetArguments<1>();
    ScStructure resultStructure = m_context.GenerateStructure();

    generateQuestions(questionModelAddr, resultStructure);
    action.SetResult(resultStructure);
    return action.FinishSuccessfully();
}

