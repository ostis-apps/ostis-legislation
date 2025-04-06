#include "DelEventAgent.hpp"
#include "keynodes/DelEventKeynodes.hpp"

ScAddr DelEventAgent::GetActionClass() const
{
    return DelEventKeynodes::action_del_event;
}

ScResult DelEventAgent::DoProgram(ScAction & action)
{
    auto const & [nameAddr] = action.GetArguments<1>();

    std::string name;
    m_context.GetLinkContent(nameAddr, name);
    
    if (!nameAddr.IsValid())
    {
        SC_LOG_ERROR("Некорректное имя события");
        return action.FinishUnsuccessfully();
    }
    
    SC_LOG_INFO("Поиск события для удаления");
    ScIterator3Ptr it = m_context.Iterator3(ScType::NodeConst, ScType::EdgeAccessConstPosPerm, nameAddr);
    
    if (!it->Next())
    {
        SC_LOG_ERROR("Событие не найдено");
        return action.FinishUnsuccessfully();
    }
    
    ScAddr eventNode = it->Get(0);
    
    SC_LOG_INFO("Удаление события");
    m_context.EraseElement(eventNode);
    
    SC_LOG_INFO("Событие успешно удалено");
    return action.FinishSuccessfully();
}
