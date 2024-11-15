#include "GenerateQuestionsAgent.hpp"

#include "keynodes/GenerateQuestionsKeynodes.hpp"

std::string replacePlaceholder(const std::string &str, const std::string &placeholder, const std::string &replacement) {
    std::string result = str; // Копируем исходную строку, чтобы не изменять её
    size_t pos = result.find(placeholder);

    while (pos != std::string::npos) {
        result.replace(pos, placeholder.length(), replacement);
        pos = result.find(placeholder, pos + replacement.length());
    }

    return result;
}

ScAddr GenerateQuestionsAgent::GetActionClass() const {
    return GenerateQuestionsKeynodes::action_generate_questions;
}

ScResult GenerateQuestionsAgent::DoProgram(ScAction &action) {
    auto const & [questionModelAddr] = action.GetArguments<1>();

    // Итератор для поиска всех моделей вопроса
    ScIterator3Ptr const questionModelsIterator = m_context.CreateIterator3(
                questionModelAddr,
                ScType::ConstPermPosArc,
                GenerateQuestionsKeynodes::define_model
            );

    // Заполнение итоговой структуры базовыми элементами
    ScStructure scStructure = m_context.GenerateStructure();
    ScAddr questionsTuple = m_context.GenerateNode(ScType::ConstNodeTuple);
    ScAddr questionsTupleConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, GenerateQuestionsKeynodes::test, questionsTuple);
    scStructure.Append(questionsTuple);
    scStructure.Append(questionsTupleConnector);
    scStructure.Append(GenerateQuestionsKeynodes::test);

    // Проходим по всем моделям вопросов
    while (questionModelsIterator->Next()) {
        scStructure.Append(questionModelsIterator->Get(2));
        SC_LOG_INFO("Модель вопроса: " + m_context.GetElementSystemIdentifier(questionModelsIterator->Get(2)));
        // Итератор для обхода структур вопросов
        ScIterator5Ptr const questionStructIterator = m_context.CreateIterator5(
                questionModelsIterator->Get(2),
                ScType::ConstCommonArc,
                ScType::ConstNodeStructure,
                ScType::ConstPermPosArc,
                GenerateQuestionsKeynodes::nrel_question_struct
                );

        // Итератор для обхода текстов вопроса
        ScIterator5Ptr const questionTextIterator = m_context.CreateIterator5(
                questionModelsIterator->Get(2),
                ScType::ConstCommonArc,
                ScType::ConstNodeLink,
                ScType::ConstPermPosArc,
                GenerateQuestionsKeynodes::nrel_question_text
        );

        // Итератор для обхода структур ответов
        ScIterator5Ptr const answerStructIterator = m_context.CreateIterator5(
                questionModelsIterator->Get(2),
                ScType::ConstCommonArc,
                ScType::ConstNodeStructure,
                ScType::ConstPermPosArc,
                GenerateQuestionsKeynodes::nrel_answer_struct
        );

        // Поиск структуры ответа
        ScAddr answerTemplateAddr;
        while (answerStructIterator->Next()) {
            answerTemplateAddr = answerStructIterator->Get(2);
        }

        // Поиск текста вопроса(без подстановки)
        std::string questionFullText;

        while (questionTextIterator->Next()) {
            m_context.GetLinkContent(questionTextIterator->Get(2), questionFullText);
            SC_LOG_INFO(questionFullText);
        }

        // Обход по стуктурам вопроса
        while (questionStructIterator->Next()) {
            // Сборка шаблона для вопросов
            ScTemplate questionTemplate;
            m_context.BuildTemplate(questionTemplate, questionStructIterator->Get(2));

            // Поиск результатов поиска по шаблону
            ScTemplateSearchResult result;
            m_context.SearchByTemplate(questionTemplate, result);
            SC_LOG_INFO(result.Size());

            // Генерация 10 вопросов по модели
            for (size_t i = 0; i < 10; ++i) {
                ScTemplateResultItem concreteResult;
                result.Get(i, concreteResult);

                ScAddr questionStructAddr;
                std::string placeholderText;
                concreteResult.Get("_question_placeholder", questionStructAddr);
                m_context.GetLinkContent(questionStructAddr, placeholderText);

                ScTemplate answerTemplate;
                ScTemplateParams concreteTermParams;
                ScAddr questionTerm;
                concreteResult.Get("_term", questionTerm);
                concreteTermParams.Add("_term", questionTerm);
                m_context.BuildTemplate(answerTemplate, answerTemplateAddr, concreteTermParams);

                ScTemplateSearchResult answerTemplateSearchResult;
                m_context.SearchByTemplate(answerTemplate, answerTemplateSearchResult);
                ScTemplateResultItem answerTemplateResultItem;
                answerTemplateSearchResult.Get(0, answerTemplateResultItem);
                ScAddr correctAnswerAddr;
                std::string correctAnswerText;
                answerTemplateResultItem.Get("_correct_answer", correctAnswerAddr);
                m_context.GetLinkContent(correctAnswerAddr, correctAnswerText);

                SC_LOG_INFO("Текст вопроса: " + replacePlaceholder(questionFullText, "{}", placeholderText));
                SC_LOG_INFO("Ответ на вопрос: " + correctAnswerText);

                ScAddr questionTextLink = m_context.GenerateLink(ScType::ConstNodeLink);
                m_context.SetLinkContent(questionTextLink, replacePlaceholder(questionFullText, "{}", placeholderText));
                ScAddr generatedQuestionNode = m_context.GenerateNode(ScType::ConstNode);
                ScAddr generateQuestionNodeClassConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, GenerateQuestionsKeynodes::generated_question, generatedQuestionNode);
                ScAddr answerTermConnector = m_context.GenerateConnector(ScType::ConstCommonArc, generatedQuestionNode, correctAnswerAddr);
                ScAddr questionTextLinkConnector = m_context.GenerateConnector(ScType::ConstCommonArc, generatedQuestionNode, questionTextLink);
                ScAddr generatedQuestionCorrectAnswerNonRelationConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, GenerateQuestionsKeynodes::nrel_generated_question_correct_answer, answerTermConnector);
                ScAddr generatedQuestionTextNonRelationConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, GenerateQuestionsKeynodes::nrel_generated_question_text, questionTextLinkConnector);
                ScAddr questionOrderNode = m_context.SearchElementBySystemIdentifier("rrel_" + std::to_string(i + 1));
                ScAddr tupleQuestionConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, questionsTuple, generatedQuestionNode);
                ScAddr questionOrderNodeNonRelationConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, questionOrderNode, tupleQuestionConnector);
                ScAddr generatedQuestionModelConnector = m_context.GenerateConnector(ScType::ConstCommonArc, generatedQuestionNode, questionModelsIterator->Get(2));
                ScAddr generatedQuestionModelConnectorNonRelationConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, GenerateQuestionsKeynodes::nrel_generated_question_model, generatedQuestionModelConnector);
                ScAddr relatedTermConnector = m_context.GenerateConnector(ScType::ConstCommonArc, generatedQuestionNode, questionTerm);
                ScAddr relatedTermRelationConnector = m_context.GenerateConnector(ScType::ConstPermPosArc, GenerateQuestionsKeynodes::nrel_related_term, relatedTermConnector);

                scStructure.Append(GenerateQuestionsKeynodes::nrel_related_term);
                scStructure.Append(relatedTermConnector);
                scStructure.Append(questionTerm);
                scStructure.Append(relatedTermRelationConnector);
                scStructure.Append(questionTextLink);
                scStructure.Append(answerTermConnector);
                scStructure.Append(questionTextLinkConnector);
                scStructure.Append(generatedQuestionCorrectAnswerNonRelationConnector);
                scStructure.Append(generatedQuestionTextNonRelationConnector);
                scStructure.Append(correctAnswerAddr);
                scStructure.Append(GenerateQuestionsKeynodes::generated_question);
                scStructure.Append(GenerateQuestionsKeynodes::nrel_generated_question_correct_answer);
                scStructure.Append(GenerateQuestionsKeynodes::nrel_generated_question_text);
                scStructure.Append(questionOrderNode);
                scStructure.Append(questionOrderNodeNonRelationConnector);
                scStructure.Append(tupleQuestionConnector);
                scStructure.Append(generateQuestionNodeClassConnector);
                scStructure.Append(generatedQuestionNode);
                scStructure.Append(generatedQuestionModelConnectorNonRelationConnector);
                scStructure.Append(generatedQuestionModelConnector);
            }
        }
    }

    // TODO: генерировать не все вопросы, а 10
    // TODO: добавить функции для повторных элементов

    action.SetResult(scStructure);
    return action.FinishSuccessfully();
}

