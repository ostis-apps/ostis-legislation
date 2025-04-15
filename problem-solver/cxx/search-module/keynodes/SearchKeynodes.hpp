#pragma once

#include <sc-memory/sc_keynodes.hpp>

class SearchKeynodes : public ScKeynodes {
public:
  // В какой части понятия искать вхождение слова/фразы
  // CONCEPT_FULL_SEARCH - Искать в оглавлении и теле понятия
  // CONCEPT_TITLE_SEARCH - Искать в оглавлении понятия
  // CONCEPT_BODY_SEARCH - Искать в теле понятия
  static inline ScKeynode const CONCEPT_FULL_SEARCH{"CONCEPT_FULL_SEARCH", ScType::NodeConstClass};
  static inline ScKeynode const CONCEPT_TITLE_SEARCH{"CONCEPT_TITLE_SEARCH", ScType::NodeConstClass};
  static inline ScKeynode const CONCEPT_BODY_SEARCH{"CONCEPT_BODY_SEARCH", ScType::NodeConstClass};

  // Область видимости поиска
  // CONCEPTS_SEARCH - Искать в определениях
  // ARTICLES_SEARCH - Искать в статьях
  // FULL_SEARCH - Искать в определениях и статьях
  static inline ScKeynode const CONCEPTS_SEARCH{"CONCEPTS_SEARCH", ScType::NodeConstClass};
  static inline ScKeynode const ARTICLES_SEARCH{"ARTICLES_SEARCH", ScType::NodeConstClass};
  static inline ScKeynode const FULL_SEARCH{"FULL_SEARCH", ScType::NodeConstClass};

  // Вспомогательные узлы
  static inline ScKeynode const BELARUS_LEGAL_TERM{"belarus_legal_term", ScType::NodeConstClass};
  static inline ScKeynode const BELARUS_LEGAL_ARTICLE{"belarus_legal_article", ScType::NodeConstClass};
  static inline ScKeynode const NREL_SC_TEXT_TRANSLATION{"nrel_sc_text_translation", ScType::ConstNodeNonRole};
  static inline ScKeynode const RREL_KEY_SC_ELEMENT{"rrel_key_sc_element", ScType::ConstNodeRole};
  static inline ScKeynode const RREL_EXAMPLE{"rrel_example", ScType::ConstNodeRole};
  static inline ScKeynode const NREL_MAIN_IDTF{"nrel_main_idtf", ScType::ConstNodeNonRole};
  static inline ScKeynode const LANG_RU{"lang_ru", ScType::NodeConstClass};
  static inline ScKeynode const LANG_BY{"lang_by", ScType::NodeConstClass};
  static inline ScKeynode const LANG_EN{"lang_en", ScType::NodeConstClass};

  // Узел действия агента
  static inline ScKeynode const action_search{"action_search", ScType::NodeConstClass};
};