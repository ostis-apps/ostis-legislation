#include "RateMessageAgent.hpp"
#include "../keynodes/keynodes.hpp"

using namespace rateMessageModule;

ScAddr RateMessageAgent::GetActionClass() const
{
    return Keynodes::action_rate_message;
}

ScResult RateMessageAgent::DoProgram(ScAction & action)
{
    auto const & [messageAddr, ratingLink] = action.GetArguments<2>();

    std::string ratingType;
    m_context.GetLinkContent(ratingLink, ratingType);

    SC_LOG_INFO("Оценка сообщения: " + ratingType);

    bool isLike = (ratingType == "like");
    ScAddr nrelRating = isLike
        ? m_context.SearchElementBySystemIdentifier("nrel_likes")
        : m_context.SearchElementBySystemIdentifier("nrel_dislikes");

    ScTemplate searchTemplate;
    searchTemplate.Quintuple(
        messageAddr,
        ScType::VarCommonArc,
        ScType::VarNode >> "countLink",
        ScType::VarPermPosArc,
        nrelRating
    );

    ScTemplateSearchResult searchResult;
    m_context.SearchByTemplate(searchTemplate, searchResult);

    if (searchResult.Size() > 0)
    {
        ScAddr existingLink = searchResult[0]["countLink"];
        std::string countStr;
        m_context.GetLinkContent(existingLink, countStr);
        int count = 0;
        try { count = std::stoi(countStr); } catch (...) { count = 0; }
        m_context.SetLinkContent(existingLink, std::to_string(count + 1));
    }
    else
    {
        ScAddr countLink = m_context.GenerateLink();
        m_context.SetLinkContent(countLink, std::string("1"));
        ScAddr arc = m_context.GenerateConnector(ScType::ConstCommonArc, messageAddr, countLink);
        m_context.GenerateConnector(ScType::ConstPermPosArc, nrelRating, arc);
    }

    SC_LOG_INFO("Оценка успешно сохранена");
    return action.FinishSuccessfully();
}
