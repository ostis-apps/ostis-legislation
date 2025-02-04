#include <random>
#include <set>

#include "GenerateIncorrectAnswersAgent.hpp"

#include "keynodes/GenerateIncorrectAnswersKeynodes.hpp"

static int randomIntegerNumber(const int start, const int questionNumbers) {
    std::random_device randomDevice;
    std::mt19937 gen(randomDevice());
    std::uniform_int_distribution<> distribution(start, questionNumbers - 1);

    int randomIntegerNumber = distribution(gen);

    return randomIntegerNumber;
}

bool contains(const std::vector<ScAddr>& vec, const ScAddr& item) {
    for (const auto& element : vec) {
        if (element == item) { // Сравниваем по полю id
            return true;
        }
    }
    return false;
}

ScAddr GenerateIncorrectAnswersAgent::GetActionClass() const {
    return GenerateIncorrectAnswersKeynodes::action_generate_incorrect_answers;
}

ScResult GenerateIncorrectAnswersAgent::DoProgram(ScAction &action) {
    auto const & [generatedTestAddr] = action.GetArguments<1>();
    int incorrectAnswersCount = 3;

    std::vector<ScAddr> incorrectAnswerAddrVector;

    ScTemplate questionTemplate;
    questionTemplate.Quintuple(
                ScType::VarNodeTuple,
                ScType::VarPermPosArc,
                ScType::VarNode >> "_concrete_question",
                ScType::VarPermPosArc,
                generatedTestAddr
            );
    questionTemplate.Quintuple(
                "_concrete_question",
                ScType::VarCommonArc,
                ScType::VarNodeClass >> "_related_term",
                ScType::VarPermPosArc,
                GenerateIncorrectAnswersKeynodes::nrel_related_term
            );
    questionTemplate.Quintuple(
            "_concrete_question",
            ScType::VarCommonArc,
            ScType::VarNodeClass >> "_question_model",
            ScType::VarPermPosArc,
            GenerateIncorrectAnswersKeynodes::nrel_generated_question_model
            );
    questionTemplate.Quintuple(
                "_question_model",
                ScType::VarCommonArc,
                ScType::VarNodeStructure >> "_question_struct",
                ScType::VarPermPosArc,
                GenerateIncorrectAnswersKeynodes::nrel_question_struct
            );
    questionTemplate.Quintuple(
            "_question_model",
            ScType::VarCommonArc,
            ScType::VarNodeStructure >> "_answer_struct",
            ScType::VarPermPosArc,
            GenerateIncorrectAnswersKeynodes::nrel_answer_struct
            );

    ScTemplateSearchResult questionTemplateSearchResult;
    m_context.SearchByTemplate(questionTemplate, questionTemplateSearchResult);
    SC_LOG_INFO(questionTemplateSearchResult.Size());

    for (int i = 0; i < questionTemplateSearchResult.Size(); i++) {
        ScTemplateResultItem questionResult;
        questionTemplateSearchResult.Get(i, questionResult);

        ScAddr questionAddr;
        questionResult.Get("_concrete_question", questionAddr);

        ScTemplateParams termPlaceholder;
        ScAddr term;
        questionResult.Get("_related_term", term);
        termPlaceholder.Add("_term", term);

        ScTemplateSearchResult sectionSearchResult;
        ScTemplate answerTemplate;
        ScAddr answerTemplateAddr;
        questionResult.Get("_answer_struct", answerTemplateAddr);
        m_context.BuildTemplate(answerTemplate, answerTemplateAddr, termPlaceholder);
        m_context.SearchByTemplate(answerTemplate, sectionSearchResult);
        ScTemplateResultItem sectionResultItem;
        sectionSearchResult.Get(0, sectionResultItem);

        ScAddr sectionAddr;
        sectionResultItem.Get("_section", sectionAddr);
        SC_LOG_INFO(m_context.GetElementSystemIdentifier(sectionAddr));

        ScTemplateParams sectionPlaceholder;
        sectionPlaceholder.Add("_section", sectionAddr);
        ScTemplate incorrectAnswerTemplate;
        m_context.BuildTemplate(incorrectAnswerTemplate, answerTemplateAddr, sectionPlaceholder);
        ScTemplateSearchResult incorrectAnswerSearchResult;
        m_context.SearchByTemplate(incorrectAnswerTemplate, incorrectAnswerSearchResult);
        SC_LOG_INFO(incorrectAnswerSearchResult.Size());

        std::set<int> answersIndexes;
        for (int k = 0; k < incorrectAnswersCount; k++) {
            int questionNumber = randomIntegerNumber(0, incorrectAnswerSearchResult.Size()-1);
            ScTemplateResultItem incorrectAnswerResultItem;
            incorrectAnswerSearchResult.Get(questionNumber, incorrectAnswerResultItem);
            ScAddr incorrectAnswerTerm;
            if (answersIndexes.find(questionNumber) == answersIndexes.end()) {
                answersIndexes.insert(questionNumber);
                ScAddr incorrectAnswerTextAddr;
                incorrectAnswerResultItem.Get("_correct_answer", incorrectAnswerTextAddr);
                std::string incorrectAnswerText;
                m_context.GetLinkContent(incorrectAnswerTextAddr, incorrectAnswerText);
                SC_LOG_INFO(incorrectAnswerText);
                ScAddr incorrectAnswerLink = m_context.GenerateNode(ScType::ConstNodeLink);
                m_context.SetLinkContent(incorrectAnswerLink, incorrectAnswerText);
                ScAddr incorrectAnswerConnector = m_context.GenerateConnector(ScType::ConstCommonArc, questionAddr, incorrectAnswerLink);
                ScAddr incorrectAnswerNonRoleConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, GenerateIncorrectAnswersKeynodes::nrel_generated_question_incorrect_answer, incorrectAnswerConnector);
                continue;
            }
            k--;
        }
    }

    return action.FinishSuccessfully();
}

