#include "GenerateIncorrectAnswersAgent.hpp"

#include "keynodes/GenerateIncorrectAnswersKeynodes.hpp"

ScAddr GenerateIncorrectAnswersAgent::GetActionClass() const {
    return GenerateIncorrectAnswersKeynodes::action_generate_incorrect_answers;
}

ScResult GenerateIncorrectAnswersAgent::DoProgram(ScAction &action) {
    auto const & [generatedTestAddr] = action.GetArguments<1>();
    int incorrectAnswersCount = 3;

    // Итератор по всем вопросам заданного теста
    ScIterator5Ptr const generatedQuestionIterator = m_context.CreateIterator5(
            ScType::ConstNodeTuple,
            ScType::ConstPermPosArc,
            ScType::ConstNode,
            ScType::ConstPermPosArc,
            generatedTestAddr
            );

    while(generatedQuestionIterator->Next()) {
        SC_LOG_INFO("generatedQuestionIterator: Enter");
        // Итератор по модели вопроса
        ScIterator5Ptr const questionModelIterator = m_context.CreateIterator5(
                generatedQuestionIterator->Get(2),
                ScType::ConstCommonArc,
                ScType::ConstNode,
                ScType::ConstPermPosArc,
                GenerateIncorrectAnswersKeynodes::nrel_generated_question_model
        );

        ScAddr questionModel;
        while (questionModelIterator->Next()) {
            SC_LOG_INFO("questionModelIterator: Enter");
            questionModel = questionModelIterator->Get(2);
        }

        // Итератор для обхода структур ответов
        ScIterator5Ptr const answerStructIterator = m_context.CreateIterator5(
                questionModel,
                ScType::ConstCommonArc,
                ScType::ConstNodeStructure,
                ScType::ConstPermPosArc,
                GenerateIncorrectAnswersKeynodes::nrel_answer_struct
        );

        ScAddr answerTemplateAddr;
        while (answerStructIterator->Next()) {
            SC_LOG_INFO("answerStructIterator: Enter");
            answerTemplateAddr = answerStructIterator->Get(2);
        }

        // Итератор для связанных понятий с вопросом
        ScIterator5Ptr relatedTermIterator = m_context.CreateIterator5(
                generatedQuestionIterator->Get(2),
                ScType::ConstCommonArc,
                ScType::ConstNodeClass,
                ScType::ConstPermPosArc,
                GenerateIncorrectAnswersKeynodes::nrel_related_term
                );

        while(relatedTermIterator->Next()) {
            SC_LOG_INFO("relatedTermIterator: Enter");
            // Итератор по классу связанной сущности
            ScIterator3Ptr relatedTermClassIterator = m_context.CreateIterator3(
                    ScType::ConstNodeClass,
                    ScType::ConstPermPosArc,
                    relatedTermIterator->Get(2)
                    );

            while (relatedTermClassIterator->Next()) {
                SC_LOG_INFO("relatedTermClassIterator: Enter");
                // Итератор по всем одноклассовым понятиям
                ScIterator3Ptr wrongAnswersIterator = m_context.CreateIterator3(
                        relatedTermClassIterator->Get(0),
                        ScType::ConstPermPosArc,
                        ScType::ConstNodeClass
                        );
                while (wrongAnswersIterator->Next()) {
                    SC_LOG_INFO("wrongAnswersIterator: Enter");
                    ScTemplate answerTemplate;
                    m_context.BuildTemplate(answerTemplate, answerTemplateAddr);
                    // Дополнение обычного шаблона для текста ответа принадлежностью к такому же классу
                    answerTemplate.Triple(
                            relatedTermClassIterator->Get(0),
                            ScType::VarPermPosArc,
                            "_term"
                            );

                    // Поиск результатов поиска по шаблону
                    ScTemplateSearchResult result;
                    m_context.SearchByTemplate(answerTemplate, result);
                    SC_LOG_INFO(result.Size());

                    while (incorrectAnswersCount > 0) {
                        SC_LOG_INFO("incorrectAnswersCount: Enter");
                        ScTemplateResultItem concreteIncorrectAnswer;
                        result.Get(incorrectAnswersCount, concreteIncorrectAnswer);

                        ScAddr concreteIncorrectAnswerTextAddr;
                        concreteIncorrectAnswer.Get("_correct_answer", concreteIncorrectAnswerTextAddr);

                        if (concreteIncorrectAnswerTextAddr == relatedTermIterator->Get(2)) {
                            continue;
                        }

                        ScAddr incorrectAnswerConnector = m_context.GenerateConnector(ScType::ConstCommonArc, generatedQuestionIterator->Get(2), concreteIncorrectAnswerTextAddr);
                        ScAddr nrelIncorrectAnswerConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, GenerateIncorrectAnswersKeynodes::nrel_generated_question_incorrect_answer, incorrectAnswerConnector);

                        incorrectAnswersCount--;
                    }
                }
            }
        }
    }

    return action.FinishSuccessfully();
}

