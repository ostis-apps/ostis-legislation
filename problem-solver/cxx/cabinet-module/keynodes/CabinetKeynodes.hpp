#pragma once

#include <sc-memory/sc_keynodes.hpp>
#include <string>

class CabinetKeynodes : public ScKeynodes
{
public:
  // -------- Actions --------
  static inline ScKeynode const action_add_bookmark{
    "action_add_bookmark", ScType::ConstNodeClass};

  static inline ScKeynode const action_add_history_entry{
    "action_add_history_entry", ScType::ConstNodeClass};

  static inline ScKeynode const action_add_note{
    "action_add_note", ScType::ConstNodeClass};

  // -------- Bookmarks --------
  static inline ScKeynode const concept_bookmark{
    "concept_bookmark", ScType::ConstNodeClass};

  static inline ScKeynode const nrel_user_bookmarks{
    "nrel_user_bookmarks", ScType::ConstNodeNonRole};

  static inline ScKeynode const nrel_bookmark_article{
    "nrel_bookmark_article", ScType::ConstNodeNonRole};

  static inline ScKeynode const nrel_bookmark_tags{
    "nrel_bookmark_tags", ScType::ConstNodeNonRole};

  static inline ScKeynode const nrel_bookmark_date{
    "nrel_bookmark_date", ScType::ConstNodeNonRole};

  // -------- History --------
  static inline ScKeynode const concept_user_query{
    "concept_user_query", ScType::ConstNodeClass};

  static inline ScKeynode const nrel_query_history{
    "nrel_query_history", ScType::ConstNodeNonRole};

  static inline ScKeynode const nrel_query_text{
    "nrel_query_text", ScType::ConstNodeNonRole};

  static inline ScKeynode const nrel_query_timestamp{
    "nrel_query_timestamp", ScType::ConstNodeNonRole};

  static inline ScKeynode const nrel_viewed_article{
    "nrel_viewed_article", ScType::ConstNodeNonRole};

  // -------- Notes --------
  static inline ScKeynode const concept_user_note{
    "concept_user_note", ScType::ConstNodeClass};

  static inline ScKeynode const nrel_user_notes{
    "nrel_user_notes", ScType::ConstNodeNonRole};

  static inline ScKeynode const nrel_note_article{
    "nrel_note_article", ScType::ConstNodeNonRole};

  static inline ScKeynode const nrel_note_text{
    "nrel_note_text", ScType::ConstNodeNonRole};

  static inline ScKeynode const nrel_note_created{
    "nrel_note_created", ScType::ConstNodeNonRole};

  static inline ScKeynode const nrel_note_updated{
    "nrel_note_updated", ScType::ConstNodeNonRole};

  // -------- Roles for action arguments --------
  // Можно использовать существующие rrel_1..rrel_9, но если хочешь отдельные — так:
  static inline ScKeynode const rrel_user{
    "rrel_user", ScType::ConstNodeRole};

  static inline ScKeynode const rrel_article{
    "rrel_article", ScType::ConstNodeRole};

  static inline ScKeynode const rrel_text{
    "rrel_text", ScType::ConstNodeRole};

  static inline ScKeynode const rrel_tags{
    "rrel_tags", ScType::ConstNodeRole};

  static inline ScKeynode const rrel_date{
    "rrel_date", ScType::ConstNodeRole};

  static inline ScKeynode const rrel_created{
    "rrel_created", ScType::ConstNodeRole};

  static inline ScKeynode const rrel_updated{
    "rrel_updated", ScType::ConstNodeRole};
};