#include "DelEventAgent.hpp"
#include <iostream>
#include <sc-memory/sc_memory.hpp>

#include "keynodes/DelEventKeynodes.hpp"

using namespace utils;

ScAddr DelEventAgent::GetActionClass() const
{
    return DelEventKeynodes::action_del_event;
}

ScResult DelEventAgent::DoProgram(ScAction & action)
{
    auto const & [user_node, eventNameAddr] = action.GetArguments<2>(); 

    // Проверка валидности узла пользователя
    if (!m_context.IsElement(user_node)) 
    {
        m_logger.Error("Недействительный ScAddr для пользователя.");
        return action.FinishUnsuccessfully();
    }

    // Проверка валидности адреса имени события
    if (!m_context.IsElement(eventNameAddr)) 
    {
        m_logger.Error("Недействительный ScAddr для имени события.");
        return action.FinishUnsuccessfully();
    }

    // Проверка, является ли узел зарегистрированным пользователем
    if (!m_context.CheckConnector(DelEventKeynodes::registered_user, user_node, ScType::ConstPermPosArc))
    {
        m_logger.Error("Указанный адрес не распознан как пользователь.");
        return action.FinishUnsuccessfully();
    }

    // Извлечение имени события
    std::string eventName;
    if (!m_context.GetLinkContent(eventNameAddr, eventName))
    {
        m_logger.Error("Не удалось получить имя события из ссылки.");
        return action.FinishUnsuccessfully();
    }

    m_logger.Info("Поиск события '" + eventName + "' для пользователя...");

    try {
        remove_event_link(user_node, eventName);
        return action.FinishSuccessfully();
    }
    catch (ScException const & exception)
    {
        m_logger.Error("Произошло исключение: " + std::string(exception.Message()));
        return action.FinishWithError();
    }
}

void DelEventAgent::remove_event_link(ScAddr const & userAddr, std::string const & eventName)
{
    // Поиск всех ссылок с содержимым eventName
    ScAddrSet const & linkAddrs1 = m_context.SearchLinksByContent(eventName);
    SC_LOG_INFO("Найдено " << linkAddrs1.size() << " ссылок с содержимым '" << eventName << "'");

    for (auto const & item : linkAddrs1)
    {
        // Создание шаблона для каждой найденной ссылки
        ScTemplate fromLinkSideTemplate;

        // Связь: event -> link через nrel_event_name
        fromLinkSideTemplate.Quintuple(
            ScType::NodeVar >> "event",
            ScType::EdgeDCommonVar >> "_event_name_edge",
            item >> "link",
            ScType::VarPermPosArc,
            DelEventKeynodes::nrel_event_name
        );

        // Связь: user -> event через nrel_user_event
        fromLinkSideTemplate.Quintuple(
            userAddr >> "user",
            ScType::EdgeDCommonVar >> "_user_event_edge",
            ScType::NodeVar >> "event",
            ScType::VarPermPosArc,
            DelEventKeynodes::nrel_user_event
        );

        ScTemplateSearchResult linkSearchResult;
        m_context.SearchByTemplate(fromLinkSideTemplate, linkSearchResult);
        SC_LOG_INFO("Размер результата поиска для ссылки: " << linkSearchResult.Size());

        if (!linkSearchResult.IsEmpty())
        {
            // Обработка первого найденного результата
            ScTemplateResultItem resultItem;
            linkSearchResult.Get(0, resultItem);

            ScAddr userEventEdge;
            resultItem.Get("_user_event_edge", userEventEdge); // Получаем дугу между user и event

            // Удаление связи между пользователем и событием
            if (m_context.IsElement(userEventEdge))
            {
                m_context.EraseElement(userEventEdge);
                m_logger.Info("Связь между пользователем и событием успешно удалена.");
            }
            else
            {
                m_logger.Error("Не удалось найти дугу для удаления.");
            }
        }
    }
}
