#include <random>

#include "GenerateRecommendationsAgent.hpp"

#include "keynodes/GenerateRecommendationsKeynodes.hpp"

ScAddr GenerateRecommendationsAgent::GetActionClass() const
{
  return GenerateRecommendationsKeynodes::action_generate_recommendations;
}

ScResult GenerateRecommendationsAgent::DoProgram(ScAction & action)
{
  auto const & [questionModelAddr] = action.GetArguments<1>();

  ScTemplate userAnswerTemplate;
  userAnswerTemplate.Triple(
      ScType::VarNode >> "_question_node", ScType::VarCommonArc >> "_answer_connector", ScType::VarNodeLink);
  userAnswerTemplate.Triple(
      GenerateRecommendationsKeynodes::nrel_user_answer, ScType::VarPermPosArc, "_answer_connector");
  userAnswerTemplate.Triple(
      GenerateRecommendationsKeynodes::nrel_generated_question_incorrect_answer,
      ScType::VarPermPosArc,
      "_answer_connector");
  userAnswerTemplate.Triple(questionModelAddr, ScType::VarPermPosArc, "_question_node");
  userAnswerTemplate.Quintuple(
      ScType::VarNodeTuple,
      ScType::VarPermPosArc,
      "_question_node",
      ScType::VarPermPosArc,
      ScType::VarNodeRole >> "_question_index");
  userAnswerTemplate.Quintuple(
      "_question_node",
      ScType::VarCommonArc,
      ScType::VarNodeLink >> "_recommendation_text",
      ScType::VarPermPosArc,
      GenerateRecommendationsKeynodes::nrel_generated_question_correct_answer);

  ScTemplateSearchResult userAnswers;
  m_context.SearchByTemplate(userAnswerTemplate, userAnswers);

  std::string recommendation;

  SC_LOG_INFO(userAnswers.Size());
  if (userAnswers.IsEmpty())
  {
    recommendation = "Вы на все вопросы ответили верно. Поздравляю!";
  }
  else
  {
    recommendation = "Вы допустили " + std::to_string(userAnswers.Size())
                     + " ошибок. Давайте рассмотрим правильные ответы на вопросы, в которых вы ошиблись:\n";
  }

  for (int i = 0; i < userAnswers.Size(); i++)
  {
    ScTemplateResultItem concreteUserAnswer;
    userAnswers.Get(i, concreteUserAnswer);
    ScAddr indexNode;
    ScAddr concreteRecommendationNode;
    ScAddr questionNode;
    concreteUserAnswer.Get("_question_index", indexNode);
    concreteUserAnswer.Get("_question_node", questionNode);

    ScIterator5Ptr correctAnswerIterator = m_context.CreateIterator5(
        questionNode,
        ScType::ConstCommonArc,
        ScType::ConstNodeLink,
        ScType::ConstPermPosArc,
        GenerateRecommendationsKeynodes::nrel_generated_question_correct_answer);

    while (correctAnswerIterator->Next())
    {
      concreteRecommendationNode = correctAnswerIterator->Get(2);
    }
    //        concreteIncorrectUserAnswer.Get("_recommendation_text", concreteRecommendationNode);
    std::string index = m_context.GetElementSystemIdentifier(indexNode).substr(5, 6);
    std::string recommendationText;
    m_context.GetLinkContent(concreteRecommendationNode, recommendationText);

    recommendation += index + ". " + recommendationText + "\n";
  }

  ScIterator5Ptr testIterator = m_context.CreateIterator5(
      ScType::ConstNodeClass,
      ScType::ConstPermPosArc,
      ScType::ConstNodeTuple,
      ScType::ConstPermPosArc,
      questionModelAddr);

  ScAddr testNode;
  while (testIterator->Next())
  {
    testNode = testIterator->Get(0);
  }

  ScAddr recommendationNode = m_context.GenerateNode(ScType::ConstNodeLink);
  m_context.SetLinkContent(recommendationNode, recommendation);
  ScAddr recommendationConnector = m_context.GenerateConnector(ScType::ConstCommonArc, testNode, recommendationNode);
  ScAddr recommendationNoRoleConnector = m_context.GenerateConnector(
      ScType::ConstPermPosArc, GenerateRecommendationsKeynodes::nrel_test_recommendations, recommendationConnector);

  return action.FinishSuccessfully();
}