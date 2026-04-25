#include "MessageSortAgent.hpp"
#include "../keynodes/keynodes.hpp"

using namespace sortMessageModule;

ScAddr SortMessageAgent::GetActionClass() const
{
    return Keynodes::action_sort_message;
}

ScResult SortMessageAgent::DoProgram(ScAction & action)
{
    auto const & [topicAddr, sortTypeLink] = action.GetArguments<2>();

    std::string sortType;
    m_context.GetLinkContent(sortTypeLink, sortType);
    SC_LOG_INFO("SortMessageAgent: sort_type = " + sortType);

    // Собираем сообщения топика
    ScTemplate searchTempl;
    searchTempl.Triple(
        topicAddr,
        ScType::VarPermPosArc,
        ScType::VarNode >> "message"
    );
    ScTemplateSearchResult searchResult;
    m_context.SearchByTemplate(searchTempl, searchResult);

    if (searchResult.Size() == 0)
        return action.FinishUnsuccessfully();

    std::vector<ScAddr> messages;
    for (size_t i = 0; i < searchResult.Size(); ++i)
        messages.push_back(searchResult[i]["message"]);

    // Сортируем по рейтингу если нужно
    if (sortType == "by_rating" || sortType == "by_relevance")
    {
        std::sort(messages.begin(), messages.end(),
            [this](ScAddr const & a, ScAddr const & b)
            {
                return GetRating(a) > GetRating(b);
            });
    }
    // by_date — порядок из sc-памяти хронологический, не трогаем

    // Формируем результирующую структуру
    ScAddr resultStruct = m_context.GenerateNode(ScType::ConstNodeClass);
    for (auto const & msg : messages)
        m_context.GenerateConnector(ScType::ConstPermPosArc, resultStruct, msg);

    // Помечаем лучшее сообщение
    if (!messages.empty())
    {
        ScAddr best = *std::max_element(messages.begin(), messages.end(),
            [this](ScAddr const & a, ScAddr const & b)
            {
                return GetRating(a) < GetRating(b);
            });

        ScAddr bestArc = m_context.GenerateConnector(
            ScType::ConstCommonArc, topicAddr, best);
        m_context.GenerateConnector(
            ScType::ConstPermPosArc, Keynodes::nrel_best_message, bestArc);
    }

    action.SetResult(resultStruct);
    return action.FinishSuccessfully();
}

int SortMessageAgent::GetRating(ScAddr const & messageAddr)
{
    int likes = 0, dislikes = 0;

    auto getCount = [this](ScAddr const & node, ScAddr const & nrel) -> int {
        ScTemplate templ;
        templ.Quintuple(
            node,
            ScType::VarCommonArc,
            ScType::VarNode >> "countLink",
            ScType::VarPermPosArc,
            nrel
        );
        ScTemplateSearchResult res;
        m_context.SearchByTemplate(templ, res);
        if (res.Size() > 0)
        {
            std::string countStr;
            m_context.GetLinkContent(res[0]["countLink"], countStr);
            try { return std::stoi(countStr); } catch (...) {}
        }
        return 0;
    };

    likes    = getCount(messageAddr, Keynodes::nrel_likes);
    dislikes = getCount(messageAddr, Keynodes::nrel_dislikes);
    return likes - dislikes;
}
