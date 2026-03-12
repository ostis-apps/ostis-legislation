#include "ChoiceNextQuestionAgent.hpp"
#include "keynodes/ChoiceNextQuestionKeynodes.hpp"
#include "keynodes/Keynodes.hpp"

using namespace utils;

ScAddr ChoiceNextQuestionAgent::GetActionClass() const
{
    return Keynodes::action_choice_next_question;
}

ScResult ChoiceNextQuestionAgent::DoProgram(ScAction & action)
{
    // Инициализируем keynodes
    ChoiceNextQuestionKeynodes::Initialize(&m_context);
    
    auto const & [user] = action.GetArguments<1>();
    
    // Ищем или создаём tuple с отвеченными вопросами
    ScTemplate answered_questions_template;
    answered_questions_template.Quintuple(
        user,
        ScType::VarCommonArc,
        ScType::VarNode >> "_answered_questions",
        ScType::VarPermPosArc,
        ChoiceNextQuestionKeynodes::nrel_asked_questions
    );
    
    ScTemplateSearchResult answered_questions_search_result;
    ScAddr answered_questions;
    
    if (m_context.SearchByTemplate(answered_questions_template, answered_questions_search_result)
        && answered_questions_search_result.Size() == 1)
    {
        ScTemplateResultItem answered_questions_item;
        answered_questions_search_result.Get(0, answered_questions_item);
        answered_questions_item.Get("_answered_questions", answered_questions);
    }
    else
    {
        // Создаём новый tuple для отвеченных вопросов
        answered_questions = m_context.GenerateNode(ScType::ConstNodeTuple);
        ScAddr answered_questions_connector = m_context.GenerateConnector(ScType::ConstCommonArc, user, answered_questions);
        m_context.GenerateConnector(
            ScType::ConstPermPosArc, 
            ChoiceNextQuestionKeynodes::nrel_asked_questions, 
            answered_questions_connector
        );
    }
    
    // Находим все доступные вопросы
    ScIterator3Ptr all_questions_iterator = m_context.CreateIterator3(
        ChoiceNextQuestionKeynodes::concept_question, 
        ScType::ConstPermPosArc, 
        ScType::ConstNode
    );
    
    std::vector<ScAddr> unanswered_questions;
    
    while (all_questions_iterator->Next())
    {
        ScAddr question = all_questions_iterator->Get(2);
        
        // Проверяем, был ли вопрос уже задан
        ScIterator3Ptr answered_iterator =
            m_context.CreateIterator3(answered_questions, ScType::ConstPermPosArc, question);
        
        if (!answered_iterator->Next())
        {
            unanswered_questions.push_back(question);
        }
    }
    
    if (unanswered_questions.empty())
    {
        return action.FinishUnsuccessfully();
    }
    
    // Выбираем первый неотвеченный вопрос
    ScAddr next_question = unanswered_questions[0];
    
    // Добавляем вопрос в список отвеченных
    m_context.GenerateConnector(ScType::ConstPermPosArc, answered_questions, next_question);
    
    // **ИСПРАВЛЕНИЕ: Удаляем старую связь nrel_current_question (если есть)**
    ScTemplate old_current_question_template;
    old_current_question_template.Quintuple(
        user,
        ScType::VarCommonArc >> "_old_arc",
        ScType::VarNode,
        ScType::VarPermPosArc >> "_old_nrel_arc",
        ChoiceNextQuestionKeynodes::nrel_current_question
    );
    
    ScTemplateSearchResult old_current_question_result;
    if (m_context.SearchByTemplate(old_current_question_template, old_current_question_result))
    {
        for (size_t i = 0; i < old_current_question_result.Size(); ++i)
        {
            ScAddr old_arc, old_nrel_arc;
            old_current_question_result[i].Get("_old_arc", old_arc);
            old_current_question_result[i].Get("_old_nrel_arc", old_nrel_arc);
            
            m_context.EraseElement(old_nrel_arc);
            m_context.EraseElement(old_arc);
        }
    }
    
    // **ИСПРАВЛЕНИЕ: Создаём новую связь nrel_current_question**
    ScAddr current_question_arc = m_context.GenerateConnector(ScType::ConstCommonArc, user, next_question);
    m_context.GenerateConnector(
        ScType::ConstPermPosArc,
        ChoiceNextQuestionKeynodes::nrel_current_question,
        current_question_arc
    );
    
    return action.FinishSuccessfully();
}
